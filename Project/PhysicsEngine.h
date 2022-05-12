#pragma once

#include "Libs.h"
#include "ResourceManager.h"

class Model;

inline bool rayTriangleIntersect(const glm::vec3& O, const glm::vec3& D, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float& t);

class PhysicsEngine {
	// variables
	// static variables
	// private functions 
public:
	// constructors and destructors
	// static functions
	static double computeGroundHeight(glm::vec3 position, unsigned int& lastFaceIndex);
	static bool checkCollision(glm::vec3 position);
	// public functions
};