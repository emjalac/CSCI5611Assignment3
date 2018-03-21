#ifdef __APPLE__
#include "include/Path.h"
#else
#include "Path.h"
#endif

using namespace std;


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Path::Path()
{
	length = 0;
}

Path::Path(Node * n)
{
	nodes.push_back(n);
	length = 0;
}

Path::~Path()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Path::setLen(float l)
{
	length = l;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
std::vector<Node*> Path::getNodes()
{
	return nodes;
}

Node * Path::getFirstNode()
{
	return nodes.front();
}

Node * Path::getLastNode()
{
	return nodes.back();
}

float Path::getLen() const
{
	return length;
}

/*----------------------------*/
// OPERATORS
/*----------------------------*/
bool operator< (const Path& lhs, const Path& rhs)
{
	return lhs.getLen() < rhs.getLen();
}

bool operator> (const Path& lhs, const Path& rhs)
{
	return lhs.getLen() > rhs.getLen();
}


/*----------------------------*/
// OTHERS
/*----------------------------*/
void Path::addNode(Node * n)
{
	//add new node
	Node * last_node = nodes.back();
	nodes.push_back(n);
	//update path length
	float d = dist(last_node->getPos(), n->getPos());
	length += d;
}

bool Path::visited(Node * n)
{
	int num = nodes.size();
	for (int i = 0; i < num; i++)
	{
		if (n == nodes[i])
		{
			return true;
		}
	}
	return false;
}


