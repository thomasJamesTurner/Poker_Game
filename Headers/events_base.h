#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

// Forward declare EventHandler (so EventDispatcher knows it exists)
class EventHandler;

enum class EventType
{
    PlayerBet,
    PlayerWin,
    RoundStart,
    RoundEnd,
    PlayerExit,
    PlayerFold,
    PlayerAllIn,
    PlayerCall,
    PlayerCheck,
    PlayerRaise
};

class Event {
public:
    virtual ~Event() = default;
    virtual EventType getType() const = 0;
};

struct Listener
{
    EventType eventType;
    std::function<void(const Event&)> callback;
};

// EventDispatcher definition
class EventDispatcher
{
private:
    std::vector<EventHandler*> eventHandlers; // Store pointers to handlers

public:
    void addHandler(EventHandler* handler);
    void dispatch(const Event& event);
};

// EventHandler definition
class EventHandler
{
private:
    EventDispatcher* dispatcher = nullptr; // Pointer to dispatcher
    std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> listeners;

public:
    void called(const Event& event);
    void sendEvent(const Event& event);
    void subscribe(const Listener& listener);
    void setDispatcher(EventDispatcher* dispatcher);
};

// Implement EventDispatcher methods outside class (after EventHandler is fully defined)
inline void EventDispatcher::addHandler(EventHandler* handler)
{
    eventHandlers.push_back(handler);
}

inline void EventDispatcher::dispatch(const Event& event)
{
    for (EventHandler* handler : eventHandlers)
    {
        handler->called(event); // Now it's safe because EventHandler is fully defined
    }
}

// Implement EventHandler methods
inline void EventHandler::called(const Event& event)
{
    auto it = listeners.find(event.getType());
    if (it != listeners.end()) {
        for (auto& listener : it->second) {
            listener(event);
        }
    }
}

inline void EventHandler::sendEvent(const Event& event)
{
    if (dispatcher)
    {
        dispatcher->dispatch(event);
    }
}

inline void EventHandler::subscribe(const Listener& listener)
{
    listeners[listener.eventType].push_back(listener.callback);
}

inline void EventHandler::setDispatcher(EventDispatcher* dispatcher)
{
    this->dispatcher = dispatcher;
}
