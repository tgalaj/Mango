#pragma once

#include "Event.h"
#include "Mango/Window/Input.h"

namespace mango
{
    struct GamepadConnectedEvent : Event
    {
        GamepadConnectedEvent(GamepadID gid, bool connected)
            : gid(gid),
              isConnected(connected) {}

        GamepadID gid;
        bool isConnected;
    };
}