#pragma once

#include "ai_battle/battle_behaviour.hpp"
#include "battle/battle_actor.hpp"
#include "battle/battle_types.hpp"

class AiActor : public BattleActor
{
    private:    
        BattleBehaviour m_behaviour;
        
    public:
        AiActor(const Team team, const ElementKey& nameId, const ElementKey& healthId, const ElementKey& pathId, 
            const std::string name, const unsigned int health, const unsigned int turnSpeed);
        
        const BattleBehaviour& GetBehaviour() const;
};