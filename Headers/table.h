#pragma once
#include "cards.h"
#include "player.h"
#include <map>
#include <string>
#include "events.h"


struct HandType
{
	int handVlaue;
	std::string cardString;
};

class Table
{
	Deck deck;
	float pot = 0.0f;
	std::vector<Card> flop;
	std::vector<Player*> players;
	std::vector<Player*> playersInRound;
	std::vector<Player*> playersToRemove;
	EventHandler* handler;
	EventDispatcher dispatcher;
	
public:
	bool gameover = false;
	void addToPot(const Event& event)
	{
		const PlayerBetEvent& betEvent = static_cast<const PlayerBetEvent&>(event);

		pot += betEvent.betAmount;

	}

	void removePlayer()
	{
		std::cout	<< "Before removal - Players: " << players.size()
					<< ", PlayersInRound: " << playersInRound.size()
					<< ", PlayersToRemove: " << playersToRemove.size() << std::endl;

		for (auto it = playersToRemove.begin(); it != playersToRemove.end();)
		{
			Player* playerToRemove = *it;
			if (!playerToRemove) {
				std::cout << "Invalid player detected" << std::endl;
				it = playersToRemove.erase(it);
				continue;
			}

			auto playerIt = std::find(players.begin(), players.end(), playerToRemove);
			auto roundIt = std::find(playersInRound.begin(), playersInRound.end(), playerToRemove);

			if (playerIt != players.end()) {
				players.erase(playerIt);
				delete playerToRemove;
			}
			if (roundIt != playersInRound.end()) {
				playersInRound.erase(roundIt);
			}
			it = playersToRemove.erase(it);
		}

		std::cout << "After removal - Players: " << players.size()
			<< ", PlayersInRound: " << playersInRound.size()
			<< ", PlayersToRemove: " << playersToRemove.size() << std::endl;

		if (players.size() <= 1) 
		{
			gameover = true;
		}
	}

	void playerExit(const Event& event)
	{
		const PlayerExitEvent& exitEvent = static_cast<const PlayerExitEvent&>(event);
		Player* playerToRemove = exitEvent.player;

		playersToRemove.push_back(playerToRemove);
	}

	void playerFold(const Event& event)
	{
		const PlayerFoldEvent& foldEvent = static_cast<const PlayerFoldEvent&>(event);

		auto it = std::find(playersInRound.begin(), playersInRound.end(), foldEvent.player);

		if (it != playersInRound.end())
		{
			playersInRound.erase(it);
		}
		else
		{
			playersInRound.pop_back();
		}

	}


	Table()
	{
		handler = new EventHandler(&dispatcher);
		dispatcher.addHandler(handler);
		handler->subscribe({ EventType::PlayerBet, std::bind(&Table::addToPot, this, std::placeholders::_1) });
		handler->subscribe({ EventType::PlayerExit, std::bind(&Table::playerExit, this, std::placeholders::_1) });
		handler->subscribe({ EventType::PlayerFold, std::bind(&Table::playerFold, this, std::placeholders::_1) });

		deck.makeDeck();
		deck.shuffleDeck();
	}

	

	

	void addPlayer(std::function<void(BotPlayer*)> playTurnStrategy = nullptr, std::function<void(BotPlayer*)> initalizeBot = nullptr)
	{
		Player* player;
		if (playTurnStrategy != nullptr)
		{
			BotPlayer* botPlayer;
			
			if (initalizeBot != nullptr)
			{
				botPlayer = new BotPlayer(&deck, &dispatcher, playTurnStrategy, initalizeBot);
			}
			else
			{
				botPlayer = new BotPlayer(&deck, &dispatcher, playTurnStrategy, [](BotPlayer* bot) {});
			}


			botPlayer->initalizerFunction(botPlayer);
			player = botPlayer;

		}
		else 
		{
			player = new UserPlayer(&deck, &dispatcher);
			
		}
		dispatcher.addHandler(player->getHandler());
		players.push_back(player);
	}

