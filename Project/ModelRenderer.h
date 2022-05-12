#pragma once

#include "Libs.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "ShadowRenderer.h"

class ModelRenderer
{
	// variables
	std::string shaderName;
	ShadowRenderer* shadowRenderer;
	// static variables
	// private functions 
public:
	// constructors and destructors
	ModelRenderer(ShadowRenderer* shadowRenderer);
	// static functions
	// public functions
	void render(const Camera& camera) const;
	void render(const Camera& camera, const Model* model) const;
};

