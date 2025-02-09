#include "GameLogic.h"
#include <sstream>
#include <string>
#include <set>
#include <tchar.h>
#include <cstring>   
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "resource.h"
#include <map>

#define TIMER_CLOSE 1 

INT_PTR CALLBACK AttackResultProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		SetDlgItemText(hDlg, IDC_STATIC_TEXT, (LPCTSTR)lParam);
		SetTimer(hDlg, TIMER_CLOSE, 1000, NULL);
		return TRUE;
	}
	case WM_TIMER: {
		if (wParam == TIMER_CLOSE) {
			EndDialog(hDlg, 0);
		}
		return TRUE;
	}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}

int boardOriginX = 500, boardOriginY = 50;
int bankX = 50, bankY = 200;
int cellSize = 40;
int gridSize = 10;

std::set<Point> neighbors;
std::map<Point, bool> vizited;
bool inBoard(int x, int y)
{
	return x >= 0 && x < 10 && y >= 0 && y < 10;
}
void GameLogic::initialize() {
	srand((unsigned)time(NULL));
	memset(board, 0, sizeof(board));
	memset(opponentBoard, 0, sizeof(opponentBoard));

	ships.clear();
	opponentShips.clear();
	int verticalSpacing = 70;
	const static int shipLens[5] = { 5, 4, 3, 3, 2 };

	for (int i = 0; i < 5; i++) {
		Ship ship;
		ship.type = shipLens[i];
		ship.horizontal = true;
		ship.placed = false;
		// Set ship color based on type (or index).
		switch (i) {
		case 4: ship.color = RGB(200, 200, 200); break; // Carrier
		case 3: ship.color = RGB(150, 150, 150); break;    // Battleship
		case 2: ship.color = RGB(100, 100, 100); break;    // Cruiser 
		case 1: ship.color = RGB(50, 50, 50); break;    // Submarine 
		case 0: ship.color = RGB(0, 0, 0); break;    // Destroyer
		}
		ship.dragPos.x = bankX;
		ship.dragPos.y = bankY;
		ships.push_back(ship);
		bankY += verticalSpacing;
	}
	selectedShipIndex = -1;
	dragging = false;
	gameStarted = false;
	roundCount = 0;
}
void GameLogic::render(HDC hdc) {
	if (activeState == 0)
		return;


	int playerBoardX = gameStarted ? 250 : boardOriginX;
	int attackBoardX = playerBoardX + 500;
	int boardY = boardOriginY;

	// Player board
	for (int i = 0; i <= gridSize; ++i) {
		MoveToEx(hdc, playerBoardX + i * cellSize, boardY, NULL);
		LineTo(hdc, playerBoardX + i * cellSize, boardY + gridSize * cellSize);
		MoveToEx(hdc, playerBoardX, boardY + i * cellSize, NULL);
		LineTo(hdc, playerBoardX + gridSize * cellSize, boardY + i * cellSize);
	}
	//On table
	for (const auto& ship : ships) {
		if (ship.placed) {
			int x = playerBoardX + ship.position.x * cellSize;
			int y = boardY + ship.position.y * cellSize;
			int width = ship.horizontal ? ship.type * cellSize : cellSize;
			int height = ship.horizontal ? cellSize : ship.type * cellSize;
			HBRUSH brush = CreateSolidBrush(ship.color);
			RECT rect = { x, y, x + width, y + height };
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);
		}
	}

	/// Bank
	for (int i = 0; i < ships.size(); i++) {
		if (!ships[i].placed) {
			int x = ships[i].dragPos.x;
			int y = ships[i].dragPos.y;
			int width = ships[i].horizontal ? ships[i].type * cellSize : cellSize;
			int height = ships[i].horizontal ? cellSize : ships[i].type * cellSize;
			HBRUSH brush = CreateSolidBrush(ships[i].color);
			RECT rect = { x, y, x + width, y + height };
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);
		}
	}

	if (gameStarted) {
		for (int i = 0; i <= gridSize; ++i) {
			MoveToEx(hdc, attackBoardX + i * cellSize, boardY, NULL);
			LineTo(hdc, attackBoardX + i * cellSize, boardY + gridSize * cellSize);
			MoveToEx(hdc, attackBoardX, boardY + i * cellSize, NULL);
			LineTo(hdc, attackBoardX + gridSize * cellSize, boardY + i * cellSize);
		}
		// Draw opponent ships.
		/*for (const auto& oppShip : opponentShips) {
			int x = attackBoardX + oppShip.position.x * cellSize;
			int y = boardY + oppShip.position.y * cellSize;
			int width = oppShip.horizontal ? oppShip.type * cellSize : cellSize;
			int height = oppShip.horizontal ? cellSize : oppShip.type * cellSize;
			HBRUSH brush = CreateSolidBrush(oppShip.color);
			RECT rect = { x, y, x + width, y + height };
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);
		}*/

		for (int row = 0; row < gridSize; row++) {
			for (int col = 0; col < gridSize; col++) {
				int cellX = playerBoardX + col * cellSize;
				int cellY = boardY + row * cellSize;
				if (board[row][col] == -1) {
					HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
					HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
					Ellipse(hdc, cellX + 10, cellY + 10, cellX + cellSize - 10, cellY + cellSize - 10);
					SelectObject(hdc, oldBrush);
					DeleteObject(redBrush);
				}
				else if (board[row][col] == -2) {
					HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
					HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);
					Ellipse(hdc, cellX + 10, cellY + 10, cellX + cellSize - 10, cellY + cellSize - 10);
					SelectObject(hdc, oldBrush);
					DeleteObject(blueBrush);
				}
			}
			for (int row = 0; row < gridSize; row++) {
				for (int col = 0; col < gridSize; col++) {
					int cellX = attackBoardX + col * cellSize;
					int cellY = boardY + row * cellSize;
					if (opponentBoard[row][col] == -1) {
						HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
						HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
						Ellipse(hdc, cellX + 10, cellY + 10, cellX + cellSize - 10, cellY + cellSize - 10);
						SelectObject(hdc, oldBrush);
						DeleteObject(redBrush);
					}
					else if (opponentBoard[row][col] == -2) {
						HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
						HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);
						Ellipse(hdc, cellX + 10, cellY + 10, cellX + cellSize - 10, cellY + cellSize - 10);
						SelectObject(hdc, oldBrush);
						DeleteObject(blueBrush);
					}
				}
			}
		}
			std::wstring roundText = _T("Round:") + std::to_wstring(roundCount);
			TextOut(hdc, attackBoardX-75, boardY + (gridSize-1)/2 * cellSize + 10, roundText.c_str(), (int)roundText.size());
	}

}
void GameLogic::genRandomShip(int type) {
	bool valid = false;
	while (!valid) {
		int x = rand() % 10;
		int y = rand() % 10;
		bool h = (rand() % 2) == 0;
		valid = true;

		if (h && x + type > 10) valid = false;
		if (!h && y + type > 10) valid = false;

		for (int i = 0; i < type; i++) {
			if (h && opponentBoard[y][x + i] != 0) valid = false;
			if (!h && opponentBoard[y + i][x] != 0) valid = false;
		}
		if (valid) {
			for (int i = 0; i < type; i++) {
				if (h)
					opponentBoard[y][x + i] = type;
				else
					opponentBoard[y + i][x] = type;
			}
			Ship opponentShip;
			opponentShip.type = type;
			opponentShip.color = RGB(rand() % 256, rand() % 256, rand() % 256);
			opponentShip.horizontal = h;
			opponentShip.placed = true;
			opponentShip.position.x = x;
			opponentShip.position.y = y;
			opponentShips.push_back(opponentShip);
		}
	}
}
void GameLogic::initializeOpponentShips() {
	const static int shipLens[5] = { 5, 4, 3, 3, 2 };
	for (int i = 0; i < 5; i++) {
		genRandomShip(shipLens[i]);
	}
}
void GameLogic::startGame() {
	if (!allShipsPlaced()) return;
	gameStarted = true;
	initializeOpponentShips();
	roundCount = 1;
	playerMoved = 0;
	gameLoop();
}
void GameLogic::handleAttack(int x, int y) {
	bool fast = GameLogic::fastMode;
	int attackBoardX = 750;
	int boardY = boardOriginY;
	int gridX = (x - attackBoardX) / cellSize;
	int gridY = (y - boardY) / cellSize;
	if (opponentBoard[gridY][gridX] < 0)
		return;
	if (gridX >= 0 && gridX < 10 && gridY >= 0 && gridY < 10) {
		if (opponentBoard[gridY][gridX] > 0) {
			opponentBoard[gridY][gridX] = -1;
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Hit!"));
			}
		}
		else if (opponentBoard[gridY][gridX] == 0) {
			opponentBoard[gridY][gridX] = -2;
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Miss!"));
			}
		}
		playerMoved = 1;
	}
	gameLoop();
}
bool GameLogic::allShipsPlaced() {
	for (const auto& ship : ships) {
		if (!ship.placed) return false;
	}
	return true;
}
int GameLogic::checkWin(bool& playerWon) {
	bool opponentShipsRemaining = false;
	bool playerShipsRemaining = false;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (opponentBoard[i][j] > 0) {
				opponentShipsRemaining = true;
			}
			if (board[i][j] > 0) {
				playerShipsRemaining = true;
			}
		}
	}
	if (opponentShipsRemaining == false)
	{
		playerWon = true;
		return 1;
	}
	if (playerShipsRemaining == false)
	{
		playerWon = false;
		return 2;
	}
	else
		return 0;
}
void GameLogic::gameLoop() {
	bool playerWon;
	bool fast = GameLogic::fastMode;
	if (playerMoved) {
		if(!fast) Sleep(600);
		handleNextOpponentMove();
		playerMoved = 0;
		if (checkWin(playerWon) != 0)
		{
			if (playerWon)
			{
				MessageBox(NULL, _T("You Win!"), _T("Game Over"), MB_OK);
				Sleep(3000);
				PostQuitMessage(0);
			}
			else
			{
				MessageBox(NULL, _T("You Lose!"), _T("Game Over"), MB_OK);
				Sleep(3000);
				PostQuitMessage(0);
			}
		}
	}
	else
	{
		return;
	}
}
bool GameLogic::inBoard(int x, int y)
{
	return x >= 0 && x < 10 && y >= 0 && y < 10;
}

