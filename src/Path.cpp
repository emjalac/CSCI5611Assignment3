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
	cur_index = 0;
}

Path::Path(Node * n)
{
	nodes.push_back(n);
	length = 0;
	cur_index = 0;
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

void Path::setCurIndex(int i)
{
	cur_index = i;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
std::vector<Node*> Path::getNodes()
{
	return nodes;
}

int Path::getNumNodes()
{
	return nodes.size();
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

int Path::getCurIndex()
{
	return cur_index;
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

void Path::deleteNode(int index)
{
	auto i = std::find(begin(nodes), end(nodes), nodes[index]);
	nodes.erase(i);
}

void Path::deleteFirstNode()
{
	std::vector<Node*>::const_iterator first = nodes.begin() + 1;
	std::vector<Node*>::const_iterator last = nodes.end();
	nodes = std::vector<Node*>(first, last);
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

void Path::copy(Path * p) //copies p onto this
{
	nodes = p->getNodes();
	length = p->getLen();
	cur_index = p->getCurIndex();
}

void Path::print()
{
	int size = nodes.size();
	printf("Path has %i nodes and path length is %f.\n", size, length);
	printf("Path contains nodes in the following positions:\n");
	for (int i = 0; i < size; i++)
	{
		nodes[i]->print();
	}
}

void Path::draw(GLuint shaderProgram)
{
	int num = nodes.size();
	for (int i = 0; i < num-1; i++)
	{
		Node * n1 = nodes[i];
		Node * n2 = nodes[i+1];

		Vec3D pos1 = n1->getPos();
		Vec3D pos2 = n2->getPos();

		float vertices[6] = {
			pos1.getX(), pos1.getY(), pos1.getZ(),
			pos2.getX(), pos2.getY(), pos2.getZ()
		};

		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);

		GLint uniModel = glGetUniformLocation(shaderProgram, "model");

		glm::mat4 model;
		glm::vec3 size_v = util::vec3DtoGLM(Vec3D(1,1,1));

		//build model mat specific to this spring
		model = glm::scale(model, size_v);
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_LINES, 0, 2);
	}
}


