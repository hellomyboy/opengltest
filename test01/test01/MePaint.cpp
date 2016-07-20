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
#include "UniformBlock.h"

using namespace std;
using glm::mat4;
using glm::vec3;

extern MePaint paint;
GLuint vertexShaderID;
GLuint fragmentShaderID;
GLuint programID;
ShapeGenerator shapeGen;

GLuint numIndicesOfPlance;
GLuint numIndicesOfTeaport;

GLuint window_width = 800;
GLuint window_height = 600;

Camera camera;

GLuint teaportVao;
GLuint planeVao;

#define ARRAY_ELEMENT_NUM(a) (sizeof(a)/sizeof(*a))

void MePaint::sendDataToShader()
{
	//plane
	ShapeData plance = shapeGen.makePlane(30);
	GLuint planeVerticesID;
	glGenBuffers(1, &planeVerticesID);
	glBindBuffer(GL_ARRAY_BUFFER, planeVerticesID);
	glBufferData(GL_ARRAY_BUFFER, plance.vertexBufferSize(), plance.vertices, GL_STATIC_DRAW);

	GLuint planceIndicesID;
	glGenBuffers(1, &planceIndicesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planceIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plance.indexBufferSize(), plance.indices, GL_STATIC_DRAW);
	numIndicesOfPlance = plance.numIndices;

	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);
	glBindBuffer(GL_ARRAY_BUFFER, planeVerticesID);
	glEnableVertexAttribArray(0);	//position
	glEnableVertexAttribArray(1);	//color
	glEnableVertexAttribArray(2);	//normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)(6 * sizeof(GL_FLOAT)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planceIndicesID);

	//teaport
	ShapeData teaport = shapeGen.makeTeapot();
	GLuint teaportVertexID;
	glGenBuffers(1, &teaportVertexID);
	glBindBuffer(GL_ARRAY_BUFFER, teaportVertexID);
	glBufferData(GL_ARRAY_BUFFER, teaport.vertexBufferSize(), teaport.vertices, GL_STATIC_DRAW);

	GLuint teaportIndicesID;
	glGenBuffers(1, &teaportIndicesID);
	glBindBuffer(GL_ARRAY_BUFFER, teaportIndicesID);
	glBufferData(GL_ARRAY_BUFFER, teaport.indexBufferSize(), teaport.indices, GL_STATIC_DRAW);
	numIndicesOfTeaport = teaport.numIndices;

	glGenVertexArrays(1, &teaportVao);
	glBindVertexArray(teaportVao);
	glBindBuffer(GL_ARRAY_BUFFER, teaportVertexID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(6 * sizeof(GL_FLOAT)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, teaportIndicesID);

	teaport.cleanup();
	plance.cleanup();
}

void doPaint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewToProjectionMatrix = glm::perspective<float>(58.0f, ((float)window_width) / window_height, 0.1f, 50.0f);
	glm::mat4 worldToProjectionMatrix = viewToProjectionMatrix * camera.getWorldToViewMatrix();
	GLuint modelToProjectionMatrixUnifromLocation = glGetUniformLocation(programID, "modelToProjectionMatrix");
	GLuint modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");
	glUniform3fv(glGetUniformLocation(programID, "eyePosition"), 1, &camera.getPosition()[0]);

	//light's reflectivity and shininess
	vec3 Ka = vec3(0.2, 0.2, 0.2), Kd = vec3(0.5, 0.5, 0.5), Ks = vec3(0.4, 0.4, 0.4);
	float shininess = 50.0f;
	glUniform3fv(glGetUniformLocation(programID, "Ka"), 1, &Ka[0]);
	glUniform3fv(glGetUniformLocation(programID, "Kd"), 1, &Kd[0]);
	glUniform3fv(glGetUniformLocation(programID, "Ks"), 1, &Ks[0]);
	glUniform1f(glGetUniformLocation(programID, "Shininess"), shininess);

	//light info
	vec3 lightPosition(0.0, 10.0, 0.0);
	vec3 lightIntensity = vec3(1.0, 1.0, 1.0);
	glUniform3fv(glGetUniformLocation(programID, "Light.position"), 1, &lightPosition[0]);
	glUniform3fv(glGetUniformLocation(programID, "Light.intensity"), 1, &lightIntensity[0]);

	//fog info
	GLfloat maxDist = 20.0f, minDist = 0.1f;
	vec3 fogColor = vec3(0.01, 0.01, 0.01);
	glUniform1f(glGetUniformLocation(programID, "Fog.minDist"), minDist);
	glUniform1f(glGetUniformLocation(programID, "Fog.maxDist"), maxDist);
	glUniform3fv(glGetUniformLocation(programID, "Fog.color"), 1, &fogColor[0]);

	mat4 modelToProjectionMatrix;
	mat4 modelToWorldMatrix;
	
	//draw plane
	glBindVertexArray(planeVao);
	modelToWorldMatrix = glm::mat4(1.0f);
	modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
	glUniformMatrix4fv(modelToProjectionMatrixUnifromLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, numIndicesOfPlance, GL_UNSIGNED_SHORT, 0);

	//draw teaport
	for (int i = 0; i < 4; i++) {
		glBindVertexArray(teaportVao);
		mat4 teaportTranslateMatrix = glm::translate(glm::mat4(1.0f), vec3(-8.0f + i * 3.5f, 2.0f, 3.0f - i * 3.0f));
		mat4 teaportRotateMatrix = glm::rotate(glm::mat4(1.0f), -glm::pi<float>() / 2, vec3(1.0f, 0.0f, 0.0f));
		modelToWorldMatrix = teaportTranslateMatrix * teaportRotateMatrix;
		modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
		glUniformMatrix4fv(modelToProjectionMatrixUnifromLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, numIndicesOfTeaport, GL_UNSIGNED_SHORT, 0);
	}

	glutSwapBuffers();
}

void keyboardPressFunc(unsigned char key, int x, int y) {
	//cout << "button:"<< key << "," << x << "," << y << endl;
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
	camera.showInfo();
	doPaint();
}

void mouseChangeFunc(int x, int y) {
	//cout << "mouse:"<< x << "," << y << endl;
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
