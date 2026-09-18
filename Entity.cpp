#include "Entity.h"
#include <algorithm>

Entity::Entity(const std::string& name, int hpMax)
    : name(name), hp(hpMax), hpMax(hpMax) {}

const std::string& Entity::getName()  const { return name; }
int                Entity::getHP()    const { return hp; }
int                Entity::getHPMax() const { return hpMax; }
bool               Entity::isAlive()  const { return hp > 0; }

void Entity::setHP(int val) {
    hp = std::max(0, std::min(val, hpMax));
}

void Entity::heal(int amount) {
    hp = std::min(hp + amount, hpMax);
}
