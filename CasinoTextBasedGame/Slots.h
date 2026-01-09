#pragma once
#include "Main.h"
#include <chrono>
#include <thread>

class Slots {
private:
    std::vector<std::string> symbols;

    void showReels(const std::vector<std::string>& reels) {
        std::cout << "\r[ " << reels[0] << " | " << reels[1] << " | " << reels[2] << " ] " << std::flush;
    }

    void spinAnimation(std::vector<std::string>& finalReels) {
        std::vector<std::string> reels = { " ", " ", " " };

        int totalCycles = 20;     // total spin cycles
        int delay = 60;           // initial delay (ms)
        for (int cycle = 0; cycle < totalCycles; ++cycle) {
            // Randomize symbols each frame
            for (int i = 0; i < 3; ++i)
                reels[i] = symbols[randint(0, (int)symbols.size() - 1)];

            showReels(reels);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            // Gradually slow down
            delay += 15;
        }

        // Reveal each reel one by one
        for (int i = 0; i < 3; ++i) {
            reels[i] = finalReels[i];
            showReels(reels);
            std::this_thread::sleep_for(std::chrono::milliseconds(350));
        }
        std::cout << std::endl;
    }

public:
    Slots() {
        symbols = { u8"🍒", u8"🍋", u8"🔔", u8"💎", u8"7️⃣", u8"⭐" };
    }

    void play(Player& player, CasinoManager& casino) {
        double bet;
        if (!casino.placeBet(bet)) { return; }

        drawAsciiBox("Press Enter to SPIN the reels!");
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();

        // Generate final result
        int a = randint(0, (int)symbols.size() - 1);
        int b = randint(0, (int)symbols.size() - 1);
        int c = randint(0, (int)symbols.size() - 1);
        std::vector<std::string> finalReels = { symbols[a], symbols[b], symbols[c] };

        // Animation before result
        spinAnimation(finalReels);

        // Outcome logic
        if (a == b && b == c) {
            drawAsciiBox("JACKPOT! You win 10x your bet!");
            casino.processWin(bet, 11.0);
        }
        else if (a == b || b == c || a == c) {
            drawAsciiBox("Two match! You win 2x!");
            casino.processWin(bet, 2.0);
        }
        else {
            drawAsciiBox("No win. Better luck next time.");
            maybeApplyRandomCurseAfterLoss(player);
            casino.processLoss(bet);
        }

        player.regenerateMana();
        player.decayCurses();
        player.clearBlessings();

        showRoundSummary(player);
    }
};
