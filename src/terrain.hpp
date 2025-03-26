#pragma once

#include "patches.hpp"

#include <vector>

namespace mat300_terrain {
    class Terrain
    {
    public:
        void Create(int divCount, const char* heightname);
        void Update(float dt);

        std::vector<Patch> GetPatches();

        std::vector<Patch> mPatches;
        int mWidth = 0;
        int mHeight = 0;
        int mDivCount = 1; 
    };

}