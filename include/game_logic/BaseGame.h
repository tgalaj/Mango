#pragma once

namespace Vertex
{
    class BaseGame
    {
    public:
        BaseGame() {};
        virtual ~BaseGame() {};

        BaseGame(const BaseGame &) = delete;
        BaseGame & operator=(const BaseGame &) = delete;

        virtual void init() = 0;

        virtual void input(float delta) {}
        virtual void onGUI(float delta) {}
    };
}