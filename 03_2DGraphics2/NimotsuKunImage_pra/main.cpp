#include "Image.h"
#include "State.h"
#include "GameLib/Framework.h"
using namespace GameLib;
#include <fstream>
using namespace std;

State* gState = 0;

void readFile(char** buffer, int* size, const char* filename);
void mainLoop();

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	if (!gState) {
		const char* filename = "stageData.txt";
		File file(filename);
		/*char* stageData;
		int fileSize;
		readFile(&stageData, &fileSize, filename);
		if (!stageData) {
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(stageData, fileSize);
		delete[] stageData;
		stageData = 0;*/

		gState = new State(file.data(), file.size());
		gState->draw();
		return;
	}
	bool cleared = false;

	if (gState->hasCleared()) {
		cleared = true;
	}
	//入力取得
	cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
	char input;
	cin >> input;
	//更新
	gState->update(input);
	//描画
	gState->draw();

	if (cleared) {
		//祝いのメッセージ
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
}