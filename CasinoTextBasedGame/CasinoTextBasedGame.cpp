#include <iostream>
#include <Windows.h>
#include <limits>
#include "Blackjack.h"
#include "Poker.h"
#include "Baccarat.h"
#include "HighLow.h"

// Reusable game loop template
template <typename Game>
void playGameLoop(Game& game, const std::string& gameName) {
    char replay;
    do {
        system("cls"); // clear console for clean display
        drawAsciiBox("=== " + gameName + " ===\n");
        game.play();

        // Loop until user enters valid y/n input
        while (true) {
            std::cout << "\nDo you want to play another round of " << gameName << "? (y/n): ";
            std::cin >> replay;

            if (replay == 'y' || replay == 'Y' || replay == 'n' || replay == 'N') break;

            std::cerr << "[!] ERROR: Invalid input. Please enter 'y' or 'n'.\n";
        }

        if (replay == 'n' || replay == 'N') {
            drawAsciiBox("=== Exiting " + gameName + " ===\n");
            drawAsciiBox("Returning to main menu...\n");
        }

    } while (replay == 'y' || replay == 'Y');
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    char playAnotherGame = 'y';

    while (playAnotherGame == 'y' || playAnotherGame == 'Y') {
        system("cls");
        drawAsciiBox("=== Welcome to the Casino! ===\n");
        std::cout << "Choose a game to play:\n";
        std::cout << "1. Blackjack\n2. Poker\n3. Baccarat\n4. High-Low\n5. Exit Casino\n";

        int choice;

        // Loop until valid game choice is entered
        while (true) {
            std::cout << "Enter the number of your choice: ";
            std::cin >> choice;

            if (std::cin.fail() || choice < 1 || choice > 5) {
                std::cerr << "[!] ERROR: Invalid input. Enter a number 1-5.\n";
                std::cin.clear();
            }
            else {
                break;
            }
        }

        switch (choice) {
        case 1: {
            Blackjack blackjackGame;
            playGameLoop(blackjackGame, "Blackjack");
            break;
        }
        case 2: {
            Poker pokerGame;
            playGameLoop(pokerGame, "Poker");
            break;
        }
        case 3: {
            Baccarat baccaratGame;
            playGameLoop(baccaratGame, "Baccarat");
            break;
        }
        case 4: {
            HighLow highLowGame;
            playGameLoop(highLowGame, "High-Low");
            break;
        }
        case 5: {
            drawAsciiBox("=== Exiting Casino ===\n");
            playAnotherGame = 'n';
            break;
        }
        }

        // Ask to switch games only if not exiting
        if (playAnotherGame != 'n') {
            while (true) {
                std::cout << "Do you want to play a different game? (y/n): ";
                std::cin >> playAnotherGame;

                if (playAnotherGame == 'y' || playAnotherGame == 'Y' ||
                    playAnotherGame == 'n' || playAnotherGame == 'N') break;

                std::cerr << "[!] ERROR: Invalid input. Please enter 'y' or 'n'.\n";
                std::cin.clear();
            }
        }
    }

    drawAsciiBox("Thank you for playing! Goodbye!\n");
    return 0;
}
