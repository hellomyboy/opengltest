#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	glm::vec3 position;
	glm::vec3 viewDirection;
	glm::vec3 UP;
	glm::vec2 oldMousePosition;
	glm::vec3 strafeDirection;

public:
	void mouseUpdate(const glm::vec2 &);
	glm::vec3 getPosition();
	glm::mat4 getWorldToViewMatrix();

	void moveForward();
	void moveBack();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	Camera();
	~Camera();
};
