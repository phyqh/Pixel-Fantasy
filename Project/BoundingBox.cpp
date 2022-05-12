#include "BoundingBox.h"

const float EPSILON = 1e-8;

BoundingBox::BoundingBox(): min(0, 0, 0), max(0, 0, 0)
{
}

bool BoundingBox::includes(const glm::vec3& point) const
{
	return ((point[0] + EPSILON >= min[0]) && (point[2] + EPSILON >= min[2]) &&
		(point[0] - EPSILON <= max[0]) && (point[2] - EPSILON <= max[2]));
}

// Does this bounding box intersect the target?
bool BoundingBox::intersects(const BoundingBox& target) const
{
	return ((target.min[0] - EPSILON <= max[0]) && (target.max[0] + EPSILON >= min[0]) &&
		(target.min[1] - EPSILON <= max[1]) && (target.max[1] + EPSILON >= min[1]) &&
		(target.min[2] - EPSILON <= max[2]) && (target.max[2] + EPSILON >= min[2]));
}

// does the box contain this point?
bool BoundingBox::intersects(const glm::vec3& point) const
{
	return ((point[0] + EPSILON >= min[0]) && (point[1] + EPSILON >= min[1]) && (point[2] + EPSILON >= min[2]) &&
		(point[0] - EPSILON <= max[0]) && (point[1] - EPSILON <= max[1]) && (point[2] - EPSILON <= max[2]));
}