#include "river.hpp"
#include "bezierMesh.hpp"
#include "glm/gtc/epsilon.hpp"

namespace mat300_terrain
{
	void River::Create(int width, int height, int divCount)
	{
        mWidth = width;
        mHeight = height;
        mDiv = divCount;
		mRiverCtrlPts.push_back({ 0, 0, 0 });
		mRiverCtrlPts.push_back({ width * 0.5, 0, height * 0.25 });
		mRiverCtrlPts.push_back({ width, 0, height * 0.5 });
		mRiverCtrlPts.push_back({ width, 0, height });
		mRiverMesh = CalculateBezierCurve(mRiverCtrlPts);
	}

	void River::CreateMesh(const std::vector<Patch>& patches)
	{
        glm::vec2 patchSize = { mWidth / mDiv, mHeight / mDiv };
        for (int i = 0; i < mRiverMesh.size(); i++)
        {
            Patch thisPatch;
            glm::vec3 pt = mRiverMesh[i];
            glm::vec3 min, max;
            int atX = 0, atZ = 0;
            
            // Select the patch that the point of the rivers' mesh is
            for (auto& patch : patches)
            {
                auto& ctrlPts = patch.controlPoints;
                min = ctrlPts.front().front();
                max = ctrlPts.back().back();
                if ((pt.x > min.x || glm::epsilonEqual(pt.x, min.x, FLT_EPSILON)) && (pt.x < max.x || glm::epsilonEqual(pt.x, max.x, FLT_EPSILON)))
                {
                    if ((pt.z > min.z || glm::epsilonEqual(pt.z, min.z, FLT_EPSILON)) && (pt.z < max.z || glm::epsilonEqual(pt.z, max.z, FLT_EPSILON)))
                    {
                        thisPatch = patch;
                        break;
                    }
                }
                atX++;
                if (atX == mDiv)
                {
                    atZ++;
                    atX = 0;
                }
            }

            // Compute the new point using the Bezier patch and the position of the point as DT
            glm::vec3 newPoint = { 0, 0, 0 };
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    // dt = point position / size of the patch; where the point position is between [0, size of the patch]
                    float Bu = Bernstein(i, (pt.z - (atZ * patchSize.y)) / patchSize.y);
                    float Bv = Bernstein(j, (pt.x - (atX * patchSize.x)) / patchSize.x);

                    newPoint += thisPatch.controlPoints[i][j] * Bv * Bu;
                }
            }

            mRiverMesh[i] = newPoint;
        }
	}
}