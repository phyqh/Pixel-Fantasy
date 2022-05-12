#pragma once

#include "Libs.h"
#include "ResourceManager.h"
#include "Camera.h"

class ShadowRenderer
{
	// variables
	std::string modelShadowShaderName;
	std::string spriteShadowShaderName;
	GLuint shadowMapFBO;
	GLuint shadowMap;
	GLuint quadVAO;
	GLuint quadVBO;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 lightPos;
	// static variables
	// private functions 
public:
	// constructors and destructors
	ShadowRenderer();
	// accessors
	const GLuint& getShadowMap() { return this->shadowMap; }
	const glm::mat4& getLightSpaceMatrix() { return this->lightSpaceMatrix; }
	const glm::vec3& getLightPos() { return this->lightPos; }
	// static functions
	// public functions
	void update();
	void render();
	void mapModelShadow() const;
	void mapSpriteShadow() const;
};
