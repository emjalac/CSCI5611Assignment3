#ifdef __APPLE__
#include "include/World.h"
#else
#include "World.h"
#endif

using namespace std;


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
World::World()
{
	characters = new WorldObject*[10]; //max of 10 characters
	starts = new Node*[10];
	goals = new Node*[10];
	obstacles = new WorldObject*[10]; //max of 10 obstacles
	cur_num_obstacles = 0;
	max_num_milestones = 100; //max of 100 milestones
	milestones = new Node**[10]; //max of 10 characters
	cur_num_milestones = new int[10];
	total_verts = 0;
	
	//default 20x20 world
	min_x = -10.0f;
	max_x = 10.0f;
	min_z = -10.0f;
	max_z = 10.0f;
	y = 0;
	border = new Line*[4];
	border[0] = new Line(Vec3D(min_x,0,min_z),Vec3D(min_x,0,max_z));
	border[1] = new Line(Vec3D(min_x,0,max_z),Vec3D(max_x,0,max_z));
	border[2] = new Line(Vec3D(max_x,0,max_z),Vec3D(max_x,0,min_z));
	border[3] = new Line(Vec3D(max_x,0,min_z),Vec3D(min_x,0,min_z));

	path_exists = new bool[10];
	shortest_paths = new Path*[10];

	show_graph = 1;
}

World::World(int length, int width)
{
	characters = new WorldObject*[10]; //max of 10 characters
	starts = new Node*[10];
	goals = new Node*[10];
	obstacles = new WorldObject*[10];
	cur_num_obstacles = 0;
	max_num_milestones = 100; //max of 100 milestones
	milestones = new Node**[10];
	cur_num_milestones = new int[10];
	total_verts = 0;

	min_x = -length/2;
	max_x = length/2;
	min_z = -width/2;
	max_z = width/2;
	y = 0;
	border = new Line*[4];
	border[0] = new Line(Vec3D(min_x,0,min_z),Vec3D(min_x,0,max_z));
	border[1] = new Line(Vec3D(min_x,0,max_z),Vec3D(max_x,0,max_z));
	border[2] = new Line(Vec3D(max_x,0,max_z),Vec3D(max_x,0,min_z));
	border[3] = new Line(Vec3D(max_x,0,min_z),Vec3D(min_x,0,min_z));

	path_exists = new bool[10];
	shortest_paths = new Path*[10];

	show_graph = 1;
}

World::~World()
{
	for (int i = 0; i < num_characters; i++)
	{
		delete characters[i];
		delete starts[i];
		delete goals[i];
	}
	delete characters;
	delete starts;
	delete goals;
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		delete obstacles[i];
	}
	delete obstacles;
	for (int i = 0; i < num_characters; i++)
	{
		for (int j = 0; j < cur_num_milestones[i]; j++) //assumes same num of milestones found for each character
		{
			delete milestones[i][j];
		}
		delete milestones[i];
	}
	delete milestones;
	for (int i = 0; i < 4; i++)
	{
		delete border[i];
	}
	delete border;
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void World::setCurNumObstacles(int num)
{
	cur_num_obstacles = num;
}

void World::setShowGraph(int s)
{
	show_graph = s;
}


/*----------------------------*/
// GETTERS
/*----------------------------*/
int World::getCurNumObstacles()
{
	return cur_num_obstacles;
}

int World::showGraph()
{
	return show_graph;
}


