#include "scene_parser.hpp"

#include <iostream>
#include <fstream>

namespace mat300_terrain
{
    Scene::Scene(const char* filename)
    {
        LoadDataFromFile(filename);
    }

    Scene::~Scene()
    {

    }
    void Scene::LoadDataFromFile(const char* filename)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cout << "Could not open scene file" << std::endl;
            return;
        }

        std::string str;

        while (!file.eof())
        {
            str = "";

            file >> str;

            // Skip comments and empty lines
            if (str.find_first_of("#") == 0)
            {
                std::getline(file, str);
                continue;
            }


            if (str == "CAMERA")
            {
                camPos = ReadVec3(file);
                FOVy = ReadFloat(file);
                nearPlane = ReadFloat(file);
                farPlane = ReadFloat(file);
            }
            else if (str == "HMAP")
            {
                file >> heightMapName;
            }
            else if (str == "PATCHCOUNT")
            {
                farPlane = ReadInt(file);
            }
        }

    }

    int Scene::ReadInt(std::ifstream& f)
    {
        std::string str;
        f >> str;
        return static_cast<int>(std::atoi(str.c_str()));
    }

    float Scene::ReadFloat(std::ifstream& f)
    {
        std::string str;
        f >> str;
        return static_cast<float>(std::atof(str.c_str()));
    }


    glm::vec3 Scene::ReadVec3(std::ifstream& f)
    {
        // String format (float,float,float)
        std::string strVec3;
        f >> strVec3;

        size_t firstComma = strVec3.find_first_of(",");
        size_t lastComma = strVec3.find_last_of(",");

        std::string strX = strVec3.substr(1, firstComma - 1);

        std::string strY = strVec3.substr(firstComma + 1, lastComma - firstComma - 1);

        std::string strZ = strVec3.substr(lastComma + 1, strVec3.size() - lastComma - 1);

        glm::vec3 vec3{ 0.0f };
        vec3.x = static_cast<float>(std::atof(strX.c_str()));
        vec3.y = static_cast<float>(std::atof(strY.c_str()));
        vec3.z = static_cast<float>(std::atof(strZ.c_str()));

        return vec3;
    }

}