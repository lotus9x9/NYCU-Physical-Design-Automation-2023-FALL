#include "raylib.h"
#include <vector>
#include <ctime>
#include <string>
using namespace std;

const int screenwidth = 600;
const int screenheight = 600;
const int gridsize = 30;
const int boardwidth = 10;
const int boardheight = 20;

int score;
int totalline;
double lastime = 0;
vector<vector<char>> board(boardheight, vector<char>(boardwidth, ' '));

struct block {
    vector<vector<char>> shape;
    Color color;
    int x;
    int y;
};

block currentblock;
block nextblock;

Color blockcolors[] = { RED, GREEN, BLUE, ORANGE, PURPLE, YELLOW, BROWN };

bool CheckCollision(const block& block, int xOffset, int yOffset) { // ??d?I??
    for (int i = 0; i < block.shape.size(); i++) {
        for (int j = 0; j < block.shape[i].size(); j++) {
            if (block.shape[i][j] != ' ') {
                int boardX = block.x + j + xOffset;
                int boardY = block.y + i + yOffset;

                if (boardX < 0 || boardX >= boardwidth || boardY >= boardheight) { // ?W?X???
                    return true;
                }

                if (boardY >= 0 && board[boardY][boardX] != ' ') { // ?w?????
                    return true;
                }
            }
        }
    }
    return false;
}

void Placeblock(const block& block) { // ??m???
    for (int i = 0; i < block.shape.size(); i++) {
        for (int j = 0; j < block.shape[i].size(); j++) {
            if (block.shape[i][j] != ' ') {
                int boardX = block.x + j;
                int boardY = block.y + i;

                if (boardY >= 0) {
                    board[boardY][boardX] = block.shape[i][j];
                }
            }
        }
    }

    int linesCleared = 0;
    for (int i = boardheight - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < boardwidth; j++) {
            if (board[i][j] == ' ') {
                fullLine = false;
                break;
            }
        }

        if (fullLine) { // ????????
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < boardwidth; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            linesCleared++;
            i++;
        }
    }
    if (linesCleared == 1) { // ?p??
        score += 50;
        totalline += 1;
    }
    else if (linesCleared == 2) {
        score += 150;
        totalline += 2;
    }
    else if (linesCleared == 3) {
        score += 300;
        totalline += 3;
    }
    else if (linesCleared == 4) {
        score += 500;
        totalline += 4;
    }
}

void Rotateblock(block& block) { // ???????
    int size = block.shape.size();
    vector<vector<char>> rotatedShape(size, vector<char>(size, ' '));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            rotatedShape[i][j] = block.shape[size - 1 - j][i];
        }
    }
    block.shape = rotatedShape;
}

void DrawBoard() { // ?e?X?C?????
    DrawRectangle(0, 0, boardwidth * gridsize, boardheight * gridsize, BLACK);
    DrawRectangleLines(0, 0, boardwidth * gridsize, boardheight * gridsize, WHITE);

    for (int i = 0; i < boardheight; i++) {
        for (int j = 0; j < boardwidth; j++) {
            if (board[i][j] != ' ') {
                DrawRectangle(j * gridsize, i * gridsize, gridsize, gridsize, blockcolors[board[i][j] - 'A']);
                DrawRectangleLines(j * gridsize, i * gridsize, gridsize, gridsize, BLACK);
            }
        }
    }
}

void Drawblock(const block& block) { // ?e?X???
    for (int i = 0; i < block.shape.size(); i++) {
        for (int j = 0; j < block.shape[i].size(); j++) {
            if (block.shape[i][j] != ' ') {
                DrawRectangle((block.x + j) * gridsize, (block.y + i) * gridsize, gridsize, gridsize, block.color);
                DrawRectangleLines((block.x + j) * gridsize, (block.y + i) * gridsize, gridsize, gridsize, BLACK);
            }
        }
    }
}
int offsetX = (boardwidth + 2) * gridsize;

