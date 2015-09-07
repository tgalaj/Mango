#pragma once

#include <SDL2\SDL.h>

#include <iterator>
#include <unordered_map>

class Input final
{
    friend class VertexEngineCore;

public:
    Input()                        = delete;
    ~Input()                       = delete;
    Input(const Input &)            = delete;
    Input & operator=(const Input &) = delete;

    static enum KeyCode
    {
        None           = SDLK_0,
        Backspace      = SDLK_BACKSPACE,
        Delete         = SDLK_DELETE,
        Tab            = SDLK_TAB,
        Return         = SDLK_RETURN,
        Pause          = SDLK_PAUSE,
        Escape         = SDLK_ESCAPE,
        Space          = SDLK_SPACE,
        Keypad0        = SDLK_KP_0,
        Keypad1        = SDLK_KP_1,
        Keypad2        = SDLK_KP_2,
        Keypad3        = SDLK_KP_3,
        Keypad4        = SDLK_KP_4,
        Keypad5        = SDLK_KP_5,
        Keypad6        = SDLK_KP_6,
        Keypad7        = SDLK_KP_7,
        Keypad8        = SDLK_KP_8,
        Keypad9        = SDLK_KP_9,
        KeypadPeriod   = SDLK_KP_PERIOD,
        KeypadDivide   = SDLK_KP_DIVIDE,
        KeypadMultiply = SDLK_KP_MULTIPLY,
        KeypadMinus    = SDLK_KP_MINUS,
        KeypadPlus     = SDLK_KP_PLUS,
        KeypadEnter    = SDLK_KP_ENTER,
        KeypadEquals   = SDLK_KP_EQUALS,
        UpArrow        = SDLK_UP,
        DownArrow      = SDLK_DOWN,
        RightArrow     = SDLK_RIGHT,
        LeftArrow      = SDLK_LEFT,
        Insert         = SDLK_INSERT,
        Home           = SDLK_HOME,
        End            = SDLK_END,
        PageUp         = SDLK_PAGEUP,
        PageDown       = SDLK_PAGEDOWN,
        F1             = SDLK_F1,
        F2             = SDLK_F2,
        F3             = SDLK_F3,
        F4             = SDLK_F4,
        F5             = SDLK_F5,
        F6             = SDLK_F6,
        F7             = SDLK_F7,
        F8             = SDLK_F8,
        F9             = SDLK_F9,
        F10            = SDLK_F10,
        F11            = SDLK_F11,
        F12            = SDLK_F12,
        F13            = SDLK_F13,
        F14            = SDLK_F14,
        F15            = SDLK_F15,
        Alpha0         = SDLK_0,
        Alpha1         = SDLK_1,
        Alpha2         = SDLK_2,
        Alpha3         = SDLK_3,
        Alpha4         = SDLK_4,
        Alpha5         = SDLK_5,
        Alpha6         = SDLK_6,
        Alpha7         = SDLK_7,
        Alpha8         = SDLK_8,
        Alpha9         = SDLK_9,
        Exclaim        = SDLK_EXCLAIM,
        DoubleQuote    = SDLK_QUOTEDBL,
        Hash           = SDLK_HASH,
        Dollar         = SDLK_DOLLAR,
        Ampersand      = SDLK_AMPERSAND,
        Quote          = SDLK_QUOTE,
        LeftParen      = SDLK_LEFTPAREN,
        RightParen     = SDLK_RIGHTPAREN,
        Asterisk       = SDLK_ASTERISK,
        Plus           = SDLK_PLUS,
        Comma          = SDLK_COMMA,
        Minus          = SDLK_MINUS,
        Period         = SDLK_PERIOD,
        Slash          = SDLK_SLASH,
        Colon          = SDLK_COLON,
        Semicolon      = SDLK_SEMICOLON,
        Less           = SDLK_LESS,
        Equals         = SDLK_EQUALS,
        Greater        = SDLK_GREATER,
        Question       = SDLK_QUESTION,
        At             = SDLK_AT,
        LeftBracket    = SDLK_LEFTBRACKET,
        RightBracket   = SDLK_RIGHTBRACKET,
        Backslash      = SDLK_BACKSLASH,
        Caret          = SDLK_CARET,
        Underscore     = SDLK_UNDERSCORE,
        BackQuote      = SDLK_BACKQUOTE,
        A              = SDLK_a,
        B              = SDLK_b,
        C              = SDLK_c,
        D              = SDLK_d,
        E              = SDLK_e,
        F              = SDLK_f,
        G              = SDLK_g,
        H              = SDLK_h,
        I              = SDLK_i,
        J              = SDLK_j,
        K              = SDLK_k,
        L              = SDLK_l,
        M              = SDLK_m,
        N              = SDLK_n,
        O              = SDLK_o,
        P              = SDLK_p,
        Q              = SDLK_q,
        R              = SDLK_r,
        S              = SDLK_s,
        T              = SDLK_t,
        U              = SDLK_u,
        V              = SDLK_v,
        W              = SDLK_w,
        X              = SDLK_x,
        Y              = SDLK_y,
        Z              = SDLK_z,
        Numlock        = SDLK_NUMLOCKCLEAR,
        CapsLock       = SDLK_CAPSLOCK,
        ScrollLock     = SDLK_SCROLLLOCK,
        RightShift     = SDLK_RSHIFT,
        LeftShift      = SDLK_LSHIFT,
        RightControl   = SDLK_RCTRL,
        LeftControl    = SDLK_LCTRL,
        RightAlt       = SDLK_RALT,
        LeftAlt        = SDLK_LALT,
        LeftCommand    = SDLK_LGUI,
        RightCommand   = SDLK_RGUI,
        AltGr          = SDLK_RALT,
        Help           = SDLK_HELP,
        Print          = SDLK_PRINTSCREEN,
        SysReq         = SDLK_SYSREQ,
        Break          = SDLK_PAUSE,
        Menu           = SDLK_MENU,
        Mouse0         = SDL_BUTTON_LEFT,
        Mouse1         = SDL_BUTTON_MIDDLE,
        Mouse2         = SDL_BUTTON_RIGHT,
        MouseLeft      = SDL_BUTTON_LEFT,
        MouseMiddle    = SDL_BUTTON_MIDDLE,
        MouseRight     = SDL_BUTTON_RIGHT
    };

    static bool getKeyDown(KeyCode key);
    static bool getKeyUp  (KeyCode key);

    /**
     * @brief Check if mouse button is pressed
     * @param int button accepted values for this param
     *                   are in range [0, 2]. Other values
     *                   will have no effect.
     */
    static bool getMouseButtonDown(int button);

    /**
    * @brief Check if mouse button is pressed
    * @param int button accepted values for this param
    *                   are in range [0, 2]. Other values
    *                   will have no effect.
    */
    static bool getMouseButtonUp  (int button);

    static bool getMouseButtonDown(KeyCode key);
    static bool getMouseButtonUp  (KeyCode key);

private:    
    /**
     * States are as follows:
     * -1: no state defined
     *  0: key down
     *  1: key up
     */
    static std::unordered_map<KeyCode, char> keyStates;

    static bool update();

    static void setKeyDown  (SDL_Keycode keycode);
    static void setKeyUp    (SDL_Keycode keycode);
    static void setMouseDown(SDL_Keycode keycode);
    static void setMouseUp  (SDL_Keycode keycode);
};

