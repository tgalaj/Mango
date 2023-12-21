#pragma once

#include <string>

namespace mango
{
    /**
     * Based on http://isthe.com/chongo/tech/comp/fnv/
     */
    constexpr uint32_t fnvHash1a32(const char* key, uint32_t len) 
    {
        const char* p = key;
        uint32_t h = 2166136261u;

        for (int i = 0; i < len; i++)
        {
            h = (h ^ p[i]) * 16777619u;
        }

        return h;
    }

    /**
     * Based on http://isthe.com/chongo/tech/comp/fnv/
     */
    constexpr uint64_t fnvHash1a64(const char* key, uint32_t len)
    {
        const char* p = key;
        uint64_t h = 14695981039346656037ull;

        for (int i = 0; i < len; i++)
        {
            h = (h ^ p[i]) * 1099511628211ull;
        }

        return h;
    }

    struct StringHash32
    {
        StringHash32(uint32_t hash) : hash(hash) {}
        
        StringHash32(const std::string & s)
        {
            hash = fnvHash1a32(s.c_str(), s.size());
        }

        StringHash32(const char* s)
        {
            hash = fnvHash1a32(s, strlen(s));
        }

        StringHash32(const char* s, uint32_t len)
        {
            hash = fnvHash1a32(s, len);
        }

        operator uint32_t() noexcept { return hash; }

        uint32_t hash;
    };

    struct StringHash64
    {
        constexpr StringHash64(uint64_t hash) : hash(hash) {}

        constexpr StringHash64(const std::string& s)
        {
            hash = fnvHash1a64(s.c_str(), s.size());
        }

        constexpr StringHash64(const char* s)
        {
            hash = fnvHash1a64(s, strlen(s));
        }

        constexpr StringHash64(const char* s, uint32_t len)
        {
            hash = fnvHash1a64(s, len);
        }

        constexpr operator uint64_t() noexcept { return hash; }

        uint64_t hash;
    };
}