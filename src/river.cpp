#include "river.hpp"
#include "bezierMesh.hpp"
#include "glm/gtc/epsilon.hpp"

namespace mat300_terrain
{
	void River::Create(int width, int height)
	{
        if (!mRiverMesh.empty())
            mRiverMesh.clear();

        mWidth = width;
        mHeight = height;
        if (mRiverCtrlPts.empty())
        {
            mRiverCtrlPts.push_back({ 0, 0, 0 });
            mRiverCtrlPts.push_back({ width * 0.5, 0, height * 0.25 });
            mRiverCtrlPts.push_back({ width, 0, height * 0.5 });
            mRiverCtrlPts.push_back({ width, 0, height });
        }
        
		mRiverMesh = CalculateBezierCurve(mRiverCtrlPts, mDt);
        mRiverNormals.resize(mRiverMesh.size());
	}

	void River::UpdateMesh(const std::vector<Patch>& patches, int divCount)
	{
        glm::vec2 patchSize = { mWidth / divCount, mHeight / divCount };
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
                min = ctrlPts[0][0];
                max = ctrlPts[3][3];
                if ((pt.x > min.x || glm::epsilonEqual(pt.x, min.x, FLT_EPSILON)) && (pt.x < max.x || glm::epsilonEqual(pt.x, max.x, FLT_EPSILON)))
                {
                    if ((pt.z > min.z || glm::epsilonEqual(pt.z, min.z, FLT_EPSILON)) && (pt.z < max.z || glm::epsilonEqual(pt.z, max.z, FLT_EPSILON)))
                    {
                        thisPatch = patch;
                        break;
                    }
                }
                atX++;
                if (atX == divCount)
                {
                    atZ++;
                    atX = 0;
                }
            }

            // Compute the new point using the Bezier patch and the position of the point as DT
            // dt = point position / size of the patch; where the point position is between [0, size of the patch]
            glm::vec3 dU = glm::vec3{ 0.0f };
            glm::vec3 dV = glm::vec3{ 0.0f };
            float localU = (pt.z - (atZ * patchSize.y)) / patchSize.y;
            float localV = (pt.x - (atX * patchSize.x)) / patchSize.x;
            glm::vec3 newPoint = { 0, 0, 0 };
            for (int k = 0; k < 4; k++)
            {
                for (int j = 0; j < 4; j++)
                {           
                    float Bu = Bernstein(k, localU);
                    float Bv = Bernstein(j, localV);
                    float dBu = dBernstein(k, localU);
                    float dBv = dBernstein(j, localV);

                    newPoint += thisPatch.controlPoints[k][j] * Bv * Bu;
                    dU += thisPatch.controlPoints[k][j] * Bv * dBu;
                    dV += thisPatch.controlPoints[k][j] * dBv * Bu;
                }
            }

            mRiverMesh[i] = newPoint;
            mRiverNormals[i] = glm::normalize(glm::cross(dU, dV));
        }
	}
}