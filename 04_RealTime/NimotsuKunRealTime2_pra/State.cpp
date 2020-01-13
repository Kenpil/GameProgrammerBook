#include "State.h"
#include "Image.h"
#include <stdio.h>

#include "GameLib/Framework.h"
using namespace GameLib;


State::State(const char* stageData, int size) : mImage(0) {
	//サイズ測定
	setSize(stageData, size);
	//配列確保
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	mMove.setSize(mWidth, mHeight);
	//初期値で埋めとく
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //あまった部分は壁
			mGoalFlags(x, y) = false; //ゴールじゃない
			mMove(x, y) = MOVE_STOP;//はじめはみんな停止している
		}
	}
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		bool goalFlag = false;
		switch (stageData[i]) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK; goalFlag = true; break;
		case '.': t = OBJ_SPACE; goalFlag = true; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN; goalFlag = true; break;
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //改行処理
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) { //知らない文字なら無視するのでこのif文がある
			mObjects(x, y) = t; //書き込み
			mGoalFlags(x, y) = goalFlag; //ゴール情報
			++x;
		}
	}
	//画像読み込み
	mImage = new Image("nimotsuKunImage2.dds");
}

State::~State() {
	delete mImage;
	mImage = 0;
}

void State::setSize(const char* stageData, int size) {
	const char* d = stageData; //読み込みポインタ
	mWidth = mHeight = 0; //初期化
	//現在位置
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		switch (stageData[i]) {
		case '#': case ' ': case 'o': case 'O':
		case '.': case 'p': case 'P':
			++x;
			break;
		case '\n':
			++y;
			//最大値更新
			mWidth = (mWidth > x) ? mWidth : x;
			mHeight = (mHeight > y) ? mHeight : y;
			x = 0;
			break;
		}
	}
}

void State::drawSmooth(const int dx, const int dy, int gMovingNowFrameCount) {// gMovingNowFrameCount>0のとき、アニメ化
	bool movingNow = 0;
	Move leftMove = MOVE_STOP;//左隣の動き.人のマスが左にあって、それがこの移動で動いて来たものならLEFTとなる
	Move upMove = MOVE_STOP;//さらにその左隣の動き
	Move leftLeftMove = MOVE_STOP;
	Move upUpMove = MOVE_STOP;
	if (gMovingNowFrameCount > 0) {
		movingNow = 1;
	}
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			Move m = mMove(x, y);
			if (y != 0) {
				upMove = mMove(x, y - 1);
			}
			if (y > 1) {
				upUpMove = mMove(x, y - 2);
			}
			if (x != 0) {
				leftMove = mMove(x - 1, y);
			}
			if (x > 1) {
				leftLeftMove = mMove(x - 2, y);
			}
			bool goalFlag = mGoalFlags(x, y);
			ImageID id = IMAGE_ID_SPACE;
			if (o != OBJ_WALL) { //壁以外なら床を描く
				if (goalFlag) {
					drawCell(x, y, IMAGE_ID_GOAL);
				}
				else {
					drawCell(x, y, IMAGE_ID_SPACE);
				}
			}

			switch (o) {
			case OBJ_SPACE: id = IMAGE_ID_GOAL; break;
			case OBJ_WALL: id = IMAGE_ID_WALL; break;
			case OBJ_BLOCK: id = IMAGE_ID_BLOCK; break;
			case OBJ_MAN: id = IMAGE_ID_PLAYER; break;
			}
			if (o != OBJ_SPACE || leftMove == MOVE_LEFT || upMove == MOVE_UP) { //床はもう描いたので不要.左か右が動いたものなら、そのままスペースにせず書き直し
				if (leftMove == MOVE_LEFT) {
					cout << "x:" << x << ", y:" << y << ", (x-1 Left)drawSmooth" << endl;
				}
				drawCellSmooth(x, y, movingNow, gMovingNowFrameCount, id, m, leftMove, upMove, leftLeftMove, upUpMove);
			}
		}
	}
	if (gMovingNowFrameCount == 63) {
		for (int y = 0; y < mHeight; ++y) {
			for (int x = 0; x < mWidth; ++x) {
				mMove(x, y) = MOVE_STOP;
			}
		}
	}
}

