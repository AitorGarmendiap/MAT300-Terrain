#include "patches.hpp"

namespace mat300_terrain
{
	void Patch::UpdateNormal(glm::vec3 newNormal)
	{
		glm::vec3 tangent = (fabs(normal.x) > 0.9f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
		right = glm::normalize(glm::cross(normal, tangent)) * size.x;

		up = glm::normalize(glm::cross(normal, GetRight())) * size.y;
	}

	glm::vec3 Patch::GetRight()
	{
		return right;
	}

	glm::vec3 Patch::GetUp()
	{
		return up;
	}
}