void DrawNextblock(const block& block) { // ?e?X?U?@????

    for (int i = 0; i < block.shape.size(); i++) {
        for (int j = 0; j < block.shape[i].size(); j++) {
            if (block.shape[i][j] != ' ') {
                DrawRectangle((j + (boardwidth + 2)) * gridsize, (i + 4) * gridsize, gridsize, gridsize, block.color);
                DrawRectangleLines((j + (boardwidth + 2)) * gridsize, (i + 4) * gridsize, gridsize, gridsize, BLACK);
            }
        }
    }
    // ????r
    DrawText("Next block", offsetX, gridsize * 2, 20, BLACK);
    DrawText(("Score: " + to_string(score)).c_str(), offsetX, gridsize * 17, 20, BLACK);
    DrawText(("Line: " + to_string(totalline)).c_str(), offsetX, gridsize * 18, 20, BLACK);
}

block GetRandomblock() {
    block block;

    // ?w?q???
    vector<vector<char>> shapes[7] = {
        {
            {' ', ' ', ' ', ' '},
            {'A', 'A', 'A', 'A'},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {'B', ' ', ' ', ' '},
            {'B', 'B', 'B', ' '},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {' ', ' ', 'C', ' '},
            {'C', 'C', 'C', ' '},
            {' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {' ', ' ', ' ', ' '},
            {' ', 'D', 'D', ' '},
            {' ', 'D', 'D', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {' ', ' ', ' ', ' '},
            {' ', 'E', 'E', ' '},
            {'E', 'E', ' ', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {' ', ' ', ' ', ' '},
            {' ', 'F', ' ', ' '},
            {'F', 'F', 'F', ' '},
            {' ', ' ', ' ', ' '}
        },
        {
            {' ', ' ', ' ', ' '},
            {'G', 'G', ' ', ' '},
            {' ', 'G', 'G', ' '},
            {' ', ' ', ' ', ' '}
        }
    };

    int index = GetRandomValue(0, 6);
    block.shape = shapes[index];
    block.color = blockcolors[index];
    block.x = boardwidth / 2 - block.shape[0].size() / 2;
    block.y = 0;

    return block;
}

bool checktime(double sec) { // ???????
    double currentime = GetTime();
    if (currentime - lastime >= sec) {
        lastime = currentime;
        return true;
    }
    return false;
}
bool isbottom = false;
bool ispause = false;

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

float speed = 0, nowspeed = 0;
int mode = 0;

int main() {
    InitWindow(screenwidth, screenheight, "Tetris");
    nextblock = GetRandomblock();
    GameState gameState = MENU;

    // ?C???D?j??
    while (!WindowShouldClose()) {
        while (gameState == MENU) { // ?}?l?e??
            SetTargetFPS(60);
            score = 0;
            totalline = 0;
            BeginDrawing();
            ClearBackground(GREEN);
            DrawText("Tetris", 220, gridsize * 2, 50, BLACK);
            DrawText("Choose the difficulty you want", 20, gridsize * 6, 20, BLACK);
            DrawText("Press 1,2,3,4,5 to select the difficulty (1 is the easiest)", 20, gridsize * 8, 20, BLACK);
            DrawText("Press ENTER to start the game", 200, gridsize * 18, 20, BLACK);

            if (IsKeyPressed(KEY_KP_1)) {
                speed = 0.25;
                mode = 1;
            }
            if (IsKeyPressed(KEY_KP_2)) {
                speed = 0.225;
                mode = 2;
            }
            if (IsKeyPressed(KEY_KP_3)) {
                speed = 0.2;
                mode = 3;
            }
            if (IsKeyPressed(KEY_KP_4)) {
                speed = 0.175;
                mode = 4;
            }
            if (IsKeyPressed(KEY_KP_5)) {
                speed = 0.15;
                mode = 5;
            }


            if (speed != 0) {
                DrawText(("The difficulty is: " + to_string(mode)).c_str(), 20, gridsize * 10, 20, BLACK);
                if (IsKeyPressed(KEY_ENTER)) {
                    gameState = PLAYING;
                }
            }

            EndDrawing();
        }
        while (gameState == PLAYING) { // ?C???i??
            DrawText("Press ASD to move the block", offsetX, gridsize * 8, 15, BLACK);
            DrawText("Press R to rotate the block", offsetX, gridsize * 9, 15, BLACK);
            DrawText("Press B to move to the bottom", offsetX, gridsize * 10, 15, BLACK);
            DrawText("Press C to change the block", offsetX, gridsize * 11, 15, BLACK);
            DrawText("Press Q to quit the game", offsetX, gridsize * 12, 15, BLACK);
            DrawText("Press P to pause", offsetX, gridsize * 13, 15, BLACK);

            if (!ispause) {
                SetTargetFPS(60);

                if (currentblock.shape.empty()) { // ??d?O?_??n????s???
                    currentblock = nextblock;
                    nextblock = GetRandomblock();

                    if (CheckCollision(currentblock, 0, 0)) { // ??d?s????O?_?M?w??????I??
                        gameState = GAME_OVER;
                    }
                }

                nowspeed = speed - (score / 1000) / 100.0; // ?H???Z?W?[?C???t??

                if (checktime(nowspeed) && !isbottom) { // ????U??
                    if (!CheckCollision(currentblock, 0, 1)) {
                        currentblock.y++;
                    }
                    else {
                        Placeblock(currentblock); // ?????F?????£cI??w??????A??m???
                        currentblock = {};  // ?M????e???
                    }
                }
                if (isbottom) {
                    while (!CheckCollision(currentblock, 0, 1)) {
                        currentblock.y++;
                    }
                    Placeblock(currentblock); // ?N?????m?b????
                    currentblock = {};
                    isbottom = false;
                }

                // ??????
                if (IsKeyPressed(KEY_A) && !CheckCollision(currentblock, -1, 0)) {
                    currentblock.x--;
                }
                if (IsKeyPressed(KEY_S) && !CheckCollision(currentblock, 1, 0)) {
                    currentblock.y++;
                }
                if (IsKeyPressed(KEY_D) && !CheckCollision(currentblock, 1, 0)) {
                    currentblock.x++;
                }
                // ??L?\??
                if (IsKeyPressed(KEY_B)) {
                    isbottom = true;
                }
                if (IsKeyPressed(KEY_R)) {
                    block tempblock = currentblock;
                    Rotateblock(tempblock);

                    if (!CheckCollision(tempblock, 0, 0)) {
                        currentblock = tempblock;
                    }
                }
                if (IsKeyPressed(KEY_Q)) {
                    exit(1);
                }
                if (IsKeyPressed(KEY_C)) {
                    currentblock = nextblock;
                    nextblock = GetRandomblock();
                }
            }

            BeginDrawing(); // ?M??e??
            ClearBackground(RAYWHITE);

            if (IsKeyPressed(KEY_P)) {
                ispause = !ispause;
            }

            // o?s?C???e??
            DrawBoard();
            Drawblock(currentblock);
            DrawNextblock(nextblock);

            EndDrawing();
        }
        while (gameState == GAME_OVER) { // ?????e??
            SetTargetFPS(60);

            BeginDrawing();
            ClearBackground(GRAY);
            DrawText("GAME OVER", 140, gridsize * 2, 50, BLACK);
            DrawText(("YOUR FINAL Score: " + to_string(score)).c_str(), 20, gridsize * 6, 20, BLACK);
            DrawText(("YOUR FINAL Line: " + to_string(totalline)).c_str(), 20, gridsize * 8, 20, BLACK);
            DrawText("Press R to restart the game", 200, gridsize * 18, 20, BLACK);
            DrawText("Press Q to quit the game", 200, gridsize * 19, 20, BLACK);
            DrawText("Press ENTER to back to MENU", 200, gridsize * 17, 20, BLACK);


            for (int i = 0; i < boardheight; ++i) { // ?M??board
                for (int j = 0; j < boardwidth; ++j) {
                    board[i][j] = ' ';
                }
            }

            if (IsKeyPressed(KEY_R)) {
                score = 0;
                totalline = 0;
                gameState = PLAYING;
            }
            if (IsKeyPressed(KEY_Q)) {
                exit(1);
            }
            if (IsKeyPressed(KEY_ENTER)) {
                gameState = MENU;
                score = 0;
                totalline = 0;
            }
            EndDrawing();
        }
    }
    // ???? Raylib
    CloseWindow();

    return 0;
}