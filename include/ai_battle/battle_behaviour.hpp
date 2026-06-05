#pragma once

#include "battle/battle_actor.hpp"

class BattleBehaviour 
{
    private:
    public:
        BattleBehaviour() = default;

        BattleActor* SelectTarget(const std::vector<BattleActor*> actors) const;
};