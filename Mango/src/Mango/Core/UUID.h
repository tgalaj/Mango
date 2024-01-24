#pragma once

namespace mango
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_uuid; }
    private:
        uint64_t m_uuid;
    };
}

namespace std
{
    template<> 
    struct hash<mango::UUID>
    {
        size_t operator()(const mango::UUID& uuid) const
        {
            return (uint64_t)uuid;
        }
    };
}