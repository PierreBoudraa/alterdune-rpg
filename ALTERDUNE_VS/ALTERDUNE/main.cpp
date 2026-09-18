// ============================================================
//  ALTERDUNE - RPG Console en C++
//  Fichier unique pour Visual Studio
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <limits>
#include <iomanip>

// ============================================================
//  UTILITAIRES
// ============================================================

static std::mt19937 rng(std::random_device{}());

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

static std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delim))
        tokens.push_back(trim(token));
    return tokens;
}

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
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
    }
}

static int rollDamage(int targetHPMax) {
    std::uniform_int_distribution<int> dist(0, targetHPMax);
    return dist(rng);
}

static void printSep(char c = '-', int len = 52) {
    std::cout << std::string(len, c) << "\n";
}

// ============================================================
//  ACTIONS ACT - CATALOGUE
// ============================================================

struct ActAction {
    std::string id;
    std::string text;
    int mercyImpact;
};

static const std::map<std::string, ActAction> ACT_CATALOGUE = {
    {"JOKE",         {"JOKE",         "Tu racontes une blague... Le monstre rit nerveusement.",          +20}},
    {"COMPLIMENT",   {"COMPLIMENT",   "Tu dis qu'il a de beaux yeux. Il rougit sous ses ecailles.",      +25}},
    {"INSULT",       {"INSULT",       "Tu insultes sa maman. Il prend ca TRES mal.",                     -30}},
    {"DISCUSS",      {"DISCUSS",      "Vous parlez de la pluie et du beau temps. Ambiance bizarre.",     +15}},
    {"OBSERVE",      {"OBSERVE",      "Tu l'observes. Il se sent tres mal a l'aise.",                    +10}},
    {"PET",          {"PET",          "Tu tentes de le caresser. Il ne sait pas quoi faire.",            +20}},
    {"OFFER_SNACK",  {"OFFER_SNACK",  "Tu sors un sandwich. Meme les monstres ont faim.",               +30}},
    {"REASON",       {"REASON",       "Tu expliques que se battre est inutile. Il reflechit...",         +20}},
    {"DANCE",        {"DANCE",        "Tu danses. Le monstre est tellement gene qu'il en oublie tout.", +15}},
    {"TAUNT",        {"TAUNT",        "Tu le nargues avec une grimace. Il devient rouge de rage.",       -20}},
    {"MOCK",         {"MOCK",         "Tu te moques de sa coupe. Il n'a pas de cheveux. Encore pire.",  -25}},
    {"SING",         {"SING",         "Tu fredonnes une chanson d'amour. Silence genant.",              +10}},
};

static const ActAction* getAction(const std::string& id) {
    auto it = ACT_CATALOGUE.find(id);
    if (it != ACT_CATALOGUE.end()) return &it->second;
    return nullptr;
}

// ============================================================
//  ITEM
// ============================================================

class Item {
private:
    std::string name;
    int value;
    int quantity;

public:
    Item(const std::string& n, int v, int q)
        : name(n), value(v), quantity(q) {}

    const std::string& getName()  const { return name; }
    int getValue()                const { return value; }
    int getQuantity()             const { return quantity; }

    void setQuantity(int q) { quantity = q; }

    bool use() {
        if (quantity <= 0) return false;
        quantity--;
        return true;
    }

    std::string getDescription() const {
        return "Soigne " + std::to_string(value) + " HP";
    }
};

// ============================================================
//  MONSTRE
// ============================================================

enum class MonsterCategory { NORMAL, MINIBOSS, BOSS };

class Monster {
private:
    std::string name;
    MonsterCategory category;
    int hp;
    int hpMax;
    int atk;
    int def;
    int mercy;
    int mercyGoal;
    std::vector<std::string> actIds;

public:
    Monster(const std::string& n, MonsterCategory cat,
            int hp, int atk, int def, int mercyGoal,
            const std::vector<std::string>& acts)
        : name(n), category(cat), hp(hp), hpMax(hp),
          atk(atk), def(def), mercy(0), mercyGoal(mercyGoal), actIds(acts) {}

    // Accesseurs
    const std::string& getName()           const { return name; }
    MonsterCategory    getCategory()       const { return category; }
    int getHP()                            const { return hp; }
    int getHPMax()                         const { return hpMax; }
    int getAtk()                           const { return atk; }
    int getDef()                           const { return def; }
    int getMercy()                         const { return mercy; }
    int getMercyGoal()                     const { return mercyGoal; }
    const std::vector<std::string>& getActIds() const { return actIds; }
    bool isAlive()                         const { return hp > 0; }
    bool isMercyFull()                     const { return mercy >= mercyGoal; }

