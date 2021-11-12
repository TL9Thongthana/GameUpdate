#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#define screen_x 40
#define screen_y 40
#define boardX 10
#define boardY 20

#define GAME_MENU 0
#define GAME_MENU_HIGHSCORE 1
#define GAME_PLAY 2
#define GAME_MENU_END 3


int game_state = GAME_MENU;

bool isEnd = false;

using namespace std;
string your_name;

struct Highscore {
	int score;
	string name;
	Highscore(string name, int score) {

		this->score = score;
		this->name = name;
	}
};

vector <Highscore> highscore_list;

int score = 0, Opiece = 0, Tpiece = 0, Zpiece = 0, Spiece = 0, Lpiece = 0, Jpiece = 0, Ipiece = 0;

HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
COORD Player = { 0,0 };
int board[boardY][boardX] = { 0 };

void addHighscore(Highscore player) {

	for (int i = 0; i < highscore_list.size(); i++) {

		// ¶éÒà¨Íª×èÍ«éÓ¤Ðá¹¹«éÓ äÁèµéÍ§ºÑ¹·Ö¡
		if (player.score == highscore_list[i].score && player.name == highscore_list[i].name) return;

		// ¶éÒ¤Ðá¹¹àÂÍÐ¡ÇèÒ ¤Ðá¹¹·ÕèµÓáË¹è§ i ãËéÂÑ´ª×èÍÅ§¡èÍ¹Ë¹éÒ i
		if (player.score > highscore_list[i].score) {
			highscore_list.insert(highscore_list.begin() + i, player);
			return;
		}
	}
	highscore_list.insert(highscore_list.end(), player);
}

void fillTextDataToBuffer(int xx, int yy, std::string text) {
	for (int y = 0; y < screen_y; y++) {
		for (int x = 0; x < screen_x; x++) {

			if (x == xx && y == yy) {
				for (int i = 0; i < text.size(); i++) {

					consoleBuffer[x + i + screen_x * y].Char.AsciiChar = text.at(i);
					consoleBuffer[x + i + screen_x * y].Attributes = 7;
				}
			}
		}
	}
}

void fillScoreDataToBuffer(int xx, int yy, std::string text) {
	for (int y = 0; y < screen_y; y++) {
		for (int x = 0; x < screen_x; x++) {

			if (x == xx && y == yy) {
				for (int i = 0; i < text.size(); i++) {

					consoleBuffer[x + i + screen_x * y].Char.AsciiChar = text.at(i);
					consoleBuffer[x + i + screen_x * y].Attributes = 7;
				}
			}
		}
	}
}

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
	{{0, 0},{1, 0},{0, 1},{1, 1}}, //O
	{{1, 0},{1, 1},{2, 1},{1, 2}}, //T
	{{0, 0},{1, 0},{1, 1},{2, 1}}, //Z
	{{1, 0},{2, 0},{0, 1},{1, 1}}, //S
	{{0, 0},{0, 1},{0, 2},{1, 2}}, //L
	{{1, 0},{1, 1},{0, 2},{1, 2}}, //J
	{{0, 0},{0, 1},{0, 2},{0, 3}}  //I
};

/*int O_data[4][2] = {
	{0, 0},{1, 0},
	{0, 1},{1, 1}
};*/

/*int T_data[4][2] = {
			{1, 0},
			{1, 1}, {2, 1},
			{1, 2}
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

class Piece {
public:
	int x;
	int y;
	int color;
	void Rotate(Piece center, int direction) {

		int x = this->y - center.y;
		int y = this->x - center.x;
		this->x = center.x + (x * -direction);
		this->y = center.y + (y * direction);
	}
}piece[4], cache[4];

bool isCollided() {
	bool is_collide = false;
	for (int j = 0; j < 4; j++) {
		if (piece[j].x < 0 || piece[j].x >= boardX || piece[j].y >= boardY - 1 || board[piece[j].y + 1][piece[j].x]) is_collide = true;
	}
	return is_collide;
}

bool isDead() {

	for (int i = 0; i < 4; i++) if (board[piece[i].y][piece[i].x]) return true;
	return false;
}

void fillPieceDataToBuffer() {
	for (int y = 0; y < boardY; y++) {
		for (int x = 0; x < boardX; x++) {
			for (int i = 0; i < 4; i++) {
				if (piece[i].x == x && piece[i].y == y) {
					consoleBuffer[x + (screen_x * y)].Char.AsciiChar = 'O';
					consoleBuffer[x + (screen_x * y)].Attributes = piece[i].color + 7;
				}
			}
		}
	}
}

void fillBoardDataToBuffer() {
	for (int y = 0; y < boardY; y++) {
		for (int x = 0; x < boardX; x++) {
			for (int i = 0; i < 4; i++) {
				if (board[y][x] != 0) {
					consoleBuffer[x + (screen_x * y)].Char.AsciiChar = '#';
					consoleBuffer[x + (screen_x * y)].Attributes = board[y][x] + 7;
				}
				else {
					consoleBuffer[x + (screen_x * y)].Char.AsciiChar = '.';
					consoleBuffer[x + (screen_x * y)].Attributes = 7;
				}
			}

			/*
			if(x == 0) {
				consoleBuffer[-1 + x + (screen_x * y)].Char.AsciiChar = '|';
				consoleBuffer[-1 + x + (screen_x * y)].Attributes = 7;
			}
			if(x == boardX - 1) {
				consoleBuffer[1 + x + (screen_x * y)].Char.AsciiChar = '|';
				consoleBuffer[1 + x + (screen_x * y)].Attributes = 7;
			}
			*/
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

