#include "Image.h"
#include "File.h"
#include "State.h"

#include "GameLib/Framework.h"
using namespace GameLib;

Image::Image(const char* filename) :
	mWidth(0),
	mHeight(0),
	mData(0) {
	File f(filename);
	mHeight = f.getUnsigned(12);
	mWidth = f.getUnsigned(16);
	mData = new unsigned[mWidth * mHeight];
	for (int i = 0; i < mWidth * mHeight; ++i) {
		mData[i] = f.getUnsigned(128 + i * 4);
	}
}

Image::~Image() {
	delete[] mData;
	mData = 0;
}

int Image::width() const {
	return mWidth;
}

int Image::height() const {
	return mHeight;
}

void Image::draw(
	int dstX,
	int dstY,
	int srcX,
	int srcY,
	int width,
	int height) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	int srcXTmp = srcX;
	State::ImageID id = static_cast<State::ImageID> (srcX / 32);
	//State::ImageID id = (srcX / 32);
	State::ImageID idSpace = State::IMAGE_ID_SPACE;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (id != State::IMAGE_ID_WALL && id != State::IMAGE_ID_SPACE && id != State::IMAGE_ID_GOAL) {//idが壁かスペース以外なら描く
				if ((mData[(y + srcY) * mWidth + (x + srcXTmp)] & 0xff000000) > 0x80000000) {
					//srcXTmp = State::IMAGE_ID_SPACE * 32;
					unsigned* dst = &vram[(y + dstY) * windowWidth + (x + dstX)];
					*dst = mData[(y + srcY) * mWidth + (x + srcXTmp)];
				}
			}
			else {//壁かスペースを書く
				unsigned* dst = &vram[(y + dstY) * windowWidth + (x + dstX)];
				*dst = mData[(y + srcY) * mWidth + (x + srcXTmp)];
			}
		}
	}
}
