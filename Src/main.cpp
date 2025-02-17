#include "../headers/cards.h"
#include "../headers/table.h"
#include "../headers/player.h"
#include <windows.h>

static void setBackgroundColour(int colour)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y; // Total console size
	COORD topLeft = { 0, 0 };                           // Top-left position
	DWORD written;

	// Fill the console with spaces using the background color
	FillConsoleOutputCharacter(hConsole, ' ', consoleSize, topLeft, &written);
	FillConsoleOutputAttribute(hConsole, colour, consoleSize, topLeft, &written);

	// Move the cursor back to the top-left
	SetConsoleCursorPosition(hConsole, topLeft);
}


static void randomStrategy(BotPlayer* bot)
{
	float account = bot->getPlayerAccount();
	if (account > 0)
	{
		float betAmount = (rand() % static_cast<int>(account * 0.5)); // Random bet up to 50% balance
		if (betAmount < bot->getMinimumBet()) {
			bot->fold();
			return;
		}
		bot->blind(betAmount);
	}
	else {
		bot->fold();
	}
}

static void assertDominanceInit(BotPlayer* bot)
{
	bot->setPlayerName("all in alan");
	bot->subscribeToEvent(EventType::PlayerAllIn,
		[bot](const Event& event)->void
		{


			const PlayerAllInEvent& allInEvent = static_cast<const PlayerAllInEvent&>(event);
			if (allInEvent.allInPlayer != bot)
			{

				bot->getHandler()->sendEvent(PlayerExitEvent(bot));
			}
			else
			{
				return;
			}
		});
}
static void assertDominance(BotPlayer* bot)
{
	bot->allIn();
}


int count = 0;
std::vector<Card> cardsInPlay;
static void cardCounterInit(BotPlayer* bot)
{
	 
	bot->subscribeToEvent(EventType::PlayerFold,
		[&](const Event& event)->void
		{
			const PlayerFoldEvent& foldEvent = static_cast<const PlayerFoldEvent&>(event);
			for (Card card : foldEvent.hand.cards)
			{
				cardsInPlay.push_back(card);
			}
		});

	bot->subscribeToEvent(EventType::RoundStart,
		[&](const Event& event)->void
		{
			count = 0;
			cardsInPlay.clear();
			cardsInPlay = (*bot->getHand()).cards;
		});



}

//TODO add play on only good hands with random buff chance

int main()
{
	//setBackgroundColour(42);
	
	Table table;
	table.addPlayer();
	table.addPlayer(randomStrategy);
	table.addPlayer(assertDominance,assertDominanceInit);
	table.addPlayer(randomStrategy);
	while(!table.gameover)
	{
		table.playRound();
	}
}