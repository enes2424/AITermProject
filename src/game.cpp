#include "../include/game.h"
#include "../include/ai.h"
#include <vector>

bool    control(int& winStatus, const int& numOfHumans, const int& numOfAis) {
    if ((numOfHumans == 0 && numOfAis == 0) || (numOfHumans == 1 && numOfAis == 1))
        return winStatus = 0, true;
    if (numOfHumans == 0)
        return winStatus = 1, true;
    if (numOfAis == 0)
        return winStatus = 2, true;
    return false;
}

int    capture(char** map, int y, int x, int *numOfHumans, AI *ai) {
    std::vector<Coordinates> elementsToBeDeleted;
    int tmpX1 = x, tmpX2 = x, tmpY1 = y, tmpY2 = y;

    for (; tmpX1 > 0 && map[y][tmpX1 - 1] != 'E'; tmpX1--);
    for (; tmpX2 < 6 && map[y][tmpX2 + 1] != 'E'; tmpX2++);
    for (; tmpY1 > 0 && map[tmpY1 - 1][x] != 'E'; tmpY1--);
    for (; tmpY2 < 6 && map[tmpY2 + 1][x] != 'E'; tmpY2++);
    if (tmpX1 == 0 && tmpX2 == 6) {
        for (int i = 0; i < 7; i++)
            elementsToBeDeleted.push_back({i, y});
    } else if (tmpX1 == 0) {
        for (tmpX2--; tmpX2 > -1; tmpX2--)
            if (map[y][tmpX2] != map[y][tmpX2 + 1])
                break;
        for (; tmpX2 > -1; tmpX2--)
            elementsToBeDeleted.push_back({ tmpX2, y });
    } else if (tmpX2 == 6) {
        for (tmpX1++; tmpX1 < 7; tmpX1++)
            if (map[y][tmpX1] != map[y][tmpX1 - 1])
                break;
        for (; tmpX1 < 7; tmpX1++)
            elementsToBeDeleted.push_back({ tmpX1, y });
    } else {
        for (tmpX1++; tmpX1 < tmpX2; tmpX1++)
            if (map[y][tmpX1] != map[y][tmpX1 - 1])
                break;
        for (tmpX2--; tmpX2 >= tmpX1; tmpX2--)
            if (map[y][tmpX2] != map[y][tmpX2 + 1])
                break;
        for (; tmpX1 <= tmpX2; tmpX1++)
            elementsToBeDeleted.push_back({ tmpX1, y });
    }

    if (tmpY1 == 0 && tmpY2 == 6) {
        for (int i = 0; i < 7; i++)
            elementsToBeDeleted.push_back({ x, i });
    } else if (tmpY1 == 0) {
        for (tmpY2--; tmpY2 > -1; tmpY2--)
            if (map[tmpY2][x] != map[tmpY2 + 1][x])
                break;
        for (; tmpY2 > -1; tmpY2--)
            elementsToBeDeleted.push_back({ x, tmpY2});
    } else if (tmpY2 == 6) {
        for (tmpY1++; tmpY1 < 7; tmpY1++)
            if (map[tmpY1][x] != map[tmpY1 - 1][x])
                break;
        for (; tmpY1 < 7; tmpY1++)
            elementsToBeDeleted.push_back({ x, tmpY1});
    } else {
        for (tmpY1++; tmpY1 < tmpY2; tmpY1++)
            if (map[tmpY1][x] != map[tmpY1 - 1][x])
                break;
        for (tmpY2--; tmpY2 >= tmpY1; tmpY2--)
            if (map[tmpY2][x] != map[tmpY2 + 1][x])
                break;
        for (; tmpY1 <= tmpY2; tmpY1++)
            elementsToBeDeleted.push_back({ x, tmpY1});
    }
    int point = 0;
    for (Coordinates element : elementsToBeDeleted) {
        if (map[element.y][element.x] == 'H') {
            point++;
            if (numOfHumans)
                (*numOfHumans)--;
        } else if (map[element.y][element.x] == 'A') {
            point--;
            if (ai)
                ai->deleteAIPiece();
        }
        map[element.y][element.x] = 'E';
    }
    return point;
}

Texture2D   createTexture2D(const char* file) {
    Image       image = LoadImage(file);
    Texture2D   texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

int    gameLoop() {
    InitWindow(800, 800, "AITermProject");
    char**      map = new char* [7] {new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'}};
    Texture2D   background = createTexture2D("images/Background.png");
    Texture2D   human = createTexture2D("images/Human.png");
    Texture2D   target = createTexture2D("images/Target.png");
    Vector2     mousePos;
    bool        isItTheHumansTurn = true;
    int         numOfHumans = 4;
    int         numOfMoves = 0;
    int         totalNumOfMoves = 0;
    int         x, y, prevMoveX, prevMoveY;
    int         oldx = 0, oldy = 0;
    int         winStatus = -1;

    AI          ai(createTexture2D("images/Ai.png"), numOfHumans, totalNumOfMoves, map);
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (totalNumOfMoves == 50) {
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
    for (int i = 0; i < 7; i++)
        delete[] map[i];
    delete[] map;
    return winStatus;
}

void    gameEnd(int winStatus) {
    if (winStatus == -1)
        return CloseWindow();
    Texture2D background;

    if (winStatus == 0)
        background = createTexture2D("images/Draw.png");
    else if (winStatus == 1)
        background = createTexture2D("images/Player1Win.png");
    else if (winStatus == 2)
        background = createTexture2D("images/Player2Win.png");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        EndDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            break;
    }
    return CloseWindow();
}