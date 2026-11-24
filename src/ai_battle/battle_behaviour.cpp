#include "ai_battle/battle_behaviour.hpp"

#include "battle/battle_actor.hpp"

BattleBehaviour::BattleBehaviour():
    m_currentAction(AiAction::Undefined)
{}

AiAction BattleBehaviour::ChooseAiAction()
{
    m_currentAction = rand()%2 == 0 ? AiAction::MakeMove : AiAction::Flee; // TODO
    return m_currentAction;
}

MoveDefinition BattleBehaviour::GetAiMove(const std::vector<MoveDefinition>& moves) const
{
    return moves.at(rand()%moves.size()); // TODO
}

BattleActor* BattleBehaviour::SelectTarget(const std::vector<BattleActor*>& actors) const
{
    if (actors.empty()) {
        Logger::GetInstance().AddLog(Level::L4, "(BattleBehaviour::SelectTarget) BattleActors list is empty"); // TODO : Level
        return nullptr;
    }
    return actors.at(rand()%actors.size()); // TODO
}