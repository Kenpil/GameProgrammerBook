#include "GameLib/Framework.h"
#include <fstream>
using namespace std;
//using namespace GameLib;

void readFile( char** buffer, int* size, const char* filename );
unsigned getUnsigned( const char* );
unsigned getUnsigned2(char a, char b, char c, char d);

bool gFirst = true;
int gImageWidth = 0;
int gImageHeight = 0;
unsigned* gImageData = 0;

namespace GameLib {
	void Framework::update() {
		if (gFirst) {
			gFirst = false;
			char* buffer = 0;
			int size = 0;
			readFile(&buffer, &size, "bar.dds");
			gImageHeight = getUnsigned(&(buffer[12]));
			gImageWidth = getUnsigned(&(buffer[16]));
			gImageData = new unsigned[gImageHeight * gImageWidth];
			for (int i = 0; i < gImageHeight * gImageWidth; i++) {
				//gImageData[i] = getUnsigned(&(buffer[128 + i * 4]));
				gImageData[i] = getUnsigned2(buffer[128 + i * 4], buffer[128 + i * 4 + 1], buffer[128 + i * 4 + 2], buffer[128 + i * 4 + 3]);
			}
		}
		unsigned* vram = videoMemory();
		unsigned windowWidth = width();
		for (int y = 0; y < gImageHeight; y++) {
			for (int x = 0; x < gImageWidth; x++) {
				vram[y*windowWidth + x] = gImageData[y*gImageWidth + x];
			}
		}
	}
}

void readFile(char** buffer, int* size, const char* filename) {
	ifstream in(filename, ifstream::binary);
	if (!in) {
		*buffer = 0;
		*size = 0;
	}
	else {
		in.seekg(0, ifstream::end);
		*size = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		*buffer = new char[*size];
		in.read(*buffer, *size);
	}
}

unsigned getUnsigned(const char* p) {
	const unsigned char* up;
	up = reinterpret_cast<const unsigned char*>(p);
	unsigned r = up[0];
	r |= up[1] << 8;
	r |= up[2] << 16;
	r |= up[3] << 24;
	return r;
}

unsigned getUnsigned2(char a, char b, char c, char d) {
	unsigned aa = static_cast<unsigned char>(a);
	unsigned bb = static_cast<unsigned char>(b);
	unsigned cc = static_cast<unsigned char>(c);
	unsigned dd = static_cast<unsigned char>(d);
	unsigned r = aa;
	r |= bb << 8;
	r |= cc << 16;
	r |= dd << 24;
	return r;
}