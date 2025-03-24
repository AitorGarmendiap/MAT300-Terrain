#include "bezierMesh.hpp"

std::vector<glm::vec3> BezierMesh::CalculateBezierMesh()
{
	std::vector<std::vector<glm::vec3>> controlPoints = { {{0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}},
														  {{1,0,0}, {1,1,1}, {1,2,1}, {1,3,0}},
														  {{2,0,0}, {2,1,2}, {2,2,2}, {2,3,0}},
														  {{3,0,0}, {3,1,0}, {3,2,0}, {3,3,0}} };

	std::vector<glm::vec3> mesh;

	float deltaT = 0.1;
	float steps = 1.0f / deltaT;

	for (unsigned i = 0; i < steps+1; i++)
	{
		float tu = i * deltaT;

		for (unsigned j = 0; j < steps+1; j++)
		{
			float tv = j * deltaT;

			glm::vec3 pointInMesh = GetMeshPointAt(tu, tv, controlPoints);

			mesh.push_back(pointInMesh);

		}
	}

	return mesh;
}

glm::vec3 BezierMesh::GetMeshPointAt(float u, float v, std::vector<std::vector<glm::vec3>>& controlPoints)
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

float BezierMesh::Bernstein(int i, float t)
{
	int n = 3;

	//calculate bernstein with precomputed binomial coeffs
	return binomialCoeffs[i] * pow(1 - t, n - i) * pow(t, i);
}
