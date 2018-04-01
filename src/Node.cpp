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
}

Node::Node(Vec3D init_pos) : WorldObject(init_pos) //call parent constructor
{
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


/*----------------------------*/
// OTHERS
/*----------------------------*/
void Node::addNeighbor(Node * m)
{
	neighbors.push_back(m);
}

void Node::print()
{
	pos.print();
}


/*----------------------------*/
// VIRTUALS
/*----------------------------*/
int Node::getType()
{
	return NODE_WOBJ;
}

