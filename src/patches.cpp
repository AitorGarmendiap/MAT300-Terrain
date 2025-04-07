#include "patches.hpp"

namespace mat300_terrain
{
	Patch::Patch()
	{
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

	int Patch::GetStepCount() const
	{
		float diff = (1.f / t) - static_cast<int>(1.f / t);
		if (diff > 0.5f)
			return static_cast<int>(ceilf(1.f / t));
		else if (diff < 0.5f)
			return static_cast<int>(floorf(1.f / t));
	}
}