#pragma once
#include "Main.h"
#include <array>
#include <numeric>
#include <algorithm>
#include <iomanip>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

class Poker {
public:
    int handCount = 0;            // counts how many hands have been played
    int blindIncreaseInterval = 5; // increase blinds every 5 hands
    int blindIncreaseAmount = 5;   // amount to increase SB/BB

    Poker(int totalPlayers = 6)
        : numPlayers(clamp(totalPlayers, 2, 9)),
        smallBlind(10), bigBlind(20), dealerPosition(0) {
    }

    struct HandRank {
        int rank;
        std::vector<int> tiebreak;
        bool operator<(const HandRank& o) const {
            if (rank != o.rank) return rank < o.rank;
            return tiebreak < o.tiebreak;
        }
        bool operator==(const HandRank& o) const {
            return rank == o.rank && tiebreak == o.tiebreak;
        }
    };

    void play(Player& player, CasinoManager &casino) {
        system("cls");
        drawAsciiBox("=== Welcome To Poker ===");

        handCount++;
        if (handCount % blindIncreaseInterval == 0) {
            smallBlind += blindIncreaseAmount;
            bigBlind += blindIncreaseAmount;
            drawAsciiBox(
                "Blinds increased!\n"
                "Small Blind: " + std::to_string(smallBlind) +
                "\nBig Blind: " + std::to_string(bigBlind)
            );
        }

        deck.refill();
        deck.shuffle();
        deck.refill();
        deck.shuffle();
        community.clear();          // <— clear previous community cards
        playersHands.clear();       // <— clear any leftover hands
        playerBets.clear();         // <— clear old bets
        folded.clear();
        active.clear();
        pot = 0;
        currentBet = 0;
        playersHands.assign(numPlayers, {});
        active.assign(numPlayers, true);
        folded.assign(numPlayers, false);
        playerBets.assign(numPlayers, 0);
        pot = 0;

        // Assign blinds
        int smallBlindPos = (dealerPosition + 1) % numPlayers;
        int bigBlindPos = (dealerPosition + 2) % numPlayers;

        // Deal hole cards
        for (int i = 0; i < 2; ++i) {
            for (int p = 0; p < numPlayers; ++p) {
                if (p == 0) playersHands[p].push_back(drawCardForPlayer(deck, player));
                else playersHands[p].push_back(deck.dealCard());
            }
        }

        if (player.hasCurse("Unlucky Hand"))
            applyUnluckyHandToStarting(playersHands[0]);

        std::cout << "\nYour hole cards:\n";
        displayCards(playersHands[0], false);

        // Pay blinds
        playerBets[smallBlindPos] = smallBlind;
        playerBets[bigBlindPos] = bigBlind;
        pot += smallBlind + bigBlind;
        currentBet = bigBlind;

        // --- PRE-FLOP ---
        // (No community cards yet)
        if (!bettingRound(player, "Pre-Flop", casino)) {
            concludeAfterFold(player, casino);
            return;
        }

        // --- FLOP ---
        community.clear();
        burn();
        for (int i = 0; i < 3; ++i)
            community.push_back(deck.dealCard()); // flop
        if (!bettingRound(player, "Flop", casino)) {
            concludeAfterFold(player, casino);
            return;
        }

        // --- TURN ---
        burn();
        community.push_back(deck.dealCard());
        if (!bettingRound(player, "Turn", casino)) {
            concludeAfterFold(player, casino);
            return;
        }

        // --- RIVER ---
        burn();
        community.push_back(deck.dealCard());
        if (!bettingRound(player, "River", casino)) {
            concludeAfterFold(player, casino);
            return;
        }

        // --- SHOWDOWN ---
        showdown(player, casino);

        // Move dealer button
        dealerPosition = (dealerPosition + 1) % numPlayers;
        player.regenerateMana();
        player.decayCurses();
        player.clearBlessings();
        pauseEnter();
    }

private:
    Deck deck;
    int numPlayers;
    int smallBlind, bigBlind;
    int dealerPosition;
    int pot;
    int currentBet;
    std::vector<std::vector<Card>> playersHands;
    std::vector<double> totalBets;
    std::vector<Card> community;
    std::vector<double> playerBets;
    std::vector<bool> active;
    std::vector<bool> folded;

