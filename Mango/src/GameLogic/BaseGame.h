#pragma once

namespace mango
{
    class BaseGame
    {
    public:
        BaseGame() = default;
        virtual ~BaseGame() = default;

        BaseGame(const BaseGame &) = delete;
        BaseGame & operator=(const BaseGame &) = delete;

        virtual void init() = 0;

        virtual void input(float delta) {}
        virtual void onGUI(float delta) {}
    };
}