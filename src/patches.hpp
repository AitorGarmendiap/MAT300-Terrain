#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {

    struct Patch
    {

        int GetDivisionCount() const { return (1 / t) + 1; }

        std::vector<std::vector<glm::vec3>> controlPoints;
        std::vector<glm::vec3> mesh;

        float t;    // step
    };
}