    void burn() { (void)deck.dealCard(); }

    void displayCards(const std::vector<Card>& cards, bool hideFirst = false) const {
        std::vector<std::vector<std::string>> arts;
        for (size_t i = 0; i < cards.size(); ++i) {
            if (i == 0 && hideFirst) arts.push_back(Card::hiddenCard());
            else arts.push_back(cards[i].displayCard());
        }
        for (size_t line = 0; line < arts[0].size(); ++line) {
            for (size_t c = 0; c < arts.size(); ++c)
                std::cout << arts[c][line] << " ";
            std::cout << "\n";
        }
    }

    void displayTable(const Player& player) const {
        drawAsciiBox("=== TABLE ===");

        std::cout << "Dealer Button: Seat " << dealerPosition + 1 << "\n";
        std::cout << "Small Blind: " << smallBlind << " | Big Blind: " << bigBlind << "\n";
        std::cout << "Pot: " << pot << " chips\n\n";

        std::cout << "Community Cards:\n";
        if (!community.empty()) displayCards(community, false);
        std::cout << "\n\nPlayers:\n";

        for (int i = 0; i < numPlayers; ++i) {
            std::cout << "Seat " << i + 1 << ": ";

            if (i == dealerPosition)
                std::cout << "(D) ";

            if (!active[i])
                std::cout << "[X] Folded";
            else if (i == 0)
                std::cout << "You - Bet: " << playerBets[i];
            else
                std::cout << "Opponent " << i << " - Bet: " << playerBets[i] << " (cards hidden)";

            std::cout << "\n";
        }
        std::cout << "\n";
        player.showStatus();
    }

