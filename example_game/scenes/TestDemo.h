#pragma once
#include "game_logic/BaseGame.h"

class TestDemo : public Vertex::BaseGame
{
public:
    TestDemo();
    ~TestDemo();

    void init() override;
    void input(float delta) override;
    void onGUI(float delta) override;
};
