#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "events_base.h"
#include "player.h"


class Player;


class PlayerStartTurn : public Event {
public:
    Player* playerStarting;
    PlayerStartTurn(Player* player = nullptr) : playerStarting(player) {}

    EventType getType() const override {
        return EventType::PlayerTurnStart;
    }
};
class PlayerEndTurn : public Event {
public:
    Player* playerEnded;
    PlayerEndTurn(Player* player = nullptr) : playerEnded(player) {}

    EventType getType() const override {
        return EventType::PlayerTurnEnd;
    }
};

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

class PlayerFoldEvent : public Event {
public:
    Player* player;
    Hand hand;
    PlayerFoldEvent(Player* playerFolding,Hand hand) : player(playerFolding) , hand(hand){}
    EventType getType() const override {
        return EventType::PlayerFold;
    }
};

class RoundStartEvent : public Event {
public:
    float smallBlindAmount;
    float bigBlindAmount;
    Player* smallBlind;
    Player* bigBlind;
    Deck* deck;

    RoundStartEvent(float smallBlindAmount, float bigBlindAmount, Player* smallBlind, Player* bigBlind,Deck* deck) :smallBlindAmount(smallBlindAmount),bigBlindAmount(bigBlindAmount), smallBlind(smallBlind), bigBlind(bigBlind),deck(deck) {}
    EventType getType() const override {
        return EventType::RoundStart;
    }
};
class RoundEndEvent : public Event {
public:
    EventType getType() const override {
        return EventType::RoundEnd;
    }
};

class GameOverEvent : public Event {
public:
    EventType getType() const override {
        return EventType::gameOver;
    }
};

class PlayerAllInEvent : public Event {
public:
    Player* allInPlayer;

    PlayerAllInEvent(Player* allInPlayer) :allInPlayer(allInPlayer) {}
    EventType getType() const override {
        return EventType::PlayerAllIn;
    }
};

class PlayerCallEvent : public Event {
public:
    Player* callPlayer;

    PlayerCallEvent(Player* callPlayer) :callPlayer(callPlayer) {}
    EventType getType() const override {
        return EventType::PlayerCall;
    }
};

class PlayerCheckEvent : public Event {
public:
    Player* checkPlayer;

    PlayerCheckEvent(Player* checkPlayer) :checkPlayer(checkPlayer) {}
    EventType getType() const override {
        return EventType::PlayerCheck;
    }
};

class PlayerRaiseEvent : public Event {
public:
    Player* raisePlayer;

    PlayerRaiseEvent(Player* raisePlayer) :raisePlayer(raisePlayer) {}
    EventType getType() const override {
        return EventType::PlayerCall;
    }
};

