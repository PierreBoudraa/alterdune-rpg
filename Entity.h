#pragma once
#include <string>

class Entity {
protected:
    std::string name;
    int         hp;
    int         hpMax;

public:
    Entity(const std::string& name, int hpMax);
    virtual ~Entity() = default;

    const std::string& getName()  const;
    int                getHP()    const;
    int                getHPMax() const;
    bool               isAlive()  const;

    void setHP(int val);
    void heal(int amount);

    virtual void display() const = 0;
};
