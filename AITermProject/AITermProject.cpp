#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <random>
using namespace std;

struct Coordinates {
    int x;
    int y;
};

struct AIPiece {
    Coordinates coordinates;
    bool        status;
    int         numberOfMoves;
};

void    capture(char**, int, int, int&, class AI&);
int     calculateCapturePoint(char**, int, int);

class Node {
public:
    Node(vector<Node*>    &allNodes, char** map, int depth) {
        allNodes.push_back(this);
        this->map = map;
        this->depth = depth;
    }

    bool    isSameMap(char **map) {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (this->map[i][j] != map[i][j])
                    return false;
        return true;
    }

    char** getMap() {
        return map;
    }

    ~Node() {
        for (int i = 0; i < 8; i++)
            delete[] map[i];
        delete[] map;
    }

private:
    char**                  map;
    int                     depth;
    int                     point;
};

class Game {
public:
    Game(char** map) {
        head = new Node(allNodes, map, 0);
        currentNode = head;
    }

    char** getMap() {
        return currentNode->getMap();
    }
    ~Game() {
        for (Node* node : allNodes)
            delete node;
    }
private:
    vector<Node*>    allNodes;
    Node* head;
    Node* currentNode;
};

class AI {
public:
    AI(Texture2D ai, int &numOfHumans, int& totalNumOfMoves, char** map) : aIPieceList({ {{0, 0}, true}, {{0, 2}, true}, {{6, 4}, true}, {{6, 6}, true} }),
                                                                           generator(static_cast<unsigned int>(time(0))),
                                                                           numOfHumans(numOfHumans),
                                                                           totalNumOfMoves(totalNumOfMoves) {
        x = y = count = 0;
        this->ai = ai;
        this->map = map;
        size = 4;
    }

    const Texture2D   &getTexture2D() const {
        int     num = 0;
        return  ai;
    }

    void    moveControl(int num) {
        if (num == 2 && count == 1)
            return;
        this_thread::sleep_for(chrono::seconds(1));
        count = 0;
        for (int i = 0; i < size; i++) {
            if (!aIPieceList[i].status)
                continue;
            x = aIPieceList[i].coordinates.x;
            y = aIPieceList[i].coordinates.y;
            aIPieceList[i].numberOfMoves = (x != 0 && map[y][x - 1] == 'E')
                                        + (y != 0 && map[y - 1][x] == 'E')
                                        + (x != 6 && map[y][x + 1] == 'E')
                                        + (y != 6 && map[y + 1][x] == 'E');
            if (aIPieceList[i].numberOfMoves)
                count++;
            else
                aIPieceList[i].status = false;
        }
        uniform_int_distribution<int> distribution(1, count);
        int randomNumber = distribution(generator);
        for (int i = 0; i < size; i++) {
            if (aIPieceList[i].status)
                randomNumber--;
            if (!randomNumber) {
                uniform_int_distribution<int> distribution(1, aIPieceList[i].numberOfMoves);
                randomNumber = distribution(generator);
                x = aIPieceList[i].coordinates.x;
                y = aIPieceList[i].coordinates.y;
                if (x != 0 && map[y][x - 1] == 'E' && !--randomNumber && move(num, i, y, x, 0, -1))
                    break;
                if (y != 0 && map[y - 1][x] == 'E' && !--randomNumber && move(num, i, y, x, -1, 0))
                    break;
                if (x != 6 && map[y][x + 1] == 'E' && !--randomNumber && move(num, i, y, x, 0, 1))
                    break;
                if (y != 6 && map[y + 1][x] == 'E' && !--randomNumber && move(num, i, y, x, 1, 0))
                    break;
            }
        }
    }

    void    deleteAIPiece(int y, int x) {
        for (int i = 0; i < size; i++)
            if (aIPieceList[i].coordinates.x == x && aIPieceList[i].coordinates.y == y) {
                aIPieceList.erase(aIPieceList.begin() + i);
                break;
            }
        size--;
    }

    const int& getSize() const {
        return size;
    }

private:
    vector<AIPiece> aIPieceList;
    Texture2D       ai;
    mt19937         generator;
    char**          map;
    int             size;
    int             count;
    int&            numOfHumans;
    int&            totalNumOfMoves;
    int             x;
    int             y;

    bool    move(int num, int i, int y, int x, int addY, int addX) {
        map[y + addY][x + addX] = 'A';
        map[y][x] = 'E';
        aIPieceList[i].coordinates.x += addX;
        aIPieceList[i].coordinates.y += addY;
        for (int j = 0; j < size; j++)
            aIPieceList[j].status = true;
        if (num == 1 && size != 1)
            aIPieceList[i].status = false;
        capture(map, y + addY, x + addX, numOfHumans, *this);
        totalNumOfMoves++;
        return true;
    }
};

