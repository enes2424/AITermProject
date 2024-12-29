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