	void printPlayers(std::vector<Player*> players)
	{
		tableMsg("[ PLAYERS ]");
		for (Player* player : players)
		{
			tableMsg(player->getPlayerName());
		}
	}

	void playRound()
	{
		deck.makeDeck();
		std::cout<<"\n" << std::endl;
		deck.shuffleDeck();
		pot = 0;
		flop.clear();
		RoundStartEvent startRound(10.0f, 20.0f,players[0],players[1],&deck);
		handler->sendEvent(startRound);
		playersInRound = players;				//players used for stuff for the overall round, players in round used for play by play
		dispatcher.printHandlers();

		for(int i = 0; i<4;i++)
		{
			
			std::cout << "\n\n" << std::endl;
			printPlayers(playersInRound);
			std::cout << "\n" << std::endl;
			switch (i)
			{
			case(1):
				addCardsToTable(3);
				break;
			case(2):
				addCardsToTable(1);
				break;
			case(3):
				addCardsToTable(1);
				break;
			}
			printTable();
			for (Player* player : playersInRound)
			{
				player->playTurn();
			}
			std::cout << "Players: " << players.size() << ", PlayersInRound: " << playersInRound.size() << std::endl;
			if (playersInRound.size() <= 1)
			{
				break;
			}
			removePlayer();
		}
		roundLeaderboard();
		removePlayer();
	}

	void sortPlayers()
	{
		std::sort
		(
			players.begin(),
			players.end(),
			[&](Player *a, Player *b)
			{
				int p1score = checkHand(*a->getHand());
				int p2score = checkHand(*b->getHand());

				return p1score > p2score;
			}

		);
	}

	void roundLeaderboard()
	{
		for (Card card : flop)
		{
			std::cout << getCardName(card) << std::endl;
		}
		sortPlayers();
		printPlayerHands();

		tableMsg("### Winner ###\n" + playersInRound[0]->getPlayerName());
		PlayerWinEvent win(pot,(playersInRound[0]->getPlayerName()));
		handler->sendEvent(win);
	}

	void tableMsg(std::string msg)
	{
		std::cout << "\033[30;42m" << msg << std::endl;
	}

	void addCardsToTable(int numOfCards)
	{
		for (int i = 0; i < numOfCards; i++)
		{
			Card card = deck.drawCard();
			flop.push_back(card);
		}
	}

	void printTable()
	{
		for (Card card : flop)
		{
			std::cout << getCardName(card) << std::endl;
		}
	}

	void setTable(std::vector<Card> cards)
	{
		flop = cards;
	}

	void addCardToTable(Card card)
	{
		flop.push_back(card);
	}

	bool compareCardsRank(const Card& a, const Card& b)
	{
		int idx_a = getRankIndex(a.value);
		int idx_b = getRankIndex(b.value);

		if (idx_a != -1 && idx_b != -1) {
			return idx_a < idx_b;
		}

		// If only one string is in the predefined order, prioritize it
		if (idx_a != -1) return true;
		if (idx_b != -1) return false;
	};

	bool compareCardsSuit(const Card& a, const Card& b)
	{
		int idx_a = getSuitIndex(a.suit);
		int idx_b = getSuitIndex(b.suit);

		if (idx_a != -1 && idx_b != -1) {
			return idx_a < idx_b;
		}

		// If only one string is in the predefined order, prioritize it
		if (idx_a != -1) return true;
		if (idx_b != -1) return false;
	}

	int findCardIndexByRank(Card searchCard,std::vector<Card> cards)
	{
		
		for (int i = 0;i<cards.size();i++)
		{
			if (searchCard.value == cards[i].value)
			{
				return i;
			}
		}
	}

	void sortCardsByRank(std::vector<Card>& cards)
	{
		std::sort
		(
			cards.begin(),
			cards.end(),
			[](const Card& a, const Card& b)
			{
				int idx_a = getRankValue(a.value);
				int idx_b = getRankValue(b.value);
				return idx_a < idx_b;

			}
			
		);
	}

