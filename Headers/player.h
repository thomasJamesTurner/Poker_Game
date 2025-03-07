#pragma once
#include "display.h"
#include "cards.h"
#include "events.h"
#include <cmath>
#include <iostream>
#include <string>
#include <Spritter/Spritter.h>


class Player
{
	logger* log;
	std::string playerName;
	Hand playerHand;
	float account;
	float minBet;
	bool folded = false;
	bool allin = false;
	EventDispatcher* dispatch;
	EventHandler* handler;
	Deck* deck;

public:

	Player(Deck* deck, EventDispatcher* dispatch) : deck(deck), account(0.0f), minBet(0.0f), folded(false), allin(false), dispatch(dispatch)
	{
		log =logger::getInstance("", "");
	    if (!dispatch) 
		{
	        throw std::invalid_argument("EventDispatcher cannot be null!");
	    }
	
		handler = new EventHandler(dispatch);
	
	    subscribeToEvent(EventType::RoundStart, std::bind(&Player::startRound, this, std::placeholders::_1));
	    subscribeToEvent(EventType::PlayerWin, std::bind(&Player::winGame, this, std::placeholders::_1));
	    subscribeToEvent(EventType::PlayerBet, std::bind(&Player::setMinimumBet, this, std::placeholders::_1));
	
	    playerName = "";
	}
	virtual ~Player()
	{
		
		log->debugMsg("DELETING PLAYER " + getPlayerName());
		dispatch->removeHandler(handler);
		delete handler;
	}


	//____ event functions ____//

	void winGame(const Event& event)
	{

		const PlayerWinEvent& winEvent = static_cast<const PlayerWinEvent&>(event);
		if (winEvent.playerName == playerName)
		{
			showAccount();
			log->msg("win amount $" + std::to_string(winEvent.winnings));
			account += winEvent.winnings;
			showAccount();
		}
		if (this->getPlayerAccount() <= 0)
		{
			log->msg("Player: " + getPlayerName() + " has no money left to play with and will exit");
			PlayerExitEvent playerExit(this);

			handler->sendEvent(playerExit);
		}

		
	}
	void startRound(const Event& event)
	{
		const RoundStartEvent& startEvent = static_cast<const RoundStartEvent&>(event);
		minBet = startEvent.bigBlindAmount;

		playerHand.cards.clear();
		playerHand.makeHand(deck, 2);
		
		folded = false;
		allin = false;

		if (startEvent.smallBlind == this)
		{
			makeBet(startEvent.smallBlindAmount);
		}
		if (startEvent.bigBlind == this)
		{
			makeBet(minBet);
		}
		
	}

	int gameInput()
	{
		int result = -1;
			if (Spritter::Input::IsKeyDown(Spritter::Key::F))
			{
				return result = 0; 
			}
			if (Spritter::Input::IsKeyDown(Spritter::Key::C))
			{
				return result = 1;
			}
			if (Spritter::Input::IsKeyDown(Spritter::Key::L))
			{
				return result = 2;
			}
			if (Spritter::Input::IsKeyDown(Spritter::Key::R))
			{
				return result = 3;
			}
			if (Spritter::Input::IsKeyDown(Spritter::Key::A))
			{
				return result = 4;
			}
		return result;
	}
	void blind()
	{
		if (allin) { return; }
		bool badAction = false;
		
		switch (gameInput())
		{
		case(0):
			fold();
			break;
		case(1):
			check();
			break;
		case(2):
			call();
			break;
		case(3):
			do
			{
				try
				{
					std::string input;
					log->msg("raise by: ");
					std::getline(std::cin, input);
					float bet = std::stof(input);
					badAction = raise(bet) == -1;
				}
				catch (...)
				{
					
					log->errorMsg("Please Enter Valid Input");
					badAction = true;
				}
			} while (badAction);
			break;
		case(4):
			allIn();
			break;
		}
		
	}
	
	int validateBet(float bet)
	{
		if (bet == 0)
		{
			return 0;
		}
		if (getMinimumBet() > bet)
		{
			return -1;
		}
		if (getMinimumBet() == bet)
		{
			return 1;
		}
		if (bet > account)
		{
			return -2;
		}
		if (bet == account)
		{
			return 3;
		}
		if (bet > getMinimumBet())
		{
			return 2;
		}
	}
	void makeBet(float betAmount)
	{
		account -= betAmount;
		PlayerBetEvent bet(playerName,betAmount);
		handler->sendEvent(bet);
		showAccount();
		log->msg("Bet $" + std::to_string(betAmount));
	}
	void fold()
	{
		folded = true;
		log->msg(getPlayerName() + " folded");
		handler->sendEvent(PlayerFoldEvent(this, *(this->getHand())));
		handler->sendEvent(PlayerEndTurn());
	}
	void check()
	{
		folded = true;
		log->msg(getPlayerName() + " checked");
		handler->sendEvent(PlayerCheckEvent(this));
		handler->sendEvent(PlayerEndTurn());
	}
	void call()
	{
		makeBet(minBet);
		log->msg(getPlayerName() + " called");
		handler->sendEvent(PlayerCallEvent(this));
		handler->sendEvent(PlayerEndTurn());
	}
	void allIn()
	{
		log->msg(getPlayerName() + " has gone ALL IN");
		allin = true;
		handler->sendEvent(PlayerAllInEvent(this));
		handler->sendEvent(PlayerEndTurn());
		makeBet(account);
	}
	int raise(float raise)
	{
		float bet = raise + getMinimumBet();
		if(validateBet(bet)== -1)
		{ 
			return -1;
		}
		makeBet(bet);
		return 1;
	}

