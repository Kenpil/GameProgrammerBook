#ifndef ___State
#define ___State

#include "Array2D.h"
#include "File.h"
//#include "Image.h"
class Image;

class State {
public:
	State(const char* stageData, int size);
	~State();
	void update(char input);
	void draw() const;
	Image* getImage();
	Image* mmImage;
	bool hasCleared() const;
private:
	enum Object {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,

		OBJ_UNKNOWN,
	};
	enum ImageId {
		ImageOBJ_PLAYER,
		ImageOBJ_WALL,
		ImageOBJ_BLOCK,
		ImageOBJ_BLOCK_ON_GOAL,
		ImageOBJ_GOAL,
		ImageOBJ_SPACE,
	};
	void setSize(const char* stageData, int size);
	//マス描画関数
	static void drawCell(int x, int y, unsigned color);
	void drawCellImage(int x, int y, ImageId id) const;//このconstで大違い。なにコレ？？？

	int mWidth;
	int mHeight;
	Image* mImage;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
};

#endif