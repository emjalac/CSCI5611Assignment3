#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <vector>

#include "WorldObject.h"

class Node : public WorldObject
{
private:
  std::vector<Node*> neighbors;

public:
  Node();
  Node(Vec3D init_pos);
  ~Node();

  //SETTERS

  //GETTERS
  std::vector<Node*> getNeighbors();
  int getNumNeighbors();

  //OTHERS
  void addNeighbor(Node * m);
  void print();

  //VIRTUALS
  int getType();

};

#endif
