#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const float MOVE_SPEED = 0.1f;
const float ROTATION_SPEED = 0.008f;

using glm::mat4;
using glm::vec4;
using glm::vec3;
using namespace std;


Camera::Camera():viewDirection(0.0f, -1.0f, -1.0f),
	position(0.0f, 4.0f, 5.0f),
	UP(0.0f, 1.0f, 0.0f)
{}

//鼠标事件处理
void Camera::mouseUpdate(const glm::vec2 & newMousePosition) {
	glm::vec2 delta = newMousePosition - oldMousePosition;
	if (glm::length(delta) > 50.0f) {
		//cout << "move to large:" << glm::length(delta) << endl;
		oldMousePosition = newMousePosition;
		return;
	}
	strafeDirection = glm::cross(viewDirection, UP);
	
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -delta.x * ROTATION_SPEED , UP);
	rotationMatrix = glm::rotate(rotationMatrix, -delta.y * ROTATION_SPEED, strafeDirection);
	viewDirection = glm::mat3(rotationMatrix) * viewDirection;
	oldMousePosition = newMousePosition;
}

glm::mat4 Camera::getWorldToViewMatrix() {
	return glm::lookAt(position, position + viewDirection, UP);
}

vec3 Camera::getPosition() {
	return position;
}

void Camera::moveForward() {
	position += MOVE_SPEED * viewDirection;
}

void Camera::moveBack() {
	position += -MOVE_SPEED * viewDirection;
}

void Camera::moveLeft() {
	position += -MOVE_SPEED * strafeDirection;
}

void Camera::moveRight() {
	position += MOVE_SPEED * strafeDirection;
}

void Camera::moveUp() {
	position += MOVE_SPEED * UP;
}

void Camera::moveDown() {
	position += -MOVE_SPEED * UP;
}

Camera::~Camera() {}