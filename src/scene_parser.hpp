#pragma once

#include <glm/glm.hpp>
#include <string>

namespace mat300_terrain
{
    class Scene
    {
    public:
        Scene(const char* filename);
        ~Scene();

        glm::vec3 camPos;
        float FOVy;
        float nearPlane;
        float farPlane;

        std::string heightMapName;
        int divCount;

    private:

        void LoadDataFromFile(const char* filename);

        int ReadInt(std::ifstream& f);
        float ReadFloat(std::ifstream& f);
        glm::vec3 ReadVec3(std::ifstream& f);


    };
}