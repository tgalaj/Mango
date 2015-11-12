#pragma once

#include <list>

class watch_ptr_base
{
    friend class Watchable;

protected:
    const Watchable * ptr;

    void link(const Watchable * p);
    void unlink();
};

template <typename T>
class watch_ptr final : public watch_ptr_base
{
public:
    watch_ptr() {}

    watch_ptr(nullptr_t) {}

    ~watch_ptr()
    {
        unlink();
    }

    explicit watch_ptr(T * t)
    {
        link(t);
    }

    watch_ptr(const watch_ptr & t)
    {
        link(t.ptr);
    }

    template <typename U>
    watch_ptr(const watch_ptr<U> & t)
    {
        link(t.ptr);
    }

    watch_ptr & operator=(T * t)
    {
        unlink();
        link(t);

        return *this;
    }

    watch_ptr & operator=(watch_ptr & o)
    {
        unlink();
        link(o.ptr);

        return *this;
    }

    T & operator*() const
    {
        return *get();
    }

    T * operator->() const
    {
        return get();
    }

    explicit operator bool() const
    {
        return ptr != nullptr;
    }

    T * get() const
    {
        return (T*)ptr;
    }
};

class Watchable
{
public:
    friend class watch_ptr_base;

    Watchable() {};

    virtual ~Watchable()
    {
        for (watch_ptr_base * w : pointers)
        {
            w->ptr = nullptr;
        }

        pointers.clear();
    };

private:
    mutable std::list<watch_ptr_base *> pointers;
};