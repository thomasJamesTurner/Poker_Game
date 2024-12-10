#include "../headers/cards.h"


void main()
{
	Deck deck;
	deck.makeDeck();
	for (int i = 0; i < 52;i++)
	{
		Card* card = deck.drawCard();
		std::cout << getCardName(*card)<< std::endl;
	}
}
