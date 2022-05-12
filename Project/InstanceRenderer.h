#pragma once

#include "Camera.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "stb_image.h"

class InstanceRenderer
{
private:
	// variables
	std::string shaderName;
	unsigned int quadVAO;
	unsigned int quadVBO;

	// static variables
	// private functions
public:
	// constructors and destructors
	InstanceRenderer();
	~InstanceRenderer();
	// accessors
	// mutators
	// public functions
	void render(const Camera& camera);
	// static functions
};
