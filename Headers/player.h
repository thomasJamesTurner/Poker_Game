#pragma once
#include "cards.h"
#include <cmath>
#include <iostream>
#include <string>


class Player
{
	std::string playerName;
	Hand playerHand;
	float account;

public:

	Player(Deck* deck)
	{
		setPlayerName();
		std::cout << "\033[30;42m" << "Player Name: " << playerName << "\033[0m" << std::endl;
		setAccountAmount();
		std::cout << "\033[30;42m" << "Balance $" << account << "\033[0m" << std::endl;
		playerHand.makeHand(deck, 2);
		playerHand.showCards();
	}

	void setAccountAmount()
	{
		std::cout << "\033[30;42m" << "Input Amount To Play With: " << "\033[0m" << std::endl;
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
		std::cout << "\033[30;42m" << "Input Player Name: " << "\033[0m" << std::endl;
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
		int(account*100);
		std::cout << "\033[30;42m" << "£" << account/100 << "\033[0m" << std::endl;
	}

	inline Hand* getHand()
	{
		return &playerHand;
	}



};