/*----------------------------*/
// OTHERS
/*----------------------------*/
//load in all models and store data into the modelData array
bool World::loadModelData()
{
	/////////////////////////////////
	//LOAD IN MODELS
	/////////////////////////////////
	//CUBE
	CUBE_START = 0;
	CUBE_VERTS = 0;
	#ifdef __APPLE__
	float* cubeData = util::loadModel("../models/cube.txt", CUBE_VERTS);
	#else
	float* cubeData = util::loadModel("models/cube.txt", CUBE_VERTS);
	#endif
	cout << "\nNumber of vertices in cube model : " << CUBE_VERTS << endl;
	total_verts += CUBE_VERTS;

	//SPHERE
	SPHERE_START = CUBE_VERTS;
	SPHERE_VERTS = 0;
	#ifdef __APPLE__
	float* sphereData = util::loadModel("../models/sphere.txt", SPHERE_VERTS);
	#else
	float* sphereData = util::loadModel("models/sphere.txt", SPHERE_VERTS);
	#endif
	cout << "\nNumber of vertices in sphere model : " << SPHERE_VERTS << endl;
	total_verts += SPHERE_VERTS;

	/////////////////////////////////
	//BUILD MODELDATA ARRAY
	/////////////////////////////////
	if (!(cubeData != nullptr && sphereData != nullptr))
	{
		cout << "ERROR. Failed to load model(s)" << endl;
		delete[] cubeData;
		delete[] sphereData;
		return false;
	}
	//each vertex has pos (3f) + norm (3) + texture coords (u,v) = 8 floats
	modelData = new float[total_verts * 8];

	//copy data into modelData array
	copy(cubeData, cubeData + CUBE_VERTS * 8, modelData);
	copy(sphereData, sphereData + SPHERE_VERTS * 8, modelData + (CUBE_VERTS * 8));
	delete[] cubeData;
	delete[] sphereData;
	return true;
}

//
bool World::setupGraphics()
{
	/////////////////////////////////
	//SETUP SHADERS
	/////////////////////////////////
	#ifdef __APPLE__
	shaderProgram = util::LoadShader("../Shaders/phongTex.vert", "../Shaders/phongTex.frag");
	#else
	shaderProgram = util::LoadShader("Shaders/phongTex.vert", "Shaders/phongTex.frag");
	#endif

	//load in textures
	#ifdef __APPLE__
	tex0 = util::LoadTexture("../textures/wood.bmp");
	tex1 = util::LoadTexture("../textures/grey_stones.bmp");
	tex2 = util::LoadTexture("../textures/uniformclouds.bmp");
	#else
	tex0 = util::LoadTexture("textures/wood.bmp");
	tex1 = util::LoadTexture("textures/grey_stones.bmp");
	tex2 = util::LoadTexture("textures/uniformclouds.bmp");
	#endif

	if (tex0 == -1 || tex1 == -1 || tex2 == -1 || shaderProgram == -1)
	{
		cout << "\nERROR. Failed to load texture(s)" << endl;
		return false;
	}

	/////////////////////////////////
	//BUILD VERTEX ARRAY OBJECT
	/////////////////////////////////
	//This stores the VBO and attribute mappings in one object
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	cout << "VAO bound to current context" << endl;

	/////////////////////////////////
	//BUILD VERTEX BUFFER OBJECTS
	/////////////////////////////////
	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	glGenBuffers(1, model_vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, model_vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, total_verts * 8 * sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	cout << "First VBO setup with model data" << endl;

	//Tell OpenGL how to set fragment shader input
	//first 3 floats are position coords
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//Attribute, vals/attrib., type, normalized?, stride, offset
	//Binds to VBO current GL_ARRAY_BUFFER
	glEnableVertexAttribArray(posAttrib);

	//next 2 floats are texture coords (u,v)
	GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	//last 3 floats are normal coords
	GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	//Allocate memory for second vbo
	glGenBuffers(1, line_vbo);  //Create 1 buffer called line_vbo
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]); //Set the buffer as the active array buffer (Only one buffer can be active at a time)
	//vertex data for line_vbo will be set later and constantly updated to draw moving lines
	cout << "Second VBO set up" << endl;

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one

	glEnable(GL_DEPTH_TEST);
	return true;
}

