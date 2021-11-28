#define _CRT_SECURE_NO_WARNINGS

// Config
#define CONSOLE_WIDTH  40
#define CONSOLE_HEIGHT 30
#define BOARD_WIDTH    10
#define BOARD_HEIGHT   20
#define GRAVITY        300

#define GAME_FONT_SIZE 30
#define GAME_FONT      L"Consolas"

#define PIECE_PLACED_ICON '#'
#define PIECE_ICON        'O'
#define GHOST_ICON        '/"'
#define BOARD_GRID_ICON   ' '
#define BOARD_BORDER_ICON '@'
#define ITEM_BOMB_ICON    'T'
#define ITEM_LINE_ICON    'I'

#define IS_CURSOR_VISIBLE false

// Const
#define GAME_MENU           0
#define GAME_MENU_HIGHSCORE 1
#define GAME_RUN            2
#define GAME_END            3
#define BOARD_X_OFFSET (CONSOLE_WIDTH/2)-(BOARD_WIDTH/2)
#define BOARD_Y_OFFSET (CONSOLE_HEIGHT/2)-(BOARD_HEIGHT/2)

int PIECE_DATA[7][4][2] = {
    {{0, 0},{1, 0},{0, 1},{1, 1}}, //O
    {{1, 0},{1, 1},{2, 1},{1, 2}}, //T
    {{0, 0},{1, 0},{1, 1},{2, 1}}, //Z
    {{1, 0},{2, 0},{0, 1},{1, 1}}, //S
    {{0, 0},{0, 1},{0, 2},{1, 2}}, //L
    {{1, 0},{1, 1},{0, 2},{1, 2}}, //J
    {{0, 0},{0, 1},{0, 2},{0, 3}}  //I
};

#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <string.h>

using namespace std;

struct Highscore {
    int score;
    string name;
    Highscore(string name, int score) {

        this->score = score;
        this->name = name;
    }
};

class Console {
private:

    HANDLE output_handle;
    HANDLE input_handle;
    DWORD num_events;
    DWORD num_events_read;
    DWORD mode;

    CHAR_INFO console_buffer[CONSOLE_WIDTH * CONSOLE_HEIGHT];
    INPUT_RECORD* input_buffer;
    COORD buffer_size;
    SMALL_RECT console_size;
    CONSOLE_FONT_INFOEX cfi;

    char input_ascii;
    WORD input_vk;

    void setConsole() {

        this->output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleWindowInfo(this->output_handle, true, &this->console_size);
        SetConsoleScreenBufferSize(this->output_handle, this->buffer_size);
    }
    void setMode() {

        this->input_handle = GetStdHandle(STD_INPUT_HANDLE);
        this->mode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

        SetConsoleMode(this->input_handle, this->mode);
    }
    void setFont() {

        this->cfi.cbSize = sizeof(this->cfi);
        this->cfi.nFont = 0;
        this->cfi.dwFontSize.X = 0;
        this->cfi.dwFontSize.Y = GAME_FONT_SIZE;
        this->cfi.FontFamily = FF_DONTCARE;
        this->cfi.FontWeight = FW_NORMAL;
        wcscpy(this->cfi.FaceName, GAME_FONT);
        SetCurrentConsoleFontEx(this->output_handle, FALSE, &this->cfi);
    }

public:

    Console() {

        this->buffer_size = { CONSOLE_WIDTH, CONSOLE_HEIGHT };
        this->console_size = { 0, 0, CONSOLE_WIDTH - 1, CONSOLE_HEIGHT - 1 };

        this->setConsole();
        this->setMode();
        this->setFont();
    }

    ~Console() {

        delete[] this->input_buffer;
    }

    void setCursor(bool is_visible) {

        this->output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursor;
        cursor.bVisible = is_visible;
        cursor.dwSize = 20;

        SetConsoleCursorInfo(this->output_handle, &cursor);
    }

    void getInput() {

        GetNumberOfConsoleInputEvents(this->input_handle, &this->num_events);

        this->input_ascii = -1;
        this->input_vk = -1;

        if (this->num_events != 0) {
            this->input_buffer = new INPUT_RECORD[this->num_events];
            ReadConsoleInput(this->input_handle, input_buffer, this->num_events, &this->num_events_read);
            for (DWORD i = 0; i < this->num_events_read; i++) {
                if (input_buffer[i].EventType == KEY_EVENT && input_buffer[i].Event.KeyEvent.bKeyDown == true) {

                    this->input_ascii = input_buffer[i].Event.KeyEvent.uChar.AsciiChar;
                    this->input_vk = input_buffer[i].Event.KeyEvent.wVirtualKeyCode;
                }
            }
            delete[] this->input_buffer;
        }
    }

