#pragma once

#include "patches.hpp"

#include <glm/glm.hpp>
#include <vector>
namespace mat300_terrain {

	float Bernstein(int i, float t);
	void CalculateBezierMesh(Patch& patch);
	std::vector<glm::vec3> CalculateBezierCurve(const std::vector<glm::vec3>& ctrlPoints, float dt);
}