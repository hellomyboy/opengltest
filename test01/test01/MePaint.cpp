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
#include "Primitives\ShapeGenerator.h"
#include "Camera.h"

using namespace std;
using glm::mat4;
using glm::vec3;

extern MePaint paint;
GLuint vertexShaderID;
GLuint fragmentShaderID;
GLuint programID;
ShapeGenerator shapeGen;

GLuint numOfIndices;

GLuint window_width = 800;
GLuint window_height = 600;

Camera camera;


void MePaint::sendDataToShader()
{
	ShapeData shape = shapeGen.makeTeapot();
	//ShapeData shape = shapeGen.makeArrow();
	GLuint verticesID;
	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);	//position
	glEnableVertexAttribArray(1);	//color
	glEnableVertexAttribArray(2);	//normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)(6 * sizeof(GL_FLOAT)));

	GLuint indicesID;
	glGenBuffers(1, &indicesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
	numOfIndices = shape.numIndices;

	shape.cleanup();
}

void doPaint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewToProjectionMatrix = glm::perspective<float>(58.0f, ((float)window_width) / window_height, 0.1f, 20.0f);
	//glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(0.0270278826f, 5, -9), glm::vec3(-0.194516003f, 2.64676142f, 3.29750848f), glm::vec3(0.0, 1.0f, 0.0));
	glm::mat4 worldToProjectionMatrix = viewToProjectionMatrix * camera.getWorldToViewMatrix();

	//glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	////glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), 10.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	//glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	//glm::mat4 modelToWorldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
	
	//glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;
	GLuint worldToProjectionMatrixUnifromLocation = glGetUniformLocation(programID, "worldToProjectionMatrix");
	glUniformMatrix4fv(worldToProjectionMatrixUnifromLocation, 1, GL_FALSE, &worldToProjectionMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, numOfIndices, GL_UNSIGNED_SHORT, 0);
	glutSwapBuffers();
}

void keyboardPressFunc(unsigned char key, int x, int y) {
	cout << "button:"<< key << "," << x << "," << y << endl;
	switch (key) {
	case 'w':
		camera.moveForward();
		break;
	case 's':
		camera.moveBack();
		break;
	case 'a':
		camera.moveLeft();
		break;
	case 'd':
		camera.moveRight();
		break;
	case 'r':
		camera.moveUp();
		break;
	case 'f':
		camera.moveDown();
		break;
	}
	doPaint();
}

void mouseChangeFunc(int x, int y) {
	cout << "mouse:"<< x << "," << y << endl;
	camera.mouseUpdate(glm::vec2(x, y));
	doPaint();
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
	const string vertexShaderCode = loadShaderAsString("VertexShader");
	const string fragmentShaderCode = loadShaderAsString("FragmentShader");
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
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("my panint");

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "glew init error: %s\n", glewGetErrorString(err));
		return ;
	}

	glEnable(GL_DEPTH_TEST);
	createShaderAndProgram();
	sendDataToShader();
	glutDisplayFunc(doPaint);
	glutKeyboardFunc(keyboardPressFunc);
	glutPassiveMotionFunc(mouseChangeFunc);

	glutMainLoop();
}

MePaint::~MePaint() {
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}
