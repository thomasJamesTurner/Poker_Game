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

static void assertDominance(BotPlayer* bot)
{
	bot->allIn();
	bot->subscribeToEvent(EventType::PlayerAllIn,
		[&](const Event& event)->void
		{
			const PlayerAllInEvent& allInEvent = static_cast<const PlayerAllInEvent&>(event);
			if (allInEvent.allInPlayer != bot)
			{
				bot->fold();
			}
		});
}

static void cardCounter(BotPlayer* bot)
{
	std::vector<Card> cards = (*bot->getHand()).cards;
	

}

int main()
{
	//setBackgroundColour(42);
	
	Table table;
	table.addPlayer();
	table.addPlayer(randomStrategy);
	table.addPlayer(assertDominance);
	table.addPlayer(randomStrategy);
	while(!table.gameover)
	{
		table.playRound();
	}
}