#pragma once
#include "display.h"
#include "cards.h"
#include "player.h"
#include <map>
#include "events.h"

struct HandType
{
	int handVlaue;
	std::string cardString;
};


class Table
{
private:
	EventHandler* handler;
	EventDispatcher dispatcher;
	logger* log;
	Deck deck;
	float pot = 0.0f;
	int currentPlayer = 0;
	int roundcounter = 0;
	std::vector<Card> flop;
	std::vector<Player*> players;
	std::vector<Player*> playersInRound;
	std::vector<Player*> playersToRemove;

	void addToPot(const Event& event)
	{
		const PlayerBetEvent& betEvent = static_cast<const PlayerBetEvent&>(event);
		pot += betEvent.betAmount;
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
	void nextPlayer(const Event& event)
	{
		if (playersInRound.size() <= 2)
		{
			RoundEndEvent roundEnd;
			handler->sendEvent(roundEnd);
			return;
		}
		for (Card card : flop)
		{
			log->msg(getCardName(card));
		}
		currentPlayer = getNextPlayer();
		PlayerStartTurn startTurn(playersInRound[currentPlayer]);
		log->msg("Next Players turn: " + playersInRound[currentPlayer]->getPlayerName());
		handler->sendEvent(startTurn);
		
	}
	void roundLeaderboard(const Event& event)
	{

		for (Card card : flop)
		{
			log->msg(getCardName(card));
		}
		if (playersInRound.size() == 1)
		{
			log->msg("### Winner ###\n" + playersInRound[0]->getPlayerName());
			PlayerWinEvent win(pot, (playersInRound[0]->getPlayerName()));
			handler->sendEvent(win);
			return;
		}
		sortPlayers();
		printPlayerHands();

		log->msg("### Winner ###\n" + playersInRound[0]->getPlayerName());
		PlayerWinEvent win(pot, (playersInRound[0]->getPlayerName()));
		handler->sendEvent(win);
		if (players.size() <= 1)
		{
			GameOverEvent game;
			handler->sendEvent(game);
			return;
		}
		
		removePlayer();
		log->msg("Player " + playersInRound[getNextPlayer()]->getPlayerName());
		log->msg("Player " + playersInRound[getNextPlayer(1)]->getPlayerName());
		startGame(10.0f, 20.0f, playersInRound[getNextPlayer()], playersInRound[getNextPlayer(1)], &deck);
	}
	void startRound(const Event& event)
	{
		if (roundcounter == 4)
		{
			RoundEndEvent endRound;
			handler->sendEvent(endRound);
			return;
		}
		switch (roundcounter)
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
		roundcounter++;
	}
	void playerExit(const Event& event)
	{
		const PlayerExitEvent& exitEvent = static_cast<const PlayerExitEvent&>(event);
		Player* playerToRemove = exitEvent.player;

		playersToRemove.push_back(playerToRemove);
	}
public:

	Table()
	{
		log = logger::getInstance("black", "green");
		handler = new EventHandler(&dispatcher);
		dispatcher.addHandler(handler);

		handler->subscribe({ EventType::PlayerBet,		std::bind(&Table::addToPot,			this, std::placeholders::_1) });
		handler->subscribe({ EventType::PlayerFold,		std::bind(&Table::playerFold,		this, std::placeholders::_1) });
		handler->subscribe({ EventType::PlayerTurnEnd,	std::bind(&Table::nextPlayer,		this, std::placeholders::_1) });
		handler->subscribe({ EventType::RoundEnd,		std::bind(&Table::roundLeaderboard, this, std::placeholders::_1) });
		handler->subscribe({ EventType::RoundStart,		std::bind(&Table::startRound, this, std::placeholders::_1) });

		deck.makeDeck();
		deck.shuffleDeck();
		
	}

	void addDisplay()
	{

	}

	void playGame()
	{
		playersInRound[currentPlayer]->playTurn();
	}

	int getNextPlayer(int offset = 0)
	{
		int nextPlayer;
		if (currentPlayer + offset >= playersInRound.size() - 1) {

			nextPlayer = 0;
		}
		else
		{
			nextPlayer = currentPlayer + 1 + offset;
		}
		return nextPlayer;
	}

	void startGame(float smallBlind = 10.0f, float bigblind = 20.0f, Player* smallblindPlayer = nullptr, Player* bigblindPlayer = nullptr, Deck* tableDeck = nullptr)
	{
		
		roundcounter = 0;
		deck.makeDeck();
		deck.shuffleDeck();

		playersInRound = players;				//players used for stuff for the overall round, players in round used for play by play
		printPlayers(playersInRound);
		if (smallblindPlayer == nullptr)
		{
			smallblindPlayer = playersInRound[getNextPlayer()];
		}
		if (bigblindPlayer == nullptr)
		{
			bigblindPlayer = playersInRound[getNextPlayer(1)];
		}
		if (tableDeck == nullptr)
		{
			tableDeck = &deck;
		}
		log->msg("Big Blind Player: " + bigblindPlayer->getPlayerName());
		currentPlayer = getNextPlayer(2);
		Player* startingPlayer = playersInRound[currentPlayer];
		PlayerStartTurn startEvent(startingPlayer);
		RoundStartEvent start_trigger(smallBlind, bigblind, smallblindPlayer, bigblindPlayer, tableDeck);
		handler->sendEvent(start_trigger);
		handler->sendEvent(startEvent);
	}

	void sortPlayers()
	{
		std::sort
		(
			players.begin(),
			players.end(),
			[&](Player* a, Player* b)
			{
				int p1score = checkHand(*a->getHand());
				int p2score = checkHand(*b->getHand());

				return p1score > p2score;
			}

		);
	}
	 
	void removePlayer()
	{

		std::string msg = ("Before removal - Players: " + std::to_string(players.size())) +
			(", PlayersInRound: " + std::to_string(playersInRound.size())) +
			(", PlayersToRemove: " + std::to_string(playersToRemove.size()));
		log->debugMsg(msg);
		printPlayers(playersInRound);


		for (auto it = playersToRemove.begin(); it != playersToRemove.end();)
		{
			Player* playerToRemove = *it;
			if (!playerToRemove) {
				log->errorMsg("Invalid player detected");
				it = playersToRemove.erase(it);
				continue;
			}

			auto playerIt = std::find(players.begin(), players.end(), playerToRemove);
			auto roundIt =  std::find(playersInRound.begin(), playersInRound.end(), playerToRemove);

			if (playerIt != players.end()) {
				players.erase(playerIt);
				delete playerToRemove;
			}

			if (roundIt != playersInRound.end()) {
				playersInRound.erase(roundIt);
			}
			it = playersToRemove.erase(it);
		}
		msg = ("After removal - Players: " + std::to_string(players.size())) +
			(", PlayersInRound: " + std::to_string(playersInRound.size())) +
			(", PlayersToRemove: " + std::to_string(playersToRemove.size()));
		log->debugMsg(msg);
	}

	void addPlayer(std::function<void(BotPlayer*)> playTurnStrategy = nullptr, std::function<void(BotPlayer*)> initalizeBot = nullptr)
	{
		Player* player;
		if (playTurnStrategy != nullptr)
		{
			BotPlayer* botPlayer;

			if (initalizeBot != nullptr)
			{
				botPlayer = new BotPlayer(&deck,&dispatcher, playTurnStrategy, initalizeBot);
			}
			else
			{
				botPlayer = new BotPlayer(&deck, &dispatcher, playTurnStrategy, [](BotPlayer* bot) {});
			}


			botPlayer->initalizerFunction(botPlayer);
			player = botPlayer;
			log->debugMsg("Bot player added: " + botPlayer->getPlayerName());

		}
		else
		{

			player = new User(&deck, &dispatcher);

		}
		if (player->getPlayerName() == "")
		{
			log->errorMsg("player name not initalised, this may cause issue");
			std::random_device rd;
			std::default_random_engine rng(rd());
			std::uniform_int_distribution<int> dist(1, 1000);
			player->setPlayerName("Player_" + std::to_string(dist(rng)));
			log->msgColour("white", "cyan", "random assigned player name: " + player->getPlayerName());
		}
		dispatcher.addHandler(player->getHandler());
		players.push_back(player);
		playersInRound.push_back(player);
	}

	void printPlayers(std::vector<Player*> players)
	{
		std::string msg;
		log->msgColour("white","magenta","[ PLAYERS ]");
		for (Player* player : players)
		{
			msg += (player->getPlayerName() + "\n");
		}
		log->msg(msg);

	}

	Deck* getDeck()
	{
		return &deck;
	}

	EventDispatcher* getDispatch()
	{
		return &dispatcher;
	}

	void addCardToTable(Card card)
	{
		flop.push_back(card);
	}


	void addCardsToTable(int numOfCards)
	{
		for (int i = 0; i < numOfCards; i++)
		{
			Card card = deck.drawCard();
			flop.push_back(card);
		}
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

	int findCardIndexByRank(Card searchCard, std::vector<Card> cards)
	{

		for (int i = 0;i < cards.size();i++)
		{
			if (searchCard.value == cards[i].value)
			{
				return i;
			}
		}
		return -1;
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
		for (Card card : cards)
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
		size_t size = cards.size();
		sortCardsByRank(cards);

		int lastCardVal = getRankValue(cards[0].value);
		int count = 1;
		for (int i = 1; i < size;i++)
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
		size_t size = hand.cards.size() - 1;
		sortCardsBySuit(hand.cards);
		Card lastCard = hand.cards[0];
		int suitCount[4] = { 0 };
		int count = 1;
		for (int i = 1;i <= size;i++)
		{

			Card currentCard = hand.cards[i];
			if (currentCard.suit == lastCard.suit)
			{

				count++;
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
		size_t size = cards.size();
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
				if (!acesHigh)
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
			log->msg("High Card");
			sortCardsByRank(hand.cards);
			val = getRankValue(hand.cards[hand.cards.size() - 1].value) + 1;
			return{ val,hand.cards[hand.cards.size() - 1].value };
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
			log->msg(std::to_string(pairs) + " Pair");
			return { pairs * 100 + val, cardString };
		}
		return { -1, "unknown" };
	}

	void printPlayerHands()
	{
		for (Player* player : playersInRound)
		{
			int handVal = checkHand(*(player->getHand()));
			switch (handVal)
			{
			case(1000):
				log->msg("Royal Flush");
				break;
			case(900):
				log->msg("Straight Flush");
				break;
			case(800):
				log->msg("Four Of A Kind");
				break;
			case(700):
				log->msg("Full House");
				break;
			case(600):
				log->msg("flush");
				break;
			case(500):
				log->msg("straight");
				break;
			case(400):
				log->msg("Three of a kind");
				break;
			}
			if (handVal > 300)
			{
				return;
			}
			HandType handType = checkPairs(*(player->getHand()));
			if (handType.handVlaue > 200 && handType.handVlaue < 300)
			{
				log->msg("2 pair" + handType.cardString);
			}
			if (handType.handVlaue > 100 && handType.handVlaue < 200)
			{
				log->msg("1 pair" + handType.cardString);
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
};