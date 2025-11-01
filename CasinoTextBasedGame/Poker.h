#pragma once
#include "Main.h"

//================== Game Definition ==================//
//------game class representing the games logic-------//
class Poker {
private:
	Deck deck;
	std::vector<std::vector<Card>> players;
	const int numPlayers = 4;

	// Convert enum ranks to integer values for ranking (Ace high)
	int getCardValue(const Card& c) const {
		switch (c.getRank()) {
		case Card::Two: return 2;
		case Card::Three: return 3;
		case Card::Four: return 4;
		case Card::Five: return 5;
		case Card::Six: return 6;
		case Card::Seven: return 7;
		case Card::Eight: return 8;
		case Card::Nine: return 9;
		case Card::Ten: return 10;
		case Card::Jack: return 11;
		case Card::Queen: return 12;
		case Card::King: return 13;
		case Card::Ace: return 14;
		default: return 0;
		}
	}

	// Evaluate the hand strength (0–9)
	int evaluateHand(const std::vector<Card>& hand) const {
		std::vector<int> values;
		std::vector<Card::Suit> suits;

		for (const auto& card : hand) {
			values.push_back(getCardValue(card));
			suits.push_back(card.getSuit());
		}

		std::sort(values.begin(), values.end());
		bool isFlush = std::all_of(suits.begin(), suits.end(), [&](Card::Suit s) { return s == suits[0]; });
		bool isStraight = true;
		for (size_t i = 1; i < values.size(); ++i) {
			if (values[i] != values[i - 1] + 1) {
				isStraight = false;
				break;
			}
		}
		std::map<int, int> valueCount;
		for (int v : values) {
			valueCount[v]++;
		}
		bool fourKind = false, threeKind = false, pair = false, twopair = false;
		for (const auto& kv : valueCount) {
			if (kv.second == 4) fourKind = true;
			if (kv.second == 3) threeKind = true;
			if (kv.second == 2) pair = !pair ? true: (twopair = true, true);
		}
		if (isStraight && isFlush && values.back() == 14) return 9; // Royal Flush
		if (isStraight && isFlush) return 8; // Straight Flush
		if (fourKind) return 7; // Four of a Kind
		if (threeKind && pair) return 6; // Full House
		if (isFlush) return 5; // Flush
		if (isStraight) return 4; // Straight
		if (threeKind) return 3; // Three of a Kind
		if (twopair) return 2; // Two Pair
		if (pair) return 1; // One Pair
		return 0; // High Card
	}

public:
	void displayCards(const std::vector<Card>& cards, bool hideFirst = false) const {
		std::vector<std::vector<std::string>> cardArts;
		for (size_t i = 0; i < cards.size(); ++i) {
			if (i == 0 && hideFirst)
				cardArts.push_back(Card::hiddenCard());
			else
				cardArts.push_back(cards[i].displayCard());
		}
		for (int line = 0; line < 7; ++line) {
			for (size_t i = 0; i < cardArts.size(); ++i)
				std::cout << cardArts[i][line] << " ";
			std::cout << "\n";
		}
	}
	void playGame() {
		std::cout << "\n == Welcome to Poker! ===\n";
		deck.shuffle();
		players.assign(numPlayers, std::vector<Card>());

		// --- Deal Initial Hands ---
		for (int round = 0; round < 5; ++round) {
			for (int p = 0; p < numPlayers; ++p) {
				players[p].push_back(deck.dealCard());
			}
		}

		//Players Hand
		std::cout << "\nYour Hand:\n";
		displayCards(players[0]);

		// --- Player Discards ---
		std::cout << "\nEnter the indices (1-5) of cards to discard separated by spaces (0 to keep all): ";
		std::string line;
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::getline(std::cin, line);
		std::stringstream ss(line);

		int index;
		std::vector<int> discard;
		while (ss >> index) {
			if (index >= 1 && index <= 5)
				discard.push_back(index - 1); // convert to 0-based
		}

		// Replace each chosen card directly
		for (int i : discard) {
			players[0][i] = deck.dealCard();
		}

		std::cout << "\nYour new Hand:\n";
		displayCards(players[0]);
		
		// --- Opponents Hands ---
		for (int p = 1; p < numPlayers; ++p) {
			int discards = rand() % 3; // Discard 0-2 cards
			for (int i = 0; i < discards; ++i) {
				int discardIndex = rand() % 5;
				players[p][discardIndex] = deck.dealCard();
			}
		}

		// --- Show Opponents Hands and Determine Winner ---
		std::vector<std::string> handRanks = { 
			"High Card", "Pair", "Two Pair", "Three of a Kind", "Straight", "Flush", "Full House", "Four of a Kind", "Straight Flush", "Royal Flush" 
		};
		
		std::vector<int> handValues;
		for (int p = 0; p < numPlayers; ++p) {
			handValues.push_back(evaluateHand(players[p]));
		}
		int best = *std::max_element(handValues.begin(), handValues.end());
		int winningValue = std::distance(handValues.begin(), std::find(handValues.begin(), handValues.end(), best));

		std::cout << "\n=== Showdown ===\n";
		for (int p = 0; p < numPlayers; ++p) {
			if (p == 0)
				std::cout << "\nYour Hand:\n";
			else
				std::cout << "\nOppnents " << p + 1 << "'s Hand:\n";
			displayCards(players[p]);
			std::cout << "Hand Rank: " << handRanks[handValues[p]] << "\n\n";
		}
		if (winningValue == 0)
			std::cout << "You win with a " << handRanks[handValues[winningValue]] << "!\n";
		else
			std::cout << "Opponent " << winningValue + 1 << " wins with a " << handRanks[handValues[winningValue]] << "!\n";
	}
};