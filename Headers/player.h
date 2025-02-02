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
	EventDispatcher* dispatcher;

public:
	Player(Deck* deck,EventDispatcher* dispatch)
	{
		this->dispatcher = dispatch;
		dispatcher->subscribe(EventType::PlayerWin, std::bind(&Player::winGame, this, std::placeholders::_1));
		setPlayerName();
		std::cout << "\033[30;42m" << "Player Name: " << playerName << "\033[0m" << std::endl;
		setAccountAmount();
		showAccount();
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

		
	}

	void blind()
	{
		std::cout << "\033[30;42m" << "Input bet amount: " << "\033[0m" << std::endl;
		std::string amount;
		float bet = 0;
		try
		{
			std::getline(std::cin, amount);
			bet = std::stof(amount);
			
		}
		catch (...)
		{
			amount = "";
		}

		if (amount == "")
		{
			bet = account/10;
			
		}

		if (bet > account)
		{
			std::cout << "\033[30;42m" << "Bet cannot be greater than the total of the account" << "\033[0m" << std::endl;
			blind();
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

	void setAccountAmount()
	{
		std::cout << "\033[30;42m" << "Input Amount To Play With: " << "\033[0m";
		std::string amount;
		try
		{
			std::getline(std::cin, amount);
			account = std::stof(amount);
		}
		catch(...)
		{
			amount = "";
		}
		if (amount == "")
		{
			account = 100.00f;
		}

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
};