    char getInputAscii() {

        return this->input_ascii;
    }
    WORD getInputVK() {

        return this->input_vk;
    }

    void clear() {

        for (int y = 0; y < CONSOLE_HEIGHT; y++) {
            for (int x = 0; x < CONSOLE_WIDTH; x++) {
                this->console_buffer[x + CONSOLE_WIDTH * y].Char.AsciiChar = ' ';
                this->console_buffer[x + CONSOLE_WIDTH * y].Attributes = 0;
            }
        }
    }
    void display() {

        COORD character_pos = { 0, 0 };
        WriteConsoleOutputA(this->output_handle, this->console_buffer, this->buffer_size, character_pos, &this->console_size);
    }

    void drawCharacter(int character_x, int character_y, char character, int color) {

        for (int y = 0; y < CONSOLE_HEIGHT; y++) {
            for (int x = 0; x < CONSOLE_WIDTH; x++) {

                if (x == character_x && y == character_y) {

                    this->console_buffer[x + CONSOLE_WIDTH * y].Char.AsciiChar = character;
                    this->console_buffer[x + CONSOLE_WIDTH * y].Attributes = color;
                }
            }
        }
    }

    void drawRegtangle(int x_1, int y_1, int x_2, int y_2, char character, int color) {

        for (int y = 0; y < CONSOLE_HEIGHT; y++) {
            for (int x = 0; x < CONSOLE_WIDTH; x++) {
                if (x >= x_1 && x <= x_2 && y >= y_1 && y <= y_2) {
                    this->console_buffer[x + CONSOLE_WIDTH * y].Char.AsciiChar = character;
                    this->console_buffer[x + CONSOLE_WIDTH * y].Attributes = color;
                }
            }
        }
    }

    void drawText(int text_x, int text_y, string text) {

        for (int y = 0; y < CONSOLE_HEIGHT; y++) {
            for (int x = 0; x < CONSOLE_WIDTH; x++) {

                if (x == text_x && y == text_y) {

                    for (int i = 0; i < text.size(); i++) {

                        this->console_buffer[x + i + CONSOLE_WIDTH * y].Char.AsciiChar = text.at(i);
                        this->console_buffer[x + i + CONSOLE_WIDTH * y].Attributes = 7;
                    }
                }
            }
        }
    }

    void drawTextColor(int text_x, int text_y, string text, int text_color) {

        for (int y = 0; y < CONSOLE_HEIGHT; y++) {
            for (int x = 0; x < CONSOLE_WIDTH; x++) {

                if (x == text_x && y == text_y) {

                    for (int i = 0; i < text.size(); i++) {

                        this->console_buffer[x + i + CONSOLE_WIDTH * y].Char.AsciiChar = text.at(i);
                        this->console_buffer[x + i + CONSOLE_WIDTH * y].Attributes = text_color;
                    }
                }
            }
        }
    }

};

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
};

class Tetris {
private:
    Console* console;
    Piece piece[4], cache[4], ghost[4], item, item_cache;
    int board[BOARD_HEIGHT][BOARD_WIDTH] = {};
    int score;
    int delay;
    int rate;
    bool is_end;
    bool is_item_mode;
    char item_type;
    bool is_can_use_item;
    int level;

public:
    Tetris(Console* console) {

        this->rate = GRAVITY;
        this->console = console;
        this->reset();
    }

    int reset() {

        int score_end = this->score;

        this->delay = 0;
        this->score = 0;
        this->is_end = false;
        this->is_item_mode = false;
        this->clearBoard();
        this->randomNewPiece();
        this->item.x = -1;
        this->item.y = -1;
        this->item.color = 2;

        this->level = 1;
        this->is_can_use_item = false;

        return score_end;
    }

    int deleteLine() {

        int line_clear_count = 0;
        int checkLine = BOARD_HEIGHT - 1;
        for (int i = BOARD_HEIGHT - 1; i > 0; i--) {

            int count = 0;
            for (int j = 0; j < BOARD_WIDTH; j++) {

                if (this->board[i][j]) count++;
                this->board[checkLine][j] = this->board[i][j];
            }
            if (count < BOARD_WIDTH) checkLine--;
            else line_clear_count++;
        }
        return line_clear_count;
    }

