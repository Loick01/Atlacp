#include "battle/battle_actor.hpp"

BattleActor::BattleActor(const Team team, const UiKey& nameId, const UiKey& healthId, const UiKey& pathId, 
const std::string name, const unsigned int health, const unsigned int turnSpeed):
    m_team(team), m_name(nameId, name), m_health(healthId, health), m_spritePath(pathId, "none"), m_lifeState(LifeState::Alive), // UiValue initialization will not be here 
    m_maxHealth(health), m_strength(10), m_turnSpeed(turnSpeed) // m_turnSpeed will not be here ?
{}

UiValue<std::string> BattleActor::GetName() const
{
    return m_name;
}

UiValue<std::string> BattleActor::GetSpritePath() const
{
    return m_spritePath;
}

UiValue<unsigned int> BattleActor::GetHealth() const
{
    return m_health;
}

Team BattleActor::GetTeam() const
{
    return m_team;
}

LifeState BattleActor::GetLifeState() const
{
    return m_lifeState;
}

float BattleActor::GetNextTurnTime() const
{
    return m_nextTurnTime;
}

unsigned int BattleActor::GetStrength() const
{
    return m_strength;
}

void BattleActor::ComputeNextTurnTime(const float turnTime)
{
    m_nextTurnTime = turnTime + 100.f/m_turnSpeed;
}

void BattleActor::SetSpritePath(const std::string& path)
{
    m_spritePath.value = path;
}

void BattleActor::AddHealth(const unsigned int hp)
{ 
    m_health.value = std::min(m_health.value+hp, m_maxHealth);
}

void BattleActor::RemoveHealth(const unsigned int hp)
{ 
    if (m_health.value <= hp) {
        m_health.value = 0;
        m_lifeState = LifeState::Dead;
    } else {
        m_health.value -= hp;
    }
}