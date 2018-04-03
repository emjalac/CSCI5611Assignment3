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
	max_num_milestones = 50; //max of 50 milestones
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
}

World::World(int max_objects)
{
	characters = new WorldObject*[10]; //max of 10 characters
	starts = new Node*[10];
	goals = new Node*[10];
	obstacles = new WorldObject*[max_objects];
	cur_num_obstacles = 0;
	max_num_milestones = 50; //max of 50 milestones
	milestones = new Node**[10];
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
	for (int i = 0; i < num_characters; i++)
	{
		characters[i]->draw(shaderProgram);
		starts[i]->draw(shaderProgram);
		goals[i]->draw(shaderProgram);
		for (int j = 0; j < cur_num_milestones[i]; j++)
		{
			milestones[i][j]->draw(shaderProgram);
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
	for (int i = 0; i < num_characters; i++)
	{
		if (path_exists[i])
		{
			shortest_paths[i]->draw(shaderProgram);
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
				shortest_paths[c]->setCurIndex(index+1);
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
			if (dist(agent->getPos(), characters[i]->getPos()) < (agent->getSize().getX()/2+characters[i]->getSize().getX()/2))
			{
				result = result - dt * agent->getSpeed() * (characters[i]->getPos() - agent->getPos());
			}
		}
	}
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		if (dist(agent->getPos(), obstacles[i]->getPos()) < (agent->getSize().getX()/2+obstacles[i]->getSize().getX()/2))
		{
			result = result - dt * agent->getSpeed() * (obstacles[i]->getPos() - agent->getPos());
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
	result = 1/(num_characters-1) * result;
	return 1/100 * (result - agent->getPos());
}

void World::moveAgentAlongPath(WorldObject * agent, Path * path, float dt)
{
	if (!agent->getPathComplete())
	{
		float dist_to_travel = dt * agent->getSpeed();
		while (dist_to_travel > 0)
		{
			int index = path->getCurIndex();
			Vec3D dest = path->getNodes()[index]->getPos();
			Vec3D dir = dest - agent->getPos();
			float dist_to_dest = dir.getMagnitude();
			dir.normalize();
			if (dist_to_dest > dist_to_travel)
			{
				agent->setPos(agent->getPos() + dist_to_travel * dir);
				dist_to_travel = 0;
			}
			else
			{
				agent->setPos(dest);
				dist_to_travel -= dist_to_dest;
				if (index < path->getNumNodes()-1)
				{
					path->setCurIndex(index+1);
				}
				else
				{
					agent->setPathComplete(true);
					return;
				}
			}
		}
	}
}

Vec3D World::percentageAlongPath(Path * path, float percent) //percent is between 0 and 1
//any percent greater than 1 should still just return the goal pos
{
	float length = path->getLen();
	float dist_to_travel = length * percent;
	Vec3D val = path->getFirstNode()->getPos();
	int index = 0;
	while (dist_to_travel > 0)
	{
		int index = path->getCurIndex();
		Vec3D dest = path->getNodes()[index]->getPos();
		Vec3D dir = dest - val;
		float dist_to_dest = dir.getMagnitude();
		dir.normalize();
		if (dist_to_dest > dist_to_travel)
		{
			return val + dist_to_travel * dir;
		}
		else
		{
			val = dest;
			dist_to_travel -= dist_to_dest;
			if (index < path->getNumNodes()-1)
			{
				index++;
			}
			else
			{
				return val;
			}
		}
	}
	return val;
}

void World::initObjects()
{
	printf("\nInitializing objects\n");
	//setup characters' starts/goals
	Node * st1 = new Node(Vec3D(-9,0,-9));
	st1->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	st1->setColor(Vec3D(1,0,0));
	st1->setSize(Vec3D(.5,.5,.5));
	Node * st2 = new Node(Vec3D(9,0,-9));
	st2->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	st2->setColor(Vec3D(1,0,0));
	st2->setSize(Vec3D(.5,.5,.5));
	Node * go1 = new Node(Vec3D(9,0,9));
	go1->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	go1->setColor(Vec3D(1,1,0));
	go1->setSize(Vec3D(.5,.5,.5));
	Node * go2 = new Node(Vec3D(-9,0,9));
	go2->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	go2->setColor(Vec3D(1,1,0));
	go2->setSize(Vec3D(.5,.5,.5));
	starts[0] = st1;
	starts[1] = st2;
	goals[0] = go1;
	goals[1] = go2;

	//setup characters
	WorldObject * ch1 = new WorldObject(starts[0]->getPos());
	ch1->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	ch1->setColor(Vec3D(1,0,1));
	ch1->setSize(Vec3D(2,2,2)); //radius of 1
	ch1->setSpeed(3);
	WorldObject * ch2 = new WorldObject(starts[1]->getPos());
	ch2->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	ch2->setColor(Vec3D(1,0,1));
	ch2->setSize(Vec3D(2,2,2)); //radius of 1
	ch2->setSpeed(3);
	characters[0] = ch1;
	characters[1] = ch2;

	num_characters = 2;
	//set up set of milestones for each character
	for (int i = 0; i < num_characters; i++)
	{
		cur_num_milestones[i] = 0;
		milestones[i] = new Node*[max_num_milestones];

		path_exists[i] = false;
	}

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
	printf("Generating milestones\n");
	int dim = floor(sqrt(max_num_milestones));
	float x_step = (max_x - min_x) / dim;
	float z_step = (max_z - min_z) / dim;
	for (int c = 0; c < num_characters; c++)
	{
		srand(time(NULL));
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
		int max_neighbors = 10; //set max num of nearest neighbors for each node here 
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
}

struct cmp_path
{
    bool operator()(Path * a, Path * b)
    {
        return a->getLen() > b->getLen();
    }
};

bool World::findShortestPaths()
{
	printf("Finding shortest path for each character\n");
	for (int c = 0; c < num_characters; c++)
	{
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
				printf("The shortest path visits %i nodes.\n", shortest_paths[c]->getNodes().size());
				done = true;
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
						q.push(new_path);
					}
				}
			}
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

// void World::colorPath()
// {
// 	//for debugging
// 	if (path_exists)
// 	{
// 		std::vector<Node*> nodes = shortest_path->getNodes();
// 		int num = nodes.size();
// 		for (int i = 0; i < num; i++)
// 		{
// 			Node * n = nodes[i];
// 			n->setSize(Vec3D(.25,.25,.25));
// 			n->setColor(Vec3D(1,1,0));
// 		}
// 	}
// }

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