    void clearBoard() {
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                this->board[i][j] = 0;
            }
        }
    }

    void randomNewPiece() {
        int pattern = this->getRandomInt(0, 6);
        for (int i = 0; i < 4; i++) {
            piece[i].x = PIECE_DATA[pattern][i][0] + (BOARD_WIDTH / 2) - 1;
            piece[i].y = PIECE_DATA[pattern][i][1];
            piece[i].color = pattern + 1;
        }
    }

    int getRandomInt(int a, int b) {
        return rand() % (b + 1 - a) + a;
    }

    bool isCollidedItem() {

        if (this->item.x < 0 || this->item.x >= BOARD_WIDTH || this->item.y >= BOARD_HEIGHT || this->board[this->item.y][this->item.x]) return true;
        return false;
    }

    bool isCollided() {
        for (int j = 0; j < 4; j++) {
            if (this->piece[j].x < 0 || this->piece[j].x >= BOARD_WIDTH || this->piece[j].y >= BOARD_HEIGHT || this->board[this->piece[j].y][this->piece[j].x]) return true;
        }
        return false;
    }

    bool isDead() {
        if (this->is_item_mode) return false;
        for (int i = 0; i < 4; i++) if (this->board[this->piece[i].y][this->piece[i].x]) return true;
        return false;
    }

    void update() {

        if (this->console->getInputVK() == VK_DOWN && !this->is_item_mode) {
            this->delay = -1;
        }
        if (this->console->getInputVK() == VK_LEFT && !this->is_item_mode) {
            for (int j = 0; j < 4; j++) {
                this->cache[j] = this->piece[j];
                this->piece[j].x -= 1;
            }
        }
        if (this->console->getInputVK() == VK_RIGHT && !this->is_item_mode) {
            for (int j = 0; j < 4; j++) {
                this->cache[j] = this->piece[j];
                this->piece[j].x += 1;
            }
        }
        if (this->console->getInputVK() == VK_UP && !this->is_item_mode) {
            for (int j = 0; j < 4; j++) {
                this->cache[j] = this->piece[j];
                this->piece[j].Rotate(this->piece[1], 1);
            }
        }
        if (this->isCollided()) for (int j = 0; j < 4; j++) this->piece[j] = this->cache[j];

        if (this->console->getInputVK() == VK_LEFT && this->is_item_mode) {

            this->item_cache = this->item;
            this->item.x -= 1;

        }
        if (this->console->getInputVK() == VK_RIGHT && this->is_item_mode) {

            this->item_cache = this->item;
            this->item.x += 1;

        }
        if (this->isCollidedItem()) this->item = this->item_cache;

        if (this->console->getInputVK() == VK_SPACE && !this->is_item_mode) {

            while (!this->isCollided()) {
                for (int j = 0; j < 4; j++) {
                    this->cache[j] = this->piece[j];
                    this->piece[j].y++;
                }
            }
            for (int j = 0; j < 4; j++) this->piece[j] = this->cache[j];

            this->delay = 0;
        }

        if (this->delay < 0) {

            if (this->level >= 1 && this->level < 5) this->delay = 300;
            if (this->level >= 5 && this->level < 10) this->delay = 250;
            if (this->level >= 10 && this->level < 15) this->delay = 200;
            if (this->level >= 15 && this->level < 20) this->delay = 150;
            if (this->level >= 20) this->delay = 100;

            if (!this->is_item_mode) {
                for (int j = 0; j < 4; j++) {
                    this->cache[j] = this->piece[j];
                    this->piece[j].y++;
                }
            }
            else {

                this->delay = 100;
                this->item_cache = this->item;
                this->item.y++;
            }

            if (this->isCollided() && !this->is_item_mode) {

                for (int i = 0; i < 4; i++) this->board[this->cache[i].y][this->cache[i].x] = this->piece[i].color;

                int line_clear_count = this->deleteLine();

                if (line_clear_count == 1) {
                    this->score += 100;
                }
                if (line_clear_count == 2) {
                    this->score += 300;
                }
                if (line_clear_count == 3) {
                    this->score += 500;
                }
                if (line_clear_count == 4) {
                    this->score += 1000;
                }

                if (this->score > 500 * this->level) {

                    this->level++;
                    this->is_can_use_item = true;
                }

                if (this->is_can_use_item) { // เปลี่ยนเป้นสุ่มโอกาส
                    for (int i = -0; i < 4; i++) {
                        this->piece[i].x = -1;
                        this->piece[i].y = -1;
                    }

                    this->is_can_use_item = false;

                    // สุ่มเลือกไอเทม
                    if (this->level % 2 == 1) this->item_type = ITEM_BOMB_ICON; // ระเบิด
                    if (this->level % 2 == 0) this->item_type = ITEM_LINE_ICON;  // เคลียแถว

                    this->item.x = getRandomInt(0, BOARD_WIDTH - 1);
                    this->item.y = 0;
                    this->is_item_mode = true;
                }
                else {

                    this->randomNewPiece();
                }

            }

            if (this->isCollidedItem() && this->is_item_mode) {

                this->randomNewPiece();
                this->is_item_mode = false;

                if (this->item_type == ITEM_BOMB_ICON) {

                    for (int i = this->item.x - 1; i <= this->item.x + 1; i++) {

                        for (int j = this->item.y - 2; j <= this->item.y; j++) {

                            if (j < 0 || j >= BOARD_HEIGHT) continue;
                            if (i < 0 || i >= BOARD_WIDTH) continue;
                            this->board[j][i] = 0;
                        }
                    }
                    this->score += 100;
                }
                else if (this->item_type == ITEM_LINE_ICON) {

                    for (int i = 0; i < BOARD_WIDTH; i++) this->board[this->item_cache.y][i] = 7 + this->item.color;
                    this->score += 100;
                }


                this->deleteLine();
            }

        }
        else this->delay--;

    }
    void draw() {

        // Draw Level
        this->console->drawText((CONSOLE_WIDTH / 2) - 5, 3, "LEVEL : ");
        this->console->drawTextColor((CONSOLE_WIDTH / 2) + 3, 3, to_string(this->level), 11);

        // Draw Score
        this->console->drawText((CONSOLE_WIDTH / 2) - 5, 27, "SCORE : ");
        this->console->drawTextColor((CONSOLE_WIDTH / 2) + 3, 27, to_string(this->score), 11);

        // Draw Border
        this->console->drawRegtangle(BOARD_X_OFFSET - 1, BOARD_Y_OFFSET - 1, BOARD_X_OFFSET + BOARD_WIDTH, BOARD_Y_OFFSET + BOARD_HEIGHT, BOARD_BORDER_ICON, 7);

        // Draw Board
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (this->board[y][x] != 0) this->console->drawCharacter(x + BOARD_X_OFFSET, y + BOARD_Y_OFFSET, PIECE_PLACED_ICON, this->board[y][x] + 7);
                else this->console->drawCharacter(x + BOARD_X_OFFSET, y + BOARD_Y_OFFSET, BOARD_GRID_ICON, 7);
            }
        }

        // Draw Ghost
        for (int j = 0; j < 4; j++) this->cache[j] = this->piece[j];
        while (!this->isCollided()) for (int j = 0; j < 4; j++) this->piece[j].y++;
        for (int j = 0; j < 4; j++) this->ghost[j] = this->piece[j];
        for (int j = 0; j < 4; j++) this->piece[j] = this->cache[j];

        for (int i = 0; i < 4; i++) {
            if (this->is_item_mode) break;
            this->console->drawCharacter(this->ghost[i].x + BOARD_X_OFFSET, this->ghost[i].y + BOARD_Y_OFFSET - 1, GHOST_ICON, this->ghost[i].color + 7);
        }


        // Draw Piece
        for (int i = 0; i < 4; i++) {
            if (this->is_item_mode) break;
            this->console->drawCharacter(this->piece[i].x + BOARD_X_OFFSET, this->piece[i].y + BOARD_Y_OFFSET, PIECE_ICON, this->piece[i].color + 7);
        }

        // Draw Item
        if (this->is_item_mode) {

            this->console->drawCharacter(this->item.x + BOARD_X_OFFSET, this->item.y + BOARD_Y_OFFSET, this->item_type, 7 + this->item.color);
        }

        this->console->drawText((CONSOLE_WIDTH / 2) - 3, 0, "TETRIS");
    }
};

