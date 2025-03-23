#pragma once
#include <glm/glm.hpp>
#include <vector>

class BezierMesh
{
	std::vector<glm::vec3> CalculateBezierMesh(/*Patch patch*/);

private:

	glm::vec3 GetMeshPointAt(float u, float v, std::vector<std::vector<glm::vec3>>& controlPoints);

	float Bernstein(int i, float t);

	const int binomialCoeffs[4] = { 1,3,3,1 }; //precomputed binomial coeffs for bernstein will always be the same

};