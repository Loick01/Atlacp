#include "event/action.hpp"

#include <stdexcept>

// KEYBOARD STATE EVENT

void KeyboardActionController::GetStateActions()
{
    m_state = SDL_GetKeyboardState(NULL);
}

bool KeyboardActionController::IsLeftActionState()
{
    return m_state[SDL_SCANCODE_A];
}

bool KeyboardActionController::IsRightActionState()
{
    return m_state[SDL_SCANCODE_D];
}

bool KeyboardActionController::IsUpActionState()
{
    return m_state[SDL_SCANCODE_W];
}

bool KeyboardActionController::IsDownActionState()
{
    return m_state[SDL_SCANCODE_S];
}

bool KeyboardActionController::IsPrimaryActionState()
{
    return m_state[SDL_SCANCODE_SPACE];
}

bool KeyboardActionController::IsSecondaryActionState()
{
    return m_state[SDL_SCANCODE_LSHIFT];
}

// KEYBOARD POLL EVENT

bool KeyboardActionController::IsPressedPoll(const SDL_Event& event)
{
    return event.key.repeat == 0 && event.type == SDL_KEYDOWN;
}

bool KeyboardActionController::IsMotionPoll(const SDL_Event& event)
{
    return event.key.repeat == 0 && event.type == SDL_KEYDOWN; // Same than KeyboardActionController::IsPressedPoll, but JoystickActionController::IsMotionPoll use a different event type
}

bool KeyboardActionController::IsLeftActionPoll(const SDL_Event& event)
{
    return event.key.keysym.scancode == SDL_SCANCODE_A;
}

bool KeyboardActionController::IsRightActionPoll(const SDL_Event& event)
{
    return event.key.keysym.scancode == SDL_SCANCODE_D;
}

bool KeyboardActionController::IsUpActionPoll(const SDL_Event& event)
{
    return event.key.keysym.scancode == SDL_SCANCODE_W;
}

bool KeyboardActionController::IsDownActionPoll(const SDL_Event& event)
{
    return event.key.keysym.scancode == SDL_SCANCODE_S;
}

bool KeyboardActionController::IsPrimaryActionPoll(const SDL_Event& event)
{
    // SDL_Scancode eventScancode = event.key.keysym.scancode;
    return event.key.keysym.scancode == SDL_SCANCODE_SPACE;
}

bool KeyboardActionController::IsSecondaryActionPoll(const SDL_Event& event)
{
    return event.key.keysym.scancode == SDL_SCANCODE_LSHIFT;
}

// JOYSTICK

JoystickActionController::JoystickActionController() :
    m_joystick(nullptr), m_joystickState(JoystickState::Neutral)
{
    m_joystick = SDL_JoystickOpen(0);
    if (m_joystick == nullptr)
        throw std::runtime_error("Failed to open joystick\n" + std::string(SDL_GetError()));
}

bool JoystickActionController::IsJoystickAvailable()
{
    const int joystick = SDL_Init(SDL_INIT_JOYSTICK);
    if (joystick==0)
        return SDL_NumJoysticks() != 0;
    else
        throw std::runtime_error("Unable to initialize joystick system\n" + std::string(SDL_GetError()));
}

/* If I need to know when the joystick is removed
for (SDL_Event event : m_events){
    switch (event.type){
        case SDL_JOYDEVICEREMOVED: {
            //m_joystick = nullptr;
            std::cout << "Joystick with index " << event.jdevice.which << " was removed.\n"; // Throw an error
            break;
        }
    }
}

// When I need to know joystick button id (move this in JoystickActionController::GetStateActions)
int buttonCount = SDL_JoystickNumButtons(m_joystick);

for (int i = 0; i < buttonCount; i++) {
    Uint8 state = SDL_JoystickGetButton(m_joystick, i);

    if (state){
        std::cout << "Button " << i << " pressed\n";
    }
}
*/

// JOYSTICK STATE EVENT

void JoystickActionController::GetStateActions()
{
    m_axisX = SDL_JoystickGetAxis(m_joystick, 0);
    m_axisY = SDL_JoystickGetAxis(m_joystick, 1);
}

bool JoystickActionController::IsLeftActionState()
{
    return m_axisX < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsRightActionState()
{
    return m_axisX > JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsUpActionState()
{
    return m_axisY < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsDownActionState()
{
    return m_axisY > JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsPrimaryActionState()
{
    return SDL_JoystickGetButton(m_joystick, 1);
}

bool JoystickActionController::IsSecondaryActionState()
{
    return SDL_JoystickGetButton(m_joystick, 2);
}

// JOYSTICK POLL EVENT

bool JoystickActionController::IsPressedPoll(const SDL_Event& event)
{
    return event.type == SDL_JOYBUTTONDOWN;
}

bool JoystickActionController::IsMotionPoll(const SDL_Event& event)
{
    if (m_joystickState != JoystickState::Neutral) {
        if (std::abs(event.jaxis.value) < JOYSTICK_DEAD_ZONE) {
            m_joystickState = JoystickState::Neutral; // Not working properly
        }
    }
    return m_joystickState == JoystickState::Neutral && event.type == SDL_JOYAXISMOTION;
}

bool JoystickActionController::IsLeftActionPoll(const SDL_Event& event)
{
    if (event.jaxis.axis == 0 && event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
        m_joystickState = JoystickState::Active;
        return true;
    }
    return false;
}

bool JoystickActionController::IsRightActionPoll(const SDL_Event& event)
{
    if (event.jaxis.axis == 0 && event.jaxis.value > JOYSTICK_DEAD_ZONE) {
        m_joystickState = JoystickState::Active;
        return true;
    }
    return false;
}

bool JoystickActionController::IsUpActionPoll(const SDL_Event& event)
{
    if (event.jaxis.axis == 1 && event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
        m_joystickState = JoystickState::Active;
        return true;
    }
    return false;
}

bool JoystickActionController::IsDownActionPoll(const SDL_Event& event)
{
    if (event.jaxis.axis == 1 && event.jaxis.value > JOYSTICK_DEAD_ZONE) {
        m_joystickState = JoystickState::Active;
        return true;
    }
    return false;
}

bool JoystickActionController::IsPrimaryActionPoll(const SDL_Event& event)
{
    return event.jbutton.button == 1;
}

bool JoystickActionController::IsSecondaryActionPoll(const SDL_Event& event)
{
    return event.jbutton.button == 2;
}