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
    double currentBet = 0;
    Player* playerRef = nullptr;
    CasinoManager* casinoRef = nullptr;
    bool revealDealerCard = false;
    bool forceTenNext = false;
    bool negateNextCurse = false;
public:
    void startNewRound() {
        playerHand.clear();
        dealerHand.clear();
        deck.shuffle();

        Player& p = *playerRef;
		CasinoManager& casino = *casinoRef;

        // Deal opening cards
        playerHand.addCard(dealCardWithFantasy());
        dealerHand.addCard(dealCardWithFantasy());
        playerHand.addCard(dealCardWithFantasy());
        dealerHand.addCard(dealCardWithFantasy());
        offerBlessings();

        // ----- Betting -----
        currentBet;
        casino.placeBet(currentBet);
    }

    void play(Player &player, CasinoManager &casino) {
        playerRef = &player;
		casinoRef = &casino;

        startNewRound();
        

        // Player turn
        playerTurn();
        if (playerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Player busts! Dealer wins!\n");
            return finalizeRound(false);
        }

        // Dealer turn
        dealerTurn();
        if (dealerHand.isBust()) {
            showHands(false);
            drawAsciiBox("Dealer busts! Player wins!\n");
            return finalizeRound(true);
        }

        showHands(false);
        int pv = playerHand.getValue();
        int dv = dealerHand.getValue();

        if (pv > dv) finalizeRound(true);
        else if (pv < dv) finalizeRound(false);
        else finalizeRound(false, true);

        // Show final hands and determine winner
        showHands(false);
    }

private:
    void offerBlessings() {
        Player& p = *playerRef;

        std::cout << "\nWould you like to use a Blessing?\n";
        std::cout << "1. Fate's Glimpse (15 mana)\n";
        std::cout << "2. Lucky Draw   (20 mana)\n";
        std::cout << "3. Mana Shield  (10 mana)\n";
        std::cout << "4. None\n> ";

        int c;
        std::cin >> c;

        switch (c) {
        case 1: p.castBlessing("Fate's Glimpse"); break;
        case 2: p.castBlessing("Lucky Draw"); break;
        case 3: p.castBlessing("Mana Shield"); break;
        }
    }
    Card dealCardWithFantasy() {
        Player& p = *playerRef;

        // handle Lucky Draw
        if (shouldForceTen()) {
            p.clearBlessings();
            return Card(Card::Ten, (Card::Suit)(rand() % 4));
        }

        return deck.dealCard();
    }
    void showHands(bool hideDealerFirstCard) const {
        Player& p = *playerRef;

        std::cout << "\nDealer's Hand:\n";

        if (shouldRevealDealer() == true)
            hideDealerFirstCard = false;

        dealerHand.displayHand(hideDealerFirstCard);

        if (!hideDealerFirstCard)
            std::cout << "Value: " << dealerHand.getValue() << "\n";

        std::cout << "\nPlayer's Hand:\n";
        playerHand.displayHand();
        std::cout << "Value: " << playerHand.getValue() << "\n";
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

    void finalizeRound(bool playerWon, bool push = false) {
        Player& p = *playerRef;
		CasinoManager& casino = *casinoRef;

        if (push) {
            drawAsciiBox("Push! You get your bet back.");
            casino.processWin(currentBet);
        }
        else if (playerWon) {
            drawAsciiBox("You win!");
			casino.processWin(currentBet, 2);
        }
        else {
            drawAsciiBox("You lose...");
            // chance to receive a curse
            if (rand() % 100 < 35) {
                p.applyCurse("Muddled Sight", 2);
                drawAsciiBox("A dark curse afflicts you: Muddled Sight!");
            }
			casino.processLoss(currentBet);
        }

        p.regenerateMana();
        p.clearBlessings();
        p.decayCurses();
    }
    bool shouldRevealDealer() const { return revealDealerCard; }
    bool shouldForceTen() const { return forceTenNext; }
    bool shouldNegateCurse() const { return negateNextCurse; }
};