void gameMenu();
void gameMenuHighscore();
void gamePlay();
void gamePlayEnd();

int maxDelay = 10;
int delay = 0;
bool play = true;
DWORD numEvents = 0;
DWORD numEventsRead = 0;
int menu_select = 0;

int main()
{
	/* ==================================
		âËÅ´¤Ðá¹¹¨Ò¡ä¿Åì Å§°Ò¹¢éÍÁÙÅ (ãÊèäÇéº¹ÊØ´ ¨Ò¡ main)
	   ================================== */

	string textRead;
	bool is_score = false;

	Highscore player_read("", 0);

	ifstream readHighscore("Highscore.txt");
	while (getline(readHighscore, textRead)) {

		// ¶éÒµÍ¹¹Õé¡ÓÅÑ§ÍèÒ¹¤èÒ ª×èÍ ãËéºÑ¹·Ö¡ª×èÍ
		if (!is_score) player_read.name = textRead;
		else {

			// á»Å§ String à»ç¹ Int
			stringstream to_convert(textRead);
			int score_int;
			to_convert >> score_int;

			// ºÑ¹·Ö¡¤Ðá¹¹
			player_read.score = score_int;

			// ÂÑ´¤Ðá¹¹Å§°Ò¹¢éÍÁÙÅ
			addHighscore(player_read);
		}

		is_score = !is_score; // False >> True
	}

	/* ==================================
		¡ÒÃºÑ¹·Ö¡¤Ðá¹¹ Å§°Ò¹¢éÍÁÙÅ
	   ================================== */

	int score = 50;
	string name = "Korn";

	// à»ÅÕèÂ¹¢¹Ò´ Font
	int newWidth = 20, newHeight = 20;
	CONSOLE_FONT_INFOEX fontStructure = { 0 };
	fontStructure.cbSize = sizeof(fontStructure);
	fontStructure.dwFontSize.X = newWidth;
	fontStructure.dwFontSize.Y = newHeight;
	wcscpy_s(fontStructure.FaceName, L"Arail");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetCurrentConsoleFontEx(hConsole, true, &fontStructure);
	//
	srand(time(NULL));

Start: //



	play = true;

	for (int i = 0; i < boardY; i++) {
		for (int j = 0; j < boardX; j++) {
			board[i][j] = 0;
		}
	}

	score = 0;

	int pattern = getRandomInt(0, 6);
	for (int i = 0; i < 4; i++) {
		piece[i].x = Piece_data[pattern][i][0] + (boardX / 2) - 1;
		piece[i].y = Piece_data[pattern][i][1];
		piece[i].color = pattern + 1;
	}
	int i, a = 7, hp = 10;

	setConsole(screen_x, screen_y);
	setMode();
	setcursor(0);
	while (play)
	{
		gameMenu();
		gameMenuHighscore();
		gamePlayEnd();
		gamePlay();
	}
	if (!isEnd) { goto Start; }

	clear_buffer();
	fill_buffer_to_console();

	/* ==================================
		¡ÒÃºÑ¹·Ö¡¤Ðá¹¹ Å§ä¿Åì
	   ================================== */

	ofstream highscore_file("Highscore.txt");
	for (int i = 0; i < highscore_list.size(); i++) {

		highscore_file << highscore_list[i].name << endl;
		highscore_file << highscore_list[i].score << endl;
	}
	highscore_file.close();

	return 0;

}