void State::drawCellSmooth(int x, int y, bool movingNow, int movingFrameCount, ImageID id, Move m, Move leftMove, Move upMove, Move leftLeftMove, Move upUpMove) {
	if (movingNow == 0 || ((id != IMAGE_ID_PLAYER && id != IMAGE_ID_BLOCK)) && leftMove != MOVE_LEFT && upMove != MOVE_UP) {
		//とくに何もないとき
		mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32);
	}
	else if (leftMove == MOVE_LEFT && id != IMAGE_ID_PLAYER) {
		//左が動いて来た人なら、その右であるこのマスをアニメになるよう書き直し
		id = IMAGE_ID_PLAYER;
		mImage->draw(32 * x - movingFrameCount / 2, y * 32, id * 32, 0, 32, 32);
		cout << "left x: " << x << ", y: " << y << "  cellSmooth" << endl;
		if (leftLeftMove == MOVE_LEFT) {
			//ブロックを左に動かしたとき
			id = IMAGE_ID_BLOCK;
			mImage->draw(32 * (x - 1) - movingFrameCount / 2, y * 32, id * 32, 0, 32, 32);
			cout << "leftleft x: " << x << ", y: " << y << "  cellSmooth" << endl;
		}
	}
	else if (upMove == MOVE_UP && id != IMAGE_ID_PLAYER) {
		id = IMAGE_ID_PLAYER;
		mImage->draw(32 * x, y * 32 - movingFrameCount / 2, id * 32, 0, 32, 32);
		cout << "left x: " << x << ", y: " << y << "  cellSmooth" << endl;
		if (upUpMove == MOVE_UP) {
			id = IMAGE_ID_BLOCK;
			//id = IMAGE_ID_GOAL;
			mImage->draw(x * 32, 32 * (y - 1) - movingFrameCount / 2, id * 32, 0, 32, 32);
			cout << "leftleft x: " << x << ", y: " << y << "  cellSmooth" << endl;
		}
	}
	else {//何か動くとき
		switch (m) {
		case MOVE_UP:mImage->draw(x * 32, 32 * (y + 1) - movingFrameCount / 2, id * 32, 0, 32, 32); break;
		case MOVE_DOWN:mImage->draw(x * 32, 32 * (y - 1) + movingFrameCount / 2, id * 32, 0, 32, 32); break;//ok
		case MOVE_LEFT:mImage->draw(32 * (x + 1) - movingFrameCount / 2, y * 32, id * 32, 0, 32, 32);  break;
		case MOVE_RIGHT:mImage->draw(32 * (x - 1) + movingFrameCount / 2, y * 32, id * 32, 0, 32, 32); break;//ok
		default:mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32); break;
		}
	}
}

State::Move State::dxdy2Move(const int dx, const int dy) const {
	switch (dx) {
	case -1:return MOVE_LEFT;
	case 1:return MOVE_RIGHT;
	default:;
	}
	switch (dy) {
	case 1:return MOVE_DOWN;
	case -1:return MOVE_UP;
	default:return MOVE_STOP;
	}
}

void State::draw() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			ImageID id = IMAGE_ID_SPACE;
			if (o != OBJ_WALL) { //壁以外なら床を描く
				if (goalFlag) {
					drawCell(x, y, IMAGE_ID_GOAL);
				}
				else {
					drawCell(x, y, IMAGE_ID_SPACE);
				}
			}
			switch (o) {
			case OBJ_SPACE: id = IMAGE_ID_GOAL; break;
			case OBJ_WALL: id = IMAGE_ID_WALL; break;
			case OBJ_BLOCK: id = IMAGE_ID_BLOCK; break;
			case OBJ_MAN: id = IMAGE_ID_PLAYER; break;
			}
			if (o != OBJ_SPACE) { //床はもう描いたので不要
				drawCell(x, y, id);
			}
		}
	}
}

void State::drawCell(int x, int y, ImageID id) const {
	mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32);
}

void State::update(int dx, int dy, int *gMovingFrameCount) {
	//短い変数名をつける。
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	Array2D< Move >& m = mMove;
	//人座標を検索
	int x, y;
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y) == OBJ_MAN) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	//移動
	//移動後座標
	int tx = x + dx;
	int ty = y + dy;
	//座標の最大最小チェック。外れていれば不許可
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.その方向が空白またはゴール。人が移動。
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		m(tx, ty) = dxdy2Move(dx, dy);//移動後のマスに移動番号付加
		if (m(tx, ty) != MOVE_STOP) {
			*gMovingFrameCount = 1;//人が動いたらアニメ動かす。gMovinFrameCount > 0のとき、updateしない
			cout << "tx:" << tx << ", ty:" << ty << ", Move:" << m(tx, ty) << endl;
		}
		o(x, y) = OBJ_SPACE;
		//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}
	else if (o(tx, ty) == OBJ_BLOCK) {
		//2マス先が範囲内かチェック
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //押せない
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			//順次入れ替え
			o(tx2, ty2) = OBJ_BLOCK;
			m(tx2, ty2) = dxdy2Move(dx, dy);//移動後のマスに移動番号付加
			o(tx, ty) = OBJ_MAN;
			m(tx, ty) = dxdy2Move(dx, dy);//移動後のマスに移動番号付加
			o(x, y) = OBJ_SPACE;
			*gMovingFrameCount = 1;//人が動いたらアニメ動かす。gMovinFrameCount > 0のとき、updateしない
			cout << "tx:" << tx << ", ty:" << ty << ", tx2:" << tx2 << ", ty2:" << ty2 << endl;
		}
	}
}

//ブロックのところのgoalFlagが一つでもfalseなら
//まだクリアしてない
bool State::hasCleared() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			if (mObjects(x, y) == OBJ_BLOCK) {
				if (mGoalFlags(x, y) == false) {
					return false;
				}
			}
		}
	}
	return true;
}
