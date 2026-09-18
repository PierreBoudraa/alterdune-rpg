#pragma once
#include "Monster.h"
#include <vector>
#include <string>

struct BestiaryEntry {
    std::string name;
    std::string category;
    int         hpMax;
    int         atk;
    int         def;
    bool        wasKilled;
};

class Bestiary {
private:
    std::vector<BestiaryEntry> entries;

public:
    void addEntry(const Monster& monster, bool killed);
    void display() const;
    bool isEmpty() const;
};
