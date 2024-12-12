#include<iostream>
#include<vector>
#include<algorithm>
#include<random>

enum class Suits
{
	hearts,
	diamonds,
	clubs,
	spades
};

enum class Values
{
	ace = 0,
	n2,
	n3,
	n4,
	n5,
	n6,
	n7,
	n8,
	n9,
	n10,
	jack,
	queen,
	king
};

struct Card
{
	Values value;
	Suits suit;
};


std::string getCardName(Card card)
{
	std::string suitName;
	std::string valueName;
	switch (card.suit)
	{
	case(Suits::hearts):
		suitName = "hearts";
		break;
	case(Suits::clubs):
		suitName = "clubs";
		break;
	case(Suits::diamonds):
		suitName = "diamonds";
		break;
	case(Suits::spades):
		suitName = "spades";
		break;
	default:
		suitName = "unknown";
	}
	switch (card.value) {
		case Values::ace:
			valueName = "ace";
			break;
		case Values::n2:
			valueName = "2";
			break;
		case Values::n3:
			valueName = "3";
			break;
		case Values::n4:
			valueName = "4";
			break;
		case Values::n5:
			valueName = "5";
			break;
		case Values::n6:
			valueName = "6";
			break;
		case Values::n7:
			valueName = "7";
			break;
		case Values::n8:
			valueName = "8";
			break;
		case Values::n9:
			valueName = "9";
			break;
		case Values::n10:
			valueName = "10";
			break;
		case Values::jack:
			valueName = "jack";
			break;
		case Values::queen:
			valueName = "queen";
			break;
		case Values::king:
			valueName = "king";
			break;
		default:
			valueName = "Unknown";
			break;
	}

	return valueName + " " + suitName;

}


class Deck
{
	std::vector<Card> cards;

public:
	Card drawCard() 
	{
		Card topCard = cards.back();
		cards.pop_back();
		return topCard;
	}

	void makeDeck() 
	{
		for (int i = 0; i < 4;i++)
		{
			for (int j = 0;j < 13; j++)
			{
				Card card;
				card.suit = static_cast<Suits>(i);
				card.value = static_cast<Values>(j);
				cards.push_back(card);
				
			}
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
	std::vector<Card> cards;
public:

	void showCards()
	{
		for (Card card : cards)
		{
			std::cout << getCardName(card) << std::endl;
		}
	}

	void addCardToHand(Card card)
	{
		cards.push_back(card);
	}

};;