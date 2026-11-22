#pragma once

#include <vector>

#include "battle/battle_types.hpp" // MoveDefinition

class BattleActor;

enum class AiAction
{
    Fight, Flee, Undefined // TODO : Remove Undefined ?
};

class BattleBehaviour 
{
    private:
        AiAction m_currentAction;

    public:
        BattleBehaviour();

        AiAction ChooseAiAction(); // TODO : Rename
        MoveDefinition GetAiMove(const std::vector<MoveDefinition>& moves) const; // TODO : Rename
        BattleActor* SelectTarget(const std::vector<BattleActor*>& actors) const; // TODO : Rename

};