#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE 
#endif

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace mango
{
    enum class KeyCode
    {
        None           = GLFW_KEY_UNKNOWN,
        Backspace      = GLFW_KEY_BACKSPACE,
        Delete         = GLFW_KEY_DELETE,
        Tab            = GLFW_KEY_TAB,
        Return         = GLFW_KEY_ENTER,
        Pause          = GLFW_KEY_PAUSE,
        Escape         = GLFW_KEY_ESCAPE,
        Space          = GLFW_KEY_SPACE,
        Keypad0        = GLFW_KEY_KP_0,
        Keypad1        = GLFW_KEY_KP_1,
        Keypad2        = GLFW_KEY_KP_2,
        Keypad3        = GLFW_KEY_KP_3,
        Keypad4        = GLFW_KEY_KP_4,
        Keypad5        = GLFW_KEY_KP_5,
        Keypad6        = GLFW_KEY_KP_6,
        Keypad7        = GLFW_KEY_KP_7,
        Keypad8        = GLFW_KEY_KP_8,
        Keypad9        = GLFW_KEY_KP_9,
        KeypadPeriod   = GLFW_KEY_KP_DECIMAL,
        KeypadDivide   = GLFW_KEY_KP_DIVIDE,
        KeypadMultiply = GLFW_KEY_KP_MULTIPLY,
        KeypadMinus    = GLFW_KEY_KP_SUBTRACT,
        KeypadPlus     = GLFW_KEY_KP_ADD,
        KeypadEnter    = GLFW_KEY_KP_ENTER,
        KeypadEquals   = GLFW_KEY_KP_EQUAL,
        UpArrow        = GLFW_KEY_UP,
        DownArrow      = GLFW_KEY_DOWN,
        RightArrow     = GLFW_KEY_RIGHT,
        LeftArrow      = GLFW_KEY_LEFT,
        Insert         = GLFW_KEY_INSERT,
        Home           = GLFW_KEY_HOME,
        End            = GLFW_KEY_END,
        PageUp         = GLFW_KEY_PAGE_UP,
        PageDown       = GLFW_KEY_PAGE_DOWN,
        F1             = GLFW_KEY_F1,
        F2             = GLFW_KEY_F2,
        F3             = GLFW_KEY_F3,
        F4             = GLFW_KEY_F4,
        F5             = GLFW_KEY_F5,
        F6             = GLFW_KEY_F6,
        F7             = GLFW_KEY_F7,
        F8             = GLFW_KEY_F8,
        F9             = GLFW_KEY_F9,
        F10            = GLFW_KEY_F10,
        F11            = GLFW_KEY_F11,
        F12            = GLFW_KEY_F12,
        F13            = GLFW_KEY_F13,
        F14            = GLFW_KEY_F14,
        F15            = GLFW_KEY_F15,
        Alpha0         = GLFW_KEY_0,
        Alpha1         = GLFW_KEY_1,
        Alpha2         = GLFW_KEY_2,
        Alpha3         = GLFW_KEY_3,
        Alpha4         = GLFW_KEY_4,
        Alpha5         = GLFW_KEY_5,
        Alpha6         = GLFW_KEY_6,
        Alpha7         = GLFW_KEY_7,
        Alpha8         = GLFW_KEY_8,
        Alpha9         = GLFW_KEY_9,
        Comma          = GLFW_KEY_COMMA,
        Minus          = GLFW_KEY_MINUS,
        Period         = GLFW_KEY_PERIOD,
        Slash          = GLFW_KEY_SLASH,
        Semicolon      = GLFW_KEY_SEMICOLON,
        Equals         = GLFW_KEY_EQUAL,
        LeftBracket    = GLFW_KEY_LEFT_BRACKET,
        RightBracket   = GLFW_KEY_RIGHT_BRACKET,
        Backslash      = GLFW_KEY_BACKSLASH,
        A              = GLFW_KEY_A,
        B              = GLFW_KEY_B,
        C              = GLFW_KEY_C,
        D              = GLFW_KEY_D,
        E              = GLFW_KEY_E,
        F              = GLFW_KEY_F,
        G              = GLFW_KEY_G,
        H              = GLFW_KEY_H,
        I              = GLFW_KEY_I,
        J              = GLFW_KEY_J,
        K              = GLFW_KEY_K,
        L              = GLFW_KEY_L,
        M              = GLFW_KEY_M,
        N              = GLFW_KEY_N,
        O              = GLFW_KEY_O,
        P              = GLFW_KEY_P,
        Q              = GLFW_KEY_Q,
        R              = GLFW_KEY_R,
        S              = GLFW_KEY_S,
        T              = GLFW_KEY_T,
        U              = GLFW_KEY_U,
        V              = GLFW_KEY_V,
        W              = GLFW_KEY_W,
        X              = GLFW_KEY_X,
        Y              = GLFW_KEY_Y,
        Z              = GLFW_KEY_Z,
        Numlock        = GLFW_KEY_NUM_LOCK,
        CapsLock       = GLFW_KEY_CAPS_LOCK,
        ScrollLock     = GLFW_KEY_SCROLL_LOCK,
        RightShift     = GLFW_KEY_RIGHT_SHIFT,
        LeftShift      = GLFW_KEY_LEFT_SHIFT,
        RightControl   = GLFW_KEY_RIGHT_CONTROL,
        LeftControl    = GLFW_KEY_LEFT_CONTROL,
        RightAlt       = GLFW_KEY_RIGHT_ALT,
        LeftAlt        = GLFW_KEY_LEFT_ALT,
        PrintScreen    = GLFW_KEY_PRINT_SCREEN,
        Menu           = GLFW_KEY_MENU,
        Mouse1         = GLFW_MOUSE_BUTTON_1,
        Mouse2         = GLFW_MOUSE_BUTTON_2,
        Mouse3         = GLFW_MOUSE_BUTTON_3,
        MouseLeft      = GLFW_MOUSE_BUTTON_LEFT,
        MouseMiddle    = GLFW_MOUSE_BUTTON_MIDDLE,
        MouseRight     = GLFW_MOUSE_BUTTON_RIGHT
    };

    enum class GamepadID
    {
        PAD_1     = GLFW_JOYSTICK_1,
        PAD_2     = GLFW_JOYSTICK_2,
        PAD_3     = GLFW_JOYSTICK_3,
        PAD_4     = GLFW_JOYSTICK_4,
        PAD_5     = GLFW_JOYSTICK_5,
        PAD_6     = GLFW_JOYSTICK_6,
        PAD_7     = GLFW_JOYSTICK_7,
        PAD_8     = GLFW_JOYSTICK_8,
        PAD_9     = GLFW_JOYSTICK_9,
        PAD_10    = GLFW_JOYSTICK_10,
        PAD_11    = GLFW_JOYSTICK_11,
        PAD_12    = GLFW_JOYSTICK_12,
        PAD_13    = GLFW_JOYSTICK_13,
        PAD_14    = GLFW_JOYSTICK_14,
        PAD_15    = GLFW_JOYSTICK_15,
        PAD_16    = GLFW_JOYSTICK_16,
        COUNT
    };

    enum class GamepadButton
    {
        A            = GLFW_GAMEPAD_BUTTON_A,
        B            = GLFW_GAMEPAD_BUTTON_B,
        X            = GLFW_GAMEPAD_BUTTON_X,
        Y            = GLFW_GAMEPAD_BUTTON_Y,
        LEFT_BUMPER  = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        BACK         = GLFW_GAMEPAD_BUTTON_BACK,
        START        = GLFW_GAMEPAD_BUTTON_START,
        GUIDE        = GLFW_GAMEPAD_BUTTON_GUIDE,
        LEFT_THUMB   = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        RIGHT_THUMB  = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        DPAD_UP      = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        DPAD_RIGHT   = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        DPAD_DOWN    = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        DPAD_LEFT    = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        COUNT,

        // PlayStation Pad
        CROSS        = A,
        CIRCLE       = B,
        SQUARE       = X,
        TRIANGLE     = Y
    };

    enum class GamepadAxis
    {
        LEFT_X        = GLFW_GAMEPAD_AXIS_LEFT_X,
        LEFT_Y        = GLFW_GAMEPAD_AXIS_LEFT_Y,
        RIGHT_X       = GLFW_GAMEPAD_AXIS_RIGHT_X,
        RIGHT_Y       = GLFW_GAMEPAD_AXIS_RIGHT_Y,
        LEFT_TRIGGER  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
        COUNT
    };

    using GamepadState = GLFWgamepadstate;

    class Input final
    {
    public:
        Input()                          = delete;
        ~Input()                         = delete;
        Input(const Input &)             = delete;
        Input & operator=(const Input &) = delete;

        static void init(GLFWwindow * window);
        static void update();

        /**
         * @brief Check if key is pressed
         * @param KeyCode keycode
         */
        static bool getKey(KeyCode keyCode);

        /**
         * @brief Check if key was pressed
         * @param KeyCode keycode
         */
        static bool getKeyDown(KeyCode keyCode);

        /**
         * @brief Check if key was released
         * @param KeyCode keycode
         */
        static bool getKeyUp(KeyCode keyCode);

        /**
         * @brief Check if mouse button is pressed
         * @param KeyCode keycode
         */
        static bool getMouse(KeyCode keyCode);

        /**
         * @brief Check if mouse button was pressed
         * @param KeyCode keycode
         */
        static bool getMouseDown(KeyCode keyCode);

        /**
         * @brief Check if mouse button was released
         * @param KeyCode keycode
         */
        static bool getMouseUp(KeyCode keyCode);

        /**
         * @brief Get current cursor position
         * @return glm::vec2 that holds x and y coordinate
         *         of the cursor position within the
         *         current window.
         */
        static glm::vec2 getMousePosition();

        /**
         * @brief Enable or disable visibility of the cursor
         * @param bool TRUE: to show the cursor
         *             FALSE: to hide the cursor
         */
        static void setMouseCursorVisibility(bool isVisible);

        /**
         * @breif Set mouse cursor position to the desired location in screen space.
         * @param glm::vec2 cursorPosition: new position of the cursor.
         */
        static void setMouseCursorPosition(const glm::vec2 & cursorPosition);

        /**
         * @brief Set number of active game pads that the application will use.
         *        By default, the count is set to 1. That means that only one game pad
         *        will be polled for input.
         * @param uint8_t count: number of game pads that the application wants to use.
         */
        static void setActiveGamepadsCount(uint8_t count);

        /**
         * @brief Check whether the specific game pad is connected.
         * @param GamepadID gid: ID of the game pad.
         */
        static bool isGamepadPresent(GamepadID gid);

        /**
         * @brief Get the name of the game pad.
         * @param GamepadID gid: ID of the game pad.
         */
        static std::string getGamepadName(GamepadID gid);

        /**
         * @brief Check if the game pad button is pressed.
         * @param GamepadID gid: ID of the game pad.
         * @param GamepadButton button
         */
        static bool getGamepadButton(GamepadID gid, GamepadButton button);

        /**
         * @brief Check if the game pad button was pressed.
         * @param GamepadID gid: ID of the game pad.
         * @param GamepadButton button
         */
        static bool getGamepadButtonDown(GamepadID gid, GamepadButton button);

        /**
         * @brief Check if the game pad button is released.
         * @param GamepadID gid: ID of the game pad.
         * @param GamepadButton button
         */
        static bool getGamepadButtonUp(GamepadID gid, GamepadButton button);

        /**
         * @brief Get state of each game pad axis.
         * @param GamepadID id
         * @param GamepadAxis axis
         * @return The state of each axis is in [-1, 1] range.
         */
        static float getGamepadAxis(GamepadID gid, GamepadAxis axis);

    private:
        static bool getGamepadState(GamepadID gid, GamepadState& state);

    private:
        static GLFWwindow * m_window;
        static uint8_t      m_activeGamepadsCount;
        /**
         * States:
         * false -> key was not pressed
         * true  -> key was pressed
         */
        static std::unordered_map<KeyCode, bool>  m_lastKeysStates;
        static std::unordered_map<KeyCode, bool>  m_lastMouseStates;

        static std::vector<GamepadState> m_lastGamepadStates;
    };
}