class Game {
private:

    Console* console;
    Tetris* tetris;

    vector <Highscore> highscore_list;
    string your_name;
    int end_score;

    bool is_quit;
    int game_state;

    int menu_select = 0;

    void gameMenuUpdate() {
        if (this->game_state != GAME_MENU) return;

        // ทำการเปลี่ยนเมนูจาก Input
        if (this->console->getInputVK() == VK_UP) {
            this->menu_select -= 1;
            if (this->menu_select < 0) this->menu_select = 2;
        }
        if (this->console->getInputVK() == VK_DOWN) {
            this->menu_select += 1;
            if (this->menu_select > 2) this->menu_select = 0;
        }
        if (this->console->getInputVK() == VK_RETURN) {
            switch (this->menu_select) {
            case 0:
                this->game_state = GAME_RUN;
                break;
            case 1:
                this->game_state = GAME_MENU_HIGHSCORE;
                break;
            case 2:
                this->is_quit = true;
                break;
            }
        }
    }
    void gameMenuDraw() {
        if (this->game_state != GAME_MENU) return;

        // วาดเมนู
        this->console->drawText((CONSOLE_WIDTH / 2) - 3, 0, "TETRIS");

        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 2, (CONSOLE_HEIGHT / 2) - 5, "PLAY", 10);
        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 5, (CONSOLE_HEIGHT / 2) - 3, "HIGH SCORE", 11);
        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 2, (CONSOLE_HEIGHT / 2) - 1, "QUIT", 12);

        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 7, (CONSOLE_HEIGHT / 2) - 5 + (this->menu_select * 2), ">", 15);
        this->console->drawTextColor((CONSOLE_WIDTH / 2) + 6, (CONSOLE_HEIGHT / 2) - 5 + (this->menu_select * 2), "<", 15);

        this->console->drawTextColor(4, (CONSOLE_HEIGHT - 5), "Made By Thongthana Laemkaothong", 15);
        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 6, (CONSOLE_HEIGHT - 4), "ID : 64010300", 15);
    }

    void gameMenuHighscoreUpdate() {
        if (this->game_state != GAME_MENU_HIGHSCORE) return;

        if (this->console->getInputVK() == VK_ESCAPE) {
            this->game_state = GAME_MENU;
            return;
        }
    }
    void gameMenuHighscoreDraw() {
        if (this->game_state != GAME_MENU_HIGHSCORE) return;

        // วาด Highscore
        this->console->drawText((CONSOLE_WIDTH / 2) - 5, 0, "HIGH SCORE");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 5, "1.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 6, "2.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 7, "3.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 8, "4.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 9, "5.");

        // วาด รายชื่อคะแนน
        this->printHighscore((CONSOLE_WIDTH / 2) - 8, 5, 5);
    }

    void gameRunUpdate() {
        if (this->game_state != GAME_RUN) return;

        if (this->console->getInputVK() == VK_ESCAPE) {

            this->game_state = GAME_MENU;
            return;
        }

        if (this->console->getInputAscii() == 'r') {

            this->tetris->reset();
            return;
        }

        if (this->tetris->isDead()) {

            this->end_score = this->tetris->reset();
            this->game_state = GAME_END;
            return;
        }

        this->tetris->update();
    }
    void gameRunDraw() {
        if (this->game_state != GAME_RUN) return;

        this->tetris->draw();
    }

    void gameEndUpdate() {
        if (this->game_state != GAME_END) return;

        // เขียน
        if ((this->console->getInputAscii() >= 'A' && this->console->getInputAscii() <= 'Z') ||
            (this->console->getInputAscii() >= 'a' && this->console->getInputAscii() <= 'z')) {
            char input_char = this->console->getInputAscii();
            if (this->your_name.size() < 10) this->your_name.push_back(input_char);
        }

        // ลบ
        if (this->console->getInputVK() == VK_BACK) {
            if (this->your_name.size() != 0) {
                this->your_name.erase(this->your_name.end() - 1);
            }
        }

        // บันทึก
        if (this->console->getInputVK() == VK_RETURN) {
            this->addHighscore(Highscore(this->your_name, this->end_score));
            this->your_name = "";
            this->end_score = 0;

            this->game_state = GAME_MENU;
        }
    }
    void gameEndDraw() {
        if (this->game_state != GAME_END) return;

        // วาด Highscore
        this->console->drawText((CONSOLE_WIDTH / 2) - 5, 0, "HIGH SCORE");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 3, "1.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 4, "2.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 5, "3.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 6, "4.");
        this->console->drawText((CONSOLE_WIDTH / 2) - 10, 7, "5.");
        this->printHighscore((CONSOLE_WIDTH / 2) - 8, 3, 5);

        // วาด Enter Name
        this->console->drawText((CONSOLE_WIDTH / 2) - 8, 9, "Your score : " + to_string(this->end_score));
        this->console->drawTextColor((CONSOLE_WIDTH / 2) + 5, 9, to_string(this->end_score), 11);
        this->console->drawText((CONSOLE_WIDTH / 2) - 8, 12, "Enter Your Name");

        this->console->drawTextColor((CONSOLE_WIDTH / 2) - 8, 14, your_name + "_", 15);
    }

