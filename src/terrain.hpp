#pragma once

#include "patches.hpp"
#include "texture.hpp"
#include "bezierMesh.hpp"
#include "river.hpp"
#include <vector>

namespace mat300_terrain {
    class Terrain
    {
    public:
        Terrain() {}
        ~Terrain();
        void LoadHeightMap(int divCount, const char* heightname);
        void Create(int divCount);
        void Update(const glm::vec3& camPos, float far);
        void Recalculate(const glm::vec3& camPos, float far);
        void Recalculate(int patch, int controlPoint, glm::vec3 prevPos);
        void MoveControlPointY(int patch, int controlPointY, int controlPointX, float deltaY, float reffY,  bool positiveDiff);
        std::vector<Patch>& GetPatches();
        River mRiver;

        int mWidth = 0;
        int mHeight = 0;
        int mDivCount = 1;
        bool sharpEdges = true;
        bool detailedPatch = false;

    private:
        std::vector<Patch> mPatches;
        bool updateDetails = false;
        int prevDivCount = 0;
        bool oldDetail = detailedPatch;
        Texture* mInput = nullptr;
    };

}