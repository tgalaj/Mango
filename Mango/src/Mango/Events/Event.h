#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#define MG_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace mango
{
    using EventHandle = uint32_t;

    class Event
    {
    public:
        virtual ~Event() {};
    };

    class EventHandlerBase
    {
    public:
        void exec(const Event& e)
        {
            call(e);
        }

        virtual EventHandle getHandle() = 0;

    private:
        virtual void call(const Event& e) = 0;
    };

    template<typename EventType>
    class EventHandlerWrapper : public EventHandlerBase
    {
        using EventHandler = std::function<void(const EventType& e)>;

    public:
        EventHandlerWrapper(const EventHandler& handler, EventHandle handle)
            : m_handler(handler), 
              m_handle (handle) {}

        EventHandle getHandle() { return m_handle; }

    private:
        void call(const Event& e) override
        {
            m_handler(static_cast<const EventType&>(e));
        }

        EventHandler m_handler;
        EventHandle m_handle;
    };

    class EventBus 
    {
    public:
        template<typename EventType>
        void emit(const EventType& event) 
        {
            auto& handlers = m_subscribers[typeid(EventType)];

            for (auto& handler : handlers) 
            {
                handler->exec(event);
            }
        }

        template<class EventType>
        EventHandle subscribe(const std::function<void(const EventType&)>& fn)
        {
            auto  handle   = nextHandle();
            auto& handlers = m_subscribers[typeid(EventType)];
            auto  handler  = std::make_shared<EventHandlerWrapper<EventType>>(fn, handle);
            handlers.emplace_back(handler);

            return handle;
        }

        template<class EventType>
        void unsubscribe(EventHandle handle)
        {
            auto& handlers = m_subscribers[typeid(EventType)];

            for (auto it = handlers.begin(); it != handlers.end(); ++it)
            {
                if (it->get()->getHandle() == handle)
                {
                    handlers.erase(it);
                    return;
                }
            }
        }

        void unsubscribeAll()
        {
            m_subscribers.clear();
        }

    private:
        std::unordered_map<std::type_index, std::vector<std::shared_ptr<EventHandlerBase>>> m_subscribers;

        inline static EventHandle nextHandle()
        {
            static EventHandle handle = 0;
            return handle++;
        }
    };
}