	void sortCardsBySuit(std::vector<Card>& cards)
	{
		std::sort
		(
			cards.begin(),
			cards.end(),
			[](const Card& a, const Card& b)
			{
				int idx_a = getSuitIndex(a.suit);
				int idx_b = getSuitIndex(b.suit);
				return idx_a < idx_b;
			}

		);

	}

	int checkKind(Hand hand)
	{
		std::vector<Card> cards = hand.cards;
		sortCardsByRank(hand.cards);
		Card lastCard = { "","" };
		int count = 1;
		for(Card card : cards)
		{
			if (lastCard.value == card.value)
			{
				count++;
			}
		}
		return count;
	}

	bool checkStraight(Hand hand)
	{
		std::vector<Card> cards = hand.cards;
		int size = cards.size();
		sortCardsByRank(cards);

		int lastCardVal = getRankValue(cards[0].value);
		int count = 1;
		for (int i = 1; i< size;i++)
		{
			Card card = cards[i];
			int currentCardVal = getRankValue(card.value);
			if (currentCardVal == lastCardVal + 1)
			{
				count++;
			}
			else
			{
				count = 1;
			}
			if (count == 5)
			{
				return true;					//retuns straight
			}
			lastCardVal = currentCardVal;

		}
		
		return false;						//returns unknown hand
	}

	bool checkFlush(Hand hand)
	{
		hand.cards;
		int size = hand.cards.size() - 1;
		sortCardsBySuit(hand.cards);
		Card lastCard = hand.cards[0];
		int suitCount[4] = {0};
		int count = 1;
		for (int i = 1;i<=size;i++)
		{
			
			Card currentCard = hand.cards[i];
			//std::cout << "last card: " << getCardName(lastCard) << "\tcurrent card: " << getCardName(currentCard) <<std::endl;
			if (currentCard.suit == lastCard.suit)
			{

				count ++;
			}
			else
			{
				int suitIdx = getSuitIndex(lastCard.suit);
				if (suitIdx != -1)
				{
					suitCount[suitIdx] = count;
				}
				count = 1;
			}


			if (count >= 5)
			{

				return true;
			}
			
			lastCard = currentCard;
			
		}
		if (std::find(std::begin(suitCount), std::end(suitCount), 5) != std::end(suitCount))
		{

			return true;					//returns flush
		}

		return false;						//returns unknown hand
	}

	bool checkRoyalFlush(Hand hand)
	{
		std::vector<Card> cards = hand.cards;
		sortCardsBySuit(cards);
		int size = cards.size();
		bool isAce = false;
		for (Card card : cards)
		{
			if (card.value == "Ace") { isAce = true; }
		}
		if (isAce)
		{
			sortCardsByRank(cards);
			
			int index = 0;
			if (!acesHigh)
			{
				
			}
			else
			{
				index = findCardIndexByRank({ "10","" }, cards);
			}
			Hand sortedHand;
			sortedHand.cards = std::vector<Card>(cards.begin() + index, cards.end());

			if (checkFlush(sortedHand))
			{
				sortCardsByRank(sortedHand.cards);
				if(!acesHigh)
				{
					if
						(
							sortedHand.cards[0].value == "Ace" &&
							sortedHand.cards[1].value == "10" &&
							sortedHand.cards[2].value == "Jack" &&
							sortedHand.cards[3].value == "Queen" &&
							sortedHand.cards[4].value == "King"
							)
					{
						return true;
					}
				}
				else
				{
					if
						(
							sortedHand.cards[4].value == "Ace" &&
							sortedHand.cards[0].value == "10" &&
							sortedHand.cards[1].value == "Jack" &&
							sortedHand.cards[2].value == "Queen" &&
							sortedHand.cards[3].value == "King"
							)
					{
						return true;
					}

				}
				
			}
		}
		return false;
		

		
	}

