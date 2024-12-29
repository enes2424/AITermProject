#ifndef GAME_H
#define GAME_H

struct Coordinates {
    int x;
    int y;
};

struct AIPiece {
    Coordinates coordinates;
    bool        status;
    int         numberOfMoves;
};

bool            control(int& winStatus, const int& numOfHumans, const int& numOfAis);
int             capture(char** map, int y, int x, int *numOfHumans, class AI *ai);
struct Texture  createTexture2D(const char* file);
int             gameLoop();
void            gameEnd(int winStatus);

#endif