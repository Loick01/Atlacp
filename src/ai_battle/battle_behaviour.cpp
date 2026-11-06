#include "ai_battle/battle_behaviour.hpp"

#include "battle/battle_actor.hpp"

BattleActor* BattleBehaviour::SelectTarget(const std::vector<BattleActor*>& actors) const
{
    // TODO : Return nullptr if actors.empty() ?
    return actors[rand()%actors.size()];
}

// TODO