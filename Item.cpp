#include "Item.h"
#include <string>

Item::Item(const std::string& name, ItemType type, int value, int quantity)
    : name(name), type(type), value(value), quantity(quantity) {}

const std::string& Item::getName()     const { return name; }
ItemType           Item::getType()     const { return type; }
int                Item::getValue()    const { return value; }
int                Item::getQuantity() const { return quantity; }

void Item::setQuantity(int q) { quantity = q; }

bool Item::use() {
    if (quantity <= 0) return false;
    quantity--;
    return true;
}

std::string Item::getDescription() const {
    return "Soigne " + std::to_string(value) + " HP";
}
