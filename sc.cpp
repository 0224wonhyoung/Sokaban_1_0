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
#define EXIT    9   // 탈출구           fixed       can't
#define PLAYER	10	// player				
#define FULL    12  //상자와 발판이 겹쳐져 있는 상황

#define BOARD_X_PIX	50	// 보드 한칸의 픽셀너비
#define BOARD_Y_PIX 50	// 보드 한칸의 픽셀높이
#define BOARD_X_NUM 10	// 보드 전체 가로 칸수
#define BOARD_Y_NUM 10	// 보드 전체 세로 칸수

#define TOTAL_STAGE_NUM		9	// 총 스테이지 갯수
#define TOTAL_OBJECT_TYPE	11	// 오브젝트 종류 갯수 +1


struct ObjectStruct { char x; char y; char type; };

SceneID sceneStage[TOTAL_STAGE_NUM];
SceneID selectStage;
ObjectID tile[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];	// 벽과 바닥(타일) 오브젝트.
ObjectID object[TOTAL_STAGE_NUM][TOTAL_OBJECT_TYPE][30];
ObjectStruct objectStruct[TOTAL_STAGE_NUM][TOTAL_OBJECT_TYPE][30];
ObjectID player[TOTAL_STAGE_NUM];
ObjectID stageButton[TOTAL_STAGE_NUM];
ObjectID backButton[TOTAL_STAGE_NUM];


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
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} },
		//STAGE 2 (이석주가 올린 첫번째 사진)
	{	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
		{-2,-1,-1,-1,-1,-1,-2,-2,-2,-2},//9
		{-2,-1, 0, 0, 0,-1,-1,-1,-1,-2},//8
		{-2,-1, 0, 0, 0,-1, 0, 0,-1,-2},//7
		{-2,-1,-1, 0, 0, 0, 0, 2,-1,-2},//6
		{-1,-1,-1, 0,-1,-1,-1, 2,-1,-2},//5
		{-1, 0, 1, 0,-1,-2,-1, 2,-1,-2},//4
		{-1, 0, 1, 1,-1,-2,-1,-1,-1,-2},//3
		{-1,10, 0, 0,-1,-2,-2,-2,-2,-2},//2
		{-1,-1,-1,-1,-1,-2,-2,-2,-2,-2} },//1
	//STAGE 3 (두번째 사진)
{   {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
	{-2,-1,-1,-1,-1,-1,-1,-1,-1,-2},//9
	{-2,-1, 0, 0, 0, 2, 0, 0,-1,-2},//8
	{-2,-1, 1, 0,12, 1, 1, 2,-1,-2},//7
	{-2,-1, 0,-1, 0, 2, 0,-1,-1,-2},//6
	{-2,-1, 2,-1,-1, 1, 0,-1,-2,-2},//5
	{-2,-1,-1,-1, 0, 1, 2,-1,-2,-2},//4
	{-2,-1, 2,10, 1, 0, 0,-1,-2,-2},//3
	{-2,-1,-1,-1, 0, 0, 0,-1,-2,-2},//2
	{-2,-2,-2,-1,-1,-1,-1,-1,-2,-2} },//1
	//STAGE 4(세번째 사진)
{   {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
	{-2,-2,-2,-2,-1,-1,-1,-2,-2,-2},//9
	{-2,-2,-2,-2,-1, 2,-1,-2,-2,-2},//8
	{-2,-1,-1,-1,-1, 1,-1,-2,-2,-2},//7
	{-2,-1, 2, 0, 1,10,-1,-1,-1,-2},//6
	{-2,-1,-1,-1, 1, 0, 1, 2,-1,-2},//5
	{-2,-2,-2,-1, 0,-1,-1,-1,-1,-2},//4
	{-2,-2,-2,-1, 2,-1,-2,-2,-2,-2},//3
	{-2,-2,-2,-1,-1,-1,-2,-2,-2,-2},//2
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2} },//1
	//STAGE 5(네번째 사진)
{   {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//9
	{-2,-1,-1,-1,-1,-1,-1,-1,-1,-2},//8
	{-1,-1, 2, 2,-1, 0, 0, 0,-1,-2},//7
	{-1, 0, 2, 2,-1, 0, 1, 0,-1,-1},//6
	{-1, 0,10, 0, 1, 0, 0, 1, 0,-1},//5
	{-1,-1, 1,-1,-1,-1, 0, 0, 0,-1},//4
	{-2,-1, 0, 0, 0, 0, 0,-1,-1,-1},//3
	{-2,-1,-1,-1,-1,-1,-1,-1,-2,-2},//2
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2} },//1
	//STAGE 6(다섯번째 사진)
{   {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},//9
	{-1, 2, 2, 2, 2, 2, 0, 0,-1,-2},//8
	{-1,-1,-1, 0, 1, 0,-1, 0,-1,-1},//7
	{-2,-2,-1, 0, 1,-1,-1, 0, 0,-1},//6
	{-2,-2,-1, 1, 0, 1, 0, 1, 0,-1},//5
	{-2,-2,-1, 0, 0, 0,-1, 0, 0,-1},//4
	{-2,-2,-1,-1, 0, 0,-1, 0,10,-1},//3
	{-2,-2,-2,-1,-1,-1,-1,-1,-1,-1},//2
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2} },//1
	//STAGE 7(여섯번째 사진)
{   {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//10
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},//9
	{-2,-2,-2,-2,-1,-1,-1,-1,-2,-2},//8
	{-2,-1,-1,-1,-1, 0, 0,-1,-2,-2},//7
	{-2,-1, 0, 1, 2, 2, 2,-1,-1,-2},//6
	{-2,-1,10, 0, 1, 2, 2, 0,-1,-2},//5
	{-2,-1,-1,-1, 1, 1, 1, 0,-1,-2},//4
	{-2,-2,-2,-1, 0, 0, 0, 0,-1,-2},//3
	{-2,-2,-2,-1,-1,-1,-1,-1,-1,-2},//2
	{-2,-2,-2,-2,-2,-2,-2,-2,-2,-2} },//1
	//STAGE8 (일곱번째 사진) 이건 가로가 10보다 커서 일단 줄였어요
{   {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},//10
	{-1, 0,-1,-1,-1,-1,-1,-1, 0,-1},//9
	{-1, 1,-1,-1,-1,-1,-1,-1, 0,-1},//8
	{-1, 0, 0, 0, 0, 0, 0, 0, 0,-1},//7
	{-1, 0,-1,-1,-1,-1,-1,-1, 0, 0},//6
	{ 0, 2,-1,-2,-2,-1, 0, 1, 0,-1},//5
	{ 0, 1,-1,-1,-1,-1, 2,-1, 0,-1},//4
	{ 2,10, 0, 0,12, 0, 0, 1, 0,-1},//3
	{ 2, 3, 1, 0, 0,-1, 0, 0, 0,-1},//2
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} }//1
};


