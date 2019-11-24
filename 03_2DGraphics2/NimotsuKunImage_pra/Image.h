#ifndef ___Image
#define ___Image

class Image {
public:
	Image(const char* filename);
	~Image();
	int width() const;
	int height() const;
	const unsigned* data() const;
	void draw(
		int dstX, int dstY,
		int srcX, int srcY,
		int width, int height)const;
private:
	int mWidth;
	int mHeight;
	unsigned* mData;
};

#endif