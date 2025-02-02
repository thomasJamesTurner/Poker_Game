#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "events_base.h"




class PlayerBetEvent : public Event {
public:
    std::string playerName;
    float betAmount;

    PlayerBetEvent(std::string id, float amount) : playerName(id), betAmount(amount) {}

    EventType getType() const override {
        return EventType::PlayerBet;
    }
};

class PlayerWinEvent : public Event {
public:
    float winnings;
    std::string playerName;

    PlayerWinEvent(float pot,std::string id) :winnings(pot), playerName(id){}

    EventType getType() const override {
        return EventType::PlayerWin;
    }
};





