#include "Bestiary.h"
#include <iostream>
#include <iomanip>

void Bestiary::addEntry(const Monster& monster, bool killed) {
    BestiaryEntry e;
    e.name      = monster.getName();
    e.category  = monster.getCategoryString();
    e.hpMax     = monster.getHPMax();
    e.atk       = monster.getAtk();
    e.def       = monster.getDef();
    e.wasKilled = killed;
    entries.push_back(e);
}

void Bestiary::display() const {
    if (entries.empty()) { std::cout << "  Aucun monstre vaincu pour l'instant.\n"; return; }
    std::cout << std::left
              << std::setw(16) << "Nom"
              << std::setw(10) << "Categorie"
              << std::setw(8)  << "HP max"
              << std::setw(6)  << "ATK"
              << std::setw(6)  << "DEF"
              << "Resultat\n";
    std::cout << std::string(52, '=') << "\n";
    for (const auto& e : entries) {
        std::cout << std::setw(16) << e.name
                  << std::setw(10) << e.category
                  << std::setw(8)  << e.hpMax
                  << std::setw(6)  << e.atk
                  << std::setw(6)  << e.def
                  << (e.wasKilled ? "TUE" : "EPARGNE") << "\n";
    }
}

bool Bestiary::isEmpty() const { return entries.empty(); }
