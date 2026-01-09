#pragma once
#include "Main.h"

// --------- Baccarat (official simplified banker draw table) ----------
class Baccarat {
private:
    Deck deck;

    void displayCardsSideBySide(const std::vector<Card>& cards, bool hideFirst = false) const {
        std::vector<std::vector<std::string>> arts;
        for (size_t i = 0; i < cards.size(); ++i) {
            if (i == 0 && hideFirst) arts.push_back(Card::hiddenCard());
            else arts.push_back(cards[i].displayCard());
        }
        for (size_t line = 0; line < arts[0].size(); ++line) {
            for (size_t c = 0; c < arts.size(); ++c) {
                std::cout << arts[c][line] << " ";
            }
            std::cout << "\n";
        }
    }

    // Official-style banker draw decision:
    // Returns true if banker should draw a third card given banker's points and player's third card (or -1 if player didn't draw)
    bool bankerShouldDraw(int bankerPoints, int playerThirdCardValue) const {
        // If banker points 0-2 => draw
        if (bankerPoints <= 2) return true;
        // If banker points 3 => draw unless player's third card was 8
        if (bankerPoints == 3) return playerThirdCardValue != 8;
        // If banker points 4 => draw if player's third card 2-7
        if (bankerPoints == 4) return playerThirdCardValue >= 2 && playerThirdCardValue <= 7;
        // If banker points 5 => draw if player's third card 4-7
        if (bankerPoints == 5) return playerThirdCardValue >= 4 && playerThirdCardValue <= 7;
        // If banker points 6 => draw if player's third card 6-7
        if (bankerPoints == 6) return playerThirdCardValue >= 6 && playerThirdCardValue <= 7;
        // banker points 7 => stand
        return false;
    }

    // card logical value (Ace=1, 2-9 numeric, 10/J/Q/K = 0)
    static int baccaratCardValue(const Card& c) {
        int r = static_cast<int>(c.getRank());
        if (r >= (int)Card::Ten) return 0;
        if (r == (int)Card::Ace) return 1;
        return r;
    }

    static int handPoints(const std::vector<Card>& h) {
        int s = 0;
        for (auto& c : h) s += baccaratCardValue(c);
        return s % 10;
    }

public:
    void play(Player& player, CasinoManager &casino) {
		system("cls");
        drawAsciiBox("=== Baccarat ===");

        player.showStatus();

        // Offer Blessing
        std::string cast = readLineTrimmed("Cast a Blessing before this round? (y/n) ");
        if (!cast.empty() && (cast[0] == 'y' || cast[0] == 'Y')) {
            std::cout << "1 Fate's Glimpse (15)\n2 Lucky Draw (20)\n3 Mana Shield (10)\n0 Skip\n";
            int pick = readInt("Choose: ", 0, 3);
            if (pick == 1) player.castBlessing("Fate's Glimpse");
            else if (pick == 2) player.castBlessing("Lucky Draw");
            else if (pick == 3) player.castBlessing("Mana Shield");
        }

        // Choose bet target
        std::cout << "\nPlace your bet on:\n1. Player\n2. Banker\n3. Tie\n";
        int target = readInt("Choose (1-3): ", 1, 3);

        // Muddled Sight may flip the choice
        if (player.hasCurse("Muddled Sight") && randint(1, 100) <= 20) {
            drawAsciiBox("Muddled Sight flips your bet choice!");
            if (target == 1) target = 2;
            else if (target == 2) target = 1;
        }

        double bet;
        if (!casino.placeBet(bet)) { pauseEnter(); return; }

        // deal initial two cards each
        deck.shuffle();
        std::vector<Card> pHand;
        std::vector<Card> bHand;

        pHand.push_back(drawCardForPlayer(deck, player));
        pHand.push_back(drawCardForPlayer(deck, player));

        bHand.push_back(deck.dealCard());
        bHand.push_back(deck.dealCard());

        // apply Unlucky Hand curse to player's starting cards
        if (player.hasCurse("Unlucky Hand")) {
            applyUnluckyHandToStarting(pHand);
            drawAsciiBox("Unlucky Hand: your starting cards are weakened.");
        }

        int pPoints = handPoints(pHand);
        int bPoints = handPoints(bHand);

        // Natural - if either has 8 or 9 -> comparison only
        if (pPoints >= 8 || bPoints >= 8) {
            // no more draws
        }
        else {
            // Player drawing rule: player draws third card if points 0-5
            int playerThirdValue = -1;
            if (pPoints <= 5) {
                Card third = drawCardForPlayer(deck, player);
                pHand.push_back(third);
                playerThirdValue = baccaratCardValue(third);
            }

            // Recompute player points
            pPoints = handPoints(pHand);

            // Banker drawing rule using player's third card value (or -1 if none)
            if (bPoints <= 5) {
                // If player did not draw, banker draws per simple rule: draw if banker <=5
                if (playerThirdValue == -1) {
                    bHand.push_back(deck.dealCard());
                }
                else {
                    if (bankerShouldDraw(bPoints, playerThirdValue)) {
                        bHand.push_back(deck.dealCard());
                    }
                }
            }
        }

        // show results
        std::cout << "\nPlayer Hand:\n"; displayCardsSideBySide(pHand);
        std::cout << "Player points: " << handPoints(pHand) << "\n";
        std::cout << "\nBanker Hand:\n"; displayCardsSideBySide(bHand);
        std::cout << "Banker points: " << handPoints(bHand) << "\n";

        int finalP = handPoints(pHand);
        int finalB = handPoints(bHand);
        bool pWin = finalP > finalB;
        bool bWin = finalB > finalP;
        bool tie = finalP == finalB;

        // resolve bet payouts
        if ((target == 1 && pWin) || (target == 2 && bWin) || (target == 3 && tie)) {
            if (target == 3) { // tie payout (example 8x)
				casino.processWin(bet, 8.0);
                drawAsciiBox("Tie! You won the tie payout.");
            }
            else {
				casino.processWin(bet, 2.0);
                drawAsciiBox("You won your bet!");
            }
        }
        else if ((target == 1 && bWin) || (target == 2 && pWin) || (!tie && target != 3)) {
            drawAsciiBox("You lost.");
            maybeApplyRandomCurseAfterLoss(player);
			casino.processLoss(bet);
        }
        else { // bet on player/banker but tie occurred
            player.refundCurrentBet();
            drawAsciiBox("Push (tie) — your bet returned.");
        }

        // round summary and cleanup
        showRoundSummary(player);
        player.regenerateMana();
        player.decayCurses();
        player.clearBlessings();
        pauseEnter();
    }
};
