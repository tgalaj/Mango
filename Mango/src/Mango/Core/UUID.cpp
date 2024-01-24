#include "mgpch.h"
#include "UUID.h"

#include <random>

namespace mango
{
    namespace
    {
        std::random_device                      randomDevice;
        std::mt19937_64                         engine(randomDevice());
        std::uniform_int_distribution<uint64_t> uniformDistribution;
    }

    UUID::UUID()
        : m_uuid(uniformDistribution(engine))
    {

    }

    UUID::UUID(uint64_t uuid)
        : m_uuid(uuid)
    {

    }

}