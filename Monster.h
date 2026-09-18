#pragma once
#include "Entity.h"
#include <vector>
#include <string>

enum class MonsterCategory { NORMAL, MINIBOSS, BOSS };

class Monster : public Entity {
private:
    MonsterCategory          category;
    int                      atk;
    int                      def;
    int                      mercy;
    int                      mercyGoal;
    std::vector<std::string> actIds;

public:
    Monster(const std::string& name, MonsterCategory category,
            int hp, int atk, int def, int mercyGoal,
            const std::vector<std::string>& actIds);

    MonsterCategory                  getCategory()       const;
    int                              getAtk()            const;
    int                              getDef()            const;
    int                              getMercy()          const;
    int                              getMercyGoal()      const;
    const std::vector<std::string>&  getActIds()         const;
    int                              getActCount()       const;
    bool                             isMercyFull()       const;
    std::string                      getCategoryString() const;

    void modifyMercy(int delta);
    void display() const override;
};
