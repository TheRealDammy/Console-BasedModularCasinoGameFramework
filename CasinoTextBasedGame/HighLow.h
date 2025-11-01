#pragma once
#include "Main.h"

//================== Game Definition ==================//
//------game class representing the games logic-------//
class HighLow {
private:
    Deck deck;
    std::vector<Card> currentCard;
    std::vector<Card> nextCard;

    int rankValue(const Card& c) const {
        return static_cast<int>(c.getRank());
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

    void play() {
		drawAsciiBox("=== Welcome to High-Low! ===\n");
        deck.shuffle();

        Card current = deck.dealCard();
        currentCard = { current };
		int score = 0;

		bool isGameOver = false;

        while (!isGameOver) {
            deck.shuffle();
            std::cout << "Current card:\n";
            displayCards(currentCard);

            std::string guess;
            std::cout << "\nWill the next card be (H)igher or (L)ower? ";
            std::cin >> guess;

            
            Card next = deck.dealCard();
            nextCard = { next };

            std::cout << "\nNext card:\n";
            displayCards(nextCard);

            int cVal = rankValue(current);
            int nVal = rankValue(next);

            bool correct = false;
            if ((guess == "H" || guess == "h" || guess == "higher" || guess == "high") && nVal > cVal) correct = true;
            if ((guess == "L" || guess == "l" || guess == "lower" || guess == "low") && nVal < cVal) correct = true;

            if (nVal == cVal)
                drawAsciiBox(" It's a tie! No one wins.\n");
            else if (correct) {
                drawAsciiBox(" Correct guess! You win!\n");
				score++;
				current = next;
				currentCard = { current };
            }
            else {
                drawAsciiBox(" Wrong guess! You lose.\n");
                isGameOver = true;
            }
        }
        drawAsciiBox("Your final score: " + std::to_string(score) + "\n");
    }

};