//loops through WObj array and draws each
//also draws floor
void World::draw(Camera * cam)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)

	//vertex shader uniforms
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");

	//build view matrix from Camera
	glm::mat4 view = glm::lookAt(
		util::vec3DtoGLM(cam->getPos()),
		util::vec3DtoGLM(cam->getPos() + cam->getDir()),  //Look at point
		util::vec3DtoGLM(cam->getUp()));

	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(3.14f / 4, 800.0f / 600.0f, 0.1f, 100.0f); //FOV, aspect, near, far
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex0"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 2);

	glBindVertexArray(vao);

	glUniform1i(uniTexID, -1); //Set texture ID to use (0 = wood texture, -1 = no texture)

	//Set vbo for regular objects
	glBindBuffer(GL_ARRAY_BUFFER, model_vbo[0]);
	//Define position attribute
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//Draw
	for (int i = 0; i < num_characters; i++)
	{
		characters[i]->draw(shaderProgram);
		if (show_graph == 1 || show_graph == 2 || show_graph == 3)
		{
			starts[i]->draw(shaderProgram);
			goals[i]->draw(shaderProgram);
		}
		if (show_graph == 3)
		{
			for (int j = 0; j < cur_num_milestones[i]; j++)
			{
				milestones[i][j]->draw(shaderProgram);
			}
		}
	}
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		obstacles[i]->draw(shaderProgram);
	}

	//Set vbo for lines
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]);
	//Define position attribute
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//Draw
	for (int i = 0; i < 4; i++)
	{
		border[i]->draw(shaderProgram);
	}
	if (show_graph == 3)
	{
		int num_edges = edges.size();
		for (int i = 0; i < num_edges; i++)
		{
			edges[i]->draw(shaderProgram);
		}
	}
	if (show_graph == 2)
	{
		for (int i = 0; i < num_characters; i++)
		{
			if (path_exists[i])
			{
				shortest_paths[i]->draw(shaderProgram);
			}
		}
	}
}

void World::update(float dt)
{
	Vec3D repel_vecs[num_characters];
	Vec3D flock_vecs[num_characters];
	for (int c = 0; c < num_characters; c++)
	{
		std::vector<Node*> nodes = shortest_paths[c]->getNodes();
		int index = shortest_paths[c]->getCurIndex();
		int total_nodes = (int)nodes.size();
		Vec3D pos = characters[c]->getPos();
		if (index < total_nodes)
		{
			Vec3D dest = nodes[index]->getPos();
			if (index < total_nodes-1)
			{
				Vec3D next_dest = nodes[index+1]->getPos();
				if (!collisionBetween(characters[c]->getPos(), next_dest, characters[c]))
				{
					dest = next_dest;
					shortest_paths[c]->setCurIndex(index+1);
				}
			}
			if (pos.getX() == dest.getX() && pos.getY() == dest.getY() && pos.getZ() == dest.getZ())
			{
				if (index+1 != total_nodes) shortest_paths[c]->setCurIndex(index+1);
			}
			characters[c]->moveToward(dest, dt);
		}
		repel_vecs[c] = boidRepel(characters[c], dt);
		flock_vecs[c] = boidFlock(characters[c]);
	}
	for (int c = 0; c < num_characters; c++)
	{
		characters[c]->setPos(characters[c]->getPos()+repel_vecs[c]+flock_vecs[c]);
	}
}

Vec3D World::boidRepel(WorldObject * agent, float dt)
{
	//reference: http://www.kfish.org/boids/pseudocode.html
	Vec3D result = Vec3D();
	for (int i = 0; i < num_characters; i++)
	{
		if (characters[i] != agent)
		{
			float d = agent->getSize().getX()/2+characters[i]->getSize().getX()/2;
			if (dist(agent->getPos(), characters[i]->getPos()) < d*1.5)
			{
				Vec3D vec = characters[i]->getPos() - agent->getPos();
				float mag = vec.getMagnitude();
				float mult = 1;
				if (mag < 1.5*d) mult = pow((1.5*d)/mag,2);
				vec.normalize();
				result = result - dt * mult * agent->getSpeed() * vec;
			}
		}
	}
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		float d = agent->getSize().getX()/2+obstacles[i]->getSize().getX()/2;
		if (dist(agent->getPos(), obstacles[i]->getPos()) < d*1.5)
		{
			Vec3D vec = obstacles[i]->getPos() - agent->getPos();
			float mag = vec.getMagnitude();
			float mult = 1;
			if (mag < 1.5*d) mult = pow((1.5*d)/mag,2);
			vec.normalize();
			result = result - dt * mult * agent->getSpeed() * vec;
		}
	}
	return result;
}

