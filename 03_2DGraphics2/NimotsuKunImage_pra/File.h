#ifndef ___File
#define ___File

class File{
public:
	File(const char* filename);
	~File();
	void readFile(int* size, const char* filename);
	unsigned getUnsigned(int p);
	const char* data();
	int size();
private:
	int filesize;
    char* mData;
};

#endif