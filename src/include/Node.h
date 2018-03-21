#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <vector>

#include "WorldObject.h"

class Node : public WorldObject
{
private:
  std::vector<Node*> neighbors;
  bool visited;

public:
  Node();
  Node(Vec3D init_pos);
  ~Node();

  //SETTERS

  //GETTERS
  std::vector<Node*> getNeighbors();
  bool getVisited();

  //OTHERS
  void addNeighbor(Node * m);
  void visit();

  //VIRTUALS
  int getType();

};

#endif
