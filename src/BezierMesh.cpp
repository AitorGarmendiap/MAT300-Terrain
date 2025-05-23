#include "bezierMesh.hpp"

namespace mat300_terrain {

	glm::vec3 GetMeshPointAt(float u, float v, glm::vec3 controlPoints[4][4]);

	const int binomialCoeffs[4] = { 1,3,3,1 }; //precomputed binomial coeffs for bernstein will always be the same
	const int binomialCoeffs2[3] = { 1, 2, 1 };


	void CalculateBezierMesh(Patch& patch)
	{
		std::vector<glm::vec3> mesh;

        int steps = patch.GetStepCount();

		for (int i = 0; i <= steps; i++)
		{
			float tu = i * patch.t;

			for (int j = 0; j <= steps; j++)
			{
				float tv = j * patch.t;

				glm::vec3 pointInMesh = GetMeshPointAt(tu, tv, patch.controlPoints);

				mesh.push_back(pointInMesh);

			}
		}
		patch.mesh = mesh;
	}

	std::vector<glm::vec3> CalculateBezierCurve(const std::vector<glm::vec3>& ctrlPoints, float dt)
	{
		std::vector<glm::vec3> res;
		res.reserve(static_cast<size_t>(ceil(1.0f / dt) + 1.0f));
		for (float t = 0.f; t < 1.f; t += dt)
		{
			glm::vec3 out_pts = { 0, 0, 0 };
			for (int i = 0; i < ctrlPoints.size(); i++)
				out_pts += ctrlPoints[i] * Bernstein(i, t);
			res.push_back(out_pts);
		}
		// add last point
		glm::vec3 last = { 0.0f, 0.0f, 0.0f };
		for (int i = 0; i < ctrlPoints.size(); i++)
			last += ctrlPoints[i] * Bernstein(i, 1);
		res.push_back(last);
		return res;
	}

	std::vector<glm::vec3> CalculateDerivativeBezierCurve(const std::vector<glm::vec3>& ctrlPoints, float dt)
	{
		std::vector<glm::vec3> res;
		const glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		res.reserve(static_cast<size_t>(ceil(1.0f / dt) + 1.0f));
		for (float t = 0.f; t < 1.f; t += dt)
		{
			glm::vec3 out_pts = { 0, 0, 0 };
			for (int i = 0; i < ctrlPoints.size(); i++)
				out_pts += ctrlPoints[i] * dBernstein(i, t);
			res.push_back(glm::cross(out_pts, up));
		}
		// add last point
		glm::vec3 last = { 0.0f, 0.0f, 0.0f };
		for (int i = 0; i < ctrlPoints.size(); i++)
			last += ctrlPoints[i] * dBernstein(i, 1);
		res.push_back(glm::cross(last, up));
		return res;
	}

	glm::vec3 GetMeshPointAt(float u, float v, glm::vec3 controlPoints[4][4])
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
		return static_cast<float>(binomialCoeffs[i] * pow(1.0f - t, n - i) * pow(t, i));
	}

	float Bernstein2(int i, float t)
	{
		int n = 2;
		return static_cast<float>(binomialCoeffs2[i] * pow(1.0f - t, n - i) * pow(t, i));
	}

	float dBernstein(int i, float t)
	{
		if (i == 0)
			return -3 * Bernstein2(0, t);
		else if (i == 3)
			return 3 * Bernstein2(2, t);
		else
			return 3 * (Bernstein2(i - 1, t) - Bernstein2(i, t));
	}
}