    int getActCount() const {
        if (category == MonsterCategory::BOSS)     return 4;
        if (category == MonsterCategory::MINIBOSS) return 3;
        return 2;
    }

    std::string getCategoryString() const {
        if (category == MonsterCategory::BOSS)     return "BOSS";
        if (category == MonsterCategory::MINIBOSS) return "MINIBOSS";
        return "NORMAL";
    }

    void setHP(int val) { hp = std::max(0, std::min(val, hpMax)); }

    void modifyMercy(int delta) {
        mercy = std::max(0, std::min(mercy + delta, mercyGoal));
    }

    // Attaque une cible (joueur ou autre), retourne les degats
    int attack(int targetHPMax, int& targetHP) {
        int dmg = rollDamage(targetHPMax);
        targetHP = std::max(0, targetHP - dmg);
        return dmg;
    }

    void display() const {
        std::cout << "[" << getCategoryString() << "] " << name
                  << " | HP: " << hp << "/" << hpMax
                  << " | ATK: " << atk << " | DEF: " << def
                  << " | Mercy: " << mercy << "/" << mercyGoal << "\n";
    }
};

// ============================================================
//  JOUEUR
// ============================================================

class Player {
private:
    std::string name;
    int hp;
    int hpMax;
    int victories;
    int killed;
    int spared;
    std::vector<Item> inventory;

public:
    Player() : name("???"), hp(100), hpMax(100), victories(0), killed(0), spared(0) {}
    Player(const std::string& n, int hpMax = 100)
        : name(n), hp(hpMax), hpMax(hpMax), victories(0), killed(0), spared(0) {}

    // Accesseurs
    const std::string& getName()    const { return name; }
    int getHP()                     const { return hp; }
    int getHPMax()                  const { return hpMax; }
    int getVictories()              const { return victories; }
    int getKilled()                 const { return killed; }
    int getSpared()                 const { return spared; }
    bool isAlive()                  const { return hp > 0; }

    std::vector<Item>&       getInventory()       { return inventory; }
    const std::vector<Item>& getInventory() const { return inventory; }

    void setHP(int val) { hp = std::max(0, std::min(val, hpMax)); }
    void heal(int amount) { hp = std::min(hp + amount, hpMax); }

    void addItem(const Item& item) {
        for (auto& i : inventory) {
            if (i.getName() == item.getName()) {
                i.setQuantity(i.getQuantity() + item.getQuantity());
                return;
            }
        }
        inventory.push_back(item);
    }

    void addVictory(bool wasKilled) {
        victories++;
        if (wasKilled) killed++;
        else spared++;
    }

    bool useItem(int index) {
        if (index < 0 || index >= (int)inventory.size()) return false;
        Item& item = inventory[index];
        if (item.getQuantity() <= 0) return false;
        if (item.use()) {
            heal(item.getValue());
            return true;
        }
        return false;
    }

    void display() const {
        std::cout << "Joueur    : " << name << "\n"
                  << "HP        : " << hp << " / " << hpMax << "\n"
                  << "Victoires : " << victories << " / 10\n"
                  << "Tues      : " << killed << "  |  Epargnes : " << spared << "\n";
    }

    void displayInventory() const {
        if (inventory.empty()) {
            std::cout << "  Inventaire vide.\n";
            return;
        }
        for (int i = 0; i < (int)inventory.size(); i++) {
            const Item& it = inventory[i];
            std::cout << "  [" << (i + 1) << "] " << it.getName()
                      << " x" << it.getQuantity()
                      << "  -  " << it.getDescription() << "\n";
        }
    }
};

// ============================================================
//  BESTIAIRE
// ============================================================

struct BestiaryEntry {
    std::string name;
    std::string category;
    int hpMax, atk, def;
    bool wasKilled;
};

class Bestiary {
private:
    std::vector<BestiaryEntry> entries;

public:
    void addEntry(const Monster& m, bool killed) {
        entries.push_back({ m.getName(), m.getCategoryString(),
                            m.getHPMax(), m.getAtk(), m.getDef(), killed });
    }

    bool isEmpty() const { return entries.empty(); }

