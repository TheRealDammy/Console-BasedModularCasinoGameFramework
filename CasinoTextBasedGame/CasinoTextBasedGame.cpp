#include <iostream>
#include <Windows.h>
#include "Blackjack.h"
#include "Poker.h"

int main()
{
    // --- Enable full UTF-8 console support ---
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "Welcome to the Casino Text-Based Game!\n";
    std::cout << "Choose a game to play:\n";
    std::cout << "1. Blackjack\n";
    std::cout << "2. Poker\n";
    std::cout << "Enter the number of your choice: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1: {
        Blackjack blackjackGame;
        blackjackGame.play();
        char choice;
		std::cout << "\nDo you want to play another round of Blackjack? (y/n): ";
		std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            while (true) {
                blackjackGame.play();
                std::cout << "\nDo you want to play another round of Blackjack? (y/n): ";
                std::cin >> choice;
                if (choice != 'y' && choice != 'Y') {
                    break;
                }
            }
        }
        else {
			std::cout << "Exiting Blackjack.\n";
            std::cout << "Do you want to play a different game? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                main(); // Restart main to choose a different game
            }
            else break;
            break;
        }
        break;
    }
    case 2: {
        Poker pokerGame;
        pokerGame.playGame();
        char choice;
        std::cout << "\nDo you want to play another hand of poker? (y/n): ";
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            while (true) {
                pokerGame.playGame();
                std::cout << "\nDo you want to play another hand of poker? (y/n): ";
                std::cin >> choice;
                if (choice != 'y' && choice != 'Y') {
                    break;
                }
            }
        }
        else {
            std::cout << "Exiting Poker.\n";
			std::cout << "Do you want to play a different game? (y/n): ";
			char choice;
			std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                main(); // Restart main to choose a different game
            }
            else break;
            break;
        }
        break;
    }
    default:
        std::cout << "Invalid choice. Exiting the game.\n";
        break;
    }
    std::cout << "Thank you for playing! Goodbye!\n";
    return 0;
}