int calculateCapturePoint(char** map, int y, int x) {
    vector<Coordinates> elementsToBeDeleted;
    int tmpX1 = x, tmpX2 = x, tmpY1 = y, tmpY2 = y;

    for (; tmpX1 > 0 && map[y][tmpX1 - 1] != 'E'; tmpX1--);
    for (; tmpX2 < 6 && map[y][tmpX2 + 1] != 'E'; tmpX2++);
    for (; tmpY1 > 0 && map[tmpY1 - 1][x] != 'E'; tmpY1--);
    for (; tmpY2 < 6 && map[tmpY2 + 1][x] != 'E'; tmpY2++);
    if (tmpX1 == 0 && tmpX2 == 6) {
        for (int i = 0; i < 7; i++)
            elementsToBeDeleted.push_back({ i, y });
    }
    else if (tmpX1 == 0) {
        for (tmpX2--; tmpX2 > -1; tmpX2--)
            if (map[y][tmpX2] != map[y][tmpX2 + 1])
                break;
        for (; tmpX2 > -1; tmpX2--)
            elementsToBeDeleted.push_back({ tmpX2, y });
    }
    else if (tmpX2 == 6) {
        for (tmpX1++; tmpX1 < 7; tmpX1++)
            if (map[y][tmpX1] != map[y][tmpX1 - 1])
                break;
        for (; tmpX1 < 7; tmpX1++)
            elementsToBeDeleted.push_back({ tmpX1, y });
    }
    else {
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
    }
    else if (tmpY1 == 0) {
        for (tmpY2--; tmpY2 > -1; tmpY2--)
            if (map[tmpY2][x] != map[tmpY2 + 1][x])
                break;
        for (; tmpY2 > -1; tmpY2--)
            elementsToBeDeleted.push_back({ x, tmpY2 });
    }
    else if (tmpY2 == 6) {
        for (tmpY1++; tmpY1 < 7; tmpY1++)
            if (map[tmpY1][x] != map[tmpY1 - 1][x])
                break;
        for (; tmpY1 < 7; tmpY1++)
            elementsToBeDeleted.push_back({ x, tmpY1 });
    }
    else {
        for (tmpY1++; tmpY1 < tmpY2; tmpY1++)
            if (map[tmpY1][x] != map[tmpY1 - 1][x])
                break;
        for (tmpY2--; tmpY2 >= tmpY1; tmpY2--)
            if (map[tmpY2][x] != map[tmpY2 + 1][x])
                break;
        for (; tmpY1 <= tmpY2; tmpY1++)
            elementsToBeDeleted.push_back({ x, tmpY1 });
    }
    int point = 0;
    for (Coordinates element : elementsToBeDeleted) {
        if (map[element.y][element.x] == 'H')
            point++;
        else if (map[element.y][element.x] == 'A')
            point--;
        map[element.y][element.x] = 'E';
    }
    return point;
}

void    capture(char** map, int y, int x, int& numOfHumans, AI& ai) {
    vector<Coordinates> elementsToBeDeleted;
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
    for (Coordinates element : elementsToBeDeleted) {
        if (map[element.y][element.x] == 'H')
            numOfHumans--;
        else if (map[element.y][element.x] == 'A')
            ai.deleteAIPiece(element.y, element.x);
        map[element.y][element.x] = 'E';
    }
}

static bool    control(int& winStatus, const int& numOfHumans, const int& numOfAis) {
    if ((numOfHumans == 0 && numOfAis == 0) || (numOfHumans == 1 && numOfAis == 1))
        return true;
    if (numOfHumans == 0)
        return winStatus = 1, true;
    if (numOfAis == 0)
        return winStatus = 2, true;
    return false;
}

int main() {
    InitWindow(800, 800, "AITermProject");
    char** map = new char* [7] {new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'A', 'E', 'E', 'E', 'E', 'E', 'H'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'},
                                new char [7] {'E', 'E', 'E', 'E', 'E', 'E', 'E'},
                                new char [7] {'H', 'E', 'E', 'E', 'E', 'E', 'A'}};
    Game game(map);
    Image image = LoadImage("Background.png");
    Texture2D background = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("Human.png");
    Texture2D human = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("Target.png");
    Texture2D target = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("Ai.png");
    Vector2 mousePos;
    bool    isItTheHumansTurn = true;
    int     numOfHumans = 4, numOfMoves = 0, totalNumOfMoves = 0, x, y, oldx = 0, oldy = 0, prevMoveX, prevMoveY, winStatus = 0;
    AI ai(LoadTextureFromImage(image), numOfHumans, totalNumOfMoves, map);
    UnloadImage(image);
    SetTargetFPS(60);
    while (WindowShouldClose() == false) {
        if (totalNumOfMoves == 50) {
            if (numOfHumans < ai.getSize())
                winStatus = 1;
            else if (numOfHumans > ai.getSize())
                winStatus = 2;
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
                ai.moveControl(1);
            else {
                ai.moveControl(2);
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
                    capture(map, y, x, numOfHumans, ai);
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
    if (winStatus == 0)
        image = LoadImage("Draw.png");
    else if (winStatus == 1)
        image = LoadImage("Player1Win.png");
    else if (winStatus == 2)
        image = LoadImage("Player2Win.png");
    background = LoadTextureFromImage(image);
    UnloadImage(image);
    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        EndDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            break;
    }
    CloseWindow();
    return 0;
}