
#include <core_engine/UIObject.h>
#include "core_engine/CoreServices.h"

namespace Vertex
{
    UIObject::UIObject()
    {
        entity = CoreServices::getCore()->entities.create();
    }
}
