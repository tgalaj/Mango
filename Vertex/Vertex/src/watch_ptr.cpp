#include "watch_ptr.h"

void watch_ptr_base::link(const Watchable * p)
{
    ptr = p;

    if (p != nullptr)
    {
        ptr->pointers.push_back(this);
    }
}

void watch_ptr_base::unlink()
{
    ptr = nullptr;
}