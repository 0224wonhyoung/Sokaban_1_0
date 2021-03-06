#include <stdio.h>
#include <bangtal.h>
#include <time.h>
#include <sys/timeb.h>


#define EMPTY	-2	// 아무것도 없는 빈공간 (벽 외부)
#define WALL	-1	// 벽							can't
#define TILE	0	// 오브젝트 없는 빈 타일		can
#define BOX		1	// 상자				moveable	can't
#define SLAB	2	// 발판				fixed		can
#define HOUSE	3	// 집				fixed		can't
#define SPEAR	4	// 창				fixed		can
#define THRON	5	// 가시				fixed		can
#define KEY		6	// 열쇠				fixed		can
#define TREASURE 7	// 보물상자			fixed		flexible
#define ROCK	8	// 돌				movable		can't
#define PLAYER	10	// player				


#define BOARD_X_PIX	50	// 보드 한칸의 픽셀너비
#define BOARD_Y_PIX 50	// 보드 한칸의 픽셀높이
#define BOARD_X_NUM 10	// 보드 전체 가로 칸수
#define BOARD_Y_NUM 10	// 보드 전체 세로 칸수

#define TOTAL_STAGE_NUM		20	// 총 스테이지 갯수
#define TOTAL_OBJECT_TYPE	11	// 오브젝트 종류 갯수 +1


struct ObjectStruct { char x; char y; char type; };

SceneID sceneStage[TOTAL_STAGE_NUM];
ObjectID tile[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];	// 벽과 바닥(타일) 오브젝트.
/*ObjectID fixedObject[TOTAL_STAGE_NUM][100];
ObjectID movableObject[TOTAL_STAGE_NUM][100];*/
ObjectID object[TOTAL_STAGE_NUM][TOTAL_OBJECT_TYPE][30];
ObjectStruct objectStruct[TOTAL_STAGE_NUM][TOTAL_OBJECT_TYPE][30];
ObjectID player[TOTAL_STAGE_NUM];


// 스테이지 구성
char initBoard[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM] = {
	// STAGE 0
	{	{-2,-2,-2,-2,-1,-1,-1,-1,-1,-2},
		{-2,-2,-1,-1,-1, 0, 0, 0,-1,-2},
		{-2,-1,-1, 0, 0, 0, 0, 0,-1,-2},
		{-1, 0, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1, 0, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1, 0, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1, 0, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1, 0, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1,10, 0, 0, 0, 0, 0, 0,-1,-2},
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-2},},
		// STAGE 1
	{	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1, 0, 0, 0, 0, 0, 0, 0, 0,-1},
		{-1, 0, 0, 0, 0, 2, 0, 2, 0,-1},
		{-1, 0, 0, 0, 1, 0, 1, 0, 0,-1},
		{-1, 0, 0, 0, 0, 0, 0, 0, 0,-1},
		{-1, 0, 0, 0, 0, 0, 0, 0, 0,-1},
		{-1, 0, 0, 1, 0, 0, 0, 0, 0,-1},
		{-1, 0, 0, 1, 0, 0, 0, 0, 0,-1},
		{-1,10, 0, 1, 0, 0, 0, 0, 0,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} }
};


char curStage;
char playerX[TOTAL_STAGE_NUM], playerY[TOTAL_STAGE_NUM];	// player의 X, Y 좌표
char dx[4] = { -1, 1, 0, 0 };	// LEFT RIGHT UP DOWN
char dy[4] = { 0, 0, 1, -1 };

char fixedObjBoard[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];
char moveableObjBoard[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];




// 함수는 이부분부터 -------------------------

short coolX(char boardX) {
	short x = (1280 - (BOARD_X_NUM * BOARD_X_PIX)) / 2 + (boardX * BOARD_X_PIX);
	return x;
}

short coolY(char boardY) {
	short y = (720 - (BOARD_Y_NUM * BOARD_Y_PIX)) / 2 + (boardY * BOARD_Y_PIX);
	return y;
}

