#pragma once

namespace mat300_terrain 
{
	class Texture
	{
	public:
		Texture() {}
		Texture(const char* filename);
		~Texture();
		int width() const { return mWidth; }
		int height() const { return mHeight; }
		unsigned char* data() const { return mData; }
		void operator=(const Texture& text);
	private:
		int mWidth;
		int mHeight;
		unsigned char* mData;
	};
}