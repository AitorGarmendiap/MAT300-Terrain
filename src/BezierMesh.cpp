#include "bezierMesh.hpp"

namespace mat300_terrain {

	glm::vec3 GetMeshPointAt(float u, float v, std::vector<std::vector<glm::vec3>>& controlPoints);

	float Bernstein(int i, float t);

	const int binomialCoeffs[4] = { 1,3,3,1 }; //precomputed binomial coeffs for bernstein will always be the same


	void CalculateBezierMesh(Patch& patch)
	{
		std::vector<glm::vec3> mesh;

		float deltaT = 0.1;
		float steps = 1.0f / deltaT;

		for (unsigned i = 0; i < steps + 1; i++)
		{
			float tu = i * deltaT;

			for (unsigned j = 0; j < steps + 1; j++)
			{
				float tv = j * deltaT;

				glm::vec3 pointInMesh = GetMeshPointAt(tu, tv, patch.controlPoints);

				mesh.push_back(pointInMesh);

			}
		}
		patch.mesh = mesh;
	}

	glm::vec3 GetMeshPointAt(float u, float v, std::vector<std::vector<glm::vec3>>& controlPoints)
	{
		glm::vec3 newPoint(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float Bu = Bernstein(i, u);
				float Bv = Bernstein(j, v);

				newPoint += controlPoints[i][j] * Bv * Bu;
			}
		}
		return newPoint;
	}

	float Bernstein(int i, float t)
	{
		int n = 3;

		//calculate bernstein with precomputed binomial coeffs
		return binomialCoeffs[i] * pow(1 - t, n - i) * pow(t, i);
	}
}