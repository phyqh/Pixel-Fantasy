#pragma once

#include "Camera.h"
#include "ResourceManager.h"
#include "ShadowRenderer.h"

class DebugRenderer
{
private:
	// variables
	std::string shaderName;
	unsigned int quadVAO;
	unsigned int quadVBO;
	ShadowRenderer* shadowRenderer;
	// static variables
	// private functions
public:
	// constructors and destructors
	DebugRenderer(ShadowRenderer* shadowRenderer);
	// accessors
	// mutators
	// public functions
	void render() const;
	void render(const GLuint& texture) const;
	// static functions
};