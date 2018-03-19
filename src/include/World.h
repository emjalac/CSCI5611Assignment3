#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#define GLM_FORCE_RADIANS
#ifdef __APPLE__
#include "../../ext/glm/glm.hpp"
#include "../../ext/glm/gtc/matrix_transform.hpp"
#include "../../ext/glm/gtc/type_ptr.hpp"
#else
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#endif

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "Vec3D.h"
#include "Camera.h"
#include "Util.h"

//objects
#include "WorldObject.h"

class World{
private:
	int total_verts;
	float* modelData;

	//description of world plane
	float min_x;
	float max_x;
	float min_z;
	float max_z;
	float y;

	//modelData indices
	int CUBE_START = 0;
	int CUBE_VERTS = 0;
	int SPHERE_START = 0;
	int SPHERE_VERTS = 0;

	//VAO and VBO GLuints
	GLuint vao;
	GLuint vbo[1];

	//Shader and Texture GLuints
	GLuint shaderProgram;
	GLuint tex0;
	GLuint tex1;
	GLuint tex2;

	//what lives in this world
	WorldObject * character;
	WorldObject ** obstacles = NULL;
	int cur_num_obstacles;

public:
	//CONSTRUCTORS AND DESTRUCTORS
	World();
	World(int max_objects);
	~World();

	//SETTERS
	void setCurNumObstacles(int num);

	//GETTERS
	int getCurNumObstacles();

	//OTHERS
	bool loadModelData();
	bool setupGraphics();
	void draw(Camera * cam);
	void initObjects();

};

#endif
