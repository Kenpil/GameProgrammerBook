#include "File.h"
#include <fstream>
using namespace std;

unsigned getUnsigned(const char* p) {
	const unsigned char* up;
	up = reinterpret_cast<const unsigned char*>(p);
	unsigned r = up[0];
	r |= up[1] << 8;
	r |= up[2] << 16;
	r |= up[3] << 24;
	return r;
}

void readFile(char** buffer, int* size, const char* filename){
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