#ifndef NODE_H
#define NODE_H

#include <vector>

#define AIPLAYER 0
#define HUMANPLAYER 1
#define MAXDEPTH 5

class Node {
public:
    Node(char** map, int depth, int who, int movementNumber, int y, int x);
    ~Node();
    void        addLeaf(int i, int j, int previ, int prevj);
    void        branchOut();
    Node*       getTarget() const;
    const int&  getWho() const;
    const int&  getPoint() const;
    char**      copyMap();

private:
    std::vector<Node*>   neighbours;
    std::vector<Node*>   targets;
    char                **map;
    int                 depth;
    int                 point;
    int                 who;
    int                 movementNumber;
    int                 x;
    int                 y;

    void    addAllLeaf(char c);
};

#endif