void gamePlayEnd() {

	if (game_state != GAME_MENU_END) return;

	clear_buffer();

	GetNumberOfConsoleInputEvents(rHnd, &numEvents);
	if (numEvents != 0) {
		INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
		ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
		for (DWORD i = 0; i < numEventsRead; ++i) {
			if (eventBuffer[i].EventType == KEY_EVENT &&
				eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					menu_select--;
					if (menu_select < 0) menu_select = 2;
				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK) {

					if (your_name.size() != 0) {
						your_name.erase(your_name.end() - 1);
					}
				}
				if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar >= 'a' && eventBuffer[i].Event.KeyEvent.uChar.AsciiChar <= 'z') ||
					eventBuffer[i].Event.KeyEvent.uChar.AsciiChar >= 'A' && eventBuffer[i].Event.KeyEvent.uChar.AsciiChar <= 'Z') {

					char input_char = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
					if (your_name.size() < 10) your_name.push_back(input_char);////µÃ§¹Õéä§¹Ð/////
				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
					game_state = GAME_MENU;
					play = false;

					addHighscore(Highscore(your_name, score));

					// ÃÕà«çµ¤èÒ
					your_name = "Your Name";
					score = 0;

				}
			}
		}
		delete[] eventBuffer;
	}

	std::string score_string = "Your Score is : " + std::to_string(score);
	fillTextDataToBuffer(0, 0, score_string);

	fillTextDataToBuffer(0, 2, your_name + "_");

	fill_buffer_to_console();
	// cout ä´é 
	// áÊ´§¼ÅÇèÒ ä´é¤Ðá¹¹à·èÒäËÃè
	// cin ª×èÍ getline

	fill_buffer_to_console();
}