	HandType checkPairs(Hand hand)
	{
		std::map<std::string, int> map;
		std::string cardString;
		std::vector<std::string> cardNames;
		int val = 0;
		// Count occurrences of each value
		for (const auto& card : hand.cards) 
		{
			val += getRankValue(card.value);
			map[card.value]++;
		}

		// Analyze the counts
		int pairs = 0;
		bool threeOfAKind = false;
		bool fourOfAKind = false;
		
		for (const auto& entry : map) 
		{
			int cardVal = getRankValue(entry.first);
			if (entry.second == 2) 
			{
				pairs++;
				val += cardVal;
				cardNames.push_back(entry.first);
			}
			else if (entry.second == 3) 
			{
				threeOfAKind = true;
				val += cardVal;
				cardNames.push_back(entry.first);
			}
			else if (entry.second == 4) 
			{
				fourOfAKind = true;
				val += cardVal;
				cardNames.push_back(entry.first);
			}
			
		}
		if (cardNames.size() != 0)
		{
			cardString.reserve(cardNames.size() * sizeof(char) * 2); // Estimate the total size
			for (const std::string& name : cardNames) {
				if (cardString.size() + name.size() + 3 > 1000) { // Arbitrary max length
					cardString += "..."; // Indicate truncation
					break;
				}
				cardString += name + ", ";
			}
		}
		else
		{
			tableMsg("High Card");
			sortCardsByRank(hand.cards);
			val = getRankValue(hand.cards[hand.cards.size() - 1].value) + 1;
			return{val,hand.cards[hand.cards.size() - 1].value};
		}
		
		if (fourOfAKind)
		{
			return { 800,cardString };
		}
		if (threeOfAKind && pairs > 0)
		{
			return{ 700 ,cardString };
		}
		if (threeOfAKind)
		{
			return { 400 ,cardString };;
		}
		if (pairs > 0)
		{
			tableMsg(std::to_string(pairs) + " Pair");
			return {pairs * 100 + val, cardString };
		}
	}


	void printPlayerHands()
	{
		for (Player* player : playersInRound)
		{
			int handVal = checkHand(*(player->getHand()));
			switch (handVal)
			{
			case(1000):
				tableMsg("Royal Flush");
				break;
			case(900):
				tableMsg("Straight Flush");
				break;
			case(800):
				tableMsg("Four Of A Kind");
				break;
			case(700):
				tableMsg("Full House");
				break;
			case(600):
				tableMsg("flush");
				break;
			case(500):
				tableMsg("straight");
				break;
			case(400):
				tableMsg("Three of a kind");
				break;
			}
			if (handVal > 300)
			{
				return;
			}
			HandType handType = checkPairs(*(player->getHand()));
			if (handType.handVlaue > 200 && handType.handVlaue < 300)
			{
				tableMsg("2 pair" + handType.cardString);
			}
			if (handType.handVlaue> 100 && handType.handVlaue < 200)
			{
				tableMsg("1 pair" + handType.cardString);
			}
		}
	}

	int checkHand(Hand hand)
	{	
		for (Card card : flop)
		{
			
			hand.cards.push_back(card);
		}

		int handScore = 0;
		bool flush = checkFlush(hand);
		bool straight = checkStraight(hand);

		if (flush && straight)
		{
			if (checkRoyalFlush(hand))
			{
				handScore = 1000;
				return handScore;
			}
		}

		if (flush && straight)
		{
			handScore = 900;
			return handScore;
		}

		if (flush)
		{
			handScore = 600;
		}

		if (straight)
		{
			handScore = 500;
		}
		if (handScore == 0)
		{
			HandType type = checkPairs(hand);
			handScore += type.handVlaue;
		}
		return handScore;
	}

	//____ getters and setters ____//

	inline Deck* getDeck()
	{
		return &deck;
	}

	inline EventDispatcher* getEventDispatcher()
	{
		return &dispatcher;
	}
};