    bool bettingRound(Player& player, const std::string& stage, CasinoManager &casino) {
        
        drawAsciiBox("=== " + stage + " ===");
        displayTable(player);

        // reset bets
        std::vector<int> totalBets(numPlayers, 0);
        currentBet = 0;

        // Player turn
        while (true) {
            std::cout << "\nYour hole cards:\n";
            displayCards(playersHands[0], false);

            std::cout << "\n1. Fold\n2. Check/Call\n3. Raise\nChoice: ";
            int choice = readInt("", 1, 3);

            if (choice == 1) {
                folded[0] = true;
                active[0] = false;
                drawAsciiBox("You folded.");
                return false;
            }
            else if (choice == 2) {
				currentBet = std::max(currentBet, static_cast<int>(playerBets[0]));
                for (int i = 0; i < numPlayers; ++i) {
                    currentBet = std::max(currentBet, static_cast<int>(playerBets[i]));
                }
                if (currentBet == playerBets[0]) {
                    drawAsciiBox("You check.");
                    break; // no bet to call
				}
                else if (currentBet > playerBets[0]) {
                    double callAmt;
					callAmt = currentBet - playerBets[0];
					std::cout << "You need to call " << callAmt << " chips.\n";
                    if (callAmt > player.getBalance()) {
                        drawAsciiBox("Insufficient funds to call. You go all-in with your remaining balance.");
                        callAmt = player.getBalance();
					}
                    else if (callAmt <= 0) {
                        drawAsciiBox("You check.");
						break;
					}
                    else {
                        if (callAmt > 0) casino.placeBet(callAmt, callAmt, callAmt);
                        playerBets[0] += callAmt;
                        pot += callAmt;
                        drawAsciiBox("You call " + std::to_string(callAmt));
                        totalBets[0] += callAmt;
                        break;
                    }
                    
                }
            }
            else if (choice == 3) {
                double raiseAmt;
                casino.placeBet(raiseAmt, currentBet + 10, currentBet + 100);
                playerBets[0] += raiseAmt;
                pot += raiseAmt;
                currentBet = raiseAmt;
                drawAsciiBox("You raised to " + std::to_string(raiseAmt));
                break;
            }
        }

        // AI decisions
        for (int p = 1; p < numPlayers; ++p) {
            if (!active[p]) continue;
            // approximate current revealed community for evaluation:
            int revealed = static_cast<int>(community.size()); // using everything dealt so far, fine for heuristic
            // Use evaluate / best-hand to decide strength
            HandRank hr = bestHandFromSeven(playersHands[p], community);
            // Make decision thresholds stage-dependent
            int requiredRankToContinue = 0; // 0 = high card, 1 = pair etc.
            if (stage == "Pre-Flop") requiredRankToContinue = 1; // need at least pair-ish to be confident
            else if (stage == "Flop") requiredRankToContinue = 0; // more flexible
            else if (stage == "Turn") requiredRankToContinue = 0;
            else requiredRankToContinue = 0;

            // AI uses a mix of hand strength and a bit of randomness (bluffing)
            bool shouldContinue = false;
            if (hr.rank >= requiredRankToContinue) shouldContinue = true;
            else {
                // weaker hands sometimes continue based on position / random bluff chance
                int bluffChance = 20 + (p < dealerPosition ? 10 : 0); // earlier positions less likely to bluff
                if (randint(1, 100) <= bluffChance) shouldContinue = true;
            }

            if (!shouldContinue) {
                active[p] = false;
                folded[p] = true;
                continue;
            }

            // If we continue, decide to call or raise
            int toCall = currentBet - playerBets[p];
            if (toCall < 0) toCall = 0;

            // small chance AI will raise if hand is strong
            bool doRaise = (hr.rank >= 2) && (randint(1, 100) <= 30); // two-pair+ raise sometimes
            if (toCall > 0) {
                // call (note: AI doesn't use CasinoManager; we just adjust pot/playerBets)
                playerBets[p] += toCall;
                pot += toCall;
                if (doRaise) {
                    int raiseAmount = std::max(10, currentBet / 2);
                    playerBets[p] += raiseAmount;
                    pot += raiseAmount;
                    currentBet = playerBets[p];
                }
            }
            else {
                // check; maybe small raise as a bet
                if (doRaise) {
                    int raiseAmount = std::max(10, currentBet / 2 + 10);
                    playerBets[p] += raiseAmount;
                    pot += raiseAmount;
                    currentBet = playerBets[p];
                }
            }
        }

        // If all folded except player
        int activeCount = 0;
        for (int i = 0; i < numPlayers; ++i) if (active[i]) ++activeCount;
        if (activeCount == 1 && active[0]) {
            drawAsciiBox("All opponents folded. You win the pot!");
            casino.processWin(static_cast<double>(pot), 1.0); // give player pot
            return true;
        }

        return true;
    }

    void showdown(Player& player, CasinoManager &casino) {
        drawAsciiBox("=== SHOWDOWN ===");
        displayCards(community, false);

        std::vector<HandRank> ranks(numPlayers);
        for (int p = 0; p < numPlayers; ++p)
            ranks[p] = active[p] ? bestHandFromSeven(playersHands[p], community) : HandRank{ -1, {} };

        HandRank best = HandRank{ -3, {} };
        for (int p = 0; p < numPlayers; ++p)
            if (active[p] && (best.rank == -3 || best < ranks[p])) best = ranks[p];

        std::vector<int> winners;
        for (int p = 0; p < numPlayers; ++p)
            if (active[p] && !(ranks[p] < best) && !(best < ranks[p])) winners.push_back(p);

        if (winners.empty()) drawAsciiBox("No winners!");
        else if (winners.size() == 1) {
            int w = winners[0];
            if (w == 0) {
                drawAsciiBox("You win the pot of " + std::to_string(pot) + "!");
				casino.processWin(pot, 1.0);
            }
            else {
                drawAsciiBox("Opponents Hand");
				displayCards(playersHands[w], false);
                drawAsciiBox("Opponent " + std::to_string(w + 1) + " has " + handRankName(ranks[w].rank) + ".");
                drawAsciiBox("Opponent " + std::to_string(w + 1) + " wins the pot.");
				casino.processLoss(playerBets[0]);
            }
        }
        else {
            drawAsciiBox("Pot is split between winners!");
            for (int i = 0; i < winners.size(); ++i) {
                displayCards(playersHands[winners[i]], false);
            }
			casino.processWin(pot / winners.size());
        }
    }

