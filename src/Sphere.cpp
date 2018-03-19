#ifdef __APPLE__
#include "include/Sphere.h"
#else
#include "Sphere.h"
#endif

using namespace std;


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Sphere::Sphere() : WorldObject() //call parent default
{
  radius = 0.5f;
}

Sphere::Sphere(Vec3D init_pos) : WorldObject(init_pos) //call parent constructor
{
  radius = 0.5f;
}

Sphere::Sphere(Vec3D init_pos, float r) : WorldObject(init_pos) //call parent constructor
{
  radius = r;
  size = Vec3D(2*r, 2*r, 2*r);
}

Sphere::~Sphere()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Sphere::setRadius(float r)
{
	radius = r;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
float Sphere::getRadius()
{
	return radius;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/


/*----------------------------*/
// VIRTUALS
/*----------------------------*/
int Sphere::getType()
{
	return SPHERE_WOBJ;
}

bool Sphere::collision(Vec3D p)
{
	Vec3D vec = p - pos;
	float dist = vec.getMagnitude();
	if (dist < radius + .001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Vec3D Sphere::getCollisionPos(Vec3D p)
{
	Vec3D vec = p - pos;
	vec.normalize();
	Vec3D xtra = .001 * vec; //make sure collision pos outside of sphere
	Vec3D collision_pos = pos + radius * vec + xtra;
	return collision_pos;
}
