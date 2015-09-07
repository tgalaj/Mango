#pragma once

class BaseGame
{
public:
    BaseGame();
    virtual ~BaseGame();

    BaseGame(const BaseGame &)             = delete;
    BaseGame & operator=(const BaseGame &) = delete;

    virtual void processInput()            = 0;
    virtual void update(float delta)       = 0;
    virtual void render()                  = 0;
};

