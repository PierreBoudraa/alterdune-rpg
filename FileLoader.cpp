#include "FileLoader.h"
#include "ActAction.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string FileLoader::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

std::vector<std::string> FileLoader::split(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delim))
        tokens.push_back(trim(token));
    return tokens;
}

std::vector<Item> FileLoader::loadItems(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERREUR] Fichier introuvable : " << filename << "\n";
        exit(1);
    }
    std::vector<Item> items;
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto t = split(line, ';');
        if (t.size() < 4) {
            std::cerr << "[AVERT] items.csv ligne " << lineNum << " ignoree.\n";
            continue;
        }
        try { items.emplace_back(t[0], ItemType::HEAL, std::stoi(t[2]), std::stoi(t[3])); }
        catch (...) { std::cerr << "[AVERT] items.csv ligne " << lineNum << " invalide.\n"; }
    }
    return items;
}

std::vector<Monster> FileLoader::loadMonsters(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERREUR] Fichier introuvable : " << filename << "\n";
        exit(1);
    }
    std::vector<Monster> monsters;
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto t = split(line, ';');
        if (t.size() < 7) {
            std::cerr << "[AVERT] monsters.csv ligne " << lineNum << " ignoree.\n";
            continue;
        }
        MonsterCategory cat;
        int actCount;
        if      (t[0] == "NORMAL")   { cat = MonsterCategory::NORMAL;   actCount = 2; }
        else if (t[0] == "MINIBOSS") { cat = MonsterCategory::MINIBOSS; actCount = 3; }
        else if (t[0] == "BOSS")     { cat = MonsterCategory::BOSS;     actCount = 4; }
        else { std::cerr << "[AVERT] Categorie inconnue ligne " << lineNum << ".\n"; continue; }
        int hp, atk, def, mercyGoal;
        try {
            hp = std::stoi(t[2]); atk = std::stoi(t[3]);
            def = std::stoi(t[4]); mercyGoal = std::stoi(t[5]);
        }
        catch (...) { std::cerr << "[AVERT] Stats invalides ligne " << lineNum << ".\n"; continue; }
        std::vector<std::string> actIds;
        for (int i = 0; i < actCount; i++) {
            size_t idx = 6 + i;
            if (idx >= t.size() || t[idx] == "-" || t[idx].empty()) continue;
            if (!ActCatalogue::getAction(t[idx])) continue;
            actIds.push_back(t[idx]);
        }
        if (actIds.empty()) continue;
        monsters.emplace_back(t[1], cat, hp, atk, def, mercyGoal, actIds);
    }
    return monsters;
}
