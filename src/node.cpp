#include "../include/node.h"
#include "../include/game.h"
#include <random>
#include <iostream>

Node::Node(char** map, int depth, int who, int movementNumber, int y, int x) {
    this->map = map;
    this->depth = depth;
    this->who = who;
    this->movementNumber = movementNumber;
    this->y = y;
    this->x = x;
    if (x == -1)
        branchOut();
    else if (!(point = capture(map, y, x, NULL, NULL)) && depth != MAXDEPTH) {
        int countAI = 0;
        int countHuman = 0;
        for (int i = 0; i < 7; i++)
            for (int j = 0; j < 7; j++) {
                if (map[i][j] == 'A')
                    countAI++;
                else if (map[i][j] == 'H')
                    countHuman++;
            }
        if (countAI != 0 && countHuman != 0 && !(countHuman == 1 && countAI == 1))
            branchOut();
    }
}

Node::~Node() {
    for (int i = 0; i < 7; i++)
        delete[] map[i];
    delete[] map;
    for (Node *neighbour : neighbours)
        delete neighbour;
}

void    Node::addLeaf(int i, int j, int previ, int prevj) {
    if (movementNumber == 2 && y == previ && x == prevj)
        return;
    char    **tmp = copyMap();
    tmp[previ][prevj] = 'E';
    tmp[i][j] = who ? 'H' : 'A';
    if (movementNumber == 1)
        neighbours.push_back(new Node(tmp, depth + 1, who, 2, i, j));
    else
        neighbours.push_back(new Node(tmp, depth + 1, !who, 1, i, j));
}

void    Node::branchOut() {
    if (who == AIPLAYER) {
        addAllLeaf('A');
        if (!neighbours.size()) {
            movementNumber = 1;
            who = HUMANPLAYER;
            addAllLeaf('H');
        }
    } else {
        addAllLeaf('H');
        if (!neighbours.size()) {
            movementNumber = 1;
            who = AIPLAYER;
            addAllLeaf('A');
        }
    }
    if (who == AIPLAYER) {
        int max = -5;
        for (Node *neighbour : neighbours)
            if (neighbour->getPoint() > max)
                max = neighbour->getPoint();
        point = max;
    } else {
        int min = 5;
        for (Node *neighbour : neighbours)
            if (neighbour->getPoint() < min)
                min = neighbour->getPoint();
        point = min;
    }
    for (Node *neighbour : neighbours)
        if (point == neighbour->getPoint())
            targets.push_back(neighbour);
}

Node*    Node::getTarget() const {
    std::mt19937                        generator(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<int>  distribution(0, targets.size() - 1);
    return targets[distribution(generator)];
}

const int& Node::getWho() const {
    return who;
}

const int&  Node::getPoint() const {
    return point;
}

char**  Node::copyMap() {
    char**  copy = new char* [7];
    for (int i = 0; i < 7; i++) {
        copy[i] = new char [7];
        for (int j = 0; j < 7; j++)
            copy[i][j] = map[i][j];
    }
    return copy;
}

void    Node::addAllLeaf(char c) {
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            if (map[i][j] == 'E') {
                if (j != 0 && map[i][j - 1] == c)
                    addLeaf(i, j, i, j - 1);
                if (i != 0 && map[i - 1][j] == c)
                    addLeaf(i, j, i - 1, j);
                if (j != 6 && map[i][j + 1] == c)
                    addLeaf(i, j, i, j + 1);
                if (i != 6 && map[i + 1][j] == c)
                    addLeaf(i, j, i + 1, j);
            }
}