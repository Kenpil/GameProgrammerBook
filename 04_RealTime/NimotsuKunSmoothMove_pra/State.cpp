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
	//�}�X�`��֐�
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

	Type mType;//Array2D�ɂ��AmObjects(x,y).mType�Ƃ���ƁA(x,y)�Ԗڂ�mType�ɃA�N�Z�X�ł���B
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

//drawCell�ɃA�N�Z�X���邽��image�^�|�C���^�����B�|�C���^�Ȃ�錾����ʂ�̃R���p�C���G���[���Ȃ�
void State::Object::drawBackGround(int x, int y, const Image* image) const {
	if (mGoalFlags == true) {
		drawCell(x, y, IMAGE_ID_GOAL, image);//IMAGE_ID~��Object�̗v�f������w���Ȉ����Ƃ�����Ȃ��Ŏg����
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
	//�T�C�Y����
	setSize(stageData, size);
	//�z��m��
	mObjects.setSize(mWidth, mHeight);
	//�����l�Ŗ��߂Ƃ�
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).mType = Object::OBJ_WALL; //���܂��������͕�
			mObjects(x, y).mGoalFlags = false; //�S�[������Ȃ�
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
		case '\n': x = 0; ++y; t = Object::OBJ_UNKNOWN; break; //���s����
		default: t = Object::OBJ_UNKNOWN; break;
		}
		if (t != Object::OBJ_UNKNOWN) { //�m��Ȃ������Ȃ疳������̂ł���if��������
			mObjects(x, y).mType = t; //��������
			mObjects(x, y).mGoalFlags = goalFlag; //�S�[�����
			++x;
		}
	}
	//�摜�ǂݍ���
	mImage = new Image("nimotsuKunImage2.dds");
}

State::~State() {
	delete mImage;
	mImage = 0;
}

void State::draw() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object::drawCell(x, y, State::Object::IMAGE_ID_SPACE, mImage);//���ׂď��Ŗ��߂Ă���
			mObjects(x, y).drawBackGround(x, y, mImage);//�܂��͔w�i
		}
	}
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawForeGround(x, y, mImage, mMoveCount);//���̎�����
		}
	}
}

void State::setSize(const char* stageData, int size) {
	const char* d = stageData; //�ǂݍ��݃|�C���^
	mWidth = mHeight = 0; //������
	//���݈ʒu
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
			//�ő�l�X�V
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
		++mMoveCount;//�ړ����Ȃ�update���Ȃ���return
		return;
	}

	//�Z���ϐ���������B
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//�l���W������
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
	//�ړ�
	//�ړ�����W
	int tx = x + dx;
	int ty = y + dy;
	//���W�̍ő�ŏ��`�F�b�N�B�O��Ă���Εs����
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.���̕������󔒂܂��̓S�[���B�l���ړ��B
	if (o(tx, ty).mType == Object::OBJ_SPACE) {
		o(tx, ty).move(dx, dy, Object::OBJ_MAN);
		o(x, y).move(dx, dy, Object::OBJ_SPACE);
		mMoveCount = 1;
		//B.���̕��������B���̕����̎��̃}�X���󔒂܂��̓S�[���ł���Έړ��B
	}
	else if (o(tx, ty).mType == Object::OBJ_BLOCK) {
		//2�}�X�悪�͈͓����`�F�b�N
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //�����Ȃ�
			return;
		}
		if (o(tx2, ty2).mType == Object::OBJ_SPACE) {
			//��������ւ�
			o(tx2, ty2).move(dx, dy, Object::OBJ_BLOCK);
			o(tx, ty).move(dx, dy, Object::OBJ_MAN);
			o(x, y).mType = Object::OBJ_SPACE;
			mMoveCount = 1;
		}
	}
}

//�u���b�N�̂Ƃ����goalFlag����ł�false�Ȃ�
//�܂��N���A���ĂȂ�
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