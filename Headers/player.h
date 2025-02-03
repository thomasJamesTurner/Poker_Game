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
	float minblind;
	EventDispatcher* dispatcher;

public:
	Player(Deck* deck,EventDispatcher* dispatch)
	{
		this->dispatcher = dispatch;
		dispatcher->subscribe(EventType::RoundStart, std::bind(&Player::startRound, this, std::placeholders::_1));
		dispatcher->subscribe(EventType::PlayerWin, std::bind(&Player::winGame, this, std::placeholders::_1));
		dispatcher->subscribe(EventType::PlayerBet, std::bind(&Player::setMinimumBet, this, std::placeholders::_1));
		playerName = "";
		account = 0.0f;
		minblind = 0.0f;
	}
	virtual ~Player() {}

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
		if (account <= 0)
		{
			std::cout<< "Player: "<< getPlayerName()<<" has no money left to play with and will exit" << std::endl;
			PlayerExitEvent playerExit(this);
			dispatcher->dispatch(playerExit);
		}

		
	}

	void startRound(const Event& event)
	{
		const RoundStartEvent& startEvent = static_cast<const RoundStartEvent&>(event);

		minblind = startEvent.bigBlindAmount;
		if (startEvent.smallBlind == this)
		{
			makeBet(startEvent.smallBlindAmount);
		}
		if (startEvent.bigBlind == this)
		{
			makeBet(minblind);
		}
		
	}

	virtual void blind()
	{
		std::string amount;
		float bet = 0.0f;
		while (bet > account || bet ==0.0f || bet<minblind && bet != account)
		{
			bet = 0.0f;
			try
			{
				std::cout << "\033[30;42m" << getPlayerName() <<" bet amount: " << "\033[0m";
				std::getline(std::cin, amount);
				bet = std::stof(amount);

			}
			catch (...)
			{
				amount = "";
			}

			if (amount == "")
			{
				bet = account;

			}

			if (bet == account)
			{
				std::cout << "\033[30;42m" << getPlayerName()<< " has gone ALL IN" << "\033[0m" << std::endl;
			}

			if (bet > account)
			{
				std::cout << "\033[30;42m" << "Bet cannot be greater than the total of the account" << "\033[0m" << std::endl;
			}
			if (bet < minblind)
			{
				std::cout << "\033[30;42m" << "Bet cannot be below the pevious bet" << "\033[0m" << std::endl;
			}
		}
		makeBet(bet);
	}

	

	void makeBet(float betAmount)
	{
		account -= betAmount;
		PlayerBetEvent bet(playerName,betAmount);
		dispatcher->dispatch(bet);
		showAccount();
		std::cout << "\033[30;42m" << "Bet $" << betAmount << "\033[0m" << std::endl;
	}

	void setMinimumBet(const Event& event)
	{
		const PlayerBetEvent& betEvent = static_cast<const PlayerBetEvent&>(event);
		minblind = betEvent.betAmount;
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
		return minblind;
	}

	virtual void playTurn()
	{
		playerHand.showCards();
		blind();
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

	
	BotPlayer(Deck* deck, EventDispatcher* dispatch, std::function<void(BotPlayer*)> playTurnStrategy): Player(deck,dispatch)
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
	}


	void playTurn() override
	{
		if (playTurnStrategy) 
		{
			playTurnStrategy(this);
		}
		else {
			std::cerr << "Error: playTurnStrategy is uninitialized!\n";
		}
	}

	void blind(float bet)
	{
		float account = this->getPlayerAccount();

		if (bet > account)
		{
			bet = account;
		}

		if (bet == account)
		{
			std::cout << "\033[30;42m" << getPlayerName() << " has gone ALL IN" << "\033[0m" << std::endl;
		}

		
		if (bet < this->getMinimumBet() && bet != account)
		{
			std::cout << "\033[30;42m" << "Bet cannot be below the pevious bet" << "\033[0m" << std::endl;
		}
		this->makeBet(bet);
		
	}

};