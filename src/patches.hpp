#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {

    struct Patch
    {
        Patches() {}

        int GetStepCount() const { return (1 / t); }

        std::vector<std::vector<glm::vec3>> controlPoints;
        std::vector<glm::vec3> mesh;

        float t = 0.1;    // step
        glm::vec3 pos;
        glm::vec3 size;
        void UpdateNormal(glm::vec3 newNormal);
        glm::vec3 GetRight();
        glm::vec3 GetUp();
    private:
        glm::vec3 normal;
        glm::vec3 right;
        glm::vec3 up;
    };
}