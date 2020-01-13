#ifndef INCLUDED_STATE_H
#define INCLUDED_STATE_H

#include "Array2D.h"

class Image; //宣言だけしておけばいい。インクルードしなくていい。

//状態クラス
class State {
private:
	enum Move {
		MOVE_STOP,
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
	};
public:
	State(const char* stageData, int size);
	~State();
	void update(int moveX, int moveY, int* gMovingFrameCount);
	void draw() const;
	void drawSmooth(const int dx, const int dy, int gMovingNowFrameCount);
	bool hasCleared() const;
	Move dxdy2Move(const int dx, const int dy)const;
private:
	enum Object {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,

		OBJ_UNKNOWN,
	};
	//マス描画関数
	enum ImageID {
		IMAGE_ID_PLAYER,
		IMAGE_ID_WALL,
		IMAGE_ID_BLOCK,
		IMAGE_ID_GOAL,
		IMAGE_ID_SPACE,
	};
	void setSize(const char* stageData, int size);
	void drawCell(int x, int y, ImageID) const;
	void drawCellSmooth(int x, int y, bool movingNow, int movingNowCount, ImageID id, Move m, Move leftMove, Move upMove, Move leftLeftMove, Move upUpMove);

	int mWidth;
	int mHeight;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
	Array2D< Move > mMove;
	Image* mImage; //画像
};

#endif
