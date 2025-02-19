#pragma once
#include "cards.h"
#include "events.h"
#include <cmath>
#include <iostream>
#include <string>


class Player
{
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
		std::cout << "DELETING PLAYER " << getPlayerName() << std::endl;
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
			std::cout << "\033[30;42m" << "win amount $" << winEvent.winnings << "\033[0m" << std::endl;
			account += winEvent.winnings;
			showAccount();
		}
		if (this->getPlayerAccount() <= 0)
		{
			std::cout<< "Player: "<< getPlayerName()<<" has no money left to play with and will exit" << std::endl;
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
		std::string input;
		std::cout << "\n\033[37;45m"	<< "[ Player Input ]" << "\033[0m" << std::endl;
		std::cout <<   "\033[37;45m"	<< "f : fold" << "\033[0m" << std::endl;
		std::cout <<   "\033[37;45m"	<< "c : check" << "\033[0m" << std::endl;
		std::cout <<   "\033[37;45m"	<< "l : call" << "\033[0m" << std::endl;
		std::cout <<   "\033[37;45m"	<< "r : raise" << "\033[0m" << std::endl;
		std::cout <<   "\033[37;45m"	<< "a : all in" << "\033[0m" << std::endl;

		int result = 0;
		do
		{
			std::cout << "\n\033[30;42m" << getPlayerName() << " action: " << "\033[0m";
			std::getline(std::cin, input);
			std::transform(input.begin(), input.end(), input.begin(), ::tolower);
			if (input == "f" || input =="fold")							{ result = 0; break; }
			if (input == "c" || input =="check")						{ result = 1; break; }
			if (input == "l" || input =="call")							{ result = 2; break; }
			if (input == "r" || input =="raise")						{ result = 3; break; }
			if (input == "a" || input == "all in" || input =="allin")	{ result = 4; break; }
			result = -1;
			std::cout << "\033[30;42m" << "Please Input A Valid Action" << "\033[0m";
		} while (result == -1);
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
					std::cout << "\033[30;42m" << "raise by: " << "\033[0m";
					std::getline(std::cin, input);
					float bet = std::stof(input);
					badAction = raise(bet) == -1;
				}
				catch (...)
				{
					std::cout << "\033[30;42m" << "Please Enter Valid Input" << "\033[0m" << std::endl;
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
		std::cout << "\033[30;42m" << "Bet $" << betAmount << "\033[0m" << std::endl;
	}
	void fold()
	{
		folded = true;
		handler->sendEvent(PlayerFoldEvent(this,*(this->getHand())));
		std::cout << "\033[30;42m" << getPlayerName() <<" folded" << "\033[0m" << std::endl;
	}
	void check()
	{
		folded = true;
		std::cout << "\033[30;42m" << getPlayerName() << " checked" << "\033[0m" << std::endl;
		handler->sendEvent(PlayerCheckEvent(this));
	}
	void call()
	{
		makeBet(minBet);
		std::cout << "\033[30;42m" << getPlayerName() << " called" << "\033[0m" << std::endl;
		handler->sendEvent(PlayerCallEvent(this));
	}
	void allIn()
	{
		std::cout << "\033[30;42m" << getPlayerName() << " has gone ALL IN" << "\033[0m" << std::endl;
		allin = true;
		handler->sendEvent(PlayerAllInEvent(this));
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

	void setPlayerName()
	{
		std::cout << "\033[30;42m" << "Input Player Name: " << "\033[0m";
		std::getline(std::cin, playerName);
		if (playerName == "")
		{
			playerName = rand();
		}
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
		std::cout << "\033[30;42m" << playerName <<" Balance $" << account << "\033[0m" << std::endl;
	}

	inline void showMinBet()
	{
		std::cout << "\033[30;42m" <<"Prevoius Bet: " << minBet << "\033[0m" << std::endl;
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
		std::cout << "getting " << playerName << " handler" << std::endl;
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
public:
	UserPlayer(Deck* deck, EventDispatcher* dispatch) : Player(deck, dispatch)
	{
		setPlayerName();
		std::cout << "\033[30;42m" << "Player Name: " << this->getPlayerName() << "\033[0m" << std::endl;
		setUserAccountAmount();
		showAccount();
	}

	void setUserAccountAmount()
	{
		std::cout << "\033[30;42m" << "Input Amount To Play With: " << "\033[0m";
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
	
public:
	std::function<void(BotPlayer*)> initalizerFunction;
	
	BotPlayer(Deck* deck, EventDispatcher* dispatch,std::function<void(BotPlayer*)> playTurnStrategy, std::function<void(BotPlayer*)> initalizeFunc): Player(deck,dispatch)
	{
		
		if (playTurnStrategy) {
			this->playTurnStrategy = playTurnStrategy;
		}
		else {
			std::cerr << "Error: BotPlayer received a null playTurnStrategy!\n";
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
			std::cerr << "Error: BotPlayer received a null playTurnStrategy!\n";
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
			std::cout << "\033[30;42m" << "Bet cannot be below the pevious bet" << "\033[0m" << std::endl;
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
				std::cout << "\n\033[37;41m" << "Bot strategy for Bot:" << "\033[0m" << " " << this->getPlayerName() << " \033[37;41m" << "Has encounted an error";
			}
		}
		else {
			std::cerr << "Error: playTurnStrategy is uninitialized!\n";
		}
	}
};