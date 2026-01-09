#pragma once
#include "Main.h"

// High/Low - draw one card, player guesses higher or lower
class HighLow {
private:
	Deck deck;

	void displayCardOne(const Card& c) const {
		auto art = c.displayCard();
		for (auto& L : art) std::cout << L << "\n";
	}

public:
	void play(Player& player, CasinoManager &casino) {
		drawAsciiBox("=== High / Low ===");

		player.showStatus();

		// Blessings prompt
		std::string cast = readLineTrimmed("Cast a Blessing before this round? (y/n) ");
		if (!cast.empty() && (cast[0] == 'y' || cast[0] == 'Y')) {
			std::cout << "1 Fate's Glimpse (15)\n2 Lucky Draw (20)\n3 Mana Shield (10)\n0 Skip\n";
			int pick = readInt("Choose: ", 0, 3);
			if (pick == 1) player.castBlessing("Fate's Glimpse");
			else if (pick == 2) player.castBlessing("Lucky Draw");
			else if (pick == 3) player.castBlessing("Mana Shield");
		}

		// Place bet
		double bet;
		if (!casino.placeBet(bet)) { pauseEnter(); return; }

		deck.shuffle();

		Card current = deck.dealCard();
		std::cout << "\nCurrent card:\n";
		displayCardOne(current);

		// player choice H/L
		std::string choice = readLineTrimmed("Will the next card be (H)igher or (L)ower? ");
		if (choice.empty()) { drawAsciiBox("No choice — round aborted."); player.refundCurrentBet(); pauseEnter(); return; }
		char ch = std::toupper(choice[0]);

		// Muddled Sight can flip the choice
		if (player.hasCurse("Muddled Sight") && randint(1, 100) <= 20) {
			drawAsciiBox("Muddled Sight flips your choice!");
			ch = (ch == 'H') ? 'L' : 'H';
		}

		// Next card uses drawCardForPlayer (Lucky Draw interacts)
		Card next = drawCardForPlayer(deck, player);

		std::cout << "\nNext card:\n";
		displayCardOne(next);

		int vcur = static_cast<int>(current.getRank());
		int vnext = static_cast<int>(next.getRank());

		if (vnext == vcur) {
			drawAsciiBox("Tie — bet returned.");
			player.refundCurrentBet();
		}
		else if ((vnext > vcur && ch == 'H') || (vnext < vcur && ch == 'L')) {
			drawAsciiBox("You win!");
			casino.processWin(bet, 2.0);
		}
		else {
			drawAsciiBox("You lose.");
			maybeApplyRandomCurseAfterLoss(player);
			casino.processLoss(bet);
		}

		player.regenerateMana();
		player.decayCurses();
		player.clearBlessings();
		pauseEnter();
		showRoundSummary(player);
	}
};