Point genRandom(int board[10][10])
{
	Point next;
	do {
		next.x = rand() % 10;
		next.y = rand() % 10;
	} while (board[next.x][next.y] < 0);
	return next;
}
Point genRandomBound(Point origin, int bound)
{
	Point next;
	bound = rand() % bound + 1;
	do {
		int dx = (rand() % (2 * bound + 1)) - bound;
		int dy = (rand() % (2 * bound + 1)) - bound;
		next.x = origin.x + dx;
		next.y = origin.y + dy;
	} while ((next.x == origin.x && next.y == origin.y) || !inBoard(next.x, next.y));
	return next;
}
void clearQueue(std::deque<Point>& attackQueue, int board[10][10])
{
	while (!attackQueue.empty() && board[attackQueue.front().x][attackQueue.front().y] < 0)
	{
		attackQueue.pop_front();
	}
}
void GameLogic::genAdjacentHit(int x, int y, int type, int AIlevel, int roundNumber)
{
	const int di[] = { 0, 0, 1, -1 };
	const int dj[] = { -1, 1, 0, 0 };
	int xv = x, yv = y;
	for (int k = 0; k < 4; k++)
	{
		Point next = { xv + di[k], yv + dj[k] };
		if (inBoard(next.x, next.y))
		{
			if (board[next.x][next.y] >= 1 && !vizited[next])
			{
				vizited[next] = true;
				float Formula = AIlevel + (roundNumber / 10.0f);
				bool Opportunity = (Formula >= rand() % 20 + 1);
				if (Opportunity)
					attackQueue.push_back(next);

				genAdjacentHit(next.x, next.y, type, AIlevel, roundNumber);
			}
			if (board[next.x][next.y] != type)
			{
				float Formula = AIlevel + (roundNumber / 10.0f);
				bool Opportunity = (Formula >= rand() % 30 + 1);
				if (Opportunity)
					neighbors.insert(next);
			}
		}
	}
}

