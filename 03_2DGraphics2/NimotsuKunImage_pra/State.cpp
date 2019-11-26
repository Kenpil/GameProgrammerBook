#include "Array2D.h"
#include "State.h"
#include "File.h"
#include "Image.h"
#include "GameLib/Framework.h"
using namespace GameLib;
#include <fstream>
#include <algorithm>

using namespace std;

State::State(const char* stageData, int size):mImage(0) {
	const char* filenameImage = "nimotsuKunImage.dds";
	//Image* mImage = State::mImage();
	mImage = new Image(filenameImage);
	//�T�C�Y����
	setSize(stageData, size);
	//�z��m��
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	//�����l�Ŗ��߂Ƃ�
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //���܂��������͕�
			mGoalFlags(x, y) = false; //�S�[������Ȃ�
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
}

State::~State() {
	delete mImage;
	mImage = 0;
}

Image* State::getImage() {
	return mImage;
}

void State::setSize(const char* stageData, int size) {
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
			mWidth = max(mWidth, x);
			mHeight = max(mHeight, y);
			x = 0;
			break;
		}
	}
}

void State::draw() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			unsigned color = 0;
			ImageId id = ImageOBJ_SPACE;
			if (goalFlag) {
				switch (o) {
				case OBJ_SPACE: cout << '.'; color = 0x0000ff; id = ImageOBJ_GOAL; break;
				case OBJ_WALL: cout << '#'; color = 0xffffff; id = ImageOBJ_WALL; break;
				case OBJ_BLOCK: cout << 'O'; color = 0xff00ff; id = ImageOBJ_BLOCK_ON_GOAL; break;
				case OBJ_MAN: cout << 'P'; color = 0x00ffff; id = ImageOBJ_PLAYER; break;
				}
			}
			else {
				switch (o) {
				case OBJ_SPACE: cout << ' '; color = 0x000000; id = ImageOBJ_SPACE; break;
				case OBJ_WALL: cout << '#'; color = 0xffffff; id = ImageOBJ_WALL; break;
				case OBJ_BLOCK: cout << 'o'; color = 0xff0000; id = ImageOBJ_BLOCK; break;
				case OBJ_MAN: cout << 'p'; color = 0x00ff00; id = ImageOBJ_PLAYER; break;
				}
			}
			drawCellImage(x, y, id);
		}
		cout << endl;
	}
}

void State::drawCell(int x, int y, unsigned color) {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 16; ++j) {
			vram[(y * 16 + i) * windowWidth + (x * 16 + j)] = color;
		}
	}
}

void State::drawCellImage(int x, int y, ImageId id) const {
	mImage->draw(x * 32, y * 32, id * 32, 0, 32, 32);
}

void State::update(char input) {
	//�ړ������ɕϊ�
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break; //��
	case 's': dx = 1; break; //�E
	case 'w': dy = -1; break; //��BY�͉����v���X
	case 'z': dy = 1; break; //���B
	}
	//�Z���ϐ���������B
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//�l���W������
	int x, y;
	x = y = -1;
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
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;
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

