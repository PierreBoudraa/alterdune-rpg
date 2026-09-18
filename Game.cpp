#include "Game.h"
#include "FileLoader.h"
#include "ActAction.h"
#include <iostream>
#include <limits>
#include <algorithm>

static void pauseConsole() {
    std::cout << "\nAppuyez sur Entree pour continuer...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(int minVal, int maxVal) {
    int val;
    while (true) {
        std::cin >> val;
        if (std::cin.fail() || val < minVal || val > maxVal) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Choix invalide (" << minVal << "-" << maxVal << ") : ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
    }
}

Game::Game() : rng(std::random_device{}()) {}

int Game::rollDamage(int targetHPMax) {
    std::uniform_int_distribution<int> dist(0, targetHPMax);
    return dist(rng);
}

Monster Game::pickRandomMonster() {
    std::uniform_int_distribution<int> dist(0, (int)monsterPool.size() - 1);
    return monsterPool[dist(rng)];
}

void Game::printSep(char c, int len) const {
    std::cout << std::string(len, c) << "\n";
}

void Game::printTitle() const {
    std::cout << "\n";
    std::cout << "   ___  _   _____  ___  ___  ___  _   _ _  _ ___ \n";
    std::cout << "  / _ \\| | |_   _|| __|| _ \\|   \\| | | | \\| | __|\n";
    std::cout << " | (_) | |__ | |  | _| |   /| |) | |_| | .` | _| \n";
    std::cout << "  \\___/|____||_|  |___||_|_\\|___/|___|_|_|\\_|___|\n";
    std::cout << "\n  Un RPG minimaliste en C++\n\n";
}

void Game::init(const std::string& itemsFile, const std::string& monstersFile) {
    printTitle();
    std::cout << "Entrez le nom de votre personnage : ";
    std::string name;
    std::getline(std::cin, name);
    size_t s = name.find_first_not_of(" \t\r\n");
    size_t e = name.find_last_not_of(" \t\r\n");
    name = (s == std::string::npos) ? "Guerrier" : name.substr(s, e - s + 1);
    if (name.empty()) name = "Guerrier";
    player = Player(name, 100);

    auto items = FileLoader::loadItems(itemsFile);
    for (const auto& it : items) player.addItem(it);

    monsterPool = FileLoader::loadMonsters(monstersFile);
    if (monsterPool.empty()) { std::cerr << "[ERREUR] Aucun monstre valide.\n"; exit(1); }

    printSep('=');
    std::cout << "Bienvenue, " << player.getName() << " !\n";
    printSep();
    player.display();
    printSep();
    std::cout << "Inventaire initial :\n";
    player.displayInventory();
    printSep('=');
    pauseConsole();
}

void Game::run() {
    while (player.getVictories() < 10 && player.isAlive()) {
        printSep('=');
        std::cout << "\n  *** MENU PRINCIPAL ***\n\n";
        std::cout << "  Victoires : " << player.getVictories() << " / 10\n";
        std::cout << "  HP        : " << player.getHP() << "/" << player.getHPMax() << "\n\n";
        printSep();
        std::cout << "  1. Bestiaire\n  2. Demarrer un combat\n";
        std::cout << "  3. Statistiques\n  4. Items\n  5. Quitter\n";
        printSep('=');
        std::cout << "Choix : ";
        int c = readInt(1, 5);
        switch (c) {
        case 1: menuBestiary(); break;
        case 2: {
            Monster m = pickRandomMonster();
            printSep('=');
            std::cout << "\n  Un monstre apparait !\n\n";
            m.display();
            printSep('=');
            pauseConsole();
            runCombat(m);
            pauseConsole();
            break;
        }
        case 3: menuStats();  break;
        case 4: menuItems();  break;
        case 5: std::cout << "Au revoir !\n"; return;
        }
        if (!player.isAlive()) {
            printSep('=');
            std::cout << "\n  GAME OVER - " << player.getName() << " a ete vaincu.\n";
            printSep('=');
            return;
        }
    }
    if (player.getVictories() >= 10) displayEndGame();
}

void Game::menuBestiary() {
    printSep('=');
    std::cout << "\n  *** BESTIAIRE ***\n\n";
    bestiary.display();
    printSep('=');
    pauseConsole();
}

void Game::menuStats() {
    printSep('=');
    std::cout << "\n  *** STATISTIQUES ***\n\n";
    player.display();
    printSep('=');
    pauseConsole();
}

void Game::menuItems() {
    while (true) {
        printSep('=');
        std::cout << "\n  *** INVENTAIRE ***\n\n";
        player.displayInventory();
        printSep();
        std::cout << "  1. Utiliser un item\n  2. Retour\n";
        printSep('=');
        std::cout << "Choix : ";
        int c = readInt(1, 2);
        if (c == 2) break;
        auto& inv = player.getInventory();
        bool hasItem = false;
        for (const auto& it : inv) if (it.getQuantity() > 0) { hasItem = true; break; }
        if (!hasItem) { std::cout << "  Inventaire vide.\n"; pauseConsole(); continue; }
        std::cout << "Quel item ? (1-" << inv.size() << ", 0=annuler) : ";
        int idx = readInt(0, (int)inv.size());
        if (idx == 0) continue;
        idx--;
        if (inv[idx].getQuantity() <= 0) { std::cout << "  Epuise.\n"; }
        else {
            int hpBefore = player.getHP();
            player.useItem(idx);
            std::cout << "  +" << (player.getHP() - hpBefore) << " HP. Total : "
                      << player.getHP() << "/" << player.getHPMax() << "\n";
        }
        pauseConsole();
    }
}