    void display() const {
        if (entries.empty()) {
            std::cout << "  Aucun monstre vaincu pour l'instant.\n";
            return;
        }
        std::cout << std::left
                  << std::setw(16) << "Nom"
                  << std::setw(10) << "Categorie"
                  << std::setw(8)  << "HP max"
                  << std::setw(6)  << "ATK"
                  << std::setw(6)  << "DEF"
                  << "Resultat\n";
        printSep('=');
        for (const auto& e : entries) {
            std::cout << std::setw(16) << e.name
                      << std::setw(10) << e.category
                      << std::setw(8)  << e.hpMax
                      << std::setw(6)  << e.atk
                      << std::setw(6)  << e.def
                      << (e.wasKilled ? "TUE" : "EPARGNE") << "\n";
        }
    }
};

// ============================================================
//  CHARGEMENT DES FICHIERS
// ============================================================

std::vector<Item> loadItems(const std::string& filename) {
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
            std::cerr << "[AVERT] items.csv ligne " << lineNum << " ignoree (mal formee).\n";
            continue;
        }
        try {
            items.emplace_back(t[0], std::stoi(t[2]), std::stoi(t[3]));
        }
        catch (...) {
            std::cerr << "[AVERT] items.csv ligne " << lineNum << " ignoree (valeurs invalides).\n";
        }
    }
    return items;
}

std::vector<Monster> loadMonsters(const std::string& filename) {
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
            std::cerr << "[AVERT] monsters.csv ligne " << lineNum << " ignoree (mal formee).\n";
            continue;
        }
        MonsterCategory cat;
        int actCount;
        if      (t[0] == "NORMAL")   { cat = MonsterCategory::NORMAL;   actCount = 2; }
        else if (t[0] == "MINIBOSS") { cat = MonsterCategory::MINIBOSS; actCount = 3; }
        else if (t[0] == "BOSS")     { cat = MonsterCategory::BOSS;     actCount = 4; }
        else {
            std::cerr << "[AVERT] monsters.csv ligne " << lineNum << " categorie inconnue, ignoree.\n";
            continue;
        }
        int hp, atk, def, mercyGoal;
        try {
            hp        = std::stoi(t[2]);
            atk       = std::stoi(t[3]);
            def       = std::stoi(t[4]);
            mercyGoal = std::stoi(t[5]);
        }
        catch (...) {
            std::cerr << "[AVERT] monsters.csv ligne " << lineNum << " stats invalides, ignoree.\n";
            continue;
        }
        std::vector<std::string> actIds;
        for (int i = 0; i < actCount; i++) {
            size_t idx = 6 + i;
            if (idx >= t.size()) break;
            if (t[idx] == "-" || t[idx].empty()) continue;
            if (!getAction(t[idx])) {
                std::cerr << "[AVERT] Action '" << t[idx] << "' inconnue ligne " << lineNum << ", ignoree.\n";
                continue;
            }
            actIds.push_back(t[idx]);
        }
        if (actIds.empty()) {
            std::cerr << "[AVERT] Monstre '" << t[1] << "' sans actions valides, ignore.\n";
            continue;
        }
        monsters.emplace_back(t[1], cat, hp, atk, def, mercyGoal, actIds);
    }
    return monsters;
}

// ============================================================
//  JEU
// ============================================================

class Game {
private:
    Player player;
    std::vector<Monster> monsterPool;
    Bestiary bestiary;

    // ── Menus ──────────────────────────────────────────────

    void menuBestiary() {
        printSep('=');
        std::cout << "\n  *** BESTIAIRE ***\n\n";
        bestiary.display();
        printSep('=');
        pauseConsole();
    }

    void menuStats() {
        printSep('=');
        std::cout << "\n  *** STATISTIQUES ***\n\n";
        player.display();
        printSep('=');
        pauseConsole();
    }

