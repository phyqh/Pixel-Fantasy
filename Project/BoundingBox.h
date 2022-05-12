#pragma once
#include "Libs.h"


class BoundingBox
{
public:
	glm::vec3 min;
	glm::vec3 max;

	glm::vec3 centroid() const { return (0.5f * min) + (0.5f * max); }

	float size() const {
		glm::vec3 diff = max - min;
		return diff.x * diff.y * diff.z;
	}

	BoundingBox();

	// Does this bounding box includes the given point in xy plane?
	bool includes(const glm::vec3& point) const;

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox& target) const;

	// does the box contain this point?
	bool intersects(const glm::vec3& point) const;
};