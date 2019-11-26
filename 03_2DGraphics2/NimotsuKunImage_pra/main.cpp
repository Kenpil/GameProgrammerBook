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
	//���͎擾
	cout << "a:left s:right w:up z:down. command?" << endl; //�������
	char input;
	cin >> input;
	//�X�V
	gState->update(input);
	//�`��
	gState->draw();

	if (cleared) {
		//�j���̃��b�Z�[�W
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
}