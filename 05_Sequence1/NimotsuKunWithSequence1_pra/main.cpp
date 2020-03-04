#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"
#include "Image.h"

//�֐��v���g�^�C�v
void mainLoop();
void game(); //�Q�[�����[�v
void title(); //�^�C�g����ʃ��[�v

enum Sequence {
	SEQUENCE_GAME,
	SEQUENCE_TITLE,
};

//�O���[�o���ϐ�
Sequence gSequence = SEQUENCE_TITLE;
//Framework gF = Framework::instance();
Image* gTitleImage = 0; //�^�C�g����ʉ摜
int gCounter = 0;

State* gState = 0;
bool gPrevInputS = false;
bool gPrevInputA = false;
bool gPrevInputW = false;
bool gPrevInputZ = false;

//���[�U�����֐��B���g��mainLoop()�Ɋۓ���
namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//�t���[�����[�g����
	Framework f = Framework::instance();
	f.setFrameRate(60); //���Ăׂ΂����̂����ʓ|�Ȃ̂ŌĂ�ł��܂��B

	if (gCounter % 60 == 0) { //60�t���[���Ɉ��t���[�����[�g�\��
		cout << " FrameRate:" << f.frameRate() << endl;
	}
	++gCounter;

	switch (gSequence) {
	case SEQUENCE_TITLE:
		title();
		break;
	case SEQUENCE_GAME:
		game();
		break;
	}
	//�I������
	if (f.isKeyOn('q')) {
		f.requestEnd();
	}
	if (f.isEndRequested()) {
		if (gState) {
			delete gState;
			gState = 0;
		}
	}
}

void game() {
	Framework f = Framework::instance();
	bool cleared = false;
	//���C�����[�v
	//�N���A�`�F�b�N
	if (gState->hasCleared()) {
		cleared = true;
	}
	//���͎擾
	int dx = 0;
	int dy = 0;
	bool inputA = f.isKeyOn('a');
	bool inputS = f.isKeyOn('s');
	bool inputW = f.isKeyOn('w');
	bool inputZ = f.isKeyOn('z');
	if (inputA && (!gPrevInputA)) {
		dx -= 1;
	}
	else if (inputS && (!gPrevInputS)) {
		dx += 1;
	}
	else if (inputW && (!gPrevInputW)) {
		dy -= 1;
	}
	else if (inputZ && (!gPrevInputZ)) {
		dy += 1;
	}
	gPrevInputA = inputA;
	gPrevInputS = inputS;
	gPrevInputW = inputW;
	gPrevInputZ = inputZ;
	//�X�V
	gState->update(dx, dy);
	//�`��
	gState->draw();

	if (cleared) {
		//�j���̃��b�Z�[�W
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
		gSequence = SEQUENCE_TITLE;
	}
}

void title() {
	if (!gTitleImage) {
		gTitleImage = new Image("title.dds");
	}

	gTitleImage->draw(0, 0, 0, 0, gTitleImage->width(), gTitleImage->height());
	bool goLoop = Framework::instance().isKeyOn(' ');
	if (goLoop) {
		gSequence = SEQUENCE_GAME;
		if (!gState) {
			File file("stageData.txt");
			if (!(file.getData())) { //�f�[�^�Ȃ��I
				cout << "stage file could not be read." << endl;
				return;
			}
			gState = new State(file.getData(), file.getSize());
		}
		mainLoop();
	}
}