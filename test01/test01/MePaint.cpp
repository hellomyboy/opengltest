#include "MePaint.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using glm::mat4;
using glm::vec3;

extern MePaint paint;
GLuint vertexShaderID;
GLuint fragmentShaderID;
GLuint programID;

GLuint vaoID;

void sendDataToShader1()
{
	GLfloat vertexPosition[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f
	};

	GLfloat vertexColor[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	GLuint vbos[2];
	glGenBuffers(2, vbos);
	GLuint vertexPositionID, vertexColorID;
	vertexPositionID = vbos[0];
	vertexColorID = vbos[1];
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexPosition, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexColorID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexColor, GL_STATIC_DRAW);

	//set vbo to vao
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexColorID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void MePaint::sendDataToShader()
{
	GLfloat vertexPosition[] = {
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	GLfloat texCoord[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f
	};

	GLuint vertexPositionID, texCoordID;
	glGenBuffers(1, &vertexPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &texCoordID);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	GLuint uniformIndex = glGetUniformBlockIndex(programID, "BlobSettings");
	GLint blockSize;
	glGetActiveUniformBlockiv(programID, uniformIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);
	const char * names[] = {"InnerColor", "OuterColor", "RadiusInner", "RadiusOuter"};
	GLuint indices[4];
	glGetUniformIndices(programID, 4, names, indices);
	GLint offset[4];
	glGetActiveUniformsiv(programID, 4, indices, GL_UNIFORM_OFFSET, offset);

	GLfloat innerColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat outerColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat innerRadius = 0.15f, outerRadius = 0.45f;
	memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
	memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));
	
	GLuint vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_UNIFORM_BUFFER, vboID);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, uniformIndex, vboID);

	free(blockBuffer);
}

void doPaint() {
	glClear(GL_COLOR_BUFFER_BIT);

	//mat4 rotationMatrix = glm::rotate(mat4(1.0f), 10.0f * glm::pi<float>() /180.0f, vec3(0.0f, 0.0f, 1.0f));
	//GLuint rotationUniformLocation = glGetUniformLocation(programID, "RotationMatrix");
	//if (rotationUniformLocation >= 0) {
	//	glUniformMatrix4fv(rotationUniformLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
	//}

	//glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();


	/*GLint nUniforms, maxLength;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &nUniforms);
	glGetProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	cout << "number of uniforms:" << nUniforms << ", maximum length of uniform:" << maxLength << endl;;

	GLchar * name = (GLchar *)malloc(maxLength);
	GLint size, written, location;
	GLenum type;
	cout << "location | name" << endl;
	cout << "-------------------------------" << endl;
	for (int i = 0; i < nUniforms; i++) {
		glGetActiveUniform(programID, i, maxLength, &written, &size, &type, name);
		location = glGetUniformLocation(programID, name);
		printf(" %-5d | %s, size: %d, type: %d\n", location, name, size, type);
	}
	free(name);*/
}

const string MePaint::loadShaderAsString(const string fileName) {
	ifstream t(fileName);
	std::stringstream buff;

	buff << t.rdbuf();
	return buff.str();
}

void MePaint::createShaderAndProgram()
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (0 == vertexShaderID) {
		fprintf(stderr, "error create vertex shader.\n");
	}
	else if (0 == fragmentShaderID) {
		fprintf(stderr, "error create fragment shader.\n");
	}

	const GLchar* codeArr[1];
	const string vertexShaderCode = loadShaderAsString("vert");
	const string fragmentShaderCode = loadShaderAsString("fragment");
	codeArr[0] = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, codeArr, NULL);
	codeArr[0] = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, codeArr, NULL);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	GLint result;
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		int logLen;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* buff = (char*)malloc(logLen);
			int writtenLen;
			glGetShaderInfoLog(vertexShaderID, logLen, &writtenLen, buff);
			fprintf(stderr, "compile vertext shader error msg: %s\n", buff);
			free(buff);
		}
		return;
	}
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		int logLen;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* buff = (char*)malloc(logLen);
			int writtenLen;
			glGetShaderInfoLog(fragmentShaderID, logLen, &writtenLen, buff);
			fprintf(stderr, "compile fragment shader error msg: %s\n", buff);
			free(buff);
		}
		return;
	}

	programID = glCreateProgram();
	if (programID == 0) {
		fprintf(stderr, "create program error\n");
	}
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);

	GLint status;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		fprintf(stderr, "link program has error\n");
		GLint logLen;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* buff = (char*)malloc(logLen);
			GLint written;
			glGetProgramInfoLog(programID, logLen, &written, buff);
			fprintf(stderr, "link program error msg: %s\n", buff);
			free(buff);
		}
		return;
	} else {
		glUseProgram(programID);
	}
		
	//cout << "compile shader ok" << endl;
}

void MePaint::MainPaint(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GL_RGBA | GL_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("my panint");

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "glew init error: %s\n", glewGetErrorString(err));
		return ;
	}

	createShaderAndProgram();
	sendDataToShader();
	glutDisplayFunc(doPaint);

	glutMainLoop();
}

MePaint::~MePaint() {
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}
