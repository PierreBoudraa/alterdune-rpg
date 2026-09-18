#include "Game.h"

int main() {
    Game game;
    game.init("data/items.csv", "data/monsters.csv");
    game.run();
    return 0;
}
