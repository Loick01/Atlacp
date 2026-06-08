#pragma once

#include <vector>

class BattleActor;

class BattleBehaviour 
{
    private:
    public:
        BattleBehaviour() = default;

        BattleActor* SelectTarget(const std::vector<BattleActor*> actors) const;
};