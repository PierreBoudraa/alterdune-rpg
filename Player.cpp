#include "Player.h"
#include <iostream>

Player::Player()
    : Entity("???", 100), victories(0), killed(0), spared(0) {}

Player::Player(const std::string& name, int hpMax)
    : Entity(name, hpMax), victories(0), killed(0), spared(0) {}

int Player::getVictories() const { return victories; }
int Player::getKilled()    const { return killed; }
int Player::getSpared()    const { return spared; }

std::vector<Item>&       Player::getInventory()       { return inventory; }
const std::vector<Item>& Player::getInventory() const { return inventory; }

void Player::addItem(const Item& item) {
    for (auto& i : inventory) {
        if (i.getName() == item.getName()) {
            i.setQuantity(i.getQuantity() + item.getQuantity());
            return;
        }
    }
    inventory.push_back(item);
}

void Player::addVictory(bool wasKilled) {
    victories++;
    if (wasKilled) killed++;
    else           spared++;
}

bool Player::useItem(int index) {
    if (index < 0 || index >= (int)inventory.size()) return false;
    Item& item = inventory[index];
    if (item.getQuantity() <= 0) return false;
    if (item.use()) { heal(item.getValue()); return true; }
    return false;
}

void Player::display() const {
    std::cout << "Joueur    : " << name << "\n"
              << "HP        : " << hp << " / " << hpMax << "\n"
              << "Victoires : " << victories << " / 10\n"
              << "Tues      : " << killed << "  |  Epargnes : " << spared << "\n";
}

void Player::displayInventory() const {
    if (inventory.empty()) { std::cout << "  Inventaire vide.\n"; return; }
    for (int i = 0; i < (int)inventory.size(); i++) {
        const Item& it = inventory[i];
        std::cout << "  [" << (i+1) << "] " << it.getName()
                  << " x" << it.getQuantity()
                  << "  -  " << it.getDescription() << "\n";
    }
}
