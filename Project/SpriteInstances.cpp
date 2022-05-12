#include "SpriteInstances.h"

SpriteInstances::SpriteInstances(std::string tx, GLuint vbo, int n)
{
	textureName = tx;
	instanceVBO = vbo;
	num = n;
}

SpriteInstances::~SpriteInstances()
{
	if (offsets) delete[] offsets;
	offsets = nullptr;
}