char curStage;
char playerX[TOTAL_STAGE_NUM], playerY[TOTAL_STAGE_NUM];	// player의 X, Y 좌표
char dx[4] = { -1, 1, 0, 0 };	// LEFT RIGHT UP DOWN
char dy[4] = { 0, 0, 1, -1 };

short buttonX[10] = { 160, 360, 560, 760, 960, 160, 360, 560, 760, 960 };     //stage 버튼 x좌표
short buttonY[10] = { 410, 410, 410, 410, 410, 310, 310, 310, 310, 310 };     //stage 버튼 y좌표

char fixedObjBoard[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];
char moveableObjBoard[TOTAL_STAGE_NUM][BOARD_Y_NUM][BOARD_X_NUM];
char numOfObject[TOTAL_STAGE_NUM][TOTAL_OBJECT_TYPE];

bool playing = false;

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


ObjectID createObject(const char* image, SceneID scene, short x, short y, bool shown) {
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);
	if (shown) {
		showObject(object);
	}
	return object;
}

ObjectID createObject(const char* imgLocate, SceneID scene, char boardX, char boardY, bool shown) {

	ObjectID object = createObject(imgLocate, scene, coolX(boardX), coolY(boardY), true);	
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
	if (objectType == SLAB || objectType == HOUSE || objectType == SPEAR || objectType == THRON || objectType == KEY || objectType == TREASURE || objectType == EXIT || objectType == FULL)
		return true;
	return false;
}

bool isMovableObject(char objectType) {
	if (objectType == BOX || objectType == ROCK || objectType == FULL) {
		return true;
	}
	return false;
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

bool isStageClear(char stageNum) {
	for (char i = 0; i < numOfObject[stageNum][SLAB]; i++) {
		ObjectStruct objStr = objectStruct[stageNum][SLAB][i];
		if (moveableObjBoard[stageNum][objStr.y][objStr.x] != BOX) {
			return false;
		}
	}
	return true;
}

void initStage(char stageNum) {
	playing = false;
	char count[TOTAL_OBJECT_TYPE];
	for (char i = 0; i < TOTAL_OBJECT_TYPE; i++) {
		count[i] = 0;
	}
	printf("stage %d\n", stageNum);
	// initPlayer()
	for (char j = 0; j < BOARD_Y_NUM; j++) {
		for (char i = 0; i < BOARD_X_NUM; i++) {
			char objectType = initBoard[stageNum][j][i];
			if (objectType == PLAYER) {
				playerX[stageNum] = i; playerY[stageNum] = j;
			}
		}
	}
	movePlayer(stageNum, playerX[stageNum], playerY[stageNum]);
	
	// init movableObjBoard to 0
	for (char j = 0; j < BOARD_Y_NUM; j++) {
		for (char i = 0; i < BOARD_X_NUM; i++) {
			moveableObjBoard[stageNum][i][j] = 0;
		}
	}

	// init movableObj
	for (char j = 0; j < BOARD_Y_NUM; j++) {
		for (char i = 0; i < BOARD_X_NUM; i++) {
			char objectType = initBoard[stageNum][j][i];
			if (objectType == FULL) { objectType = BOX; }
			if (isMovableObject(objectType)) {
				printf("%d %d has movable\n", i, j);
				moveableObjBoard[stageNum][j][i] = objectType;
				objectStruct[stageNum][objectType][count[objectType]] = { i,j,objectType };
				locateObject(object[stageNum][objectType][count[objectType]], sceneStage[stageNum], coolX(i), coolY(j));
								
				count[objectType]++;
			}
		}
	}
	printf("Movable Obj Board\n");
	for (char j = BOARD_Y_NUM - 1; j >= 0; j--) {
		for (char i = 0; i < BOARD_X_NUM; i++) {
			printf("%d ", moveableObjBoard[3][j][i]);
		}
		printf("\n");
	}

	playing = true;
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

	if (isStageClear(curStage)) {
		playing = false;
		curStage++;
		if (curStage < TOTAL_STAGE_NUM)
		{
			enterScene(sceneStage[curStage]);
			playing = true;
		}
		else if (curStage == TOTAL_STAGE_NUM) {
			//TODO : 모든 스테이지 클리어
			endGame();
		}
	}
}


void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	for (int i = 0; i < TOTAL_STAGE_NUM; i++) {
		if (object == stageButton[i]) {
			curStage = i;
			enterScene(sceneStage[i]);
			playing = true;
		}
		else if (object == backButton[i]) {
			playing = false;
			initStage(curStage);
			enterScene(selectStage);			
		}
		
	}
}

