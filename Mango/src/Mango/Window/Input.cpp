#include "mgpch.h"
#include "Input.h"
#include "imgui.h"

namespace mango
{
    GLFWwindow * Input::m_window = nullptr;

    uint8_t Input::m_activeGamepadsCount = 1;

    std::unordered_map<KeyCode, bool> Input::m_lastKeysStates = {
        { KeyCode::Backspace,      false },
        { KeyCode::Delete,         false },
        { KeyCode::Tab,            false },
        { KeyCode::Return,         false },
        { KeyCode::Pause,          false },
        { KeyCode::Escape,         false },
        { KeyCode::Space,          false },
        { KeyCode::Keypad0,        false },
        { KeyCode::Keypad1,        false },
        { KeyCode::Keypad2,        false },
        { KeyCode::Keypad3,        false },
        { KeyCode::Keypad4,        false },
        { KeyCode::Keypad5,        false },
        { KeyCode::Keypad6,        false },
        { KeyCode::Keypad7,        false },
        { KeyCode::Keypad8,        false },
        { KeyCode::Keypad9,        false },
        { KeyCode::KeypadPeriod,   false },
        { KeyCode::KeypadDivide,   false },
        { KeyCode::KeypadMultiply, false },
        { KeyCode::KeypadMinus,    false },
        { KeyCode::KeypadPlus,     false },
        { KeyCode::KeypadEnter,    false },
        { KeyCode::KeypadEquals,   false },
        { KeyCode::UpArrow,        false },
        { KeyCode::DownArrow,      false },
        { KeyCode::RightArrow,     false },
        { KeyCode::LeftArrow,      false },
        { KeyCode::Insert,         false },
        { KeyCode::Home,           false },
        { KeyCode::End,            false },
        { KeyCode::PageUp,         false },
        { KeyCode::PageDown,       false },
        { KeyCode::F1,             false },
        { KeyCode::F2,             false },
        { KeyCode::F3,             false },
        { KeyCode::F4,             false },
        { KeyCode::F5,             false },
        { KeyCode::F6,             false },
        { KeyCode::F7,             false },
        { KeyCode::F8,             false },
        { KeyCode::F9,             false },
        { KeyCode::F10,            false },
        { KeyCode::F11,            false },
        { KeyCode::F12,            false },
        { KeyCode::F13,            false },
        { KeyCode::F14,            false },
        { KeyCode::F15,            false },
        { KeyCode::Alpha0,         false },
        { KeyCode::Alpha1,         false },
        { KeyCode::Alpha2,         false },
        { KeyCode::Alpha3,         false },
        { KeyCode::Alpha4,         false },
        { KeyCode::Alpha5,         false },
        { KeyCode::Alpha6,         false },
        { KeyCode::Alpha7,         false },
        { KeyCode::Alpha8,         false },
        { KeyCode::Alpha9,         false },
        { KeyCode::Comma,          false },
        { KeyCode::Minus,          false },
        { KeyCode::Period,         false },
        { KeyCode::Slash,          false },
        { KeyCode::Semicolon,      false },
        { KeyCode::Equals,         false },
        { KeyCode::LeftBracket,    false },
        { KeyCode::RightBracket,   false },
        { KeyCode::Backslash,      false },
        { KeyCode::A,              false },
        { KeyCode::B,              false },
        { KeyCode::C,              false },
        { KeyCode::D,              false },
        { KeyCode::E,              false },
        { KeyCode::F,              false },
        { KeyCode::G,              false },
        { KeyCode::H,              false },
        { KeyCode::I,              false },
        { KeyCode::J,              false },
        { KeyCode::K,              false },
        { KeyCode::L,              false },
        { KeyCode::M,              false },
        { KeyCode::N,              false },
        { KeyCode::O,              false },
        { KeyCode::P,              false },
        { KeyCode::Q,              false },
        { KeyCode::R,              false },
        { KeyCode::S,              false },
        { KeyCode::T,              false },
        { KeyCode::U,              false },
        { KeyCode::V,              false },
        { KeyCode::W,              false },
        { KeyCode::X,              false },
        { KeyCode::Y,              false },
        { KeyCode::Z,              false },
        { KeyCode::Numlock,        false },
        { KeyCode::CapsLock,       false },
        { KeyCode::ScrollLock,     false },
        { KeyCode::RightShift,     false },
        { KeyCode::LeftShift,      false },
        { KeyCode::RightControl,   false },
        { KeyCode::LeftControl,    false },
        { KeyCode::RightAlt,       false },
        { KeyCode::LeftAlt,        false },
        { KeyCode::PrintScreen,    false },
        { KeyCode::Menu,           false }
    };

