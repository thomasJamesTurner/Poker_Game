#include "../headers/cards.h"


int main()
{
	Deck deck;
	deck.makeDeck();
	deck.shuffleDeck();
	Hand hand;
	for (int i = 0; i < 5;i++)
	{
		hand.addCardToHand(deck.drawCard());
	}
	hand.showCards();
}