char numToASCII(char num) {
	return num + 48; // 아스키 48번째 : 0
}

ObjectID createObject(const char* imgLocate, SceneID scene, char boardX, char boardY, bool shown) {
	ObjectID object = createObject(imgLocate);
	locateObject(object, scene, coolX(boardX), coolY(boardY));
	if (shown) {
		showObject(object);
	}
	return object;
}

// s1 : 최대 길이 19, num : 0~9 숫자, s2 : 최대 길이 9
void strcat(char* s1, char num, char* s2) {

	char c = 0;
	char count = 0;     //무한루프방지용
	while (true) {
		count++;
		c = *s1;
		if (c == 0 || c == 0xCC || count >= 20) {
			break;
		}
		s1++;
	}
	*s1 = numToASCII(num);
	s1++;
	c = 0, count = 0;

	while (true) {
		c = *s2;
		if (c == 0 || c == 0xCC || count >= 10) {
			break;
		}
		*s1 = c;
		s1++;
		s2++;
	}
	*s1 = NULL;
}

ObjectID createObject(char objectType, SceneID scene, char boardX, char boardY, bool shown) {	
	char s1[20] = "Images/Object";
	char s2[10] = ".png";
	strcat(s1, objectType, s2);
	ObjectID object = createObject(s1, scene, boardX, boardY, shown);
	return object;
}

bool isFixedObject(char objectType) {
	if (objectType == SLAB || objectType == HOUSE || objectType == SPEAR || objectType == THRON || objectType == KEY || objectType == TREASURE)
		return true;
	return false;
}


void initStage(char stageNum) {
	
}


// ObjectType 이 있는곳으로 움직일 수 있는가?
bool canMove() {

	return false;

}

// ObjectID 를 반환하는게 아님. objectStruct[sceneNum][objectType][objectNum] 에서 마지막값을 찾기위한 함수.
char findObjectNum(char sceneNum, char objectType, char x, char y) {
	for (int i = 0; i < 30; i++) {
		if (objectStruct[sceneNum][objectType][i].x == x &&
			objectStruct[sceneNum][objectType][i].y == y) {
			return i;
		}			
	}
	printf("at func findObjectNum, cant find num\n"); // 오류메세지
	return -1; 
}

void moveObject(char stageNum, char objectType, char fromX, char fromY, char toX, char toY) {
	if (objectType == BOX || objectType == ROCK) {
		char objNum = findObjectNum(stageNum, objectType, fromX, fromY);
		moveableObjBoard[stageNum][fromY][fromX] = 0;
		moveableObjBoard[stageNum][toY][toX] = objectType;		
		objectStruct[stageNum][objectType][objNum] = { toX, toY, objectType };
		locateObject(object[stageNum][objectType][objNum], sceneStage[stageNum], coolX(toX), coolY(toY));
	}
	else {
		printf("at func moveObject, tried to move unmovable object\n");	//오류메세지
	}
}

void movePlayer(char stageNum, char toX, char toY) {
	playerX[stageNum] = toX;
	playerY[stageNum] = toY;
	locateObject(player[stageNum], sceneStage[stageNum], coolX(toX), coolY(toY));
}

