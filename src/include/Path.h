#ifndef PATH_INCLUDED
#define PATH_INCLUDED

#include <vector>

#include "Vec3D.h"
#include "Node.h"
#include "Util.h"

class Path
{
private:
  std::vector<Node*> nodes;
  float length;
  float heuristic;
  int cur_index;

public:
  Path();
  Path(Node * n);
  ~Path();

  //SETTERS
  void setLen(float l);
  void setHeuristic(float h);
  void setCurIndex(int i);

  //GETTERS
  std::vector<Node*> getNodes();
  int getNumNodes();
  Node * getFirstNode();
  Node * getLastNode();
  float getLen() const;
  float getHeuristic();
  int getCurIndex();

  //OPERATORS
  friend bool operator< (const Path& lhs, const Path& rhs);
  friend bool operator> (const Path& lhs, const Path& rhs);

  //OTHERS
  void addNode(Node * n);
  void deleteNode(int index);
  void deleteFirstNode();
  bool visited(Node * n);
  void copy(Path * p);
  void print();
  void draw(GLuint shaderProgram);

};

#endif
