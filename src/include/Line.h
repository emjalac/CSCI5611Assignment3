#ifndef LINE_INCLUDED
#define LINE_INCLUDED

#include "Vec3D.h"
#include "Util.h"

class Line
{
private:
  Vec3D p1;
  Vec3D p2;

public:
  Line();
  Line(Vec3D p1_, Vec3D p2_);
  ~Line();

  //SETTERS
  void setP1(Vec3D p);
  void setP2(Vec3D p);

  //GETTERS
  Vec3D getP1();
  Vec3D getP2();

  //OTHERS
  void draw(GLuint shaderProgram);

};

#endif
