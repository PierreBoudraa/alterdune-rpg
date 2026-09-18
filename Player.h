#pragma once
#include "Entity.h"
#include "Item.h"
#include <vector>

class Player : public Entity {
private:
    int               victories;
    int               killed;
    int               spared;
    std::vector<Item> inventory;

public:
    Player();
    Player(const std::string& name, int hpMax = 100);

    int getVictories() const;
    int getKilled()    const;
    int getSpared()    const;

    std::vector<Item>&       getInventory();
    const std::vector<Item>& getInventory() const;

    void addItem(const Item& item);
    void addVictory(bool wasKilled);
    bool useItem(int index);

    void display()          const override;
    void displayInventory() const;
};
