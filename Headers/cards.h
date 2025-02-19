#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
#include<random>

bool acesHigh = true;
std::string suits[4] = { "spades","hearts","clubs","diamonds" };
std::string ranks[13] = {"Ace","2","3","4","5","6","7","8","9","10","Jack","Queen","King"};

struct Card
{
	std::string value;
	std:: string suit;
	Card(std::string v, std::string s) { value = v; suit = s; };
};

int getRankIndex(std::string rank)
{
	std::string* objectPtr =std::find(ranks, ranks + 13, rank);
	int idx = objectPtr - ranks;
	if (objectPtr == ranks + 13)
	{
		return -1;
	}
	return idx;
}

int getSuitIndex(std::string suit)
{
	std::string* objectPtr = std::find(suits, suits + 4, suit);
	int idx = objectPtr - suits;
	if (objectPtr == ranks + 4)
	{
		return -1;
	}
	return idx;
}

int getRankValue(std::string rank)
{
	if (acesHigh && rank == "Ace")
	{
		return 14;
	}
	else
	{
		return getRankIndex(rank) + 1;
	}
}

std::string getCardName(Card card)
{
	std::string output;
	if (card.suit == "hearts" || card.suit == "diamonds")
	{
		output = "\033[31;42m" + card.value + " of " + card.suit + "\033[0m";
	}
	else
	{
		output = "\033[30;42m" + card.value + " of " + card.suit + "\033[0m";
	}
	return output;
}


class Deck
{
public:
	std::vector<Card> cards;
	Card drawCard() 
	{
		try {
			Card topCard = cards.back();
			cards.pop_back();
			return topCard;
		}
		catch(...)
		{ 
			std::cout << "couldnt draw card: deck depleated" << std::endl;
		}
		
	}

	void makeDeck() 
	{
		cards.clear();
		for (int i = 0; i < 4;i++)
		{
			Card card = {"Ace",suits[i]};
			cards.push_back(card);
			for (int j = 1;j < 13; j++)
			{
				card.suit = suits[i];
				card.value = ranks[j];
				cards.push_back(card);
				
			}
		}
	}

	void printDeck()
	{
		for (Card card : cards)
		{
			std::cout << getCardName(card) << std::endl;
		}
	}

	void shuffleDeck()
	{
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::shuffle(std::begin(cards), std::end(cards), rng);
	}
};

class Hand
{
public:
	std::vector<Card> cards;

	void showCards()
	{
		std::cout << "\n";
		for (Card card : cards)
		{
			std::cout << getCardName(card) << std::endl;
		}
	}

	void makeHand(Deck* deck,int numOfCards)
	{
		cards.clear();

		for (int i = 0; i < numOfCards;i++)
		{
			addCardToHand(deck->drawCard());
		}
	}

	void addCardToHand(Card card)
	{
		cards.push_back(card);
	}

	std::vector<Card> getHandCards()
	{
		return cards;
	}
};;