#pragma once

#include "patches.hpp"
#include "texture.hpp"
#include <vector>

namespace mat300_terrain {
    class Terrain
    {
    public:
        Terrain() {}
        ~Terrain();
        void LoadHeightMap(int divCount, const char* heightname);
        void Create(int divCount);
        void Update();
        void Update(int patch, int controlPoint, glm::vec3 prevPos);
        void MoveControlPointY(int patch, int controlPointY, int controlPointX, float deltaY, float reffY,  bool positiveDiff);
        std::vector<Patch> GetPatches();

        std::vector<Patch> mPatches;
        int mWidth = 0;
        int mHeight = 0;
        int mDivCount = 1; 
        bool sharpEdges = true;

    private:
        int prevDivCount = 0;
        Texture* mInput = nullptr;
    };

}