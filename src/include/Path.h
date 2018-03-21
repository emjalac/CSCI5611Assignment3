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
  float getLen() const;

  //OPERATORS
  friend bool operator< (const Path& lhs, const Path& rhs);
  friend bool operator> (const Path& lhs, const Path& rhs);

  //OTHERS
  void addNode(Node * n);
  bool visited(Node * n);

};

#endif