Vec3D World::boidFlock(WorldObject * agent)
{
	//reference: http://www.kfish.org/boids/pseudocode.html
	Vec3D result = Vec3D();
	for (int i = 0; i < num_characters; i++)
	{
		if (characters[i] != agent)
		{
			result = result + characters[i]->getPos();
		}
	}
	result = Vec3D(result.getX()/(num_characters-1),result.getY()/(num_characters-1),result.getZ()/(num_characters-1));
	result = Vec3D((result.getX()-agent->getPos().getX())/600,(result.getY()-agent->getPos().getY())/600,(result.getZ()-agent->getPos().getZ())/600);
	return result;
}

void World::initScene(int num)
{
	//setup characters' starts/goals
	switch(num)
	{
		WorldObject * ob;
		case 1:
			printf("\nInitializing scene #1 (20x20)\n");
			starts[0] = new Node(Vec3D(-9,0,-9));
			starts[1] = new Node(Vec3D(9,0,-9));
			goals[0] = new Node(Vec3D(9,0,9));
			goals[1] = new Node(Vec3D(-9,0,9));
			num_characters = 2;
			initCharacters();
			//setup obstacles
			ob = new WorldObject(Vec3D(0,0,0));
			ob->setSize(Vec3D(4,4,4)); //radius of 2
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			break;
		case 2:
			printf("\nInitializing scene #2 (20x20)\n");
			starts[0] = new Node(Vec3D(9,0,-9));
			starts[1] = new Node(Vec3D(9,0,0));
			starts[2] = new Node(Vec3D(9,0,9));
			starts[3] = new Node(Vec3D(0,0,9));
			starts[4] = new Node(Vec3D(-9,0,9));
			starts[5] = new Node(Vec3D(-9,0,0));
			starts[6] = new Node(Vec3D(-9,0,-9));
			starts[7] = new Node(Vec3D(0,0,-9));
			goals[0] = new Node(Vec3D(0,0,9));
			goals[1] = new Node(Vec3D(-9,0,9));
			goals[2] = new Node(Vec3D(-9,0,0));
			goals[3] = new Node(Vec3D(-9,0,-9));
			goals[4] = new Node(Vec3D(0,0,-9));
			goals[5] = new Node(Vec3D(9,0,-9));
			goals[6] = new Node(Vec3D(9,0,0));
			goals[7] = new Node(Vec3D(9,0,9));
			num_characters = 8;
			initCharacters();
			//setup obstacles
			ob = new WorldObject(Vec3D(0,0,0));
			ob->setSize(Vec3D(4,4,4)); //radius of 2
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			break;
		case 3:
			printf("\nInitializing scene #3 (20x20)\n");
			starts[0] = new Node(Vec3D(9,0,-9));
			starts[1] = new Node(Vec3D(9,0,0));
			starts[2] = new Node(Vec3D(9,0,9));
			starts[3] = new Node(Vec3D(0,0,9));
			starts[4] = new Node(Vec3D(-9,0,9));
			starts[5] = new Node(Vec3D(-9,0,0));
			starts[6] = new Node(Vec3D(-9,0,-9));
			starts[7] = new Node(Vec3D(0,0,-9));
			goals[7] = new Node(Vec3D(0,0,9));
			goals[0] = new Node(Vec3D(-9,0,9));
			goals[1] = new Node(Vec3D(-9,0,0));
			goals[3] = new Node(Vec3D(-9,0,-9));
			goals[2] = new Node(Vec3D(0,0,-9));
			goals[5] = new Node(Vec3D(9,0,-9));
			goals[4] = new Node(Vec3D(9,0,0));
			goals[6] = new Node(Vec3D(9,0,9));
			num_characters = 8;
			initCharacters();
			//setup obstacles
			ob = new WorldObject(Vec3D(4,0,4));
			ob->setSize(Vec3D(1.5,1.5,1.5)); //radius of 2
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(-4,0,2));
			ob->setSize(Vec3D(1.5,1.5,1.5)); //radius of 2
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(0,0,-4));
			ob->setSize(Vec3D(1.5,1.5,1.5)); //radius of 2
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			break;
		case 4:
			printf("Initializing scene #4 (10x40)\n");
			starts[0] = new Node(Vec3D(4,0,-19));
			starts[1] = new Node(Vec3D(-4,0,-19));
			starts[2] = new Node(Vec3D(4,0,-15));
			starts[3] = new Node(Vec3D(-4,0,-15));
			starts[4] = new Node(Vec3D(0,0,-17));
			goals[0] = new Node(Vec3D(4,0,19));
			goals[1] = new Node(Vec3D(-4,0,19));
			goals[2] = new Node(Vec3D(4,0,15));
			goals[3] = new Node(Vec3D(-4,0,15));
			goals[4] = new Node(Vec3D(0,0,17));
			num_characters = 5;
			initCharacters();
			break;
		case 5:
			printf("Initializing scene #5 (22x30)\n");
			starts[0] = new Node(Vec3D(10,0,-14));
			starts[1] = new Node(Vec3D(6,0,-14));
			starts[2] = new Node(Vec3D(2,0,-14));
			starts[3] = new Node(Vec3D(-2,0,-14));
			starts[4] = new Node(Vec3D(-6,0,-14));
			starts[5] = new Node(Vec3D(-10,0,-14));
			goals[0] = new Node(Vec3D(10,0,14));
			goals[1] = new Node(Vec3D(6,0,14));
			goals[2] = new Node(Vec3D(2,0,14));
			goals[3] = new Node(Vec3D(-2,0,14));
			goals[4] = new Node(Vec3D(-6,0,14));
			goals[5] = new Node(Vec3D(-10,0,14));
			num_characters = 6;
			initCharacters();
			ob = new WorldObject(Vec3D(9.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(6.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(3.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(-3.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(-6.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			ob = new WorldObject(Vec3D(-9.5,0,0));
			ob->setSize(Vec3D(3,3,3)); 
			ob->setColor(Vec3D(.2,.2,.2));
			ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			obstacles[cur_num_obstacles] = ob;
			cur_num_obstacles++;
			max_num_milestones = 150;
			for (int i = 0; i < num_characters; i++)
			{
				milestones[i] = new Node*[max_num_milestones];
				milestones[i][0] = new Node(Vec3D(0,0,0));
				milestones[i][1] = new Node(Vec3D(0,0,1));
				milestones[i][2] = new Node(Vec3D(0,0,-1));
				cur_num_milestones[i] = 3;
			}
			max_neighbors = 12;
			break;
	}
}

void World::initCharacters() //called after scene is initialized
{
	srand(time(NULL));
	for (int i = 0; i < num_characters; i++)
	{
		starts[i]->setVertexInfo(SPHERE_START, SPHERE_VERTS);
		starts[i]->setColor(getRandomColor(.5,1));
		starts[i]->setSize(Vec3D(.5,.5,.5));
		goals[i]->setVertexInfo(SPHERE_START, SPHERE_VERTS);
		goals[i]->setColor(starts[i]->getColor());
		goals[i]->setSize(Vec3D(.5,.5,.5));

		//setup characters
		characters[i] = new WorldObject(starts[i]->getPos());
		characters[i]->setVertexInfo(SPHERE_START, SPHERE_VERTS);
		characters[i]->setColor(starts[i]->getColor());
		characters[i]->setSize(Vec3D(2,2,2));
		characters[i]->setSpeed(4);

		//set up set of milestones for each character
		if (milestones[i] == NULL)
		{
			cur_num_milestones[i] = 0;
			milestones[i] = new Node*[max_num_milestones];
		}
		path_exists[i] = false;
	}
}

Vec3D World::getRandomColor(float min, float max)
{
	float rand_r = ((float) rand()) / (float) RAND_MAX;
	rand_r = rand_r * (max-min);
	rand_r = rand_r + min;
	float rand_g = ((float) rand()) / (float) RAND_MAX;
	rand_g = rand_g * (max-min);
	rand_g = rand_g + min;
	float rand_b = ((float) rand()) / (float) RAND_MAX;
	rand_b = rand_b * (max-min);
	rand_b = rand_b + min;
	return Vec3D(rand_r, rand_g, rand_b);
}

void World::generateMilestones()
{
	printf("Generating milestones\n");
	int dim = floor(sqrt(max_num_milestones));
	float x_step = (max_x - min_x) / dim;
	float z_step = (max_z - min_z) / dim;
	for (int c = 0; c < num_characters; c++)
	{
		srand(time(NULL)+c);
		//evenly space milestones in the scene
		for (int i = 0; i < dim; i++)
		{
			for (int j = 0; j < dim; j++)
			{
				float random_x = ((float) rand()) / (float) RAND_MAX;
				random_x = random_x * x_step;
				random_x = random_x + min_x + i * x_step;
				float random_z = ((float) rand()) / (float) RAND_MAX;
				random_z = random_z * z_step;
				random_z = random_z + min_z + j * z_step;

				Vec3D pos = Vec3D(random_x,y,random_z);

				if (!collision(pos, characters[c]))
				{
					Node * mi = new Node(pos);
					mi->setSize(Vec3D(.1,.1,.1));
					mi->setColor(Vec3D(.1,.1,.1));
					mi->setVertexInfo(SPHERE_START, SPHERE_VERTS);
					milestones[c][cur_num_milestones[c]] = mi;
					cur_num_milestones[c]++;
				}
			}
		}
		//then fill in extra milestones randomly until cur_num = max
		while (cur_num_milestones[c] < max_num_milestones)
		{
			float random_x = ((float) rand()) / (float) RAND_MAX; //random b/w 0 and 1
			random_x = random_x * (max_x - min_x);
			random_x = random_x + min_x; //random b/w min_x and max_x
			float random_z = ((float) rand()) / (float) RAND_MAX; //random b/w 0 and 1
			random_z = random_z * (max_z - min_z);
			random_z = random_z + min_z; //random b/w min_z and max_z

			Vec3D pos = Vec3D(random_x,y,random_z);

			if (!collision(pos, characters[c]))
			{
				Node * mi = new Node(pos);
				mi->setSize(Vec3D(.1,.1,.1));
				mi->setColor(Vec3D(.1,.1,.1));
				mi->setVertexInfo(SPHERE_START, SPHERE_VERTS);
				milestones[c][cur_num_milestones[c]] = mi;
				cur_num_milestones[c]++;
			}
		}
	}
}

struct cmp_dist
{
	cmp_dist(Vec3D pos) : p(pos) {}

	bool operator()(Node * lhs, Node * rhs)
	{
		return dist(p, lhs->getPos()) < dist(p, rhs->getPos());
	}

private:
	Vec3D p;
};

void World::initMilestoneNeighbors()
{
	printf("Initializing nearest neighbors\n");
	for (int c = 0; c < num_characters; c++)
	{
		//set up vector of all milestones + start + goal
		std::vector<Node*> potential_neighbors;
		potential_neighbors.push_back(starts[c]);
		potential_neighbors.push_back(goals[c]);
		for (int i = 0; i < cur_num_milestones[c]; i++)
		{
			potential_neighbors.push_back(milestones[c][i]);
		}
		int num = potential_neighbors.size();
		//sort potential neighbors for start
		std::sort(potential_neighbors.begin(), potential_neighbors.end(), cmp_dist(starts[c]->getPos()));
		for (int i = 0; i < max_neighbors; i++)
		{
			Node * neighbor = potential_neighbors[i];
			if (starts[c] != neighbor && !collisionBetween(starts[c]->getPos(), neighbor->getPos(), characters[c]))
			{
				starts[c]->addNeighbor(neighbor);
				neighbor->addNeighbor(starts[c]);
			}
		}
		//sort potential neighbors for goal
		std::sort(potential_neighbors.begin(), potential_neighbors.end(), cmp_dist(goals[c]->getPos()));
		for (int i = 0; i < max_neighbors; i++)
		{
			Node * neighbor = potential_neighbors[i];
			if (goals[c] != neighbor && !collisionBetween(goals[c]->getPos(), neighbor->getPos(), characters[c]))
			{
				goals[c]->addNeighbor(neighbor);
				neighbor->addNeighbor(goals[c]);
			}
		}
		//for each milestone,
		for (int i = 0; i < cur_num_milestones[c]; i++)
		{
			//sort potential neighbors for milestone
			Node * mile = milestones[c][i];
			std::sort(potential_neighbors.begin(), potential_neighbors.end(), cmp_dist(mile->getPos()));
			for (int j = 0; j < max_neighbors; j++)
			{
				Node * neighbor = potential_neighbors[j];
				if (mile != neighbor && !collisionBetween(mile->getPos(), neighbor->getPos(), characters[c]))
				{
					mile->addNeighbor(neighbor);
					neighbor->addNeighbor(mile);
				}
			}
		}
	}
	//create edges graph
	for (int c = 0; c < num_characters; c++)
	{
		for (int i = 0; i < cur_num_milestones[c]; i++)
		{
			std::vector<Node*> neighbors = milestones[c][i]->getNeighbors();
			int num = milestones[c][i]->getNumNeighbors();
			for (int j = 0; j < num; j++)
			{
				edges.push_back(new Line(milestones[c][i]->getPos(), neighbors[j]->getPos()));
			}
		}
	}
}

struct cmp_path
{
    bool operator()(Path * a, Path * b)
    {
        return a->getLen()+a->getHeuristic() > b->getLen()+b->getHeuristic();
    }
};

bool World::findShortestPaths()
{
	printf("Finding shortest path for each character\n");
	for (int c = 0; c < num_characters; c++)
	{
		float timer = SDL_GetTicks();

		bool done = false;
		
		std::priority_queue<Path*, vector<Path*>, cmp_path> q;
		q.push(new Path(starts[c]));

		while (!q.empty() && !done)
		{
			Path * path = q.top();
			q.pop();
			//path->print();
			Node * last = path->getLastNode();
			if (last == goals[c])
			{
				path_exists[c] = true;
				shortest_paths[c] = path;
				printf("Path found for character %i!\n", c+1);
				printf("The shortest path visits %i nodes.\n", (int)shortest_paths[c]->getNodes().size());
				done = true;
				printf("Path found in %f seconds\n", (SDL_GetTicks()-timer)/1000);
			}
			else
			{
				std::vector<Node*> neighbors = last->getNeighbors();
				int num = neighbors.size();
				for (int i = 0; i < num; i++)
				{	
					Node * neighbor = neighbors[i];
					if (!path->visited(neighbor))
					{
						float d = dist(last->getPos(), neighbor->getPos());
						Path * new_path = new Path();
						new_path->copy(path); 
						new_path->addNode(neighbor);
						float prev_len = path->getLen();
						new_path->setLen(prev_len+d);
						new_path->setHeuristic(dist(neighbor->getPos(), goals[c]->getPos()));
						q.push(new_path);
					}
				}
			}
			if (!done) path->~Path();
		}

		if (!done)
		{
			printf("No path found for character %i.\n", c+1);
			return false;
		}
		else if (done && c == num_characters-1)
		{
			return true;
		}
	}
	return false;
}

bool World::collision(Vec3D pos, WorldObject * ch)
{
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		if (obstacles[i]->collision(pos, ch->getSize().getX()/2)) //assumes character size x, y, and z all same
		{
			return true;
		}
	}
	return false;
}

bool World::collisionBetween(Vec3D pos1, Vec3D pos2, WorldObject * ch)
{
	//ray/sphere intersection: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	Vec3D D = pos1 - pos2;
	D.normalize();
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		WorldObject * cur_obj = obstacles[i];
		//assume obstacle is a sphere
		Vec3D L = Vec3D(cur_obj->getPos().getX()-pos1.getX(),cur_obj->getPos().getY()-pos1.getY(),cur_obj->getPos().getZ()-pos1.getZ());
		float t_ca = dotProduct(L, D);
		Vec3D test_p = pos1.castRay(D, t_ca);
		if (cur_obj->collision(test_p, ch->getSize().getX()/2)) //assumes character size x, y, and z all same
		{
			return true;
		}
	}
	return false;
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/