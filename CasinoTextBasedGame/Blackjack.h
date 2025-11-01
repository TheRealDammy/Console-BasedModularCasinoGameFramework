#pragma once
#include "Main.h"

//================== Hand Definition ==================//
//------Hand class representing a player's hand-------//
class Hand {
public:
	std::vector<Card> cards;
	void addCard(const Card& card) {
		cards.push_back(card);
	}
	int getValue() const {
		int total = 0;
		int aceCount = 0;
		for (const auto& card : cards) {
			total += card.getValue();
			if (card.getRank() == Card::Ace) {
				aceCount++;
			}
		}
		while (total > 21 && aceCount > 0) {
			total -= 10; // Count Ace as 1 instead of 11
			aceCount--;
		}
		return total;
	}
	bool isBust() const {
		return getValue() > 21;
	}
	bool isBlackjack() const {
		return cards.size() == 2 && getValue() == 21;
	}
	bool hasPair() const {
		return cards.size() == 2 && cards[0].getRank() == cards[1].getRank();
	}
	bool canSplit() const {
		return hasPair();
	}
	void split(Hand& newHand) {
		if (canSplit()) {
			newHand.addCard(cards.back());
			cards.pop_back();
		}
		else {
			throw std::logic_error("Cannot split this hand");
		}
	}
	void clear() {
		cards.clear();
	}
	void displayHand(bool hideFirstCard = false) const {
		std::vector<std::vector<std::string>> cardLines;
		for (size_t i = 0; i < cards.size(); ++i) {
			if (i == 0 && hideFirstCard) {
				cardLines.push_back(Card::hiddenCard());
			}
			else {
				cardLines.push_back(cards[i].displayCard());
			}
		}
		for (size_t line = 0; line < 7; ++line) {
			for (const auto& card : cardLines) {
				std::cout << card[line] << " ";
			}
			std::cout << std::endl;
		}
	}
};

//================== Game Definition ==================//
//------game class representing the games logic-------//
class Blackjack {
private:
	Deck deck;
	Hand playerHand;
	Hand dealerHand;
public:
	Blackjack() {
		deck.shuffle();
	}
	void startNewRound() {
		playerHand.clear();
		dealerHand.clear();
		playerHand.addCard(deck.dealCard());
		dealerHand.addCard(deck.dealCard());
		playerHand.addCard(deck.dealCard());
		dealerHand.addCard(deck.dealCard());
	}
	void play() {
		std::cout << "\n=== Welcome to Blackjack! ===\n";

		startNewRound();
		
		playerHand = Hand();
		dealerHand = Hand();

		playerHand.addCard(deck.dealCard());
		dealerHand.addCard(deck.dealCard());
		playerHand.addCard(deck.dealCard());
		dealerHand.addCard(deck.dealCard());

		if (playerHand.isBlackjack() || dealerHand.isBlackjack()) {
			std::cout << "Checking for Blackjack...\n";
			dealerHand.displayHand();
			if (playerHand.isBlackjack() && dealerHand.isBlackjack()) {
				std::cout << "Both player and dealer have Blackjack! It's a push.\n";
			}
			else if (playerHand.isBlackjack()) {
				std::cout << "Player has Blackjack! Player wins!\n";
			}
			else {
				std::cout << "Dealer has Blackjack! Dealer wins!\n";
			}
			return;
		}

		playerTurn();
		if (playerHand.isBust()) {
			std::cout << "Player busts! Dealer wins!\n";
			return;
		}

		dealerTurn();
		if (dealerHand.isBust()) {
			std::cout << "Dealer busts! Player wins!\n";
			return;
		}
		showHands(false);
		determineWinner();
	}
private:
	void showHands(bool hideDealerFirstCard) const {
		std::cout << "\nDealer's Hand:\n";
		dealerHand.displayHand(hideDealerFirstCard);
		if (!hideDealerFirstCard) {
			std::cout << "Dealer's Hand Value: " << dealerHand.getValue() << "\n";
		}
		std::cout << "\nPlayer's Hand:\n";
		playerHand.displayHand();
		std::cout << "Player's Hand Value: " << playerHand.getValue() << "\n";
	}
	void playerTurn() {
		char choice;
		do {
			showHands(true);
			std::cout << "Do you want to (h)it or (s)tand? ";
			std::cin >> choice;
			if (choice == 'h' || choice == 'H') {
				playerHand.addCard(deck.dealCard());
				if (playerHand.isBust()) {
					showHands(false);
					return;
				}
			}
			if (choice != 'h' && choice != 'H' && choice != 's' && choice != 'S') {
				std::cout << "Invalid choice. Please enter 'h' to hit or 's' to stand.\n";
			}
			if (playerHand.canSplit()) {
				char splitChoice;
				std::cout << "You have a pair! Do you want to split? (y/n): ";
				std::cin >> splitChoice;
				if (splitChoice == 'y' || splitChoice == 'Y') {
					Hand newHand;
					playerHand.split(newHand);
					playerHand.addCard(deck.dealCard());
					newHand.addCard(deck.dealCard());
					std::cout << "First Hand:\n";
					playerHand.displayHand();
					std::cout << "Second Hand:\n";
					newHand.displayHand();
				}
			}
		} while (choice != 's' && choice != 'S');
	}
	void dealerTurn() {
		std::cout << "\nDealer's turn...\n";
		showHands(true);
		while (dealerHand.getValue() < 17) {
			dealerHand.addCard(deck.dealCard());
		}
	}
	void determineWinner() const {
		int playerValue = playerHand.getValue();
		int dealerValue = dealerHand.getValue();
		if (playerValue > dealerValue) {
			std::cout << "Player wins with " << playerValue << " against dealer's " << dealerValue << "!\n";
		}
		else if (playerValue < dealerValue) {
			std::cout << "Dealer wins with " << dealerValue << " against player's " << playerValue << "!\n";
		}
		else {
			std::cout << "It's a push with both at " << playerValue << "!\n";
		}
	}
};