void keyboardCallback(KeyCode code, KeyState state)
{
	// 누르는 동작에만 반응. 떼는것엔 반응 X
	if (state == KeyState::KEYBOARD_PRESSED && playing && 0<=curStage && curStage<TOTAL_STAGE_NUM) {
		if (code >= 82 && code <= 85) {		// 방향키
			move(code);

		}
		else if (code == 75 || code == 18) {	// SPACEBAR or 'R'
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
	setMouseCallback(mouseCallback);

	selectStage = createScene("스테이지 선택", "Images/Background.png");
	stageButton[0] = createObject("Images/Button1.png", selectStage, (short)buttonX[0], (short)buttonY[0], true);
	for (char n = 1; n < TOTAL_STAGE_NUM; n++) {
		char s1[20] = "Images/Button";
		char s2[10] = ".png";
		strcat(s1, n, s2);		
		stageButton[n] = createObject(s1, selectStage, (short)buttonX[n], (short)buttonY[n], true);				
	}



	for (char n = 0; n < TOTAL_STAGE_NUM; n++) {
		sceneStage[n] = createScene("소코반", "Images/Background.png");
		backButton[n] = createObject("Images/Back.png", sceneStage[n], (short)100, (short)610, true);
	}


	// n : 스테이지 번호,	j : y성분,	i : x성분
	for (char n = 0; n < TOTAL_STAGE_NUM; n++) {
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

					if (objectType == FULL) {
						fixedObjBoard[n][j][i] = SLAB;
						moveableObjBoard[n][j][i] = BOX;
					}
					else if (isFixedObject(objectType)) {
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
	printf("Fixed Obj Board\n");
	for (char j = BOARD_Y_NUM-1; j >=0 ; j--) {
		for (char i = 0; i < BOARD_X_NUM; i++) {
			printf("%d ", fixedObjBoard[3][j][i]);
		}
		printf("\n");
	}
	printf("Movable Obj Board\n");
	for (char j = BOARD_Y_NUM - 1; j >= 0; j--) {
		for (char i = 0; i < BOARD_X_NUM; i++) {			
			printf("%d ", moveableObjBoard[3][j][i]);
		}
		printf("\n");
	}

	

	// 오브젝트만들기
	for (char n = 0; n < TOTAL_STAGE_NUM; n++) {
		for (char j = 0; j < BOARD_Y_NUM; j++) {
			for (char i = 0; i < BOARD_X_NUM; i++) {
				char fxObjType = fixedObjBoard[n][j][i];				
				if (isFixedObject(fxObjType)) {
					object[n][fxObjType][numOfObject[n][fxObjType]] = createObject(fxObjType, sceneStage[n], i, j, true);
					objectStruct[n][fxObjType][numOfObject[n][fxObjType]] = { i, j, fxObjType};
					numOfObject[n][fxObjType] += 1;
				}								
			}
		}
	}
	for (char n = 0; n < TOTAL_STAGE_NUM; n++) {
		for (char j = 0; j < BOARD_Y_NUM; j++) {
			for (char i = 0; i < BOARD_X_NUM; i++) {				
				char mvObjType = moveableObjBoard[n][j][i];
				if (isMovableObject(mvObjType)) {
					object[n][mvObjType][numOfObject[n][mvObjType]] = createObject(mvObjType, sceneStage[n], i, j, true);
					objectStruct[n][mvObjType][numOfObject[n][mvObjType]] = { i, j, mvObjType };
					numOfObject[n][mvObjType] += 1;
				}
			}
		}
	}


	
	
	// player 만들기
	for (char n = 0; n < TOTAL_STAGE_NUM; n++) {
		player[n] = createObject("Images/Player.png", sceneStage[n], playerX[n], playerY[n], true);
	}



	playing = false;
	curStage = 0;
	startGame(selectStage);
}

