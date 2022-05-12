#pragma once

#include "Libs.h"
#include "ResourceManager.h"

class SpriteInstances {
private:
	std::string textureName;
	GLuint instanceVBO;
	glm::vec3* offsets;
	int num;
public:
	SpriteInstances(std::string tx, GLuint vbo, int n);
	~SpriteInstances();

	std::string getTextureName() const { return this->textureName; }
	const GLuint& getInstanceVBO() const { return this->instanceVBO; }
	int getNum() const { return this->num; }
};