#include "../include/game.h"
#include "../include/ai.h"

int main() {
    InitWindow(800, 800, "AITermProject");
    char** map = new char* [7] {new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'}};
    Image       image;
    Texture2D   background = LoadTextureFromImage(image = LoadImage("images/Background.png"));
    UnloadImage(image);
    Texture2D   human = LoadTextureFromImage(image = LoadImage("images/Human.png"));
    UnloadImage(image);
    Texture2D   target = LoadTextureFromImage(image = LoadImage("images/Target.png"));
    UnloadImage(image);
    Vector2     mousePos;
    bool        isItTheHumansTurn = true;
    int         numOfHumans = 4, numOfMoves = 0, totalNumOfMoves = 0, x, y, oldx = 0, oldy = 0, prevMoveX, prevMoveY, winStatus = -1;

    AI          ai(LoadTextureFromImage(image = LoadImage("images/Ai.png")), numOfHumans, totalNumOfMoves, map);
    UnloadImage(image);
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (totalNumOfMoves == 150) {
            if (numOfHumans < ai.getSize())
                winStatus = 1;
            else if (numOfHumans > ai.getSize())
                winStatus = 2;
            else
                winStatus = 0;
            break;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        for (int i = 0; i < 7; i++)
            for (int j = 0; j < 7; j++) {
                if (map[i][j] == 'A')
                    DrawTexture(ai.getTexture2D(), 100 * j + 100, 100 * i + 100, WHITE);
                else if (map[i][j] == 'H')
                    DrawTexture(human, 100 * j + 100, 100 * i + 100, WHITE);
                else if (map[i][j] == 'T')
                    DrawTexture(target, 100 * j + 100, 100 * i + 100, WHITE);
            }
        DrawRectangle(0, 0, 100, 100, { 170, 170, 170, 255 });
        DrawText(TextFormat("number\n    of\n  moves\n--------\n    %d", totalNumOfMoves), 10, 10, 20, BLACK);
        EndDrawing();
        if (!isItTheHumansTurn) {
            numOfMoves++;
            if (numOfMoves == 1)
                map = ai.moveControl(1);
            else {
                map = ai.moveControl(2);
                isItTheHumansTurn = true;
                numOfMoves = 0;
            }
            if (control(winStatus, numOfHumans, ai.getSize()))
                break;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (isItTheHumansTurn) {
                mousePos = GetMousePosition();
                x = (int)(mousePos.x / 100) - 1;
                y = (int)(mousePos.y / 100) - 1;
                if (x < 0 || y < 0 || map[y][x] == 'E' || map[y][x] == 'A') {
                    for (int i = 0; i < 7; i++)
                        for (int j = 0; j < 7; j++)
                            if (map[i][j] == 'T')
                                map[i][j] = 'E';
                    continue;
                }
                if (map[y][x] == 'T') {
                    map[prevMoveY = y][prevMoveX = x] = 'H';
                    map[oldy][oldx] = 'E';
                    capture(map, y, x, &numOfHumans, &ai);
                    if (control(winStatus, numOfHumans, ai.getSize()))
                        break;
                    numOfMoves++;
                    totalNumOfMoves++;
                }
                for (int i = 0; i < 7; i++)
                    for (int j = 0; j < 7; j++)
                        if (map[i][j] == 'T')
                            map[i][j] = 'E';
                if (numOfHumans == 1 && numOfMoves == 1) {
                    for (int i = 0; i < 7; i++)
                        for (int j = 0; j < 7; j++)
                            if (map[i][j] == 'H') {
                                if (prevMoveX == j && prevMoveY == i) {
                                    isItTheHumansTurn = false;
                                    numOfMoves = 0;
                                }
                                i = 7;
                                break;
                            }
                    if (!isItTheHumansTurn)
                        continue;
                }
                if (numOfMoves == 2) {
                    isItTheHumansTurn = false;
                    numOfMoves = 0;
                } else if (map[y][x] == 'H') {
                    if (numOfMoves == 1 && prevMoveX == x && prevMoveY == y)
                        continue;
                    if (x != 0 && map[y][x - 1] == 'E')
                        map[y][x - 1] = 'T';
                    if (y != 0 && map[y - 1][x] == 'E')
                        map[y - 1][x] = 'T';
                    if (x != 6 && map[y][x + 1] == 'E')
                        map[y][x + 1] = 'T';
                    if (y != 6 && map[y + 1][x] == 'E')
                        map[y + 1][x] = 'T';
                    oldx = x;
                    oldy = y;
                }
            }
        }
    }
    if (winStatus == -1) {
        for (int i = 0; i < 7; i++)
            delete[] map[i];
        delete[] map;
        return CloseWindow(), 0;
    }
    else if (winStatus == 0)
        image = LoadImage("images/Draw.png");
    else if (winStatus == 1)
        image = LoadImage("images/Player1Win.png");
    else if (winStatus == 2)
        image = LoadImage("images/Player2Win.png");
    background = LoadTextureFromImage(image);
    UnloadImage(image);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        EndDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            break;
    }
    for (int i = 0; i < 7; i++)
        delete[] map[i];
    delete[] map;
    return CloseWindow(), 0;
}