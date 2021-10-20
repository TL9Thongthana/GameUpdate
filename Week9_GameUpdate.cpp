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

int O_data[4][2] = {
	{0, 0},{1, 0},
	{0, 1},{1, 1}
};

int T_data[4][2] = {
			{1, 0},
	{0, 1}, {1, 1}, {2, 1}
};

int Z_data[4][2] = {
	{0, 0}, {1, 0},
			{1, 1}, {2, 1}
};

int S_data[4][2] = {
			{1, 0}, {2, 0},
	{0, 1}, {1, 1}
};

int L_data[4][2] = {
	{0, 0},
	{0, 1}, {1, 1}, {2, 1},
};

int J_data[4][2] = {
			{1, 0},
			{1, 1},
	{0, 2}, {1, 2}
};

int I_data[4][2] = {
	{0, 0},
	{0, 1},
	{0, 2},
	{0, 3}
};

struct Piece {
	int x;
	int y;
}piece[4], cache[4];

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


void setPlayerPosition(int x, int y)
{
	Player.X = x;
	Player.Y = y;
}

/*void ShipColor(int c)
{
	for (int y = 0; y < screen_y; y++)
	{
		for (int x = 0; x < screen_x; x++)
		{
			for (int i = 0; i < 4; i++) {
				piece[i].x = T_data[i][0];
				piece[i].y = T_data[i][1];
			}
		}
	}
}*/

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
/*void init_block()
{
	for (int i = 0; i < scount; i++)
	{
		star[i].X = getRandomInt(0, screen_x);
		star[i].Y = getRandomInt(0, screen_y);
	}
}*/
/*void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i].X = rand() % screen_x;
			star[i].Y = 1;
		}
		else {
			star[i].Y++;
		}
	}
}*/
/*void fill_star_to_buffer()
{
	for (int y = 0; y < screen_y; y++)
	{
		for (int x = 0; x < screen_x; x++)
		{
			for (int i = 0; i < scount; i++)
			{
				if (star[i].X == x && star[i].Y == y)
				{
					consoleBuffer[x + screen_x * y].Char.AsciiChar = '*';
					consoleBuffer[x + screen_x * y].Attributes = 7;
				}
			}
		}
	}
}*/

/*bool shipCollideStar()
{
	for (int i = 0; i < scount; i++)
	{
		if (star[i].X >= ship.X - 2 && star[i].X <= ship.X + 2 && star[i].Y == ship.Y)
		{
			star[i].X = getRandomInt(0, screen_x);
			star[i].Y = getRandomInt(0, screen_y);

			return true;
		}
	}
	return false;
}*/

int main()
{
	for (int i = 0; i < 4; i++) {
		piece[i].x = Z_data[i][0];
		piece[i].y = Z_data[i][1];
	}
	int i, a = 7, hp = 10;

	srand(time(NULL));

	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	setcursor(0);
	setPlayerPosition(screen_x/2, screen_y/2);
	//ShipColor(7);
	//init_star();
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
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c')
					{
						a = getRandomInt(1, 255);
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						a = getRandomInt(1, 255);
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						if (posx > 1 && posx < screen_x - 2)
						{
							setPlayerPosition(posx, posy);
						}
					}
				}
			}
			delete[] eventBuffer;
		}

		if (hp <= 0)
		{
			play = false;
		}

		//bool isshipCollideStar = shipCollideStar();
		//if (isshipCollideStar) hp--;

		//star_fall();

		//fillPicebuffer();
		fillPieceDataToBuffer();
		//ShipColor(a);
		fill_buffer_to_console();

		Sleep(100);
	}
	clear_buffer();
	fill_buffer_to_console();

	return 0;

}