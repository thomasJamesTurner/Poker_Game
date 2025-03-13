#pragma once
#include "display.h"
#include "cards.h"
#include "events.h"
#include <cmath>
#include <iostream>
#include <string>
#include <Spritter/Spritter.h>


//forward declaration in for table pointer


class Player
{
protected:
	std::string playerName;
	Hand playerHand;
	float account;
	float minBet;

	EventHandler* handler;
	logger* log;
	Deck* deck;
	void updateMinBet(const Event& event)
	{
		const PlayerBetEvent playerBet = static_cast<const PlayerBetEvent&>(event);
		minBet = playerBet.betAmount;
	}
	inline	void updateAccount(const Event& event)
	{
		const PlayerWinEvent playerWin = static_cast<const PlayerWinEvent&>(event);
		if (playerWin.playerName == playerName)
		{
			account += playerWin.winnings;
		}
	}
	void startGame(const Event& event)
	{
		const RoundStartEvent& startEvent = static_cast<const RoundStartEvent&>(event);
		minBet = startEvent.bigBlindAmount;

		playerHand.cards.clear();
		playerHand.makeHand(deck, 2);
		if (startEvent.smallBlind == this)
		{
			makeBet(startEvent.smallBlindAmount);
		}
		if (startEvent.bigBlind == this)
		{
			makeBet(minBet);
		}
	}
public:


	/*
	*		EVENT FUNCTIONS
	*	GETS CALLED ON RESPECTIVE 
	*			EVENTS
	*/
	

	Player(Deck* deck, EventDispatcher* dispatch)
	{
		log = logger::getInstance("", "");
		handler = new EventHandler(dispatch);
		handler->subscribe({EventType::PlayerBet,std::bind(&Player::updateMinBet,this,std::placeholders::_1)});
		handler->subscribe({EventType::PlayerWin, std::bind(&Player::updateAccount, this, std::placeholders::_1)});
		handler->subscribe({ EventType::RoundStart, std::bind(&Player::startGame, this, std::placeholders::_1) });
	}
	virtual ~Player()
	{
		log->debugMsg("DELETING PLAYER " + playerName);
		delete handler;
	}

	/*
	*		GETTERS
	*/

	inline std::string getPlayerName()
	{
		return playerName;
	}
	inline Hand* getHand()
	{
		return &playerHand;
	}
	inline EventHandler* getHandler()
	{
		log->debugMsg("getting " + playerName + " handler");
		return handler;
	}
	inline float getMinimumBet()
	{
		return minBet;
	}
	inline float getPlayerAccount()
	{
		return account;
	}

	void setPlayerName(std::string name)
	{
		playerName = name;
	}

	/*
	*		SHOW VARIABLES
	*/

	inline void showAccount()
	{
		log->msg(playerName + " Balance $" + std::to_string(account));
	}

	virtual inline void showCards()
	{
		playerHand.showCards();
	}

	/*
	*		PLAYER GAME LOGIC
	*/

	void makeBet(float bet)
	{

		account -= bet;
		PlayerBetEvent playerBet(playerName, bet);
		handler->sendEvent(playerBet);
	}

	void fold()
	{
		log->msg(playerName + " folded");
		handler->sendEvent(PlayerFoldEvent(this, playerHand));
		handler->sendEvent(PlayerEndTurn());
	}
	void check()
	{
		log->msg(playerName + " checked");
		handler->sendEvent(PlayerCheckEvent(this));
		handler->sendEvent(PlayerEndTurn());
	}
	void call()
	{
		log->msg(playerName + " called");
		handler->sendEvent(PlayerCallEvent(this));
		handler->sendEvent(PlayerEndTurn());
		makeBet(minBet);
	}
	void allIn()
	{
		log->msg(playerName + " has gone ALL IN");
		handler->sendEvent(PlayerAllInEvent(this));
		handler->sendEvent(PlayerEndTurn());
		makeBet(account);
	}
	void raise(float raise)
	{
		log->msg(playerName + " has raised");
		handler->sendEvent(PlayerRaiseEvent(this));
		handler->sendEvent(PlayerEndTurn());
		makeBet(account);
	}
};

class User : public Player
{
public:
	User(Deck* deck, EventDispatcher* dispatch) : Player(deck,dispatch)
	{
		setPlayerName("player");
	}


	int gameInput()
	{
		int result = -1;
		if (Spritter::Input::IsKeyDown(Spritter::Key::F))
		{
			fold();
			return result = 0;
		}
		if (Spritter::Input::IsKeyDown(Spritter::Key::C))
		{
			check();
			return result = 1;
		}
		if (Spritter::Input::IsKeyDown(Spritter::Key::L))
		{
			call();
			return result = 2;
		}
		if (Spritter::Input::IsKeyDown(Spritter::Key::R))
		{
			raise(2.0f);
			return result = 3;
		}
		if (Spritter::Input::IsKeyDown(Spritter::Key::A))
		{
			allIn();
			return result = 4;
		}
		return result;
	}

	void playTurn()
	{
		gameInput();
	}
};

class BotPlayer : public Player
{
	std::function<void(BotPlayer*)> playTurnStrategy;
	logger* log;
public:
	std::function<void(BotPlayer*)> initalizerFunction;

	BotPlayer(Deck* deck, EventDispatcher* dispatch, std::function<void(BotPlayer*)> playTurnStrategy, std::function<void(BotPlayer*)> initalizeFunc) : Player(deck,dispatch), log(logger::getInstance("", ""))
	{
		//log = logger::getInstance("", "");
		log->debugMsg("creating bot player");
		if (playTurnStrategy) {
			this->playTurnStrategy = playTurnStrategy;
		}
		if (playTurnStrategy) {
			this->initalizerFunction = initalizeFunc;
		}
		else {
			log->errorMsg("Error: BotPlayer received a null playTurnStrategy!");
		}
	}

	void subscribeToEvent(EventType eventType, std::function<void(const Event&)> callback)
	{
		handler->subscribe({ eventType, callback });
	}
};