void gameMenu() {

	if (game_state != GAME_MENU) return;

	clear_buffer();

	GetNumberOfConsoleInputEvents(rHnd, &numEvents);
	if (numEvents != 0) {
		INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
		ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
		for (DWORD i = 0; i < numEventsRead; ++i) {
			if (eventBuffer[i].EventType == KEY_EVENT &&
				eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					menu_select--;
					if (menu_select < 0) menu_select = 2;
				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
					menu_select++;
					if (menu_select > 2) menu_select = 0;
				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
					switch (menu_select) {

					case 0:
						game_state = GAME_PLAY;
						break;
					case 1:
						game_state = GAME_MENU_HIGHSCORE;
						break;
					case 2:
						play = false;
						isEnd = true;
						break;
					}
				}
			}
		}
		delete[] eventBuffer;
	}
	fillTextDataToBuffer(0, 0, "Please Select Menu");
	fillTextDataToBuffer(4, 2, "PLAY");
	fillTextDataToBuffer(4, 3, "HIGHSCORE");
	fillTextDataToBuffer(4, 4, "QUIT");

	fillTextDataToBuffer(0, 2 + menu_select, ">");
	fill_buffer_to_console();
}
void gameMenuHighscore() {

	if (game_state != GAME_MENU_HIGHSCORE) return;
	clear_buffer();

	GetNumberOfConsoleInputEvents(rHnd, &numEvents);
	if (numEvents != 0) {
		INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
		ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
		for (DWORD i = 0; i < numEventsRead; ++i) {
			if (eventBuffer[i].EventType == KEY_EVENT &&
				eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
					game_state = GAME_MENU;
					return;
				}
			}
		}
		delete[] eventBuffer;
	}
	fillTextDataToBuffer(0, 0, "HIGH SCORE");
	/* ============================
		áÊ´§¤èÒ¤Ðá¹¹
	   ============================ */

	for (int i = 0; i < highscore_list.size(); i++) {

		string name_to_show = highscore_list[i].name;

		fillTextDataToBuffer(0, 3 + i, name_to_show);
	}
	for (int i = 0; i < highscore_list.size(); i++) {

		string score_to_show = to_string(highscore_list[i].score);

		fillTextDataToBuffer(12, 3 + i, score_to_show);
	}



	fill_buffer_to_console();
}/////////////////////////////////////////////////////////////////////////////////////
void gamePlay() {

	if (game_state != GAME_PLAY) return;
	clear_buffer();
	maxDelay = 30;
	GetNumberOfConsoleInputEvents(rHnd, &numEvents);
	if (numEvents != 0) {
		INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
		ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
		for (DWORD i = 0; i < numEventsRead; ++i) {
			if (eventBuffer[i].EventType == KEY_EVENT &&
				eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
					game_state = GAME_MENU;
					return;
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
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP)
				{
					for (int j = 0; j < 4; j++) {
						cache[j] = piece[j];
						piece[j].Rotate(piece[1], 1);
					}
					if (isCollided()) for (int j = 0; j < 4; j++)piece[j] = cache[j];

				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
				{
					maxDelay = 5;
				}
				if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
				{
					maxDelay = 5;
					/*if (harddrop) {

						while (true) {

							for (int j = 0; j < 4; j++) {
								cache[j] = piece[j];
								piece[j].y++;
							}
							if ()
						}

						goto MakeNewPiece;
					}*/
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
					if (posx > 1 && posx < boardX - 2)
					{

					}
				}
			}
		}
		delete[] eventBuffer;
	}

	if (isDead()) {

		game_state = GAME_MENU_END;
		your_name = "Your Name";
		return;
	}

	if (delay < 0) {
		delay = maxDelay;
		for (int j = 0; j < 4; j++) {
			cache[j] = piece[j];
			piece[j].y++;
		}

		if (isCollided())
		{
		makeNewPiece:

			for (int y = 0; y < boardY; y++) {
				for (int x = 0; x < boardX; x++) {
					for (int i = 0; i < 4; i++) {
						if (piece[i].x == x && piece[i].y == y) {
							board[y][x] = piece[i].color;
						}
					}
				}
			}
			int pattern = getRandomInt(0, 6);
			for (int i = 0; i < 4; i++) {
				piece[i].x = Piece_data[pattern][i][0] + (boardX / 2) - 1;
				piece[i].y = Piece_data[pattern][i][1];
				piece[i].color = pattern + 1;
			}

			int line_clear_count = 0; // à¼×èÍÍÂÒ¡¹ÑºÇèÒà¤ÅÕÂÃìä»¡Õèá¶Ç
			int checkLine = boardY - 1; // àÅ×Í¡á¶Ç·Õè¨Ðàªç¤à¤ÅÕÂÃìá¶ÇáÃ¡ à»ç¹á¶ÇÅèÒ§ÊØ´
			for (int i = boardY - 1; i > 0; i--) { // ÅÙ»¨Ò¡àÃÔèÁ¨Ò¡á¶ÇáÃ¡ÅèÒ§ÊØ´

				int count = 0; // ÊÓËÃÑº¹ÑºÇèÒÁÑ¹àµçÁá¶ÇÁÑéÂ
				for (int j = 0; j < boardX; j++) { // ÅÙ»¨Ò¡ªèÍ§áÃ¡ã¹á¶Ç i

					if (board[i][j]) count++; // ¶éÒªèÍ§¹Ñé¹äÁèÇèÒ§ ÁÕ Piece ÍÂÙè ãËé¹Ñº¤èÒà¾ÔèÁ
					board[checkLine][j] = board[i][j]; // ¤×ÍÍÑ¹¹Õé¨ÐàÍÒ¤èÒá¶Ç·Õè i ÁÒãÊèá¶Ç·Õè checkLine
				}
				if (count < boardX) checkLine--; // ¶éÒá¶Ç¹Ñé¹ÁÑ¹äÁè¤Ãºá¶Ç ¡çãËéÅº¤èÒ»¡µÔ áµè¶éÒÁÑ¹¤Ãºá¶ÇÁÑ¹¨ÐäÁèÅº áÊ´§ÇèÒã¹¡ÒÃ loop ¤ÃÑé§¶Ñ´ä»ºÃÃ·Ñ´·Õè 10 ¨ÐàÍÒ¤èÒá¶Ç·Õè i ÁÒãÊèá¶Ç checkLine à´ÔÁ ·ÓãËéá¶ÇÁÑ¹Å´Å§ à¾ÃÒÐá¶Çà¡èÒâ´¹·Ñºä»¹Ñè¹àÍ§
				else line_clear_count++; // ¶éÒá¶Ç¹Ñé¹¤Ãºá¶Ç ¹ÑººÇ¡¤èÒÇèÒà¤ÅÕÂÃìä»¡Õèá¶Ç
			}
			// line_clear_count < ä´éà»ç¹á¶Ç·Õèà¤ÅÕÂÃìä´é¨Ò¡¡ÒÃÇÒ§ªÔé¹¹Ñé¹ æ äÇé¤Ó¹Ç¹¤Ðá¹¹

			if (line_clear_count == 1) {
				score += 100;
			}
			if (line_clear_count == 2) {
				score += 300;
			}
			if (line_clear_count == 3) {
				score += 500;
			}
			if (line_clear_count == 4) {
				score += 1000;
			}

			if (pattern == 0) {
				Opiece += 1;
			}
			if (pattern == 1) {
				Tpiece += 1;
			}
			if (pattern == 2) {
				Zpiece += 1;
			}
			if (pattern == 3) {
				Spiece += 1;
			}
			if (pattern == 4) {
				Lpiece += 1;
			}
			if (pattern == 5) {
				Jpiece += 1;
			}
			if (pattern == 6) {
				Ipiece += 1;
			}
		}

	}
	else delay--;

	std::string score_string = "Score : " + std::to_string(score);
	fillScoreDataToBuffer(30, 0, score_string);


	fillBoardDataToBuffer();
	fillPieceDataToBuffer();
	fill_buffer_to_console();

	Sleep(10);
}