public:

    Game() {

        this->console = new Console();
        this->console->setCursor(IS_CURSOR_VISIBLE);

        this->tetris = new Tetris(this->console);

        this->is_quit = false;
        this->game_state = GAME_MENU;

        this->menu_select = 0;
        this->your_name = "";
        this->loadData();
    }

    ~Game() {

        ofstream highscore_file("Highscore.txt");
        for (int i = 0; i < highscore_list.size(); i++) {

            highscore_file << highscore_list[i].name << endl;
            highscore_file << highscore_list[i].score << endl;
        }
        highscore_file.close();
        this->console->clear();
        this->console->display();

        delete this->tetris;
    }

    void printHighscore(int x, int y, int max_print) {

        for (int i = 0; i < this->highscore_list.size(); i++) {

            if (i >= max_print) break;
            string name_to_show = this->highscore_list[i].name;
            this->console->drawText(x, y + i, name_to_show);
        }
        for (int i = 0; i < this->highscore_list.size(); i++) {

            if (i >= max_print) break;
            string score_to_show = to_string(this->highscore_list[i].score);
            this->console->drawText(x + 12, y + i, score_to_show);
        }
    }

    void loadData() {

        string textRead;
        bool is_score = false;
        Highscore player_read("", 0);
        ifstream readHighscore("Highscore.txt");
        while (getline(readHighscore, textRead)) {

            // ถ้าตอนนี้กำลังอ่านค่า ชื่อ ให้บันทึกชื่อ
            if (!is_score) player_read.name = textRead;
            else {

                // แปลง String เป็น Int
                stringstream to_convert(textRead);
                int score_int;
                to_convert >> score_int;

                // บันทึกคะแนน
                player_read.score = score_int;

                // ยัดคะแนนลงฐานข้อมูล
                addHighscore(player_read);
            }

            is_score = !is_score; // False >> True
        }
    }

    void addHighscore(Highscore player) {

        for (int i = 0; i < this->highscore_list.size(); i++) {

            // ถ้าเจอชื่อซ้ำคะแนนซ้ำ ไม่ต้องบันทึก
            if (player.score == this->highscore_list[i].score && player.name == this->highscore_list[i].name) return;

            // ถ้าคะแนนเยอะกว่า คะแนนที่ตำแหน่ง i ให้ยัดชื่อลงก่อนหน้า i
            if (player.score > this->highscore_list[i].score) {
                this->highscore_list.insert(this->highscore_list.begin() + i, player);
                return;
            }
        }
        this->highscore_list.insert(this->highscore_list.end(), player);
    }

    bool isQuit() {

        return this->is_quit;
    }

    void update() {

        this->console->getInput();

        this->gameMenuUpdate();
        this->gameMenuHighscoreUpdate();
        this->gameRunUpdate();
        this->gameEndUpdate();
    }

    void draw() {

        this->console->clear();

        this->console->drawRegtangle(1, 1, CONSOLE_WIDTH - 2, CONSOLE_HEIGHT - 2, '#', 7);
        this->console->drawRegtangle(2, 2, CONSOLE_WIDTH - 3, CONSOLE_HEIGHT - 3, ' ', 7);

        this->gameMenuDraw();
        this->gameMenuHighscoreDraw();
        this->gameRunDraw();
        this->gameEndDraw();

        this->console->display();
    }
};

Game game;

int main() {

    srand(time(0));

    while (true) {

        game.update();
        game.draw();
        if (game.isQuit()) return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}