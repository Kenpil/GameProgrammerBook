#include "File.h"
#include <fstream>
using namespace std;

File::File(const char* filename) {
	readFile(&filesize, filename);
}

File::~File() {
	delete mData;
	mData = 0;
}

unsigned File::getUnsigned(int p) {
	const unsigned char* up;
	//up = reinterpret_cast<const unsigned char*>(p);
	up = reinterpret_cast<const unsigned char*> (mData);
	unsigned r = up[0+p];
	r |= up[1+p] << 8;
	r |= up[2+p] << 16;
	r |= up[3+p] << 24;
	return r;
}

void File::readFile(int* size, const char* filename){
	ifstream in(filename, ifstream::binary);

		in.seekg(0, ifstream::end);
		*size = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		mData = new char[*size];
		in.read(mData, *size);
}

const char* File::data() {
	return mData;
}

int File::size() {
	return filesize;
}