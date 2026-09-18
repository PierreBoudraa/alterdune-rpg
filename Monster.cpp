#include "Monster.h"
#include <iostream>
#include <algorithm>

Monster::Monster(const std::string& name, MonsterCategory category,
                 int hp, int atk, int def, int mercyGoal,
                 const std::vector<std::string>& actIds)
    : Entity(name, hp),
      category(category), atk(atk), def(def),
      mercy(0), mercyGoal(mercyGoal), actIds(actIds) {}

MonsterCategory Monster::getCategory()  const { return category; }
int             Monster::getAtk()       const { return atk; }
int             Monster::getDef()       const { return def; }
int             Monster::getMercy()     const { return mercy; }
int             Monster::getMercyGoal() const { return mercyGoal; }

const std::vector<std::string>& Monster::getActIds() const { return actIds; }

int Monster::getActCount() const {
    if (category == MonsterCategory::BOSS)     return 4;
    if (category == MonsterCategory::MINIBOSS) return 3;
    return 2;
}

bool Monster::isMercyFull() const { return mercy >= mercyGoal; }

std::string Monster::getCategoryString() const {
    if (category == MonsterCategory::BOSS)     return "BOSS";
    if (category == MonsterCategory::MINIBOSS) return "MINIBOSS";
    return "NORMAL";
}

void Monster::modifyMercy(int delta) {
    mercy = std::max(0, std::min(mercy + delta, mercyGoal));
}

void Monster::display() const {
    std::cout << "[" << getCategoryString() << "] " << name
              << " | HP: " << hp << "/" << hpMax
              << " | ATK: " << atk << " | DEF: " << def
              << " | Mercy: " << mercy << "/" << mercyGoal << "\n";
}
