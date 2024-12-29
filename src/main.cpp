#include "../include/game.h"

int main() {
    int winStatus = gameLoop();
    gameEnd(winStatus);
    return 0;
}