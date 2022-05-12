#pragma once

#include "Camera.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "stb_image.h"

class SpriteRenderer
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
	SpriteRenderer();
	~SpriteRenderer();
	// accessors
	// mutators
	// public functions
	void render(const Camera& camera) const;
	// static functions
};

