#include "Image.h"
#include "File.h"
#include "GameLib/Framework.h"
#include "File.h"
using namespace GameLib;

Image::Image(const char* filename){
	File fn(filename);
	//char* buffer = 0;
	//int size = 0;
	//fn.readFile(&buffer, &size, filename);
	mHeight = fn.getUnsigned(12);
	mWidth = fn.getUnsigned(16);
	mData = new unsigned[mWidth * mHeight];
	for (int i = 0; i < mWidth * mHeight; ++i) {
		mData[i] =fn. getUnsigned(128 + i * 4);
	}
}

Image::~Image() {
	delete[] mData;
	mData = 0;
}
int Image::width() const{ 
	return mWidth;
}
int Image::height() const{
	return mHeight;
}
const unsigned* Image::data() const{
	return mData;
}

void Image::draw(int dstX, int dstY,
	int srcX, int srcY,
	int width, int height) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			unsigned* dst = &vram[(y + dstY) * windowWidth + (x + dstX)];
			*dst = mData[(y + srcY) * mWidth + (x + srcX)];
		}
	}
}