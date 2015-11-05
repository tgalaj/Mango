#include "Input.h"
#include <imgui\imgui_impl_sdl.h>

bool Input::getKeyDown(KeyCode key)
{
    return keyStates[key] == 0;
}

bool Input::getKeyUp(KeyCode key)
{
    return keyStates[key] == 1;
}
     
bool Input::getMouseButtonDown(int button)
{
    bool state = false;

    switch (button)
    {
        case 0 /* left */:
            state = keyStates[Mouse0] == 0;
            break;
        case 1 /* middle */:
            state = keyStates[Mouse1] == 0;
            break;
        case 2 /* right */:
            state = keyStates[Mouse2] == 0;
            break;
    }

    return state;
}

bool Input::getMouseButtonUp(int button)
{
    bool state = false;

    switch (button)
    {
        case 0 /* left */:
            state = keyStates[Mouse0] == 1;
            break;
        case 1 /* middle */:
            state = keyStates[Mouse1] == 1;
            break;
        case 2 /* right */:
            state = keyStates[Mouse2] == 1;
            break;
    }

    return state;
}
     
bool Input::getMouseButtonDown(KeyCode key)
{
    return keyStates[key] == 0;
}

bool Input::getMouseButtonUp(KeyCode key)
{
    return keyStates[key] == 1;
}
     
void Input::setKeyDown(SDL_Keycode keycode)
{
    keyStates[(KeyCode)keycode] = 0;
}

void Input::setKeyUp(SDL_Keycode keycode)
{
    keyStates[(KeyCode)keycode] = 1;
}

void Input::setMouseDown(SDL_Keycode keycode)
{
    keyStates[(KeyCode)keycode] = 0;
}

void Input::setMouseUp(SDL_Keycode keycode)
{
    keyStates[(KeyCode)keycode] = 1;
}

bool Input::update()
{
    static SDL_Event e;

    /* Reset key states */
    for (auto it = std::begin(keyStates); it != std::end(keyStates); ++it)
    {
        it->second = -1;
    }

    while (SDL_PollEvent(&e) != 0)
    {
        ImGui_ImplSdl_ProcessEvent(&e);

        if (e.type == SDL_QUIT)
        {
            return true;
        }
        else
        if (e.type == SDL_KEYDOWN)
        {
            setKeyDown(e.key.keysym.sym);
        }
        else
        if (e.type == SDL_KEYUP)
        {
            setKeyUp(e.key.keysym.sym);
        }
        else
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            setMouseDown(e.button.button);
        }
        else
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            setMouseUp(e.button.button);
        }
    }

    return false;
}

