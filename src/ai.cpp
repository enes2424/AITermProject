#include "../include/ai.h"
#include "../include/node.h"
#include <thread>

AI::AI(Texture2D texture, int& numOfHumans, int& totalNumOfMoves, char** map) : numOfHumans(numOfHumans), totalNumOfMoves(totalNumOfMoves) {
    this->texture = texture;
    this->map = map;
    size = 4;
}

char**  AI::moveControl(int num) {
    if (num == 1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        node = new Node(map, 0, AIPLAYER, 1, -1, -1);
        map = (target = node->getTarget())->copyMap();
        totalNumOfMoves++;
        size = 0;
        numOfHumans = 0;
        for (int i = 0; i < 7; i++)
            for (int j = 0; j < 7; j++) {
                if (map[i][j] == 'A')
                    size++;
                else if (map[i][j] == 'H')
                    numOfHumans++;
            }
        return map;
    } else if (target->getWho() == HUMANPLAYER) {
        delete node;
        return map;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 7; i++)
        delete[] map[i];
    delete[] map;
    map = target->getTarget()->copyMap();
    totalNumOfMoves++;
    size = 0;
    numOfHumans = 0;
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++) {
            if (map[i][j] == 'A')
                size++;
            else if (map[i][j] == 'H')
                numOfHumans++;
        }
    delete node;
    return map;
}

const Texture2D&    AI::getTexture2D() const {
    return texture;
}

void    AI::deleteAIPiece() {
    size--;
}

const int& AI::getSize() const {
    return size;
}