// 
void move(char keycode) {
	char code = keycode - 82;	// LEFT : 0, RIGHT : 1, UP : 2, DOWN : 3
	char mPlayerX = playerX[curStage] + dx[code];	// 이동할 곳
	char mPlayerY = playerY[curStage] + dy[code];
	char mvObjType = moveableObjBoard[curStage][mPlayerY][mPlayerX];
	char fxObjType = fixedObjBoard[curStage][mPlayerY][mPlayerX];
	
	
	// 가려는곳이 박스면
	if (mvObjType == BOX) {		
		// 그 다음칸이 박스가 움직일 수 있는 곳인지 확인하고
		char mBoxX = mPlayerX + dx[code];
		char mBoxY = mPlayerY + dy[code];
		char mvObjTypeB = moveableObjBoard[curStage][mBoxY][mBoxX];
		char fxObjTypeB = fixedObjBoard[curStage][mBoxY][mBoxX];
		// movableType이 이미 있는 곳이면 못감
		if (mvObjTypeB > 0) {			
			// TODO : 못움직임을 표현하는 뭔가가 있으면 좋겠음.
			// 예를들면 삐빅 소리가 나거나, 움직일 수 없습니다! 하고 알려주거나
			// noticeCantMove() ? 
			return;
		}
		// fixed중 벽이면 못감
		if (fxObjTypeB == WALL || fxObjTypeB == TREASURE) {			
			// noticeCantMove() ? 
			return;
		}
		//상자 움직임		
		moveObject(curStage, BOX, mPlayerX, mPlayerY, mBoxX, mBoxY);
	}
	else if (fxObjType == WALL) {
		return;
	}
	//TODO 열쇠없는 상태면 TREASURE도 못지나가게 해야함.

	movePlayer(curStage, mPlayerX, mPlayerY);
}



void keyboardCallback(KeyCode code, KeyState state)
{
	// 누르는 동작에만 반응. 떼는것엔 반응 X
	if (state == KeyState::KEYBOARD_PRESSED) {
		if (code >= 82 && code <= 85) {		// UP
			move(code);

		}
		else if (code == 75) {
			initStage(curStage);
		}
		else {
			printf("undefined keycode : %d\n", code);
		}
	}

}


// 메인함수
int main() {

	setKeyboardCallback(keyboardCallback);

	sceneStage[0] = createScene("소코반", "Images/Background.png");
	sceneStage[1] = createScene("소코반", "Images/Background.png");


	// n : 스테이지 번호,	j : y성분,	i : x성분
	for (char n = 0; n < 2; n++) {
		for (char j = 0; j < BOARD_Y_NUM; j++) {
			for (char i = 0; i < BOARD_X_NUM; i++) {
				char objectType = initBoard[n][j][i];
				
				if (objectType == EMPTY)	continue;
				// 벽과 바닥 만들기		
				if (objectType == WALL) {
					tile[n][j][i] = createObject("Images/Wall.png", sceneStage[n], i, j, true);

					fixedObjBoard[n][j][i] = objectType;					
				}
				else {
					tile[n][j][i] = createObject("Images/Tile.png", sceneStage[n], i, j, true);		

					if (isFixedObject(objectType)) {
						fixedObjBoard[n][j][i] = objectType;
					}
					else if (objectType == PLAYER) {
						playerX[n] = i; playerY[n] = j;
					}
					else{
						moveableObjBoard[n][j][i] = objectType;
					}
				}								
			}
		}
	}

	// 발판만들기 -> todo : fixedObject 만들기
	for (char n = 0; n < 2; n++) {
		char count = 0;
		for (char j = 0; j < BOARD_Y_NUM; j++) {
			for (char i = 0; i < BOARD_X_NUM; i++) {
				if (fixedObjBoard[n][j][i] == SLAB) {
					object[n][SLAB][count] = createObject(SLAB, sceneStage[n], i, j, true);
					objectStruct[n][SLAB][count] = { i, j, SLAB };
					count++;
				}
			}
		}
	}

	// 박스만들기 -> todo : movableObject 만들기
	for (char n = 0; n < 2; n++) {
		char count = 0;
		for (char j = 0; j < BOARD_Y_NUM; j++) {
			for (char i = 0; i < BOARD_X_NUM; i++) {
				if (moveableObjBoard[n][j][i] == BOX) {
					object[n][BOX][count] = createObject(BOX, sceneStage[n], i, j, true);
					objectStruct[n][BOX][count] = { i, j, BOX };
					count++;
				}
			}
		}
	}
	
	// player 만들기
	for (char n = 0; n < 2; n++) {
		player[n] = createObject("Images/Player.png", sceneStage[n], playerX[n], playerY[n], true);
	}

	curStage = 1;
	startGame(sceneStage[1]);
}

