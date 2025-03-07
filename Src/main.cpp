#include "../headers/cards.h"
#include "../headers/table.h"
#include "../headers/player.h"
#include <Spritter/Spritter.h>

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
std::vector<Card> cardsPlayed;
Deck cardsInPlay;
static void cardCounterInit(BotPlayer* bot)
{
	bot->setPlayerName("count cardula");
	bot->subscribeToEvent(EventType::PlayerFold,
		[&](const Event& event)->void
		{
			const PlayerFoldEvent& foldEvent = static_cast<const PlayerFoldEvent&>(event);
			for (Card card : foldEvent.hand.cards)
			{
				cardsPlayed.push_back(card);
			}
		});

	bot->subscribeToEvent(EventType::RoundStart,
		[&](const Event& event)->void
		{
			count = 0;
			cardsPlayed.clear();
			cardsInPlay.makeDeck();
			cardsPlayed = (*bot->getHand()).cards;
		});
}

static void cardCounter(BotPlayer* bot)
{
	float cardChance = 1.0f / cardsInPlay.cards.size();
	Hand hand = *bot->getHand();
	if (hand.cards[0].suit == hand.cards[1].suit)
	{

	}
}

//TODO add play on only good hands with random buff chance

using namespace Spritter;
using namespace Spritter::Math;
using namespace Spritter::Graphics;

class PokerGame : public Game
{
	Table table;
	std::unique_ptr<SpriteRenderer> _renderer;
	std::unique_ptr<Texture> _sprite;
	void Initialize() override 
	{ 
		_renderer = std::make_unique<SpriteRenderer>(*GraphicsDevice);

		_sprite = GraphicsDevice->CreateTexture("Content/small_hands.jpg");

		table.addPlayer();
		table.addPlayer(randomStrategy);
		table.addPlayer(assertDominance, assertDominanceInit);
		table.addPlayer(randomStrategy);

	}

	void Update(const float dt) override 
	{
		if (!table.gameover)
		{
			table.playRound();
		}
		else
		{
			return;
		}
		
	}

	void Draw() override
	{
		GraphicsDevice->Clear(Color::CornflowerBlue());
		_renderer->Draw(_sprite.get(), Vector2f::Zero());
		_renderer->Render();
	}
};

int main()
{
	GameOptions options
	{
		/* Name: */ "Poker Game",
		/* Size: */ { 1280, 720 },
		/* Resizable: */ true,
	};
	PokerGame game;
	game.Run(options);

	return 0;
}