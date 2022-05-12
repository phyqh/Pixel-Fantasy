#pragma once

#include "Libs.h"
#include "ResourceManager.h"

class Sprite
{
private:
	// variable
	glm::vec3 position;
	glm::vec2 size;
	glm::vec3 rotation;
	std::string textureName;
	GLuint instanceVBO;
	int renderState;
	double currentDirection; // current direction indicate direction of normal vector from character surface
	double ke;
	// static variable
	// private function
public:
	// constructors and destructors
	Sprite(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName);
	// accessors
	glm::vec3 getPosition() const { return this->position; };
	glm::vec2 getSize() const { return this->size; }
	glm::vec3 getRotation() const { return this->rotation; }
	std::string getTextureName() const { return this->textureName; }
	double getDirection() const;
	double getke() const { return ke; }
	int getRenderState() const { return renderState; }
	// mutators
	void setPosition(glm::vec3 position) { this->position = position; }
	void setDirection(float direction);
	void setSize(glm::vec2 size) { this->size = size; }
	void setRotation(GLfloat rotation) { this->rotation = glm::vec3(0.0f, rotation, 0.0f); }
	void setRotation(glm::vec3 rotation) { this->rotation = rotation; }
	void setTextureName(std::string textureName) { this->textureName = textureName; }
	void setRenderState(int state) { renderState = state; }
	void setke(double k) { ke = k; }
	// public functions
	// static functions
};

