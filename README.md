# ALTERDUNE

RPG minimaliste au tour par tour en C++, jouable en console, avec un système de combat inspiré d'*Undertale* : chaque monstre peut être vaincu par la force... ou convaincu d'abandonner.

## Le jeu

Le joueur affronte des monstres tirés d'un bestiaire chargé depuis un fichier de données. À chaque tour de combat, 4 choix sont possibles :
- **FIGHT** — attaque directe, inflige des dégâts
- **ACT** — interagit avec le monstre (blague, compliment, insulte...) pour faire évoluer sa jauge de "Mercy"
- **ITEM** — utilise un objet de l'inventaire (soin)
- **MERCY** — épargne le monstre, disponible uniquement une fois sa jauge de Mercy remplie

Après 10 victoires, la partie se termine sur l'une de trois fins possibles selon le style de jeu adopté :
- **Fin pacifiste** — aucun monstre tué
- **Fin génocidaire** — aucun monstre épargné
- **Fin neutre** — un mélange des deux

## Architecture

- **`Entity`** — classe abstraite de base (nom, HP, méthode virtuelle pure `display()`), héritée par `Player` et `Monster`
- **`Monster`** — catégories NORMAL / MINIBOSS / BOSS, chacune avec un nombre d'actions ACT disponibles et une jauge de Mercy à remplir
- **`Player`** — inventaire, suivi des victoires (tués / épargnés)
- **`Item`** — objets de soin, quantité et valeur
- **`ActAction`** / **`ActCatalogue`** — catalogue statique des interactions ACT disponibles, chacune avec un texte et un impact sur la jauge de Mercy
- **`FileLoader`** — parsing des fichiers `.csv` (monstres, items) avec gestion des lignes invalides
- **`Bestiary`** — historique des monstres rencontrés (statistiques, vaincu ou épargné)
- **`Game`** — boucle principale, menu, gestion du combat

## Design orienté données

Le contenu du jeu (monstres, items, actions) est entièrement défini dans des fichiers `.csv` externes plutôt que codé en dur — ajouter un nouveau monstre ou objet ne nécessite aucune modification du code source.

## Stack

C++ · Programmation orientée objet (héritage, polymorphisme, classes abstraites) · Parsing de fichiers CSV

## Compiler et lancer

**Avec Visual Studio :** ouvrir `ALTERDUNE.sln`, compiler et exécuter (F5).

**En ligne de commande (g++) :**
```
g++ -std=c++17 *.cpp -o alterdune
./alterdune
```

Le jeu attend un dossier `data/` à la racine, contenant `items.csv` et `monsters.csv`.
