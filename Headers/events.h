#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "events_base.h"
#include "player.h"


class Player;

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

class PlayerExitEvent : public Event {
public:
    Player* player;

    PlayerExitEvent(Player* playerExiting) : player(playerExiting) {}

    EventType getType() const override {
        return EventType::PlayerExit;
    }
};

class RoundStartEvent : public Event {
public:
    float smallBlindAmount;
    float bigBlindAmount;
    Player* smallBlind;
    Player* bigBlind;

    RoundStartEvent(float smallBlindAmount, float bigBlindAmount, Player* smallBlind, Player* bigBlind) :smallBlindAmount(smallBlindAmount),bigBlindAmount(bigBlindAmount), smallBlind(smallBlind), bigBlind(bigBlind) {}
    EventType getType() const override {
        return EventType::RoundStart;
    }
};


