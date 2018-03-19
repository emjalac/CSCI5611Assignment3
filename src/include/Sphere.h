#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include "WorldObject.h"

class Sphere : public WorldObject
{
private:
  float radius;

public:
  Sphere();
  Sphere(Vec3D init_pos);
  Sphere(Vec3D init_pos, float r);
  ~Sphere();

  //SETTERS
  void setRadius(float r);

  //GETTERS
  float getRadius();

  //OTHERS

  //VIRTUALS
  int getType();
  bool collision(Vec3D p);
  Vec3D getCollisionPos(Vec3D p);

};

#endif
