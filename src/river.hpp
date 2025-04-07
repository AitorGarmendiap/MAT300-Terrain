#pragma once
#include "patches.hpp"
namespace mat300_terrain
{
	class Patch;
	class River
	{
	public:
		River(){}
		~River(){}
		void Create(int width, int height, int divCount);
		void CreateMesh(const std::vector<Patch>& patches);
		std::vector<glm::vec3> GetMesh() const { return mRiverMesh; }
		std::vector<glm::vec3> GetCtrlPts() const { return mRiverCtrlPts; }
		float GetDt() const { return mDt; }

		void SetDt(float newDt) { mDt = newDt; }
	private:
		std::vector<glm::vec3> mRiverMesh, mRiverCtrlPts;
		int mWidth = 0, mHeight = 0, mDiv = 0;
		float mThickness = 10.f;
		float mDt = 0.01f;
	};
}