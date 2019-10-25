#include <iostream>
using namespace std;

//#壁 _空間 .ゴール oブロック p人
const char gStageData[] = "\
########\n\
# .. p #\n\
# oo   #\n\
#      #\n\
########";
const int gStageWidth = 8;
const int gStageHeight = 5;
int gState[gStageWidth*gStageHeight] = { -1 };

enum Object {
	Wall,
	Player,
	Goal,
	Ball,
	BallOnGoal,
	PlayerOnGoal,
	Space,
	Unknown
};

char gGameChar[8] = { '#', 'p', '.', 'o', 'O', 'P',' ', 'U'};

void initialize(const char* gStageData, int* gState) {
	int n = 0;
	int sn = 0;
	int t = -1;
	while(gStageData[n] != '\0'){
		//printf("%c", gStageData[n]);
		switch (gStageData[n]) {
		case '#': t = Wall; break;
		case '.': t = Goal; break;
		case 'p': t = Player; break;
		case 'o': t = Ball; break;
		case 'O': t = BallOnGoal; break;
		case 'P': t = PlayerOnGoal; break;
		case ' ': t = Space; break;
		default: t = Unknown; break;
		}
		if (Wall <= t && t <= Space) {
			gState[sn] = t;
			sn++;
		}
		n++;
	}
	printf("\ngState = ");
	for (int i = 0; i < gStageWidth * gStageHeight; i++) {
		printf("%d", gState[i]);
	}
	printf("\n");
}

void step(int* gState, int dx = 0, int dy = 0) {
	int pP = 0;
	int p = 0;
	while (gState[p] != Player){
		if ((gState[p] == PlayerOnGoal)) {
			pP = 1;
			break;
		}
		p++;
	}
	printf("p = %d\n", p);
	//p = 0;
	//while ((gState[p] != PlayerOnGoal)) {
	//	p++;
	//}
	printf("p = %d\n", p);
	int pAfter = p + dx - dy * gStageWidth;
	//printf("pAfter = %d, char = %d\n", pAfter, gState[pAfter]);
	if (gState[pAfter] == Space) {
		gState[pAfter] = Player;
		gState[p] = Space;
	}
	if (gState[pAfter] == Goal) {
		gState[pAfter] = PlayerOnGoal;
		gState[p] = Space;
	}
	if (gState[pAfter] == BallOnGoal) {
		int pAfter2 = pAfter + dx - dy * gStageWidth;
		if (gState[pAfter2] == Space) {
			gState[pAfter2] = Ball;
			gState[pAfter] = Player;
			gState[p] = Goal;
		}
		if (gState[pAfter2] == Goal) {
			gState[pAfter2] = BallOnGoal;
			gState[pAfter] = Player;
			gState[p] = Goal;
		}
		gState[pAfter] = Player;
		gState[p] = Goal;
	}
	if (gState[pAfter] == Ball) {
		int pAfter2 = pAfter + dx - dy * gStageWidth;
		printf("pAfter2 = %d, char = %d\n", pAfter2, gState[pAfter2]);
		if (gState[pAfter2] == Space) {
			gState[pAfter2] == Ball;
			gState[pAfter] == Player;
			gState[p] == Space;
		}
		if (gState[pAfter2] == Goal) {
			//printf("OnGoal\n");
			gState[pAfter2] = BallOnGoal;
			gState[pAfter] = Player;
			gState[p] = Space;
		}
	}
	if (pP == 1) {
		gState[p] = Goal;
	}
}

void draw(int* gState) {
	int width = 0;
	int t = -1;
	for (int i = 0; i < gStageWidth * gStageHeight; i++) {
		printf("%c", gGameChar[gState[i]]);
		if ((i+1) % gStageWidth == 0) {
			printf("\n");
		}
	}
}

int ClearCheck(int* gState) {
	int t = 0;
	for (int i = 0; i < gStageWidth * gStageHeight; i++) {
		if (gState[i] == Ball) {
			return 0;
		}
	}
	return 1;
}

int main() {
	//printf("\naaa");
	initialize(gStageData, gState);
	
	while (true) {
		draw(gState);
		int correct = 1;
		int dx = 0;
		int dy = 0;
		cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
		char input;
		cin >> input;
		printf("cin = %c\n", input);
		switch (input) {
		case'a': dx = -1; break;
		case'd': dx = 1; break;
		case'w': dy = 1; break;
		case'z': dy = -1; break;
		default: printf("type correct key!\n"); correct = 0; break;
		}

		if (correct == 1) {
			step(gState, dx, dy);
		}
		if (ClearCheck(gState) == 1) {
			break;
		}
	}

	draw(gState);
	printf("You won!");
}