    void concludeAfterFold(Player& player, CasinoManager &casino) {
        drawAsciiBox("You folded the hand.");
		casino.processLoss(playerBets[0]);
        maybeApplyRandomCurseAfterLoss(player);
        player.regenerateMana();
        player.decayCurses();
        player.clearBlessings();
        pauseEnter();
    }

    // ---------------------
    // Hand evaluation helpers
    // ---------------------

    // Return list of indices combinations (n choose k). We'll use this to generate all 5-card combos from 7 cards.
    void combinations5of7(std::vector<std::array<int, 5>>& out) const {
        static const std::array<std::array<int, 5>, 21> combos = { {
            {0,1,2,3,4},{0,1,2,3,5},{0,1,2,3,6},{0,1,2,4,5},{0,1,2,4,6},{0,1,2,5,6},
            {0,1,3,4,5},{0,1,3,4,6},{0,1,3,5,6},{0,1,4,5,6},{0,2,3,4,5},{0,2,3,4,6},
            {0,2,3,5,6},{0,2,4,5,6},{0,3,4,5,6},{1,2,3,4,5},{1,2,3,4,6},{1,2,3,5,6},
            {1,2,4,5,6},{1,3,4,5,6},{2,3,4,5,6}
        } };
        out.assign(combos.begin(), combos.end());
    }

