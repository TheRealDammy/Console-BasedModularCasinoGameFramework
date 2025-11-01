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
    void startNewRound() {
        playerHand.clear();
        dealerHand.clear();
        deck.shuffle();
        playerHand.addCard(deck.dealCard());
        dealerHand.addCard(deck.dealCard());
        playerHand.addCard(deck.dealCard());
        dealerHand.addCard(deck.dealCard());
    }

    void play() {
        drawAsciiBox("=== Welcome to Blackjack! ===\n");

        startNewRound();

        // Player turn
        playerTurn();
        if (playerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Player busts! Dealer wins!\n");
            return;
        }

        // Dealer turn
        dealerTurn();
        if (dealerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Dealer busts! Player wins!\n");
            return;
        }

        // Show final hands and determine winner
        showHands(false);
        determineWinner();
    }

    // ------------------ Wallet Integration ------------------
    bool playWithBet(int bet) {
        startNewRound();

        // Player turn
        playerTurn();
        if (playerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Player busts! Dealer wins!\n");
            return false; // player lost
        }

        // Dealer turn
        dealerTurn();
        if (dealerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Dealer busts! Player wins!\n");
            return true; // player won
        }

        // Final comparison
        int playerValue = playerHand.getValue();
        int dealerValue = dealerHand.getValue();

        showHands(false);

        if (playerValue > dealerValue) {
            drawAsciiBox("Player wins with " + std::to_string(playerValue) + " against dealer's " + std::to_string(dealerValue) + "!\n");
            return true;
        }
        else if (playerValue < dealerValue) {
            drawAsciiBox("Dealer wins with " + std::to_string(dealerValue) + " against player's " + std::to_string(playerValue) + "!\n");
            return false;
        }
        else {
            drawAsciiBox("It's a push! Both player and dealer have " + std::to_string(playerValue) + "!\n");
            return false; // tie treated as no coins lost, can adjust if needed
        }
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
                std::cerr << "[!] ERROR: Invalid choice. Please enter 'h' to hit or 's' to stand.\n";
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
            drawAsciiBox("Player wins with " + std::to_string(playerValue) + " against dealer's " + std::to_string(dealerValue) + "!\n");
        }
        else if (playerValue < dealerValue) {
            drawAsciiBox("Dealer wins with " + std::to_string(dealerValue) + " against player's " + std::to_string(playerValue) + "!\n");
        }
        else {
            drawAsciiBox("It's a push! Both player and dealer have " + std::to_string(playerValue) + "!\n");
        }
    }
};
