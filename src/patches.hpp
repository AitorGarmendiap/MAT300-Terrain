#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {

    struct Patch
    {

        int GetStepCount() const { return (1 / t); }

        std::vector<std::vector<glm::vec3>> controlPoints;
        std::vector<glm::vec3> mesh;

        float t = 0.1;    // step
    };
}