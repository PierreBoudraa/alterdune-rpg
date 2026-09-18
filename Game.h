#pragma once
#include "Player.h"
#include "Monster.h"
#include "Bestiary.h"
#include <vector>
#include <random>

class Game {
private:
    Player               player;
    std::vector<Monster> monsterPool;
    Bestiary             bestiary;
    std::mt19937         rng;

    void menuBestiary();
    void menuStats();
    void menuItems();

    bool runCombat(Monster monster);
    void doFight(Monster& m, bool& over, bool& won, bool& killed);
    void doAct(Monster& m);
    void doItem(bool& turnConsumed);
    void doMercy(Monster& m, bool& over, bool& won);

    Monster pickRandomMonster();
    int     rollDamage(int targetHPMax);
    void    printSep(char c = '-', int len = 52) const;
    void    printTitle()     const;
    void    displayEndGame() const;

public:
    Game();
    void init(const std::string& itemsFile, const std::string& monstersFile);
    void run();
};
