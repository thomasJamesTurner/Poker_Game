#pragma once

enum class EventType {
    PlayerBet,
    PlayerWin,
    RoundStart,
    RoundEnd,
    PlayerExit,
    PlayerFold,
    PlayerAllIn
};

class Event {
public:
    virtual ~Event() = default;
    virtual EventType getType() const = 0;
};

class EventDispatcher {
private:
    std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> listeners;

public:
    void subscribe(EventType type, std::function<void(const Event&)> callback) {
        listeners[type].push_back(callback);
    }

    void unsubscribe(EventType type, const std::function<void(const Event&)>& callback) {
        auto& listenerList = listeners[type];

        listenerList.erase(
            std::remove_if(listenerList.begin(), listenerList.end(),
                [&](const std::function<void(const Event&)>& storedCallback) {
                    return storedCallback.target_type() == callback.target_type(); // Compare type info
                }),
            listenerList.end());
    }

    void dispatch(const Event& event) {
        auto it = listeners.find(event.getType());
        if (it != listeners.end()) {
            for (auto& listener : it->second) {
                listener(event);
            }
        }
    }
};
