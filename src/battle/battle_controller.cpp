#include "battle/battle_controller.hpp"

BattleController::BattleController(UiController& uiController):
    m_uiController(uiController), m_currentTurn(Turn::Init), m_currentActor(nullptr),
    m_allyList(uiController, "../data/ui/template/battle_actor.uit"), m_opponentList(uiController, "../data/ui/template/battle_actor.uit"),
    m_allyMoveList(uiController, "../data/ui/file/ally_move_selection.uif"), m_selector(uiController, "../data/ui/template/selector.uit"),
    m_textSeries(uiController, "../data/ui/file/single_text_frame.uif")
{
    // Will not be here
    m_actors.push_back(BattleActor(Team::Ally, "actorName0_0", "actorHealth0_0", "Howler 1", 10));
    m_actors.push_back(BattleActor(Team::Ally, "actorName1_0", "actorHealth1_0", "Howler 2", 100));
    m_actors.push_back(BattleActor(Team::Opponent, "actorName0_1", "actorHealth0_1", "Bone Appetit 1", 20));
    m_actors.push_back(BattleActor(Team::Opponent, "actorName1_1", "actorHealth1_1", "Slime 2", 10));
    for (BattleActor& b : m_actors) // TODO 
        m_turns.push(&b);
}

void BattleController::InitializeActors()
{
    // Will not be here
    m_allyList.SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::RightIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, -0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));
    
    // Will not be here
    m_opponentList.SetFirstItemParams(
        UiParams(m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::TopIn, // Anchor
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Width, 0.2f)), // Padding
        m_uiController.GetResultFromPartialSize(PartialSize("background", Axis::Height, 0.05f))));
        
    m_allyList.SetNrItem(2);
    m_opponentList.SetNrItem(2);
    m_allyList.Open();
    m_opponentList.Open();
    
    // Will be removed (I used actor_placeholder.png in actor template file)
    m_uiController.UpdatePath("actorSprite0_0", "../assets/battle/ally_sprite/werewolf.png");
    m_uiController.UpdatePath("actorSprite1_0", "../assets/battle/ally_sprite/werewolf.png");
    m_uiController.UpdatePath("actorSprite0_1", "../assets/battle/opponent_sprite/bone_appetit.png");
    m_uiController.UpdatePath("actorSprite1_1", "../assets/battle/opponent_sprite/slime.png");
    
    for (unsigned int i = 0 ; i < m_actors.size() ; i++) {
        m_uiController.UpdateText(m_actors[i].GetName());
        m_uiController.UpdateText(m_actors[i].GetHealth()); 
    }
}

bool BattleController::HasAliveActor(const Team team)
{
    for (const BattleActor& b : m_actors) {
        if (b.GetTeam() == team && b.GetLifeState() == LifeState::Alive)
            return true;
    }
    return false;
}

void BattleController::CheckActorsHealth()
{
    if (!HasAliveActor(Team::Ally))
        Notify(ExitEvent::ExitLost);
    else if (!HasAliveActor(Team::Opponent))
        Notify(ExitEvent::ExitWin);
}

void BattleController::OpenAllyMoveSelection()
{
    m_allyMoveList.Open();
    m_selector.Open();
    m_selector.SetParents(m_allyMoveList.GetItemsKey());
    // When selector file is build, scale is based on root element
    m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{m_allyMoveList.GetKey(), Axis::Height, 0.8f}); 
}

void BattleController::CloseAllyMoveSelection()
{
    m_selector.Close();
    m_allyMoveList.Close();
}

unsigned int BattleController::TakeDamage(BattleActor& source, BattleActor& target)
{
    const unsigned int damage = source.GetStrength();
    target.RemoveHealth(damage);
    CheckActorsHealth();
    m_uiController.UpdateText(target.GetHealth());
    return damage;
}

unsigned int BattleController::TakeHealth(BattleActor& source) // Will use Object from inventory
{
    const unsigned int hp = source.GetStrength(); // For now, I use the source strength
    source.AddHealth(hp);
    m_uiController.UpdateText(source.GetHealth());
    return hp;
}