    // Evaluate a 5-card hand and return HandRank
    HandRank evaluate5(const std::array<Card, 5>& hand) const {
        // Convert ranks to ints (Ace high = 14)
        std::array<int, 5> vals;
        std::array<int, 5> suits;
        for (int i = 0; i < 5; ++i) {
            vals[i] = (int)hand[i].getRank();
            suits[i] = (int)hand[i].getSuit();
        }
        // Sort descending for kicker logic
        std::array<int, 5> svals = vals;
        std::sort(svals.begin(), svals.end(), std::greater<int>());

        // Count occurrences
        std::map<int, int> cnt;
        for (int v : vals) cnt[v]++;

        // Check flush
        bool flush = std::all_of(suits.begin(), suits.end(), [&](int x) { return x == suits[0]; });

        // Check straight
        std::vector<int> uniq;
        uniq.reserve(5);
        for (int v : svals) if (uniq.empty() || uniq.back() != v) uniq.push_back(v);
        bool straight = false;
        int topStraight = 0;
        if (uniq.size() >= 5) {
            // Normal straight check
            std::sort(uniq.begin(), uniq.end());
            // check consecutive sequences of length 5
            for (int i = 0; i + 4 < (int)uniq.size(); ++i) {
                if (uniq[i] + 1 == uniq[i + 1] && uniq[i + 1] + 1 == uniq[i + 2] && uniq[i + 2] + 1 == uniq[i + 3] && uniq[i + 3] + 1 == uniq[i + 4]) {
                    straight = true;
                    topStraight = uniq[i + 4];
                }
            }
        }
        // Special wheel case: A,5,4,3,2
        {
            std::vector<int> tmp(svals.begin(), svals.end());
            std::sort(tmp.begin(), tmp.end());
            if (tmp == std::vector<int>{2, 3, 4, 5, 14}) { straight = true; topStraight = 5; }
        }

        // Check groups (4,3,2)
        std::vector<std::pair<int, int>> groups; // (count, value)
        for (auto& kv : cnt) groups.emplace_back(kv.second, kv.first);
        std::sort(groups.begin(), groups.end(), [](auto& a, auto& b) {
            if (a.first != b.first) return a.first > b.first; // more first
            return a.second > b.second; // higher rank first
            });

        HandRank hr;
        hr.rank = 0;
        hr.tiebreak.clear();

        // Straight flush / royal
        if (straight && flush) {
            if (topStraight == 14) { hr.rank = 9; hr.tiebreak = { 14 }; return hr; } // royal
            hr.rank = 8; hr.tiebreak = { topStraight }; return hr;
        }

        // Four of a kind
        if (groups[0].first == 4) {
            hr.rank = 7;
            int quadVal = groups[0].second;
            int kicker = -1;
            for (int v : svals) if (v != quadVal) { kicker = v; break; }
            hr.tiebreak = { quadVal, kicker };
            return hr;
        }

        // Full house (3 + 2)
        if (groups[0].first == 3 && groups.size() > 1 && groups[1].first >= 2) {
            hr.rank = 6;
            int trip = groups[0].second;
            int pairv = groups[1].second;
            hr.tiebreak = { trip, pairv };
            return hr;
        }

        // Flush
        if (flush) {
            hr.rank = 5;
            hr.tiebreak = { svals[0], svals[1], svals[2], svals[3], svals[4] };
            return hr;
        }

        // Straight
        if (straight) {
            hr.rank = 4; hr.tiebreak = { topStraight }; return hr;
        }

        // Three of a kind
        if (groups[0].first == 3) {
            hr.rank = 3;
            int trip = groups[0].second;
            std::vector<int> kickers;
            for (int v : svals) if (v != trip) kickers.push_back(v);
            hr.tiebreak = { trip, kickers[0], kickers[1] };
            return hr;
        }

        // Two pair
        if (groups[0].first == 2 && groups.size() > 1 && groups[1].first == 2) {
            hr.rank = 2;
            int highPair = std::max(groups[0].second, groups[1].second);
            int lowPair = std::min(groups[0].second, groups[1].second);
            int kicker = -1;
            for (int v : svals) if (v != highPair && v != lowPair) { kicker = v; break; }
            hr.tiebreak = { highPair, lowPair, kicker };
            return hr;
        }

        // One pair
        if (groups[0].first == 2) {
            hr.rank = 1;
            int pairV = groups[0].second;
            std::vector<int> kickers;
            for (int v : svals) if (v != pairV) kickers.push_back(v);
            hr.tiebreak = { pairV, kickers[0], kickers[1], kickers[2] };
            return hr;
        }

        // High card
        hr.rank = 0;
        hr.tiebreak = { svals[0], svals[1], svals[2], svals[3], svals[4] };
        return hr;
    }

    HandRank bestHandFromSeven(const std::vector<Card>& hole, const std::vector<Card>& comm) const {
        std::array<Card, 7> all{{
            Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts),
            Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts),
            Card(Card::Two, Card::Hearts)
        }};
        all[0] = hole[0]; all[1] = hole[1];
        int commCount = std::min((int)comm.size(), 5);
        for (int i = 0; i < commCount; ++i)
            all[2 + i] = comm[i];
        for (int i = commCount; i < 5; ++i)
            all[2 + i] = Card(Card::Two, Card::Hearts);

        std::vector<std::array<int, 5>> combos;
        combinations5of7(combos);
        HandRank best = HandRank{ -3, {} };
        for (auto& c : combos) {
            std::array<Card, 5> hand5{{
                Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts),
                Card(Card::Two, Card::Hearts), Card(Card::Two, Card::Hearts),
                Card(Card::Two, Card::Hearts)
            }};
            for (int i = 0; i < 5; ++i) hand5[i] = all[c[i]];
            HandRank hr = evaluate5(hand5);
            if (best.rank == -3 || best < hr) best = hr;
        }
        return best;
    }

    // names for rank
    std::string handRankName(int r) const {
        static const char* names[] = { "High Card","One Pair","Two Pair","Three of a Kind","Straight","Flush","Full House","Four of a Kind","Straight Flush","Royal Flush" };
        if (r < 0 || r > 9) return "Unknown";
        return names[r];
    }
};
