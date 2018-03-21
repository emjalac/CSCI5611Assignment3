#ifdef __APPLE__
#include "include/Node.h"
#else
#include "Node.h"
#endif

using namespace std;

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Node::Node() : WorldObject() //call parent default
{
	visited = false;
}

Node::Node(Vec3D init_pos) : WorldObject(init_pos) //call parent constructor
{
	visited = false;
}

Node::~Node()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/


/*----------------------------*/
// GETTERS
/*----------------------------*/
std::vector<Node*> Node::getNeighbors()
{
	return neighbors;
}

bool Node::getVisited()
{
	return visited;
}


/*----------------------------*/
// OTHERS
/*----------------------------*/
void Node::addNeighbor(Node * m)
{
	neighbors.push_back(m);
}

void Node::visit()
{
	visited = true;
}


/*----------------------------*/
// VIRTUALS
/*----------------------------*/
int Node::getType()
{
	return NODE_WOBJ;
}

