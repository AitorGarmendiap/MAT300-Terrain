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
		void Create(int width, int height);
		void UpdateMesh(const std::vector<Patch>& patches, int divCount);
		std::vector<glm::vec3> GetMesh() const { return mRiverMesh; }
		std::vector<glm::vec3> GetNormals() const { return mRiverNormals; }
		void Remove();
		float GetDt() const { return mDt; }

		void SetDt(float newDt) { mDt = newDt; }

		bool start = false;
		bool end = false;
		int selectedCtrlPt = -1;
		std::vector<glm::vec3> mRiverCtrlPts;
	private:
		std::vector<glm::vec3> mRiverMesh, mRiverNormals;
		int mWidth = 0, mHeight = 0, mDiv = 0;
		float mThickness = 10.f;
		float mDt = 0.01f;
	};
}