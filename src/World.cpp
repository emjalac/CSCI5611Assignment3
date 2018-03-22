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
	obstacles = new WorldObject*[10]; //max of 10 obstacles
	cur_num_obstacles = 0;
	max_num_milestones = 25; //max of 25 milestones
	milestones = new Node*[max_num_milestones];
	cur_num_milestones = 0;
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

	path_exists = false;
}

World::World(int max_objects)
{
	obstacles = new WorldObject*[max_objects];
	cur_num_obstacles = 0;
	max_num_milestones = 25; //max of 25 milestones
	milestones = new Node*[max_num_milestones];
	cur_num_milestones = 0;
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

	path_exists = false;
}

World::~World()
{
	delete start;
	delete goal;
	delete character;
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		delete obstacles[i];
	}
	delete obstacles;
	for (int i = 0; i < cur_num_milestones; i++)
	{
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


/*----------------------------*/
// GETTERS
/*----------------------------*/
int World::getCurNumObstacles()
{
	return cur_num_obstacles;
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
		printf(strerror(errno));
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
	start->draw(shaderProgram);
	goal->draw(shaderProgram);
	character->draw(shaderProgram);
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		obstacles[i]->draw(shaderProgram);
	}
	for (int i = 0; i < cur_num_milestones; i++)
	{
		milestones[i]->draw(shaderProgram);
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
	if (path_exists)
	{
		shortest_path.draw(shaderProgram);
	}
}

void World::update(float dt)
{

}

void World::initObjects()
{
	//setup start/goal
	Node * st = new Node(Vec3D(-9,0,-9));
	st->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	st->setColor(Vec3D(1,0,0));
	st->setSize(Vec3D(.5,.5,.5));
	Node * go = new Node(Vec3D(9,0,9));
	go->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	go->setColor(Vec3D(1,1,0));
	go->setSize(Vec3D(.5,.5,.5));
	start = st;
	goal = go;

	//setup character
	WorldObject * ch = new WorldObject(start->getPos());
	ch->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	ch->setColor(Vec3D(1,0,1));
	ch->setSize(Vec3D(2,2,2)); //radius of 1
	character = ch;

	//setup obstacles
	WorldObject * ob = new WorldObject(Vec3D(0,0,0));
	ob->setSize(Vec3D(4,4,4)); //radius of 2
	ob->setColor(Vec3D(0,0,1));
	ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	obstacles[cur_num_obstacles] = ob;
	cur_num_obstacles++;
}

void World::generateMilestones()
{
	srand(time(NULL));
	while (cur_num_milestones < max_num_milestones)
	{
		float random_x = ((float) rand()) / (float) RAND_MAX; //random b/w 0 and 1
		random_x = random_x * (max_x - min_x);
		random_x = random_x + min_x; //random b/w min_x and max_x
		float random_z = ((float) rand()) / (float) RAND_MAX; //random b/w 0 and 1
		random_z = random_z * (max_z - min_z);
		random_z = random_z + min_z; //random b/w min_z and max_z

		Vec3D pos = Vec3D(random_x,y,random_z);

		if (!collision(pos))
		{
			Node * mi = new Node(pos);
			mi->setSize(Vec3D(.1,.1,.1));
			mi->setColor(Vec3D(.1,.1,.1));
			mi->setVertexInfo(SPHERE_START, SPHERE_VERTS);
			milestones[cur_num_milestones] = mi;
			cur_num_milestones++;
		}
	}
}

void World::initMilestoneNeighbors()
{
	for (int i = 0; i < cur_num_milestones; i++)
	{
		Node * mile1 = milestones[i];
		if (!collisionBetween(mile1->getPos(), start->getPos()))
		{
			mile1->addNeighbor(start);
			start->addNeighbor(mile1);
		}
		if (!collisionBetween(mile1->getPos(), goal->getPos()))
		{
			mile1->addNeighbor(goal);
			goal->addNeighbor(mile1);
		}
		for (int j = i+1; j < cur_num_milestones; j++)
		{
			Node * mile2 = milestones[j];
			if (!collisionBetween(mile1->getPos(), mile2->getPos()))
			{
				mile1->addNeighbor(mile2);
				mile2->addNeighbor(mile1);
			}
		}
	}
}

void World::findShortestPath()
{
	std::priority_queue<Path, vector<Path>, greater<Path>> q;
	q.push(Path(start));

	while (!q.empty())
	{
		//printf("hello\n");
		Path path = q.top();
		q.pop();
		Node * last = path.getLastNode();
		if (last == goal)
		{
			path_exists = true;
			shortest_path = path;
			printf("\nPath found!\n");
			printf("The shortest path visits %i nodes.\n\n", shortest_path.getNodes().size());
			return;
		}
		std::vector<Node*> neighbors = last->getNeighbors();
		int num = neighbors.size();
		for (int i = 0; i < num; i++)
		{	
			//printf("neighbor #%i\n", i);
			Node * neighbor = neighbors[i];
			if (!path.visited(neighbor))
			{
				float d = dist(last->getPos(), neighbor->getPos());
				Path new_path = path;
				new_path.addNode(neighbor);
				float prev_len = path.getLen();
				new_path.setLen(prev_len+d);
				q.push(new_path);
			}
		}
	}
}

void World::colorPath()
{
	//for debugging
	if (path_exists)
	{
		std::vector<Node*> nodes = shortest_path.getNodes();
		int num = nodes.size();
		for (int i = 0; i < num; i++)
		{
			Node * n = nodes[i];
			n->setSize(Vec3D(.25,.25,.25));
			n->setColor(Vec3D(1,1,0));
		}
	}
}

bool World::collision(Vec3D pos)
{
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		if (obstacles[i]->collision(pos, character->getSize().getX()/2)) //assumes character size x, y, and z all same
		{
			return true;
		}
	}
	return false;
}

bool World::collisionBetween(Vec3D pos1, Vec3D pos2)
{
	//find from raytracing assignment
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
		if (cur_obj->collision(test_p, character->getSize().getX()/2)) //assumes character size x, y, and z all same
		{
			return true;
		}
	}
	return false;
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
