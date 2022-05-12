#pragma once

#include "Libs.h"
#include "ResourceManager.h"
#include "Camera.h"

class SkyboxRenderer
{
private:
	// variables
	std::string shaderName;
	unsigned int textureID;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	// static variables
	// private functions 
	void loadCubemap();
public:
	// constructors and destructors
	SkyboxRenderer();
	// static functions
	// public functions
	void render(const Camera& camera) const;
};