void GameLogic::handleNextOpponentMove() {
	roundCount++;
	clearQueue(attackQueue, board);
	neighbors.clear();

	Point current, next;
	if (attackQueue.empty())
	{
		current = genRandom(board);
		attackQueue.push_back(current);
	}
	else if (roundCount > 1)
	{
		current = attackQueue.front();
	}

	if (roundCount % 10 == 0 && AILevel < 20)
		AILevel++;

	bool Opportunity = (AILevel + (roundCount / 10)) >= (rand() % 20 + 1);
	bool fast = GameLogic::fastMode;

	int type = board[current.x][current.y];

	if (!Opportunity)
	{
		if (type > 0)
		{
			board[current.x][current.y] = -1;
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Opponent hit your ship!"));
			}
			next = genRandomBound(current, 3);
			attackQueue.push_back(next);
		}
		else
		{
			board[current.x][current.y] = -2; 
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Opponent Missed!"));
			}
			if (rand() % 6 == 0)
				next = genRandomBound(current, 3);
			else
				next = genRandom(board);
			attackQueue.push_back(next);
		}
	}
	else
	{
		if (type > 0)
		{
			board[current.x][current.y] = -1;
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Opponent hit your ship!"));
			}
			genAdjacentHit(current.x, current.y, type, AILevel, roundCount);
			if (!neighbors.empty())
			{
				for (auto x : neighbors)
				{
					if (board[x.x][x.y] > 0)
						attackQueue.push_back(x);
				}
			}
			else
			{
				if (rand() % 6 == 0)
					next = genRandom(board);
				else
					next = genRandomBound(current, 3);
				attackQueue.push_back(next);
			}
		}
		else
		{
			board[current.x][current.y] = -2;
			if (!fast)
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATTACK_RESULT),
					NULL, AttackResultProc, (LPARAM)_T("Opponent Missed!"));
			}
			if (rand() % 20 == 0)
				next = genRandom(board);
			else
				next = genRandomBound(current, 4);
			attackQueue.push_back(next);
		}
	}
	return;
}
void GameLogic::handleMouseDown(int x, int y, bool rightButton) {

	if (rightButton && dragging && selectedShipIndex != -1) {
		ships[selectedShipIndex].horizontal = !ships[selectedShipIndex].horizontal;
		return;
	}
	if (!rightButton) {
		for (int i = 0; i < ships.size(); i++) {
			if (!ships[i].placed) {
				int bx = ships[i].dragPos.x;
				int by = ships[i].dragPos.y;
				int bw = ships[i].horizontal ? ships[i].type * cellSize : cellSize;
				int bh = ships[i].horizontal ? cellSize : ships[i].type * cellSize;
				if (x >= bx && x <= bx + bw && y >= by && y <= by + bh) {
					selectedShipIndex = i;
					dragging = true;
					dragOffset.x = x - bx;
					dragOffset.y = y - by;
					return;
				}
			}
		}
	}
}
void GameLogic::handleMouseMove(int x, int y) {
	if (dragging && selectedShipIndex != -1) {
		ships[selectedShipIndex].dragPos.x = x - dragOffset.x;
		ships[selectedShipIndex].dragPos.y = y - dragOffset.y;
	}
}
void GameLogic::handleMouseUp(int x, int y) {
	if (dragging && selectedShipIndex != -1) {
		int playerBoardX = boardOriginX;
		int boardY = boardOriginY;
		int gridX = (x - playerBoardX) / cellSize;
		int gridY = (y - boardY) / cellSize;
		bool validPlacement = false;
		int shipLen = ships[selectedShipIndex].type;
		if (ships[selectedShipIndex].horizontal) {
			if (gridX >= 0 && gridX + shipLen <= 10 && gridY >= 0 && gridY < 10) {
				validPlacement = true;
				for (int i = 0; i < shipLen; i++) {
					if (board[gridY][gridX+i] != 0) {
						validPlacement = false;
						break;
					}
				}
			}
		}
		else {
			if (gridY >= 0 && gridY + shipLen <= 10 && gridX >= 0 && gridX < 10) {
				validPlacement = true;
				for (int i = 0; i < shipLen; i++) {
					if (board[gridY+i][gridX] != 0) {
						validPlacement = false;
						break;
					}
				}
			}
		}

		if (validPlacement) {
			ships[selectedShipIndex].position.x = gridX;
			ships[selectedShipIndex].position.y = gridY;
			ships[selectedShipIndex].placed = true;
			if (ships[selectedShipIndex].horizontal) {
				for (int i = 0; i < shipLen; i++) {
					board[gridY][gridX + i] = ships[selectedShipIndex].type;
				}
			}
			else {
				for (int i = 0; i < shipLen; i++) {
					board[gridY + i][gridX] = ships[selectedShipIndex].type;
				}
			}

		}
		dragging = false;
		selectedShipIndex = -1;

	}

}