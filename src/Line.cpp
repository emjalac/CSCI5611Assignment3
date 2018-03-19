#ifdef __APPLE__
#include "include/Line.h"
#else
#include "Line.h"
#endif

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Line::Line()
{
	p1 = Vec3D();
	p2 = Vec3D();
}

Line::Line(Vec3D p1_, Vec3D p2_)
{
	p1 = p1_;
	p2 = p2_;
}

Line::~Line()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Line::setP1(Vec3D p)
{
	p1 = p;
}

void Line::setP2(Vec3D p)
{
	p2 = p;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Line::getP1()
{
	return p1;
}

Vec3D Line::getP2(){
	return p2;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Line::draw(GLuint shaderProgram)
{
	Vec3D pos1 = p1;
	Vec3D pos2 = p2;

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