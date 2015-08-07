#pragma once

#include "BaseGame.h"
#include "Input.h"

class SimpleGame : public BaseGame
{
public:
    SimpleGame();
    ~SimpleGame();

    void processInput() override;
    void update()       override;
};