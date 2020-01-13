#include "State.h"
#include "Image.h"
#include <stdio.h>

#include "GameLib/Framework.h"
using namespace GameLib;


State::State(const char* stageData, int size) : mImage(0) {
	//�T�C�Y����
	setSize(stageData, size);
	//�z��m��
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	mMove.setSize(mWidth, mHeight);
	//�����l�Ŗ��߂Ƃ�
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //���܂��������͕�
			mGoalFlags(x, y) = false; //�S�[������Ȃ�
			mMove(x, y) = MOVE_STOP;//�͂��߂݂͂�Ȓ�~���Ă���
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
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //���s����
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) { //�m��Ȃ������Ȃ疳������̂ł���if��������
			mObjects(x, y) = t; //��������
			mGoalFlags(x, y) = goalFlag; //�S�[�����
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

void State::drawSmooth(const int dx, const int dy, int gMovingNowFrameCount) {// gMovingNowFrameCount>0�̂Ƃ��A�A�j����
	bool movingNow = 0;
	Move leftMove = MOVE_STOP;//���ׂ̓���.�l�̃}�X�����ɂ����āA���ꂪ���̈ړ��œ����ė������̂Ȃ�LEFT�ƂȂ�
	Move upMove = MOVE_STOP;//����ɂ��̍��ׂ̓���
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
			if (o != OBJ_WALL) { //�ǈȊO�Ȃ珰��`��
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
			if (o != OBJ_SPACE || leftMove == MOVE_LEFT || upMove == MOVE_UP) { //���͂����`�����̂ŕs�v.�����E�����������̂Ȃ�A���̂܂܃X�y�[�X�ɂ�����������
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
		//�Ƃ��ɉ����Ȃ��Ƃ�
		mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32);
	}
	else if (leftMove == MOVE_LEFT && id != IMAGE_ID_PLAYER) {
		//���������ė����l�Ȃ�A���̉E�ł��邱�̃}�X���A�j���ɂȂ�悤��������
		id = IMAGE_ID_PLAYER;
		mImage->draw(32 * x - movingFrameCount / 2, y * 32, id * 32, 0, 32, 32);
		cout << "left x: " << x << ", y: " << y << "  cellSmooth" << endl;
		if (leftLeftMove == MOVE_LEFT) {
			//�u���b�N�����ɓ��������Ƃ�
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
	else {//���������Ƃ�
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
			if (o != OBJ_WALL) { //�ǈȊO�Ȃ珰��`��
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
			if (o != OBJ_SPACE) { //���͂����`�����̂ŕs�v
				drawCell(x, y, id);
			}
		}
	}
}

void State::drawCell(int x, int y, ImageID id) const {
	mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32);
}

void State::update(int dx, int dy, int *gMovingFrameCount) {
	//�Z���ϐ���������B
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	Array2D< Move >& m = mMove;
	//�l���W������
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
	//�ړ�
	//�ړ�����W
	int tx = x + dx;
	int ty = y + dy;
	//���W�̍ő�ŏ��`�F�b�N�B�O��Ă���Εs����
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.���̕������󔒂܂��̓S�[���B�l���ړ��B
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		m(tx, ty) = dxdy2Move(dx, dy);//�ړ���̃}�X�Ɉړ��ԍ��t��
		if (m(tx, ty) != MOVE_STOP) {
			*gMovingFrameCount = 1;//�l����������A�j���������BgMovinFrameCount > 0�̂Ƃ��Aupdate���Ȃ�
			cout << "tx:" << tx << ", ty:" << ty << ", Move:" << m(tx, ty) << endl;
		}
		o(x, y) = OBJ_SPACE;
		//B.���̕��������B���̕����̎��̃}�X���󔒂܂��̓S�[���ł���Έړ��B
	}
	else if (o(tx, ty) == OBJ_BLOCK) {
		//2�}�X�悪�͈͓����`�F�b�N
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //�����Ȃ�
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			//��������ւ�
			o(tx2, ty2) = OBJ_BLOCK;
			m(tx2, ty2) = dxdy2Move(dx, dy);//�ړ���̃}�X�Ɉړ��ԍ��t��
			o(tx, ty) = OBJ_MAN;
			m(tx, ty) = dxdy2Move(dx, dy);//�ړ���̃}�X�Ɉړ��ԍ��t��
			o(x, y) = OBJ_SPACE;
			*gMovingFrameCount = 1;//�l����������A�j���������BgMovinFrameCount > 0�̂Ƃ��Aupdate���Ȃ�
			cout << "tx:" << tx << ", ty:" << ty << ", tx2:" << tx2 << ", ty2:" << ty2 << endl;
		}
	}
}

//�u���b�N�̂Ƃ����goalFlag����ł�false�Ȃ�
//�܂��N���A���ĂȂ�
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
