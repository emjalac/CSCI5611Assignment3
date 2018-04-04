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
#include <vector>
#include <queue>
#include <ctime>
#include <cmath>

#include <math.h>

#include "Vec3D.h"
#include "Camera.h"
#include "Util.h"

//objects
#include "WorldObject.h"
#include "Node.h"
#include "Path.h"
#include "Line.h"

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
	Line ** border = NULL;

	//modelData indices
	int CUBE_START = 0;
	int CUBE_VERTS = 0;
	int SPHERE_START = 0;
	int SPHERE_VERTS = 0;

	//VAO and VBO GLuints
	GLuint vao;
	GLuint model_vbo[1];
	GLuint line_vbo[1];

	//Shader and Texture GLuints
	GLuint shaderProgram;
	GLuint tex0;
	GLuint tex1;
	GLuint tex2;

	//what lives in this world
	WorldObject ** characters = NULL;
	Node ** starts = NULL;
	Node ** goals = NULL;
	int num_characters;
	Node *** milestones = NULL;
	WorldObject ** obstacles = NULL;
	int max_num_milestones;
	int * cur_num_milestones;
	int cur_num_obstacles;
	std::vector<Line*> edges;

	bool * path_exists;
	Path ** shortest_paths = NULL;

	int show_graph;

public:
	//CONSTRUCTORS AND DESTRUCTORS
	World();
	World(int length, int width);
	~World();

	//SETTERS
	void setCurNumObstacles(int num);
	void setShowGraph(int s);

	//GETTERS
	int getCurNumObstacles();
	int showGraph();

	//OTHERS
	bool loadModelData();
	bool setupGraphics();
	void draw(Camera * cam);
	void initScene(int num);
	void initCharacters();
	Vec3D getRandomColor(float min, float max);
	void update(float dt);
	void generateMilestones();
	void initMilestoneNeighbors();
	bool findShortestPaths();
	Vec3D boidRepel(WorldObject * agent, float dt);
	Vec3D boidFlock(WorldObject * agent);
	bool collision(Vec3D pos, WorldObject * ch);
	bool collisionBetween(Vec3D pos1, Vec3D pos2, WorldObject * ch);

};

#endif