    std::unordered_map<KeyCode, bool> Input::m_lastMouseStates = {
        { KeyCode::Mouse1,      false},
        { KeyCode::Mouse2,      false},
        { KeyCode::Mouse3,      false}
    };

    std::vector<GamepadState> Input::m_lastGamepadStates = {};

    void Input::init(GLFWwindow* window)
    {
        m_window = window;
        m_lastGamepadStates.resize(m_activeGamepadsCount);
    }

    void Input::update()
    {
        MG_PROFILE_ZONE_SCOPED;
        for (auto & kv : m_lastKeysStates)
        {
            kv.second = getKey(kv.first);
        }

        for (auto & kv : m_lastMouseStates)
        {
            kv.second = getMouse(kv.first);
        }

        for (uint8_t i = 0; i < m_activeGamepadsCount; ++i)
        {
            getGamepadState(GamepadID(uint8_t(GamepadID::PAD_1) + i), m_lastGamepadStates[i]);
        }
    }

    bool Input::getKey(KeyCode keyCode)
    {
        if (ImGuiIO& io = ImGui::GetIO(); io.WantCaptureKeyboard) return false;

        return glfwGetKey(m_window, static_cast<int>(keyCode)) == GLFW_PRESS;
    }

    bool Input::getKeyDown(KeyCode keyCode)
    {
        return getKey(keyCode) && !m_lastKeysStates[keyCode];
    }

    bool Input::getKeyUp(KeyCode keyCode)
    {
        return !getKey(keyCode) && m_lastKeysStates[keyCode];
    }

    bool Input::getMouse(KeyCode keyCode)
    {
        if (ImGuiIO& io = ImGui::GetIO(); io.WantCaptureMouse) return false;

        return glfwGetMouseButton(m_window, static_cast<int>(keyCode)) == GLFW_PRESS;
    }

    bool Input::getMouseDown(KeyCode keyCode)
    {
        return getMouse(keyCode) && !m_lastMouseStates[keyCode];
    }

    bool Input::getMouseUp(KeyCode keyCode)
    {
        return !getMouse(keyCode) && m_lastMouseStates[keyCode];
    }

    glm::vec2 Input::getMousePosition()
    {
        MG_PROFILE_ZONE_SCOPED;
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);

        return glm::vec2(x, y);
    }

    void Input::setMouseCursorVisibility(bool isVisible)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (isVisible)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    void Input::setMouseCursorPosition(const glm::vec2 & cursorPosition)
    {
        MG_PROFILE_ZONE_SCOPED;
        glfwSetCursorPos(m_window, cursorPosition.x, cursorPosition.y);
    }

    void Input::setActiveGamepadsCount(uint8_t count)
    {
        m_activeGamepadsCount = glm::clamp(count, static_cast<uint8_t>(1), static_cast<uint8_t>(GamepadID::COUNT));
        m_lastGamepadStates.resize(m_activeGamepadsCount);
    }

    bool Input::isGamepadPresent(GamepadID gid)
    {
        return glfwJoystickIsGamepad(static_cast<int>(gid));
    }

    std::string Input::getGamepadName(GamepadID gid)
    {
        return std::string(glfwGetGamepadName(static_cast<int>(gid)));
    }

    bool Input::getGamepadButton(GamepadID gid, GamepadButton button)
    {   
        MG_CORE_ASSERT_MSG(static_cast<int>(gid) < m_activeGamepadsCount, "Can't check game pad button state for not active pad!");

        GamepadState state;
        if (getGamepadState(gid, state))
            return state.buttons[static_cast<int>(button)];

        return false;
    }

    bool Input::getGamepadButtonDown(GamepadID gid, GamepadButton button)
    {
        return getGamepadButton(gid, button) && !m_lastGamepadStates[int(gid)].buttons[static_cast<int>(button)];
    }

    bool Input::getGamepadButtonUp(GamepadID gid, GamepadButton button)
    {
        return !getGamepadButton(gid, button) && m_lastGamepadStates[int(gid)].buttons[static_cast<int>(button)];
    }

    float Input::getGamepadAxis(GamepadID gid, GamepadAxis axis)
    {
        MG_CORE_ASSERT_MSG(static_cast<int>(gid) < m_activeGamepadsCount, "Can't check game pad axis state for not active pad!");
        return m_lastGamepadStates[static_cast<int>(gid)].axes[static_cast<int>(axis)];
    }

    bool Input::getGamepadState(GamepadID gid, GamepadState& state)
    {
        return glfwGetGamepadState(static_cast<int>(gid), &state);
    }
}