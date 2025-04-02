#include "patches.hpp"

namespace mat300_terrain
{
	Patch::Patch()
	{
		controlPoints.reserve(16);
	}

	void Patch::ComputeNormal()
	{
		// bottom right - bottom left (B - A)
		glm::vec3 v1 = controlPoints[3][0] - controlPoints[0][0];
		// top right - bottom left (D - A)
		glm::vec3 v2 = controlPoints[3][3] - controlPoints[0][0];
		normal = glm::normalize(glm::cross(v1, v2));
	}
	glm::vec3 Patch::GetCenterPoint()
	{ 
		return (controlPoints[0][0] + controlPoints[3][0] + controlPoints[0][3] + controlPoints[3][3]) / 4.0f;
	}
}