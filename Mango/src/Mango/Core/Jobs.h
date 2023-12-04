#pragma once
#include "taskflow/taskflow.hpp"

namespace mango
{
    class Jobs final
    {
    public:
        static tf::Executor executor;
        static tf::Taskflow taskflow;

    private:
        Jobs() {}
        ~Jobs() {}
    };
}