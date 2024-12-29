#ifndef AI_H
#define AI_H
#include "raylib.h"

#define AIPLAYER 0
#define HUMANPLAYER 1

class AI {
public:
    AI(Texture2D texture, int& numOfHumans, int& totalNumOfMoves, char** map);
    char**              moveControl(int num);
    const Texture2D     &getTexture2D() const;
    void    deleteAIPiece();
    const int& getSize() const;

private:
    Texture2D       texture;
    class Node      *node;
    class Node      *target;
    char            **map;
    int&            numOfHumans;
    int&            totalNumOfMoves;
    int             size;
};

#endif