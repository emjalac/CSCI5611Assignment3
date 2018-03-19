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
	total_verts = 0;
	
	//default 20x20 world
	min_x = -10.0f;
	max_x = 10.0f;
	min_z = -10.0f;
	max_z = 10.0f;
	y = 0;
}

World::World(int max_objects)
{
	obstacles = new WorldObject*[max_objects];
	cur_num_obstacles = 0;
	total_verts = 0;

	//default 20x20 world
	min_x = -10.0f;
	max_x = 10.0f;
	min_z = -10.0f;
	max_z = 10.0f;
	y = 0;
}

World::~World()
{
	delete character;
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		delete obstacles[i];
	}
	delete obstacles;
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
	//BUILD VERTEX ARRAY OBJECT
	/////////////////////////////////
	//This stores the VBO and attribute mappings in one object
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	cout << "VAO bound to current context" << endl;

	/////////////////////////////////
	//BUILD VERTEX BUFFER OBJECT
	/////////////////////////////////
	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, total_verts * 8 * sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	cout << "VBO setup with model data" << endl;

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

	character->draw(shaderProgram);
	for (int i = 0; i < cur_num_obstacles; i++)
	{
		obstacles[i]->draw(shaderProgram);
	}

}

void World::initObjects()
{
	//setup character
	WorldObject * ch = new WorldObject(Vec3D(-9,0,-9));
	ch->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	ch->setColor(Vec3D(1,0,0));
	character = ch;

	//setup obstacles
	WorldObject * ob = new WorldObject(Vec3D(0,0,0));
	ob->setSize(Vec3D(2,2,2));
	ob->setColor(Vec3D(0,0,1));
	ob->setVertexInfo(SPHERE_START, SPHERE_VERTS);
	obstacles[cur_num_obstacles] = ob;
	cur_num_obstacles++;
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