std::unordered_map<Input::KeyCode, char> Input::keyStates
{
    { Input::None          , -1 },
    { Input::Backspace     , -1 },
    { Input::Delete        , -1 },
    { Input::Tab           , -1 },
    { Input::Return        , -1 },
    { Input::Pause         , -1 },
    { Input::Escape        , -1 },
    { Input::Space         , -1 },
    { Input::Keypad0       , -1 },
    { Input::Keypad1       , -1 },
    { Input::Keypad2       , -1 },
    { Input::Keypad3       , -1 },
    { Input::Keypad4       , -1 },
    { Input::Keypad5       , -1 },
    { Input::Keypad6       , -1 },
    { Input::Keypad7       , -1 },
    { Input::Keypad8       , -1 },
    { Input::Keypad9       , -1 },
    { Input::KeypadPeriod  , -1 },
    { Input::KeypadDivide  , -1 },
    { Input::KeypadMultiply, -1 },
    { Input::KeypadMinus   , -1 },
    { Input::KeypadPlus    , -1 },
    { Input::KeypadEnter   , -1 },
    { Input::KeypadEquals  , -1 },
    { Input::UpArrow       , -1 },
    { Input::DownArrow     , -1 },
    { Input::RightArrow    , -1 },
    { Input::LeftArrow     , -1 },
    { Input::Insert        , -1 },
    { Input::Home          , -1 },
    { Input::End           , -1 },
    { Input::PageUp        , -1 },
    { Input::PageDown      , -1 },
    { Input::F1            , -1 },
    { Input::F2            , -1 },
    { Input::F3            , -1 },
    { Input::F4            , -1 },
    { Input::F5            , -1 },
    { Input::F6            , -1 },
    { Input::F7            , -1 },
    { Input::F8            , -1 },
    { Input::F9            , -1 },
    { Input::F10           , -1 },
    { Input::F11           , -1 },
    { Input::F12           , -1 },
    { Input::F13           , -1 },
    { Input::F14           , -1 },
    { Input::F15           , -1 },
    { Input::Alpha0        , -1 },
    { Input::Alpha1        , -1 },
    { Input::Alpha2        , -1 },
    { Input::Alpha3        , -1 },
    { Input::Alpha4        , -1 },
    { Input::Alpha5        , -1 },
    { Input::Alpha6        , -1 },
    { Input::Alpha7        , -1 },
    { Input::Alpha8        , -1 },
    { Input::Alpha9        , -1 },
    { Input::Exclaim       , -1 },
    { Input::DoubleQuote   , -1 },
    { Input::Hash          , -1 },
    { Input::Dollar        , -1 },
    { Input::Ampersand     , -1 },
    { Input::Quote         , -1 },
    { Input::LeftParen     , -1 },
    { Input::RightParen    , -1 },
    { Input::Asterisk      , -1 },
    { Input::Plus          , -1 },
    { Input::Comma         , -1 },
    { Input::Minus         , -1 },
    { Input::Period        , -1 },
    { Input::Slash         , -1 },
    { Input::Colon         , -1 },
    { Input::Semicolon     , -1 },
    { Input::Less          , -1 },
    { Input::Equals        , -1 },
    { Input::Greater       , -1 },
    { Input::Question      , -1 },
    { Input::At            , -1 },
    { Input::LeftBracket   , -1 },
    { Input::RightBracket  , -1 },
    { Input::Backslash     , -1 },
    { Input::Caret         , -1 },
    { Input::Underscore    , -1 },
    { Input::BackQuote     , -1 },
    { Input::A             , -1 },
    { Input::B             , -1 },
    { Input::C             , -1 },
    { Input::D             , -1 },
    { Input::E             , -1 },
    { Input::F             , -1 },
    { Input::G             , -1 },
    { Input::H             , -1 },
    { Input::I             , -1 },
    { Input::J             , -1 },
    { Input::K             , -1 },
    { Input::L             , -1 },
    { Input::M             , -1 },
    { Input::N             , -1 },
    { Input::O             , -1 },
    { Input::P             , -1 },
    { Input::Q             , -1 },
    { Input::R             , -1 },
    { Input::S             , -1 },
    { Input::T             , -1 },
    { Input::U             , -1 },
    { Input::V             , -1 },
    { Input::W             , -1 },
    { Input::X             , -1 },
    { Input::Y             , -1 },
    { Input::Z             , -1 },
    { Input::Numlock       , -1 },
    { Input::CapsLock      , -1 },
    { Input::ScrollLock    , -1 },
    { Input::RightShift    , -1 },
    { Input::LeftShift     , -1 },
    { Input::RightControl  , -1 },
    { Input::LeftControl   , -1 },
    { Input::RightAlt      , -1 },
    { Input::LeftAlt       , -1 },
    { Input::LeftCommand   , -1 },
    { Input::RightCommand  , -1 },
    { Input::AltGr         , -1 },
    { Input::Help          , -1 },
    { Input::Print         , -1 },
    { Input::SysReq        , -1 },
    { Input::Break         , -1 },
    { Input::Menu          , -1 },
    { Input::Mouse0        , -1 },
    { Input::Mouse1        , -1 },
    { Input::Mouse2        , -1 },
    { Input::MouseLeft     , -1 },
    { Input::MouseMiddle   , -1 },
    { Input::MouseRight    , -1 }
};