    void menuItems() {
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
            for (auto& it : inv) if (it.getQuantity() > 0) { hasItem = true; break; }
            if (!hasItem) { std::cout << "  Inventaire vide.\n"; pauseConsole(); continue; }

            std::cout << "Quel item ? (1-" << inv.size() << ", 0=annuler) : ";
            int idx = readInt(0, (int)inv.size());
            if (idx == 0) continue;
            idx--;
            if (inv[idx].getQuantity() <= 0) {
                std::cout << "  Cet item est epuise.\n";
            }
            else {
                int hpBefore = player.getHP();
                player.useItem(idx);
                std::cout << "  Vous utilisez " << inv[idx].getName()
                          << " et recuperez " << (player.getHP() - hpBefore) << " HP.\n";
                std::cout << "  HP : " << player.getHP() << "/" << player.getHPMax() << "\n";
            }
            pauseConsole();
        }
    }

    // ── Combat ─────────────────────────────────────────────

    void doFight(Monster& m, bool& over, bool& won, bool& killed) {
        int targetHP = m.getHP();
        int dmg = rollDamage(m.getHPMax());
        targetHP = std::max(0, targetHP - dmg);
        m.setHP(targetHP);
        std::cout << "\n  Vous attaquez " << m.getName() << " !\n";
        if (dmg == 0) std::cout << "  Coup rate !\n";
        else std::cout << "  " << dmg << " degats ! HP monstre : "
                       << m.getHP() << "/" << m.getHPMax() << "\n";
        if (!m.isAlive()) {
            over = won = killed = true;
            std::cout << "\n  " << m.getName() << " est vaincu !\n";
        }
    }

    void doAct(Monster& m) {
        const auto& ids = m.getActIds();
        int count = std::min(m.getActCount(), (int)ids.size());
        std::cout << "\n  Actions ACT disponibles :\n";
        for (int i = 0; i < count; i++)
            std::cout << "  " << (i + 1) << ". " << ids[i] << "\n";
        std::cout << "  0. Annuler\nChoix : ";
        int c = readInt(0, count);
        if (c == 0) { std::cout << "  Annule.\n"; return; }

        const ActAction* action = getAction(ids[c - 1]);
        if (!action) return;

        std::cout << "\n  >> " << action->text << "\n";
        int before = m.getMercy();
        m.modifyMercy(action->mercyImpact);
        int delta = m.getMercy() - before;
        if      (delta > 0) std::cout << "  Mercy +" << delta;
        else if (delta < 0) std::cout << "  Mercy " << delta;
        else                std::cout << "  Mercy inchangee";
        std::cout << " (" << m.getMercy() << "/" << m.getMercyGoal() << ")\n";
        if (m.isMercyFull())
            std::cout << "  Le monstre est apaise ! Utilisez MERCY pour l'epargner.\n";
    }

    void doItem(bool& turnConsumed) {
        auto& inv = player.getInventory();
        bool hasItem = false;
        for (auto& it : inv) if (it.getQuantity() > 0) { hasItem = true; break; }
        if (!hasItem) { std::cout << "\n  Inventaire vide.\n"; turnConsumed = false; return; }

        std::cout << "\n  Inventaire :\n";
        player.displayInventory();
        std::cout << "  0. Annuler\nChoix : ";
        int idx = readInt(0, (int)inv.size());
        if (idx == 0) { std::cout << "  Annule.\n"; turnConsumed = false; return; }
        idx--;
        if (inv[idx].getQuantity() <= 0) {
            std::cout << "  Cet item est epuise.\n"; turnConsumed = false; return;
        }
        int hpBefore = player.getHP();
        player.useItem(idx);
        std::cout << "  Vous utilisez " << inv[idx].getName()
                  << " et recuperez " << (player.getHP() - hpBefore) << " HP. "
                  << "HP : " << player.getHP() << "/" << player.getHPMax() << "\n";
        turnConsumed = true;
    }

    void doMercy(Monster& m, bool& over, bool& won) {
        if (!m.isMercyFull()) {
            std::cout << "\n  Mercy insuffisante (" << m.getMercy()
                      << "/" << m.getMercyGoal() << "). Utilisez ACT.\n";
            return;
        }
        over = won = true;
        std::cout << "\n  Vous epargnez " << m.getName()
                  << ".\n  Il vous regarde avec incredulite... puis disparait.\n";
    }

    bool runCombat(Monster monster) {
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
                case 2: doAct(monster);  break;
                case 3: doItem(turnConsumed); break;
                case 4: doMercy(monster, over, won); break;
            }

            if (over) break;
            if (!turnConsumed) continue; // annulation : le monstre n'attaque pas

            // Tour du monstre
            int playerHP = player.getHP();
            int dmg = rollDamage(player.getHPMax());
            playerHP = std::max(0, playerHP - dmg);
            player.setHP(playerHP);
            std::cout << "\n  " << monster.getName() << " attaque !\n";
            if (dmg == 0) std::cout << "  Coup rate !\n";
            else std::cout << "  Vous subissez " << dmg << " degats ! HP : "
                           << player.getHP() << "/" << player.getHPMax() << "\n";

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

    // ── Fin de partie ──────────────────────────────────────

    void displayEndGame() const {
        int k = player.getKilled(), s = player.getSpared();
        printSep('=');
        std::cout << "\n  *** FIN DE PARTIE ***\n\n";
        std::cout << "  " << player.getName() << " a accompli 10 victoires !\n\n";
        std::cout << "  Monstres tues     : " << k << "\n";
        std::cout << "  Monstres epargnes : " << s << "\n\n";

        if (k == 0 && s > 0) {
            std::cout << "  ~ FIN PACIFISTE ~\n";
            std::cout << "  Vous avez traverse ce monde sans verser une goutte de sang.\n";
            std::cout << "  Les monstres se souviennent de votre compassion.\n";
        }
        else if (s == 0 && k > 0) {
            std::cout << "  ~ FIN GENOCIDAIRE ~\n";
            std::cout << "  Vous avez tout ecrase sur votre passage.\n";
            std::cout << "  Le silence qui vous entoure est lourd de consequence.\n";
        }
        else {
            std::cout << "  ~ FIN NEUTRE ~\n";
            std::cout << "  Vous avez montre a la fois violence et clemence.\n";
            std::cout << "  Deux chemins, une seule verite.\n";
        }
        printSep('=');
    }

    // ── Titre ──────────────────────────────────────────────

    void printTitle() const {
        std::cout << "\n";
        std::cout << "   ___  _   _____  ___  ___  ___  _   _ _  _ ___ \n";
        std::cout << "  / _ \\| | |_   _|| __|| _ \\|   \\| | | | \\| | __|\n";
        std::cout << " | (_) | |__ | |  | _| |   /| |) | |_| | .` | _| \n";
        std::cout << "  \\___/|____||_|  |___||_|_\\|___/|___|_|_|\\_|___|\n";
        std::cout << "\n  Un RPG minimaliste en C++\n\n";
    }

