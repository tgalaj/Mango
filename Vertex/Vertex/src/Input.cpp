#include "Input.h"

bool Input::getKeyDown(KeyCode key)
{
    return true;
}

bool Input::getKeyUp(KeyCode key)
{
    return true;
}
     
bool Input::getMouseButtonDown(int button)
{
    return true;
}

bool Input::getMouseUp(int button)
{
    return true;
}
     
bool Input::getMouseButtonDown(KeyCode key)
{
    return true;
}

bool Input::getMouseUp(KeyCode key)
{
    return true;
}
     
void Input::setKeyDown(SDL_Keycode keycode)
{

}

void Input::setKeyUp(SDL_Keycode keycode)
{

}

void Input::setMouseDown(SDL_Keycode keycode)
{

}

void Input::setMouseUp(SDL_Keycode keycode)
{

}

bool Input::update()
{
    static SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        
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
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            setMouseUp(e.key.keysym.sym);
        }
        else
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            setMouseDown(e.key.keysym.sym);
        }
    }

    return false;
}