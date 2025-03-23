#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {

    struct Patches
    {
        std::vector<glm::vec3> controlPoints;

        std::vector<glm::vec3> mesh;

        float t;    // step
    };
}