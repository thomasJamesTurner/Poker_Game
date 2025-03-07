#pragma once
#include "display.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include "event_types.h"

// Forward declare EventHandler (so EventDispatcher knows it exists)
class EventHandler;



class Event {

public:
    
    Event() = default;
    virtual ~Event() = default;
    virtual EventType getType() const = 0;
    void printEvent () const
    {
        logger* log = logger::getInstance("", "");
        log = logger::getInstance("", "");
        
    }
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
    void removeHandler(EventHandler* handler);
    void dispatch(const Event& event);
    void printHandlers();
};


//handles the events for entities in the event system
class EventHandler
{
private:
    EventDispatcher* dispatcher = nullptr; 
    std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> listeners;

public:
    EventHandler(EventDispatcher* dispatch) : dispatcher(dispatch) {}
    ~EventHandler() 
    { 
        logger* log = logger::getInstance("", "");
        log->debugMsg("deleting event handler");
    }
    void called(const Event& event);
    void sendEvent(const Event& event);
    void subscribe(const Listener& listener);
};

//have to implement functions extenally of the class structure due to circular dependancies
inline void EventDispatcher::addHandler(EventHandler* handler)
{
    eventHandlers.push_back(handler);
}

inline void EventDispatcher::removeHandler(EventHandler* handler)
{
    auto handlerIt = std::find(eventHandlers.begin(), eventHandlers.end(), handler);
    if (handlerIt != eventHandlers.end())
    {
        eventHandlers.erase(handlerIt);
    }
}

inline void EventDispatcher::dispatch(const Event& event)
{
#ifdef _DEBUG
    event.printEvent();
#endif
    for (EventHandler* handler : eventHandlers)
    {
        handler->called(event);
    }
}

inline void EventDispatcher::printHandlers()
{
    logger* log = logger::getInstance("", "");
    log->debugMsg("Number Of Handlers In Dispatcher: " + std::to_string(eventHandlers.size()));
}

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
    else
    {
        logger* log = logger::getInstance("", "");
        log->errorMsg("Dispatcher Was Null Pointer");
    }
}

inline void EventHandler::subscribe(const Listener& listener)
{
    listeners[listener.eventType].push_back(listener.callback);
}
