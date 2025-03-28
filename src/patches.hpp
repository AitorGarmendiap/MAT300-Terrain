#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {

    struct Patch
    {
        Patch();

        int GetStepCount() const { return (1 / t); }
        void ComputeNormal();
        void Update();

        std::vector<std::vector<glm::vec3>> controlPoints;
        std::vector<glm::vec3> mesh;

        float t = 0.1;    // step
        glm::vec3 normal;
    };
}