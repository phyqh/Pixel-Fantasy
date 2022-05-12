#pragma once

#include "ResourceManager.h"

class uiRenderer
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
	uiRenderer();
	~uiRenderer();
	// accessors
	// mutators
	// public functions
	void render(int lifeCount, int StaminaCount, int BossLifeCount, int gameState, int bossDieRenderState, int ninjaState, int ninjalifeCount) const;
	// static functions
};
