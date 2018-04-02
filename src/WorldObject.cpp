#ifdef __APPLE__
#include "include/WorldObject.h"
#else
#include "WorldObject.h"
#endif

using namespace std;

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
WorldObject::WorldObject()
{
	pos = Vec3D();
	vel = Vec3D();
	acc = Vec3D();
	size = Vec3D(1, 1, 1);
	start_vertex_index = 0;
	total_vertices = 0;
	mat = Material();
	speed = 0;
	path_complete = false;
	just_collided = false;
	wait_count = 0;
}

WorldObject::WorldObject(Vec3D init_pos)
{
	pos = init_pos;
	vel = Vec3D();
	acc = Vec3D();
	size = Vec3D(1, 1, 1);
	start_vertex_index = 0;
	total_vertices = 0;
	mat = Material();
	speed = 0;
	path_complete = false;
	just_collided = false;
	wait_count = 0;
}

WorldObject::~WorldObject()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void WorldObject::setPos(Vec3D p)
{
  pos = p;
}

void WorldObject::setVel(Vec3D v)
{
  vel = v;
}

void WorldObject::setAcc(Vec3D a)
{
  acc = a;
}

void WorldObject::setSpeed(float s)
{
	speed = s;
}

void WorldObject::setPathComplete(bool c)
{
	path_complete = c;
}

void WorldObject::setJustCollided(bool j)
{
	just_collided = j;
}

void WorldObject::setWaitCount(int i)
{
	wait_count = i;
}

void WorldObject::setVertexInfo(int start, int total)
{
  start_vertex_index = start;
  total_vertices = total;
}

void WorldObject::setMaterial(Material m)
{
	mat = m;
}

void WorldObject::setSize(Vec3D s)
{
	size = s;
}

void WorldObject::setColor(Vec3D color)
{
	glm::vec3 c = util::vec3DtoGLM(color);
	mat.setAmbient(c);
	mat.setDiffuse(c);
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D WorldObject::getPos()
{
  return pos;
}

Vec3D WorldObject::getVel()
{
  return vel;
}

Vec3D WorldObject::getAcc()
{
  return acc;
}

float WorldObject::getSpeed()
{
	return speed;
}

bool WorldObject::getPathComplete()
{
	return path_complete;
}

bool WorldObject::getJustCollided()
{
	return just_collided;
}

int WorldObject::getWaitCount()
{
	return wait_count;
}

Material WorldObject::getMaterial()
{
	return mat;
}

Vec3D WorldObject::getSize()
{
	return size;
}

/*----------------------------*/
// VIRTUALS
/*----------------------------*/
int WorldObject::getType()
{
	return DEFAULT_WOBJ;
}

bool WorldObject::collision(Vec3D p, float extent) //NEED TO FIX THIS DEPENDING ON OBJECT TYPE LATER
													//CURRENTLY ASSUMING OBJECTS ARE SPHERES
{
	Vec3D vec = p - pos;
	if (vec.getMagnitude() < size.getX()/2 + extent + .01) //assumes size x, y, and z all same
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void WorldObject::moveToward(Vec3D dest, float dt)
{
	Vec3D dir = dest - pos;
	if (dir.getMagnitude() < .05)
	{
		setPos(dest);
		return;
	}
	dir.normalize();
	Vec3D new_pos = pos + dt * 5 * dir;
	pos = new_pos;
}

// void WorldObject::moveAlongPath(Path * path, float dt)
// {
// 	if (!path_complete)
// 	{
// 		Vec3D dest;
// 		float dist_to_travel = dt * speed;
// 		while (dist_to_travel > 0)
// 		{
// 			int index = path->getCurIndex();
// 			dest = &path[index];
// 			Vec3D dir = dest - pos;
// 			float dist_to_dest = dir.getMagnitude();
// 			dir.normalize();
// 			if (dist_to_dest > dist_to_travel)
// 			{
// 				pos = pos + dist_to_travel * dir;
// 				dist_to_travel = 0;
// 			}
// 			else
// 			{
// 				pos = dest.getPos();
// 				dist_to_travel -= dist_to_dest;
// 				if (index < path->getNumNodes())
// 				{
// 					path->setCurIndex(index+1);
// 				}
// 				else
// 				{
// 					path_complete = true;
// 				}
// 			}
// 		}
// 	}
// }

//assumes that the models have already been loaded into the VBO before this call
void WorldObject::draw(GLuint shaderProgram)
{
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");

	glm::mat4 model;
	glm::vec3 size_v = util::vec3DtoGLM(size);
	glm::vec3 pos_v = util::vec3DtoGLM(pos);

	//build model mat specific to this WObj
	model = glm::translate(model, pos_v);
	model = glm::scale(model, size_v);
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	//fragment shader uniforms (from Material)
	GLint uniform_ka = glGetUniformLocation(shaderProgram, "ka");
	GLint uniform_kd = glGetUniformLocation(shaderProgram, "kd");
	GLint uniform_ks = glGetUniformLocation(shaderProgram, "ks");
	GLint uniform_s = glGetUniformLocation(shaderProgram, "s");

	glm::vec3 mat_AMB = mat.getAmbient();
	glUniform3f(uniform_ka, mat_AMB[0], mat_AMB[1], mat_AMB[2]);

	glm::vec3 mat_DIF = mat.getDiffuse();
	glUniform3f(uniform_kd, mat_DIF[0], mat_DIF[1], mat_DIF[2]);

	glm::vec3 mat_SPEC = mat.getSpecular();
	glUniform3f(uniform_ks, mat_SPEC[0], mat_SPEC[1], mat_SPEC[2]);

	glUniform1f(uniform_s, mat.getNS());

	//starts at an offset of start_vertex_index
	//(Primitive Type, Start Vertex, End Vertex)
	glDrawArrays(GL_TRIANGLES, start_vertex_index, total_vertices);
}
