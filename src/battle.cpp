#include "battle.hpp"

BattleActor::BattleActor(const ElementKey& nameId, const ElementKey& healthId, const std::string name, const unsigned int health):
    m_name(nameId, name), m_health(healthId, health), m_strength(10)
{}

ElementKey BattleActor::GetHealthId() const
{
    return m_health.id;
}

ElementKey BattleActor::GetNameId() const
{
    return m_name.id;
}

std::string BattleActor::GetName() const
{
    return m_name.value;
}

unsigned int BattleActor::GetHealth() const
{
    return m_health.value;
}

unsigned int BattleActor::GetStrength() const
{
    return m_strength;
}

void BattleActor::TakeDamage(const unsigned int strength)
{
    m_health.value -= strength;
}

BattleController::BattleController(const BattleActor actorA, const BattleActor actorB):
    m_uiController(nullptr), m_actorA(actorA), m_actorB(actorB), m_currentTurn(Turn::ActorA), m_selectedOption(0)
{}

void BattleController::SetUiController(UiController* uiController)
{
    m_uiController = uiController;
}

void BattleController::UpdateStatus()
{
    m_uiController->UpdateText(m_actorA.GetNameId(), m_actorA.GetName()); 
    m_uiController->UpdateText(m_actorB.GetNameId(), m_actorB.GetName());
    m_uiController->UpdateText(m_actorA.GetHealthId(), std::to_string(m_actorA.GetHealth())); 
    m_uiController->UpdateText(m_actorB.GetHealthId(), std::to_string(m_actorB.GetHealth()));
}

void BattleController::CheckActorHealth()
{
    if (m_actorA.GetHealth() <= 0)
        Notify(ExitEvent::ExitLost);
    else if (m_actorB.GetHealth() <= 0)
        Notify(ExitEvent::ExitWin);
}

void BattleController::CreateSelect(const unsigned int targetOption)
{
    std::unique_ptr<UiElement> select = m_uiController->CreateElement("select", "../assets/ui/arrow.png");
    UiParams& selectParams = select->GetParams();
    const std::string parentId = "option" + std::to_string(targetOption);
    selectParams.scale = m_uiController->GetPartialElementSizeOnAxis(parentId, Axis::Height, 0.8f);
    selectParams.scaleAxis = Axis::Width;
    selectParams.xAnchor = Anchor::LeftOut;
    selectParams.yAnchor = Anchor::Center;
    m_uiController->GetElement(parentId)->BuildChild(std::move(select));
}

void BattleController::InitPlayerTurn()
{
    for (unsigned int i = 0 ; i < 4 ; i++) {
        std::unique_ptr<TextArea> option = m_uiController->CreateTextElement("option"+std::to_string(i));
        option->SetText("Option " + std::to_string(i));
        UiParams& optionParams = option->GetParams();
        optionParams.scale = m_uiController->GetPartialElementSizeOnAxis("mainBox", Axis::Width, 0.5f);
        optionParams.xAnchor = Anchor::LeftIn;
        optionParams.yAnchor = Anchor::TopIn;
        optionParams.yPadding = m_uiController->GetPartialElementSizeOnAxis("mainBox", Axis::Height, 0.18f);
        if (i==0) {
            optionParams.xPadding = m_uiController->GetPartialElementSizeOnAxis("mainBox", Axis::Width, 0.05f);
            m_uiController->GetElement("mainBox")->BuildChild(std::move(option));
        } else
            m_uiController->GetElement("option"+std::to_string(i-1))->BuildChild(std::move(option));
    }

    CreateSelect(m_selectedOption);

    m_uiController->GetElement("mainBox")->UpdatePosition();
}

void BattleController::PlayTurn(BattleActor& source, BattleActor& target)
{
    target.TakeDamage(source.GetStrength());
    CheckActorHealth();
    m_uiController->UpdateText(target.GetHealthId(), std::to_string(m_actorB.GetHealth()));  // I will add UiController::UpdateText(UiValue)
}

void BattleController::PlayFight()
{
    switch(m_currentTurn) {
        case Turn::ActorA : {
            if (m_eventState.uiDirection == Direction::Down) {
                m_selectedOption = (m_selectedOption+1)%4;
                m_uiController->DeleteElement("select");
                CreateSelect(m_selectedOption);
                m_uiController->GetElement("mainBox")->UpdatePosition();
            } else if (m_eventState.uiDirection == Direction::Up) {
                m_selectedOption = (m_selectedOption-1+4)%4;
                m_uiController->DeleteElement("select");
                CreateSelect(m_selectedOption);
                m_uiController->GetElement("mainBox")->UpdatePosition();
            } else if (m_eventState.isAction) { // TODO
                PlayTurn(m_actorA, m_actorB);
                // Will not be here ?
                // m_uiController->UpdateText("mainText", "Turn B");
                m_uiController->DeleteElement("option0");
                m_currentTurn = Turn::ActorB; // SwitchTurn function (could be in PlayTurn) ?
            }   
            break;
        }
        case Turn::ActorB : {
            if (m_eventState.isAction) { // TODO
                PlayTurn(m_actorB, m_actorA);
                // Will not be here ?
                // m_uiController->UpdateText("mainText", "Turn A");
                InitPlayerTurn();
                m_currentTurn = Turn::ActorA; // SwitchTurn function (could be in PlayTurn) ?
            }
            break;
        }
    }
}