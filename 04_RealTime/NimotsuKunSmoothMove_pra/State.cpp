#include "State.h"
#include "Image.h"

class State::Object {
public:
	enum Type {
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
	Object() :mType(OBJ_WALL), mGoalFlags(false), mMoveX(0), mMoveY(0) {}
	void set(char c);
	void drawForeGround(int x, int y, const Image* image, int moveCount) const;
	void drawBackGround(int x, int y, const Image* image) const;
	static void drawCell(int x, int y, Object::ImageID id, const Image* image);
	void move(int dx, int dy, Type replacedType);

	Type mType;//Array2Dにより、mObjects(x,y).mTypeとすると、(x,y)番目のmTypeにアクセスできる。
	bool mGoalFlags;
	int mMoveX;
	int mMoveY;
};

void State::Object::drawCell(int x, int y, Object::ImageID id, const Image* image) {
	image->draw(x * 32, y * 32, id * 32, 0, 32, 32);
}

void State::Object::drawForeGround(int x, int y, const Image* image, int moveCount) const {
	if (moveCount == 0) {
		switch (mType) {
		case OBJ_BLOCK: drawCell(x, y, IMAGE_ID_BLOCK, image); break;
		case OBJ_MAN: drawCell(x, y, IMAGE_ID_PLAYER, image); break;
		}
	}
	else {
		int xWay = (x - mMoveX) * 32 + mMoveX * moveCount;
		int yWay = (y - mMoveY) * 32 + mMoveY * moveCount;
		switch (mType) {
		case OBJ_BLOCK: image->draw(xWay, yWay, IMAGE_ID_BLOCK * 32, 0, 32, 32); break;
		case OBJ_MAN: image->draw(xWay, yWay, IMAGE_ID_PLAYER * 32, 0, 32, 32); break;
		}
	}
}

//drawCellにアクセスするためimage型ポインタを作る。ポインタなら宣言うんぬんのコンパイルエラーがない
void State::Object::drawBackGround(int x, int y, const Image* image) const {
	if (mGoalFlags == true) {
		drawCell(x, y, IMAGE_ID_GOAL, image);//IMAGE_ID~はObjectの要素だからヘンな引数とかいらないで使える
	}
	else {
		switch (mType) {
		case OBJ_SPACE: drawCell(x, y, IMAGE_ID_SPACE, image); break;
		case OBJ_WALL: drawCell(x, y, IMAGE_ID_WALL, image); break;
		}
	}
}

void State::Object::move(int dx, int dy, State::Object::Type replacedType) {
	mMoveX = dx;
	mMoveY = dy;
	mType = replacedType;
}

State::State(const char* stageData, int size) : mImage(0) {
	//サイズ測定
	setSize(stageData, size);
	//配列確保
	mObjects.setSize(mWidth, mHeight);
	//初期値で埋めとく
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).mType = Object::OBJ_WALL; //あまった部分は壁
			mObjects(x, y).mGoalFlags = false; //ゴールじゃない
		}
	}
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object::Type t;
		bool goalFlag = false;
		switch (stageData[i]) {
		case '#': t = Object::OBJ_WALL; break;
		case ' ': t = Object::OBJ_SPACE; break;
		case 'o': t = Object::OBJ_BLOCK; break;
		case 'O': t = Object::OBJ_BLOCK; goalFlag = true; break;
		case '.': t = Object::OBJ_SPACE; goalFlag = true; break;
		case 'p': t = Object::OBJ_MAN; break;
		case 'P': t = Object::OBJ_MAN; goalFlag = true; break;
		case '\n': x = 0; ++y; t = Object::OBJ_UNKNOWN; break; //改行処理
		default: t = Object::OBJ_UNKNOWN; break;
		}
		if (t != Object::OBJ_UNKNOWN) { //知らない文字なら無視するのでこのif文がある
			mObjects(x, y).mType = t; //書き込み
			mObjects(x, y).mGoalFlags = goalFlag; //ゴール情報
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

void State::draw() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object::drawCell(x, y, State::Object::IMAGE_ID_SPACE, mImage);//すべて床で埋めておく
			mObjects(x, y).drawBackGround(x, y, mImage);//まずは背景
		}
	}
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawForeGround(x, y, mImage, mMoveCount);//その次物体
		}
	}
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


void State::update(int dx, int dy) {
	if (mMoveCount == 32) {
		mMoveCount = 0;
		for (int y = 0; y < mHeight; y++) {
			for (int x = 0; x < mWidth; x++) {
				mObjects(x, y).mMoveX = 0;
				mObjects(x, y).mMoveY = 0;
			}
		}
	}
	if (mMoveCount > 0) {
		++mMoveCount;//移動中ならupdateしないでreturn
		return;
	}

	//短い変数名をつける。
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//人座標を検索
	int x, y;
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y).mType == Object::OBJ_MAN) {
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
	if (o(tx, ty).mType == Object::OBJ_SPACE) {
		o(tx, ty).move(dx, dy, Object::OBJ_MAN);
		o(x, y).move(dx, dy, Object::OBJ_SPACE);
		mMoveCount = 1;
		//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}
	else if (o(tx, ty).mType == Object::OBJ_BLOCK) {
		//2マス先が範囲内かチェック
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //押せない
			return;
		}
		if (o(tx2, ty2).mType == Object::OBJ_SPACE) {
			//順次入れ替え
			o(tx2, ty2).move(dx, dy, Object::OBJ_BLOCK);
			o(tx, ty).move(dx, dy, Object::OBJ_MAN);
			o(x, y).mType = Object::OBJ_SPACE;
			mMoveCount = 1;
		}
	}
}

//ブロックのところのgoalFlagが一つでもfalseなら
//まだクリアしてない
bool State::hasCleared() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			if (mObjects(x, y).mType == Object::OBJ_BLOCK) {
				if (mObjects(x, y).mGoalFlags == false) {
					return false;
				}
			}
		}
	}
	return true;
}