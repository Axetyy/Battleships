#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <windows.h>
#include <vector>
#include <queue>

struct Point {
	int x, y;
	bool operator<(const Point& other) const {
		if (x == other.x) return y < other.y;
		else return x < other.x;
	}
};

struct Ship {
	int type;           
	bool horizontal;
	bool placed;       
	COLORREF color;   
	Point position;   
	Point dragPos;
};

class GameLogic {
public:
std::vector<Ship> opponentShips;
	std::deque<Point> attackQueue;
	std::vector<Ship> ships;
	Point dragOffset;
	bool dragging;
	int selectedShipIndex;
	int opponentBoard[10][10];
	
	
	int roundCount;
	int playerMoved;
	int AILevel=5;
	int board[10][10];

	bool gameStarted;
	bool fastMode = false;
	bool activeState ;

	void initialize();
	void render(HDC hdc);
	void handleMouseDown(int x, int y, bool rightButton);
	void handleMouseMove(int x, int y);
	void handleMouseUp(int x, int y);
	void handleAttack(int x, int y);
	void startGame();
	void initializeOpponentShips();
	void genRandomShip(int type);
	void handleNextOpponentMove();
	void genAdjacentHit(int x, int y, int type, int AIlevel, int roundNumber);
	void gameLoop();

	int checkWin(bool& playerWon);

	bool allShipsPlaced();
	bool inBoard(int x, int y);
};

#endif
