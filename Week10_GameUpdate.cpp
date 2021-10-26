#include <stdio.h>
#include <windows.h>
#include <time.h>
#define screen_x 80
#define screen_y 40
#define scount 80

HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
COORD star[scount];
COORD Player = { 0,0 };
int board[screen_y][screen_x] = { 0 };

int getRandomInt(int a, int b)
{
	return rand() % (b + 1 - a) + a;
}

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}
void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

int Piece_data[7][4][2] = {
	{{0, 0},{1, 0},{0, 1},{1, 1}},
	{{1, 0},{0, 1},{1, 1},{2, 1}},
	{{0, 0},{1, 0},{1, 1},{2, 1}},
	{{1, 0},{2, 0},{0, 1},{1, 1}},
	{{0, 0},{0, 1},{0, 2},{1, 2}},
	{{1, 0},{1, 1},{0, 2},{1, 2}},
	{{0, 0},{0, 1},{0, 2},{0, 3}}
};

/*int O_data[4][2] = {
	{0, 0},{1, 0},
	{0, 1},{1, 1}
};*/

/*int T_data[4][2] = {
			{1, 0},
	{0, 1}, {1, 1}, {2, 1}
};*/

/*int Z_data[4][2] = {
	{0, 0}, {1, 0},
			{1, 1}, {2, 1}
};*/

/*int S_data[4][2] = {
			{1, 0}, {2, 0},
	{0, 1}, {1, 1}
};*/

/*int L_data[4][2] = {
	{0, 0},
	{0, 1},
	{0, 2}, {1, 2},
};*/

/*int J_data[4][2] = {
			{1, 0},
			{1, 1},
	{0, 2}, {1, 2}
};*/

/*int I_data[4][2] = {
	{0, 0},
	{0, 1},
	{0, 2},
	{0, 3}
};*/

struct Piece {
	int x;
	int y;
}piece[4], cache[4];

bool isCollided() {
	bool is_collide = false;
	for (int j = 0; j < 4; j++) {
		if (piece[j].x - 1 < 0 || piece[j].x + 1 >= screen_x || piece[j].y >= screen_y-1) is_collide = true;
	}
	return is_collide;
}

void fillPieceDataToBuffer(){
	for (int y = 0; y < screen_y; y++){
		for (int x = 0; x < screen_x; x++){
			for (int i = 0; i < 4; i++) {
				if (piece[i].x == x && piece[i].y == y) {
					consoleBuffer[x + screen_x * y].Char.AsciiChar = '#';
					consoleBuffer[x + screen_x * y].Attributes = 7;
				}
			}
		}
	}
}

void fillBoardDataToBuffer() {
	for (int y = 0; y < screen_y; y++) {
		for (int x = 0; x < screen_x; x++) {
			for (int i = 0; i < 4; i++) {
				if (board[y][x] != 0) {
					consoleBuffer[x + screen_x * y].Char.AsciiChar = '#';
					consoleBuffer[x + screen_x * y].Attributes = 7;
				}
			}
		}
	}
}

void clear_buffer()
{
	for (int y = 0; y < screen_y; y++)
	{
		for (int x = 0; x < screen_x; x++)
		{
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos, &windowSize);
}


int main()
{
	srand(time(NULL));
	int pattern = getRandomInt(0, 6);
	for (int i = 0; i < 4; i++) {
		piece[i].x = Piece_data[pattern][i][0]+screen_x/2;
		piece[i].y = Piece_data[pattern][i][1];
	}
	int i, a = 7, hp = 10;

	int delay = 0;

	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	setcursor(0);
	while (play)
	{
		clear_buffer();

		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT)
					{

						for (int j = 0; j < 4; j++) {
							cache[j] = piece[j];
							piece[j].x -= 1;
						}
						if (isCollided()) for (int j = 0; j < 4; j++)piece[j] = cache[j];

					}
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT)
					{

						for (int j = 0; j < 4; j++) {
							cache[j] = piece[j];
							piece[j].x += 1;
						}
						if (isCollided()) for (int j = 0; j < 4; j++)piece[j] = cache[j];
								
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						if (posx > 1 && posx < screen_x - 2)
						{
							
						}
					}
				}
			}
			delete[] eventBuffer;
		}

		if (delay < 0) {
			delay = 5;
			for (int j = 0; j < 4; j++) {
				cache[j] = piece[j];
				piece[j].y++;
			}
			if (isCollided())
			{
				for (int y = 0; y < screen_y; y++) {
					for (int x = 0; x < screen_x; x++) {
						for (int i = 0; i < 4; i++) {
							if (piece[i].x == x && piece[i].y == y) {
								board[y][x] = 1;
							}
						}
					}
				}
				int pattern = getRandomInt(0, 6);
				for (int i = 0; i < 4; i++) {
					piece[i].x = Piece_data[pattern][i][0] + screen_x / 2;
					piece[i].y = Piece_data[pattern][i][1];
				}
			}
		}
		else delay--;


		fillPieceDataToBuffer();
		fillBoardDataToBuffer();
		fill_buffer_to_console();

		Sleep(10);
	}
	clear_buffer();
	fill_buffer_to_console();

	return 0;

}