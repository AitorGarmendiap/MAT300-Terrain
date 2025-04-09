#include "river.hpp"
#include "bezierMesh.hpp"
#include "glm/gtc/epsilon.hpp"

namespace mat300_terrain
{
	void River::Create(int width, int depth)
	{
        if (mRiverLineLeft.size() > 1 && mRiverLineRight.size() > 1)
        {
            mRiverLineLeft.clear();
            mRiverLineRight.clear();
        }

        mWidth = width;
        mDepth = depth;

        for (int i = 0; i < mRiverCtrlPts.size() - 1; ++i)
        {
            const glm::vec3 up = glm::vec3(0, 1, 0);
            glm::vec3 right, left;
            glm::vec3 tanget = glm::normalize(mRiverCtrlPts[i + 1] - mRiverCtrlPts[i]);
            right = glm::normalize(glm::cross(tanget, up)) * mThickness;
            left = -right;

            mRiverCtrlPtsLeft.push_back(mRiverCtrlPts[i] + left);
            mRiverCtrlPtsRight.push_back(mRiverCtrlPts[i] + right);
        }

        const glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right, left;
        glm::vec3 tanget = -glm::normalize(mRiverCtrlPts[2] - mRiverCtrlPts[3]);
        right = glm::normalize(glm::cross(tanget, up)) * mThickness;
        left = -right;

        mRiverLineLeft = CalculateBezierCurve(mRiverCtrlPtsLeft, mDt);
        mRiverLineRight = CalculateBezierCurve(mRiverCtrlPtsRight, mDt);
        //mRiverNormals.resize(mRiverLine.size());
	}

	void River::UpdateMesh(const std::vector<Patch>& patches, int divCount)
	{
        ProjectLine(patches, divCount, mRiverLineLeft);
        ProjectLine(patches, divCount, mRiverLineRight);

        for (int i = 0; i < mRiverLineLeft.size() - 1; i++)
        {
            mRiverMesh.push_back(mRiverLineLeft[i]);
            mRiverMesh.push_back(mRiverLineRight[i]);
            mRiverMesh.push_back(mRiverLineRight[i + 1]);

            mRiverMesh.push_back(mRiverLineLeft[i]);
            mRiverMesh.push_back(mRiverLineRight[i + 1]);
            mRiverMesh.push_back(mRiverLineLeft[i + 1]);
        }
    }

    void River::Remove()
    {
        start = false;
        end = false;
        mRiverCtrlPts.clear();
        mRiverMesh.clear();
        mRiverNormals.clear();
    }

    void River::ProjectLine(const std::vector<Patch>& patches, int divCount, std::vector<glm::vec3>& line)
    {
        std::vector<glm::vec3> res;
        glm::vec2 patchSize = { mWidth / divCount, mDepth / divCount };
        for (int i = 0; i < line.size() - 1; i++)
        {
            Patch thisPatch;
            glm::vec3 pt = line[i];
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
                    /*float dBu = dBernstein(k, localU);
                    float dBv = dBernstein(j, localV);*/

                    newPoint += thisPatch.controlPoints[k][j] * Bv * Bu;
                    /*dU += thisPatch.controlPoints[k][j] * Bv * dBu;
                    dV += thisPatch.controlPoints[k][j] * dBv * Bu;*/
                }
            }

            line[i] = newPoint;
            /*mRiverNormals[i] = glm::normalize(glm::cross(dU, dV));*/
        }
    }
}