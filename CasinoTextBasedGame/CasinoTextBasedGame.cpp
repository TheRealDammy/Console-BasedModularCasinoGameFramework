#include <iostream>
#include <Windows.h>
#include <limits>
#include <thread>
#include <chrono>
#include "Main.h"
#include "Blackjack.h"
#include "Poker.h"
#include "Baccarat.h"
#include "HighLow.h"
#include "Slots.h"
#include "SplashScreen.h"

Player initializePlayer() {
    std::string playerName;
    drawAsciiBox("=== Welcome to the Casino! ===");
    std::cout << "Enter your player name: ";
    std::getline(std::cin, playerName);
    if (playerName.empty()) playerName = "Player";
    drawAsciiBox("Welcome, " + playerName + "! Starting with £500.");
    return Player(playerName, 500.0);
}
template <typename Game>
void playGameLoop(Game& game, Player& player, const std::string& gameName, CasinoManager& casino) {
    char replay;
    playSplashScreen(gameName);
    do {
        system("cls");
        drawAsciiBox("=== " + gameName + " ===");
        player.showStatus();

        try {
            game.play(player, casino);
        }
        catch (const std::exception& e) {
            drawAsciiBox(std::string("[!] Game crashed: ") + e.what());
        }

        // Loop until user enters valid y/n input
        while (true) {
            std::cout << "\nDo you want to play another round of " << gameName << "? (y/n): ";
            std::cin >> replay;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (replay == 'y' || replay == 'Y' || replay == 'n' || replay == 'N') break;
            drawAsciiBox("Invalid input. Please enter 'y' or 'n'.");
        }

        if (replay == 'n' || replay == 'N') {
            drawAsciiBox("Exiting " + gameName + "...");
            casino.showStats();
        }

    } while (replay == 'y' || replay == 'Y');
}
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    srand(static_cast<unsigned>(time(nullptr)));

    openSplashScreen("Welcome to Dammy's Casino");

    Player player = initializePlayer();
	CasinoManager casino(player);

    char playAnotherGame = 'y';

    while (playAnotherGame == 'y' || playAnotherGame == 'Y') {
        system("cls");
        drawAsciiBox("=== CASINO MAIN MENU ===");
        std::cout << u8"Your balance: £" << player.getBalance() << "\n";
        std::cout << "Choose a game to play:\n";
        std::cout << "1. Blackjack\n";
        std::cout << "2. Poker (Texas Hold'em)\n";
        std::cout << "3. Baccarat\n";
        std::cout << "4. High-Low\n";
        std::cout << "5. Slots\n";
        std::cout << "6. View Status / Mana / Curses\n";
        std::cout << "7. Exit Casino\n";

        int choice;
        while (true) {
            std::cout << "\nEnter the number of your choice: ";
            std::cin >> choice;

            if (std::cin.fail() || choice < 1 || choice > 7) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                drawAsciiBox("Invalid input. Please enter a number 1–7.");
            }
            else break;
        }

        system("cls");

        switch (choice) {
        case 1: {
            Blackjack blackjackGame;
            playGameLoop(blackjackGame, player, "Blackjack", casino);
            break;
        }
        case 2: {
            Poker pokerGame;
            playGameLoop(pokerGame, player, "Poker", casino);
            break;
        }
        case 3: {
            Baccarat baccaratGame;
            playGameLoop(baccaratGame, player, "Baccarat", casino);
            break;
        }
        case 4: {
            HighLow highLowGame;
            playGameLoop(highLowGame, player, "High-Low", casino);
            break;
        }
        case 5: {
            Slots slotsGame;
            playGameLoop(slotsGame, player, "Slots", casino);
            break;
        }
        case 6: {
            system("cls");
            player.showStatus();
            std::cout << "\nPress Enter to return to menu...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
        case 7: {
            drawAsciiBox("=== Exiting Casino ===");
            playAnotherGame = 'n';
            break;
        }
        }

        // Ask to continue only if not exiting
        if (playAnotherGame != 'n') {
            while (true) {
                std::cout << "\nDo you want to play another game? (y/n): ";
                std::cin >> playAnotherGame;

                if (playAnotherGame == 'y' || playAnotherGame == 'Y' ||
                    playAnotherGame == 'n' || playAnotherGame == 'N') break;

                drawAsciiBox("Invalid input. Please enter 'y' or 'n'.");
                std::cin.clear();
            }
        }
    }

    exitSplashScreen("=== Exiting Casino ===\n");
    drawAsciiBox("Thank you for visiting the Casino, " + player.getName() + "! Goodbye!");
	casino.showStats();
    return 0;
}
