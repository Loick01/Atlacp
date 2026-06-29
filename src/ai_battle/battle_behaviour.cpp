#include "ai_battle/battle_behaviour.hpp"

#include "battle/battle_actor.hpp"

BattleActor* BattleBehaviour::SelectTarget(const std::vector<BattleActor*> actors) const
{
    return actors[rand()%actors.size()]; // TODO : Actor selection
}