#pragma once

#include "Libs.h"
#include "PhysicsEngine.h"
#include "SkyboxRenderer.h"
#include "ModelRenderer.h"
#include "SpriteRenderer.h"
#include "ShadowRenderer.h"
#include "DebugRenderer.h"
#include "InstanceRenderer.h"
#include "uiRenderer.h"

class Game;

class RenderMaster
{
private:
	// variables
	SkyboxRenderer* skyboxRenderer;
	ModelRenderer* modelRenderer;
	SpriteRenderer* spriteRenderer;
	ShadowRenderer* shadowRenderer;
	DebugRenderer* debugRenderer;
	InstanceRenderer* instanceRenderer;
	uiRenderer* UiRenderer;

	std::string shaderName;

	GLuint quadVAO;
	GLuint quadVBO;

	GLuint fbo;
	GLuint rbo;
	GLuint colorbuffer;
	GLuint depthbuffer;
	

	Game* game;
	// static variables
	// private funcitons
	void initialize();
	void initShader();
	void initFramebuffer();
	void initResources();
	void initInstances();
	void renderFramebuffer() const;
public:
	// constructors and destructors
	RenderMaster(Game* game);
	~RenderMaster();
	// accessors
	// mutators
	// public functions
	void render(const Camera& camera) const;
};