	//____ getters and setters ____//

	void setMinimumBet(const Event& event)
	{
		const PlayerBetEvent& betEvent = static_cast<const PlayerBetEvent&>(event);
		minBet = betEvent.betAmount;
	}

	void setAccountAmount(float amount)
	{
		account = amount;
	}


	void subscribeToEvent(EventType eventType, std::function<void(const Event&)> callback)
	{
		handler->subscribe({ eventType, callback });
	}

	virtual void setPlayerName(std::string name)
	{
		playerName = name;
	}

	inline std::string getPlayerName()
	{
		return playerName;
	}

	inline void showAccount()
	{
		log->msg(playerName + " Balance $" + std::to_string(account));
	}

	inline void showMinBet()
	{
		log->msg("Prevoius Bet: " + std::to_string(minBet));
	}

	inline Hand* getHand()
	{
		return &playerHand;
	}

	inline float getPlayerAccount()
	{
		return account;
	}

	inline float getMinimumBet()
	{
		return minBet;
	}

	inline EventHandler* getHandler()
	{
		log->debugMsg("getting " + playerName + " handler");
		return handler;
	}

	inline bool isAllIN() const { return allin; }
	inline bool isFolded() const { return folded; }

	//____ game logic ____//

	virtual void playTurn()
	{
		if (!folded)
		{
			playerHand.showCards();
			showAccount();
			showMinBet();
			blind();
		}
	}
};

class UserPlayer : public Player
{
	logger* log;
public:
	UserPlayer(Deck* deck, EventDispatcher* dispatch) : Player(deck, dispatch)
	{
		log = logger::getInstance("", "");
		setPlayerName("player");

		log->msg("Player Name: " + this->getPlayerName());
		this->setAccountAmount(100);
		showAccount();
	}

	void setUserAccountAmount()
	{
		log->msg("Input Amount To Play With: ");
		std::string amount;
		float amountFloat;
		try
		{
			std::getline(std::cin, amount);
			amountFloat = std::stof(amount);
		}
		catch (...)
		{
			amount = "";
		}
		if (amount == "")
		{
			amountFloat = 100.00f;
		}
		this->setAccountAmount(amountFloat);

	}
};


class BotPlayer : public Player
{
	std::function<void(BotPlayer*)> playTurnStrategy;
	logger* log;
	
public:
	std::function<void(BotPlayer*)> initalizerFunction;
	
	BotPlayer(Deck* deck, EventDispatcher* dispatch,std::function<void(BotPlayer*)> playTurnStrategy, std::function<void(BotPlayer*)> initalizeFunc): Player(deck,dispatch)
	{
		log = logger::getInstance("","");
		
		if (playTurnStrategy) {
			this->playTurnStrategy = playTurnStrategy;
		}
		else {
			log->errorMsg("Error: BotPlayer received a null playTurnStrategy!");
		}
		
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_int_distribution<int> dist(1, 1000);
		setPlayerName("Bot_" + std::to_string(dist(rng)));
		this->setAccountAmount(100.00f);
		showAccount();
		if (initalizeFunc) {
			this->initalizerFunction = initalizeFunc;
		}
		else {
			log->errorMsg("Error: BotPlayer received a null playTurnStrategy!");
		}
	}

	void blind(float bet)
	{
		if (this->isAllIN()) { return; }
		int betType = validateBet(bet);
		switch (betType)
		{
		case(0):
			this->fold();
			break;
		case(1):
			this->call();
			break;
		case(2):
			this->raise(bet - this->getMinimumBet());
			break;
		case(3):
			this->allIn();
			break;
		case(-1):
			if (getMinimumBet() > getPlayerAccount())
			{
				this->allIn();
			}
			else
			{
				this->call();
			}
			break;
		case(-2):
			this->allIn();
		}

		if (bet < this->getMinimumBet() && bet != getPlayerAccount())
		{
			log->errorMsg("Bet cannot be below the pevious bet");
			throw;
		}
		this->makeBet(bet);

	}

	void playTurn() override
	{

		if (this->isFolded()) { return; }
		if (this->isAllIN()) { return; }
		if (playTurnStrategy) 
		{
			try 
			{
				playTurnStrategy(this);
			}
			catch(...)
			{
				log->errorMsg("Bot strategy for Bot: " + this->getPlayerName() +" Has encounted an error");
			}
		}
		else {
			log->errorMsg("Error: playTurnStrategy is uninitialized!");
		}
	}
};