bool Game::runCombat(Monster monster) {
    bool over = false, won = false, killed = false;
    while (!over) {
        printSep('=');
        std::cout << "\n  [ COMBAT ]\n\n";
        monster.display();
        std::cout << "\n  " << player.getName()
                  << " | HP : " << player.getHP() << "/" << player.getHPMax() << "\n";
        printSep();
        std::cout << "  1. FIGHT   2. ACT   3. ITEM   4. MERCY\n";
        printSep('=');
        std::cout << "Choix : ";
        int choice = readInt(1, 4);
        bool turnConsumed = true;
        switch (choice) {
        case 1: doFight(monster, over, won, killed); break;
        case 2: doAct(monster);                      break;
        case 3: doItem(turnConsumed);                break;
        case 4: doMercy(monster, over, won);         break;
        }
        if (over) break;
        if (!turnConsumed) continue;
        int dmg = rollDamage(player.getHPMax());
        player.setHP(std::max(0, player.getHP() - dmg));
        std::cout << "\n  " << monster.getName() << " attaque ! ";
        if (dmg == 0) std::cout << "Coup rate !\n";
        else std::cout << dmg << " degats. HP : " << player.getHP() << "/" << player.getHPMax() << "\n";
        if (!player.isAlive()) {
            over = true;
            std::cout << "\n  Vous etes tombe... GAME OVER.\n";
            printSep('=');
        }
    }
    if (won) {
        player.addVictory(killed);
        bestiary.addEntry(monster, killed);
        std::cout << "\n  Victoire ! (" << player.getVictories() << "/10)\n";
    }
    return won;
}

void Game::doFight(Monster& m, bool& over, bool& won, bool& killed) {
    int dmg = rollDamage(m.getHPMax());
    m.setHP(std::max(0, m.getHP() - dmg));
    std::cout << "\n  Vous attaquez " << m.getName() << " ! ";
    if (dmg == 0) std::cout << "Coup rate !\n";
    else std::cout << dmg << " degats. HP monstre : " << m.getHP() << "/" << m.getHPMax() << "\n";
    if (!m.isAlive()) {
        over = won = killed = true;
        std::cout << "\n  " << m.getName() << " est vaincu !\n";
    }
}

void Game::doAct(Monster& m) {
    const auto& ids = m.getActIds();
    int count = std::min(m.getActCount(), (int)ids.size());
    std::cout << "\n  Actions ACT :\n";
    for (int i = 0; i < count; i++)
        std::cout << "  " << (i+1) << ". " << ids[i] << "\n";
    std::cout << "  0. Annuler\nChoix : ";
    int c = readInt(0, count);
    if (c == 0) { std::cout << "  Annule.\n"; return; }
    const ActAction* action = ActCatalogue::getAction(ids[c-1]);
    if (!action) return;
    std::cout << "\n  >> " << action->text << "\n";
    int before = m.getMercy();
    m.modifyMercy(action->mercyImpact);
    int delta = m.getMercy() - before;
    if      (delta > 0) std::cout << "  Mercy +" << delta;
    else if (delta < 0) std::cout << "  Mercy " << delta;
    else                std::cout << "  Mercy inchangee";
    std::cout << " (" << m.getMercy() << "/" << m.getMercyGoal() << ")\n";
    if (m.isMercyFull()) std::cout << "  Le monstre est apaise ! Utilisez MERCY.\n";
}

void Game::doItem(bool& turnConsumed) {
    auto& inv = player.getInventory();
    bool hasItem = false;
    for (const auto& it : inv) if (it.getQuantity() > 0) { hasItem = true; break; }
    if (!hasItem) { std::cout << "\n  Inventaire vide.\n"; turnConsumed = false; return; }
    std::cout << "\n"; player.displayInventory();
    std::cout << "  0. Annuler\nChoix : ";
    int idx = readInt(0, (int)inv.size());
    if (idx == 0) { turnConsumed = false; return; }
    idx--;
    if (inv[idx].getQuantity() <= 0) { std::cout << "  Epuise.\n"; turnConsumed = false; return; }
    int hpBefore = player.getHP();
    player.useItem(idx);
    std::cout << "  +" << (player.getHP() - hpBefore) << " HP. Total : "
              << player.getHP() << "/" << player.getHPMax() << "\n";
    turnConsumed = true;
}

void Game::doMercy(Monster& m, bool& over, bool& won) {
    if (!m.isMercyFull()) {
        std::cout << "\n  Mercy insuffisante (" << m.getMercy()
                  << "/" << m.getMercyGoal() << "). Utilisez ACT.\n";
        return;
    }
    over = won = true;
    std::cout << "\n  Vous epargnez " << m.getName() << ".\n"
              << "  Il disparait dans un nuage de gratitude.\n";
}

void Game::displayEndGame() const {
    int k = player.getKilled(), s = player.getSpared();
    printSep('=');
    std::cout << "\n  *** FIN DE PARTIE ***\n\n"
              << "  " << player.getName() << " a accompli 10 victoires !\n\n"
              << "  Monstres tues     : " << k << "\n"
              << "  Monstres epargnes : " << s << "\n\n";
    if      (k == 0) std::cout << "  ~ FIN PACIFISTE ~\n  Aucun sang verse. Les monstres s'en souviennent.\n";
    else if (s == 0) std::cout << "  ~ FIN GENOCIDAIRE ~\n  Tout a ete ecrase. Le silence est lourd.\n";
    else             std::cout << "  ~ FIN NEUTRE ~\n  Violence et clemence melees. Deux chemins, une verite.\n";
    printSep('=');
}