void BattleController::HandleAllyMoveSelection(BattleActor& srcActor, const int selectorIndex)
{
    switch (selectorIndex) {
        case 0: {
            CloseAllyMoveSelection();
            m_textSeries.Open();
            m_textSeries.AddText({"Choose an opponent to attack"});
            m_textSeries.NextText();
            m_selector.Open();
            m_selector.SetParents(m_opponentList.GetItemsKey()); // TODO : BattleActor with LifeState::Dead are still selectable
            m_uiController.UpdateScalingSize(m_selector.GetKey(), PartialSize{"actorSprite0_0", Axis::Height, 0.2f}); // Not "actorSprite0_0"
            m_currentTurn = Turn::ActorSelection;
            break;
        }
        
        case 1: {
            CloseAllyMoveSelection();
            ApplyHealth(*m_currentActor);
            break;
        }
        case 2:
            break;
        case 3: {
            Notify(ExitEvent::ExitWin); // Not ExitWin
            break;
        }

        default:
            throw std::runtime_error("Selector is not supposed to be on this index : " + selectorIndex);
    }
}

void BattleController::HandleActorSelection()
{
    if (m_eventState.uiDirection == Direction::Down) {
        m_selector.Next();
    } else if (m_eventState.uiDirection == Direction::Up) {
        m_selector.Previous();
    } else if (m_eventState.isAction) {
        m_textSeries.Close();
        BattleActor& targetActor = m_actors[m_selector.GetIndex()+2]; // Remove +2 (I need it because the first 2 actor are Team::Ally, I would need a function to filter actors according to their Team)
        ApplyDamage(*m_currentActor, targetActor);
        m_selector.Close();
    }
}

void BattleController::HandleOpponentMoveSelection(BattleActor& srcActor)
{
    BattleActor& targetActor = m_actors[rand()%2]; // TODO : Behaviour
    ApplyDamage(srcActor, targetActor);
}

void BattleController::ApplyDamage(BattleActor& srcActor, BattleActor& targetActor)
{
    const unsigned int damage = TakeDamage(srcActor, targetActor); 
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " attacks " + targetActor.GetName().value + " !",
                        targetActor.GetName().value + " lost " + std::to_string(damage) + " HP !"});
    
    if (targetActor.GetLifeState() == LifeState::Dead) {
        m_textSeries.AddText({targetActor.GetName().value + " fainted !"});    
    }
    
    m_textSeries.NextText();
    m_currentTurn = Turn::Waiting;
}

void BattleController::ApplyHealth(BattleActor& srcActor)
{

    const unsigned int hp = TakeHealth(srcActor); 
    m_textSeries.Open();
    m_textSeries.AddText({srcActor.GetName().value + " drinks a potion",
                          srcActor.GetName().value + " recovered " + std::to_string(hp) + " HP !"});

    m_textSeries.NextText();
    m_currentTurn = Turn::Waiting;
}

BattleActor* BattleController::GetNextTurn()
{
    // TODO
    BattleActor* actor = nullptr;
    while (actor == nullptr) { // Should try m_turns.empty() (even if it's not possible) ?
        actor = m_turns.front();
        m_turns.pop();
        if (actor->GetLifeState() == LifeState::Dead)
            actor = nullptr; // Dead actor is not pushed in m_turns
    }
    
    m_turns.push(actor);
    return actor;
}

void BattleController::PlayNextTurn()
{
    switch (m_currentTurn) {
        case Turn::Init : {
            m_currentActor = GetNextTurn();
            if (m_currentActor->GetTeam() == Team::Ally) OpenAllyMoveSelection();
            m_currentTurn = Turn::MoveSelection; 
        }
        
        case Turn::MoveSelection : {
            switch (m_currentActor->GetTeam()) {
                case Team::Ally : {
                    if (m_eventState.uiDirection == Direction::Down) {
                        m_selector.Next();
                    } else if (m_eventState.uiDirection == Direction::Up) {
                        m_selector.Previous();
                    } else if (m_eventState.isAction) {
                        HandleAllyMoveSelection(*m_currentActor, m_selector.GetIndex());
                    }   
                    break;
                }
                case Team::Opponent : {
                    HandleOpponentMoveSelection(*m_currentActor);
                    break;
                }
                default:
                    throw std::runtime_error("Unknown Team value");
            }
            break;
        }

        case Turn::ActorSelection : {
            HandleActorSelection();
            break;
        }

        case Turn::Waiting : {
            if (m_eventState.isAction) {
                if (!m_textSeries.NextText()) {
                    m_textSeries.Close();
                    m_currentTurn = Turn::Init;
                }
            } 
            break;
        }

        default:
            throw std::runtime_error("Unknown Turn value");
    }
}