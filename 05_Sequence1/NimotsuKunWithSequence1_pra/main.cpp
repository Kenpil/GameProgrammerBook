#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"
#include "Image.h"
#include <stdio.h>
#include <iostream>

//関数プロトタイプ
void mainLoop();
void game(); //ゲームループ
void menu();
void load(int stageNumeber);
int sellectStage();
void title(); //タイトル画面ループ

enum Sequence {
	SEQUENCE_GAME,
	SEQUENCE_MENU,
	SEQUENCE_LOAD,
	SEQUENCE_SELECTSTAGE,
	SEQUENCE_TITLE,
};

//グローバル変数
Sequence gSequence = SEQUENCE_TITLE;
//Framework gF = Framework::instance();
Image* gTitleImage = 0; //タイトル画面画像
int gCounter = 0;

State* gState = 0;
bool gPrevInputS = false;
bool gPrevInputA = false;
bool gPrevInputW = false;
bool gPrevInputZ = false;
bool gPrevInputM = false;

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//フレームレート調整
	Framework f = Framework::instance();
	f.setFrameRate(60); //一回呼べばいいのだが面倒なので呼んでしまう。

	if (gCounter % 60 == 0) { //60フレームに一回フレームレート表示
		cout << " FrameRate:" << f.frameRate() << endl;
	}
	++gCounter;
	static int stageNumber = 0;

	switch (gSequence) {
	case SEQUENCE_TITLE:
		title();
		break;
	case SEQUENCE_LOAD:
		load(stageNumber);
		break;
	case SEQUENCE_MENU:
		menu();
		break;
	case  SEQUENCE_SELECTSTAGE:
		stageNumber = sellectStage();
		cout << "stageNumber " << stageNumber << endl;
		break;
	case SEQUENCE_GAME:
		game();
		break;
	}
	//終了判定
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
	//メインループ
	//クリアチェック
	if (gState->hasCleared()) {
		cleared = true;
	}
	//入力取得
	int dx = 0;
	int dy = 0;
	bool inputA = f.isKeyOn('a');
	bool inputS = f.isKeyOn('s');
	bool inputW = f.isKeyOn('w');
	bool inputZ = f.isKeyOn('z');
	bool inputM = f.isKeyOn('m');
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
	else if (inputM && (!gPrevInputM)) {
		gSequence = SEQUENCE_MENU;
	}
	gPrevInputA = inputA;
	gPrevInputS = inputS;
	gPrevInputW = inputW;
	gPrevInputZ = inputZ;
	//更新
	gState->update(dx, dy);
	//描画
	gState->draw();

	if (cleared) {
		//祝いのメッセージ
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
		gSequence = SEQUENCE_TITLE;
	}
}

void menu() {
	//cout << "MENU!" << endl;
	static bool firstMenu = true;
	Image* mMenuImage = new Image("data/image/menu.dds");
	mMenuImage->draw(0, 0, 0, 0, mMenuImage->width(), mMenuImage->height());
	enum changeItem {
		CHANGE_REDO,
		CHANGE_SELECT,
		CHANGE_TITLE,
		CHANGE_CONTINUE,
		OTHER,
	};
	int change = -1;
	if (firstMenu == false) {

		char numberChars[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		for (int i = 0; i < OTHER; ++i) {
			if (Framework::instance().isKeyTriggered(numberChars[i])) {
				change = i;
				cout << "change!" << change << endl;
			}
		}

		switch (change) {
		case CHANGE_REDO:
			gSequence = SEQUENCE_LOAD;
			SAFE_DELETE(gState);
			break;
		case CHANGE_SELECT:
			gSequence = SEQUENCE_SELECTSTAGE;
			SAFE_DELETE(gState);
			break;
		case CHANGE_TITLE:
			gSequence = SEQUENCE_TITLE;
			SAFE_DELETE(gState);
			break;
		case CHANGE_CONTINUE:
			gSequence = SEQUENCE_GAME;
			break;
		default: break;
		}
		firstMenu = true;
	}
	firstMenu = false;
}

void load(int stageNumeber) {
	if (!gState) {
		File file("stageData.txt");
		if (!(file.getData())) { //データない！
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(file.getData(), file.getSize());
	}
	gSequence = SEQUENCE_GAME;
}

int sellectStage() {
	Image* mTitleImage = new Image("data/image/stageSelect.dds");
	mTitleImage->draw(0, 0, 0, 0, mTitleImage->width(), mTitleImage->height());
	int stageNumber = 0;
	char numberChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	for (int i = 0; i < 10; ++i) {
		if (Framework::instance().isKeyTriggered(numberChars[i])) {
			stageNumber = i;
		}
	}
	if (stageNumber != 0) {
		gSequence = SEQUENCE_LOAD;
		return stageNumber;
	}
}

void title() {
	if (!gTitleImage) {
		gTitleImage = new Image("title.dds");
	}

	gTitleImage->draw(0, 0, 0, 0, gTitleImage->width(), gTitleImage->height());
	bool goLoop = Framework::instance().isKeyOn(' ');
	if (goLoop) {
		gSequence = SEQUENCE_SELECTSTAGE;
	}
}