#pragma once
#include <GL/glew.h>

class UniformBlock {
private:
	GLuint programID;
	GLint blockIndex;
public:
	UniformBlock(const GLuint programID, const GLchar * blockName);
	void sendData(const GLchar * ele[], const GLfloat *val[], const GLuint eleSize[], const GLint size);
};
