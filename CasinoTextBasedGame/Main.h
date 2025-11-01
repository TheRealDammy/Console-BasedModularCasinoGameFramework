#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <cctype>
#include <map>
#include <sstream>

//================== Playing Card Definition ==================//
//------Card class representing a playing card-------//
class Card {
public : 
	enum Suit { Hearts, Diamonds, Clubs, Spades };
	enum Rank { Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };
private:
	Rank rank;
	Suit suit;
public:
	Card(Rank r, Suit s) : rank(r), suit(s) {}
	std::string toString() const {
		static const char* RankStrings[] = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
		static const char* SuitStrings[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
		return std::string(RankStrings[rank]) + " of " + SuitStrings[suit];
	}
	Rank getRank() const { return rank; }
	Suit getSuit() const { return suit; }

	int getValue() const {
		if (rank >= Two && rank <= Ten) return rank;
		if (rank >= Jack && rank <= King) return 10;
		if (rank == Ace) return 11; // Ace can also be 1, handled in game logic
		return 0;
	}

	std::vector<std::string> displayCard() const {
		std::vector<std::string> card(7);
		card[0] = u8"┌─────────┐";
		card[1] = u8"│" + rankToString() + u8"       │";
		card[2] = u8"│         │";
		card[3] = u8"│    " + suitToString() + u8"    │";
		card[4] = u8"│         │";
		card[5] = u8"│       " + rankToString() + u8"│";
		card[6] = u8"└─────────┘";
		return card;
	}
private:
	// Return a 2-char rank string for aligning the card art:
	// "10" for Ten, otherwise single char plus a trailing space (e.g. "A ", "K ", "2 ")
	std::string rankToString() const {
		switch (rank) {
		case Two:   return "2 ";
		case Three: return "3 ";
		case Four:  return "4 ";
		case Five:  return "5 ";
		case Six:   return "6 ";
		case Seven: return "7 ";
		case Eight: return "8 ";
		case Nine:  return "9 ";
		case Ten:   return "10";
		case Jack:  return "J ";
		case Queen: return "Q ";
		case King:  return "K ";
		case Ace:   return "A ";
		default:    return "  ";
		}
	}

	// Return a single-character suit symbol for the card art
	std::string suitToString() const {
		switch (suit) {
		case Hearts:   return u8"♥";
		case Diamonds: return u8"♦";
		case Clubs:    return u8"♣";
		case Spades:   return u8"♠";
		default:       return u8" ";
		}
	}
public:
	static std::vector<std::string> hiddenCard() {
		std::vector<std::string> card(7);
		card[0] = u8"┌─────────┐";
		card[1] = u8"│░░░░░░░░░│";
		card[2] = u8"│░░░░░░░░░│";
		card[3] = u8"│░░░░░░░░░│";
		card[4] = u8"│░░░░░░░░░│";
		card[5] = u8"│░░░░░░░░░│";
		card[6] = u8"└─────────┘";
		return card;
	}
};

//================== Deck Definition ==================//
//------Deck class representing a deck of cards-------//
class Deck {
private:
	std::vector<Card> cards;
	int currentCardIndex;
public:
	Deck() {
		for (int s = Card::Hearts; s <= Card::Spades; ++s) {
			for (int r = Card::Two; r <= Card::Ace; ++r) {
				cards.emplace_back(static_cast<Card::Rank>(r), static_cast<Card::Suit>(s));
			}
		}
		currentCardIndex = 0;
		shuffle();
	}
	void shuffle() {
		std::srand(static_cast<unsigned int>(std::time(0)));
		std::random_shuffle(cards.begin(), cards.end());
		currentCardIndex = 0;
	}
	Card dealCard() {
		if (currentCardIndex < cards.size()) {
			return cards[currentCardIndex++];
		}
		else {
			throw std::out_of_range("No more cards in the deck");
		}
	}
};