public:

    void init(const std::string& itemsFile, const std::string& monstersFile) {
        printTitle();
        std::cout << "Entrez le nom de votre personnage : ";
        std::string name;
        std::getline(std::cin, name);
        name = trim(name);
        if (name.empty()) name = "Guerrier";
        player = Player(name, 100);

        auto items = loadItems(itemsFile);
        for (const auto& it : items) player.addItem(it);

        monsterPool = loadMonsters(monstersFile);
        if (monsterPool.empty()) {
            std::cerr << "[ERREUR] Aucun monstre valide. Arret.\n";
            exit(1);
        }

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

    void run() {
        while (player.getVictories() < 10 && player.isAlive()) {
            printSep('=');
            std::cout << "\n  *** MENU PRINCIPAL ***\n\n";
            std::cout << "  Victoires : " << player.getVictories() << " / 10\n";
            std::cout << "  HP : " << player.getHP() << "/" << player.getHPMax() << "\n\n";
            printSep();
            std::cout << "  1. Bestiaire\n";
            std::cout << "  2. Demarrer un combat\n";
            std::cout << "  3. Statistiques\n";
            std::cout << "  4. Items\n";
            std::cout << "  5. Quitter\n";
            printSep('=');
            std::cout << "Choix : ";
            int c = readInt(1, 5);

            switch (c) {
                case 1: menuBestiary(); break;
                case 2: {
                    std::uniform_int_distribution<int> d(0, (int)monsterPool.size() - 1);
                    Monster m = monsterPool[d(rng)];
                    printSep('=');
                    std::cout << "\n  Un monstre apparait !\n\n";
                    m.display();
                    printSep('=');
                    pauseConsole();
                    runCombat(m);
                    pauseConsole();
                    break;
                }
                case 3: menuStats(); break;
                case 4: menuItems(); break;
                case 5:
                    std::cout << "Au revoir !\n";
                    return;
            }

            if (!player.isAlive()) {
                printSep('=');
                std::cout << "\n  GAME OVER — " << player.getName() << " a ete vaincu.\n";
                printSep('=');
                return;
            }
        }

        if (player.getVictories() >= 10)
            displayEndGame();
    }
};

// ============================================================
//  MAIN
// ============================================================

int main() {
    Game game;
    game.init("data/items.csv", "data/monsters.csv");
    game.run();
    return 0;
}
