#ifndef PATH_INCLUDED
#define PATH_INCLUDED

#include <vector>

#include "Vec3D.h"
#include "Node.h"

class Path
{
private:
  std::vector<Node*> nodes;
  float length;

public:
  Path();
  Path(Node * n);
  ~Path();

  //SETTERS
  void setLen(float l);

  //GETTERS
  std::vector<Node*> getNodes();
  Node * getFirstNode();
  Node * getLastNode();
  float getLen();

  //OPERATORS
  friend bool operator< (Path& lhs, Path& rhs);
  friend bool operator> (Path& lhs, Path& rhs);

  //OTHERS
  void addNode(Node * n);

};

#endif
