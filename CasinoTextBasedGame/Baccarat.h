#pragma once
#include "Main.h"

//================== Game Definition ==================//
//------game class representing the games logic-------//
class Baccarat {
private:
    Deck deck;
    std::vector<Card> playerHand;
    std::vector<Card> bankerHand;

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

    int cardValue(const Card& card) const {
        int r = static_cast<int>(card.getRank());
        if (r > 10) return 0;  
        return r;              
    }

    int handValue(const std::vector<Card>& hand) const {
        int total = 0;
        for (auto& c : hand) total += cardValue(c);
        return total % 10;
    }

    void displayHands(bool revealAll = true) const {
        std::cout << "\nPlayer's Hand:\n";
        for (auto& c : playerHand) displayCards(playerHand);
        std::cout << "Total: " << handValue(playerHand) << "\n\n";

        std::cout << "Banker's Hand:\n";
        for (auto& c : bankerHand) displayCards(bankerHand);
        std::cout << "Total: " << handValue(bankerHand) << "\n\n";
    }

public:
    void playerDraw() {
        int pVal = handValue(playerHand);
        if (pVal <= 5) {
            playerHand.push_back(deck.dealCard());
            std::cout << "Player draws a third card!\n";
        }
    }
    void bankerDraw(bool playerDrew, int playerThirdVal = -1) {
        int bVal = handValue(bankerHand);

        if (!playerDrew) {
            if (bVal <= 5) {
                bankerHand.push_back(deck.dealCard());
                std::cout << "Banker draws a third card!\n";
            }
            return;
        }

        // Banker rules when player drew
        if (bVal <= 2) bankerHand.push_back(deck.dealCard());
        else if (bVal == 3 && playerThirdVal != 8) bankerHand.push_back(deck.dealCard());
        else if (bVal == 4 && (playerThirdVal >= 2 && playerThirdVal <= 7)) bankerHand.push_back(deck.dealCard());
        else if (bVal == 5 && (playerThirdVal >= 4 && playerThirdVal <= 7)) bankerHand.push_back(deck.dealCard());
        else if (bVal == 6 && (playerThirdVal == 6 || playerThirdVal == 7)) bankerHand.push_back(deck.dealCard());
        // Banker stands on 7, 8, 9
    }
    void play() {
		drawAsciiBox("=== Welcome to Baccarat! ===\n");

		std::cout << "Do you want to bet on (P)layer, (B)anker, or (T)ie? ";
		std::string betChoice;
		std::cin >> betChoice;

        if (betChoice != "P" && betChoice != "p" &&
            betChoice != "B" && betChoice != "b" &&
            betChoice != "T" && betChoice != "t") {
            std::cerr << "[!] ERROR: Invalid bet choice. Please enter P, B, or T.\n";
            return;
		}

        switch (tolower(betChoice[0])) {
        case 'p': std::cout << "You bet on Player.\n"; break;
        case 'b': std::cout << "You bet on Banker.\n"; break;
        case 't': std::cout << "You bet on Tie.\n"; break;
        }

        deck.shuffle();
        playerHand = { deck.dealCard(), deck.dealCard() };
        bankerHand = { deck.dealCard(), deck.dealCard() };

		int playerThirdVal = -1;
		playerDraw();
        if (playerHand.size() == 3) {
            playerThirdVal = cardValue(playerHand[2]);
		}

		bankerDraw(playerHand.size() == 3, playerThirdVal);

        displayHands();

        int pVal = handValue(playerHand);
        int bVal = handValue(bankerHand);

        char result;
        if (pVal > bVal) {
            drawAsciiBox(" Player wins!\n");
            result = 'p';
        }
        else if (bVal > pVal) {
            drawAsciiBox(" Banker wins!\n");
            result = 'b';
        }
        else {
            drawAsciiBox(" It's a tie!\n");
            result = 't';
        }

        if (tolower(betChoice[0]) == result) {
            drawAsciiBox(" Congratulations! You won your bet!\n");
        }
        else {
            drawAsciiBox(" Sorry! You lost your bet.\n");
        }
    }
};