#pragma once
#include "Item.h"
#include "Monster.h"
#include <vector>
#include <string>

class FileLoader {
public:
    static std::vector<Item>    loadItems(const std::string& filename);
    static std::vector<Monster> loadMonsters(const std::string& filename);

private:
    static std::string              trim(const std::string& s);
    static std::vector<std::string> split(const std::string& line, char delim);
};
