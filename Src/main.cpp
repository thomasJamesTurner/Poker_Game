#include "../headers/cards.h"
#include "../headers/table.h"
#include "../headers/player.h"
#include <windows.h>

void setBackgroundColour(int colour)
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

int main()
{
	setBackgroundColour(42);
	
	Table table;
	table.addPlayer();
	for (int i = 0;i < 4;i++)
	{
		table.playRound();
	}
}