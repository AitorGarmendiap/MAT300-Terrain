#pragma once

#include "patches.hpp"

#include <vector>

namespace mat300_terrain {
    class Terrain
    {
    public:
        Terrain(){}
        ~Terrain(){}

        void Update(float dt);

        std::vector<Patch> GetPatches();

    private:
        std::vector<Patch> mPatches;

        int mPatchCount;
    };

}