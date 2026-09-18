#pragma once
#include <string>

enum class ItemType { HEAL };

class Item {
private:
    std::string name;
    ItemType    type;
    int         value;
    int         quantity;

public:
    Item(const std::string& name, ItemType type, int value, int quantity);

    const std::string& getName()     const;
    ItemType           getType()     const;
    int                getValue()    const;
    int                getQuantity() const;

    void setQuantity(int q);
    bool use();
    std::string getDescription() const;
};
