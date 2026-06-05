#include "ai_battle/battle_behaviour.hpp"

BattleActor* BattleBehaviour::SelectTarget(const std::vector<BattleActor*> actors) const
{
    return actors[rand()%actors.size()];
}