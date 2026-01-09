#pragma once
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


//================== Playing Card Definition ==================//
//------Card class representing a playing card-------//
class Card {
public:
	enum Suit { Hearts, Diamonds, Clubs, Spades };
	enum Rank { Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };	
private:
	Rank rank;
	Suit suit;
public:	
	Card(Rank r, Suit s) : rank(r), suit(s) {}
	std::string toString() const {
		static const char* RankStrings[] = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
		static const char* SuitStrings[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
		return std::string(RankStrings[rank]) + " of " + SuitStrings[suit];
	}
	Rank getRank() const { return rank; }
	Suit getSuit() const { return suit; }

	int getValue() const {
		if (rank >= Two && rank <= Ten) return rank;
		if (rank >= Jack && rank <= King) return 10;
		if (rank == Ace) return 11; // Ace can also be 1, handled in game logic
		return 0;
	}

	std::vector<std::string> displayCard() const {
		std::vector<std::string> card(7);
		card[0] = u8"┌─────────┐";
		card[1] = u8"│" + rankToString() + u8"       │";
		card[2] = u8"│         │";
		card[3] = u8"│    " + suitToString() + u8"    │";
		card[4] = u8"│         │";
		card[5] = u8"│       " + rankToString() + u8"│";
		card[6] = u8"└─────────┘";
		return card;
	}
private:
	// Return a 2-char rank string for aligning the card art:
	// "10" for Ten, otherwise single char plus a trailing space (e.g. "A ", "K ", "2 ")
	std::string rankToString() const {
		switch (rank) {
		case Two:   return "2 ";
		case Three: return "3 ";
		case Four:  return "4 ";
		case Five:  return "5 ";
		case Six:   return "6 ";
		case Seven: return "7 ";
		case Eight: return "8 ";
		case Nine:  return "9 ";
		case Ten:   return "10";
		case Jack:  return "J ";
		case Queen: return "Q ";
		case King:  return "K ";
		case Ace:   return "A ";
		default:    return "  ";
		}
	}

	// Return a single-character suit symbol for the card art
	std::string suitToString() const {
		switch (suit) {
		case Hearts:   return u8"♥";
		case Diamonds: return u8"♦";
		case Clubs:    return u8"♣";
		case Spades:   return u8"♠";
		default:       return u8" ";
		}
	}
public:
	static std::vector<std::string> hiddenCard() {
		std::vector<std::string> card(7);
		card[0] = u8"┌─────────┐";
		card[1] = u8"│░░░░░░░░░│";
		card[2] = u8"│░░░░░░░░░│";
		card[3] = u8"│░░░░░░░░░│";
		card[4] = u8"│░░░░░░░░░│";
		card[5] = u8"│░░░░░░░░░│";
		card[6] = u8"└─────────┘";
		return card;
	}
};

//================== Deck Definition ==================//
//------Deck class representing a deck of cards-------//
class Deck {
public:
	Deck() { refill(); shuffle(); }

	// Build a standard 52-card deck
	void refill() {
		cards.clear();
		for (int s = Card::Hearts; s <= Card::Spades; ++s) {
			for (int r = Card::Two; r <= Card::Ace; ++r) {
				cards.emplace_back(static_cast<Card::Rank>(r), static_cast<Card::Suit>(s));
			}
		}
		idx = 0;
	}

	void shuffle() {
		std::random_shuffle(cards.begin(), cards.end());
		idx = 0;
	}

	size_t remaining() const { return (idx <= cards.size()) ? cards.size() - idx : 0; }

	// Deal the next card (wraps by refilling & shuffling if exhausted)
	Card dealCard() {
		if (idx >= cards.size()) {
			refill();
			shuffle();
		}
		return cards[idx++];
	}
<<<<<<< Updated upstream
	template<typename Pred>
	Crad findAndRemove(Pred p) {
		for 
	}
=======

	// Search for a card matching predicate among the remaining cards and remove it.
	// If none found, returns dealCard() as fallback.
	template<typename Pred>
	Card findAndRemove(Pred p) {
		for (size_t i = idx; i < cards.size(); ++i) {
			if (p(cards[i])) {
				Card found = cards[i];
				cards.erase(cards.begin() + i);
				return found;
			}
		}
		// fallback -> normal next deal
		return dealCard();
	}

	// Replace at absolute index (careful - mostly unused)
	void replaceAt(size_t position, const Card& c) {
		if (position < cards.size()) cards[position] = c;
	}
private:
	std::vector<Card> cards;
	size_t idx = 0;
>>>>>>> Stashed changes
};

//================== Utility Functions ==================//
//------Function to draw an ASCII box around text-------//
static size_t utf8_codepoints(const std::string& s) {
	size_t count = 0;
	for (unsigned char c : s) {
		if ((c & 0xC0) != 0x80) ++count; // count only lead bytes
	}
	return count;
}
void drawAsciiBox(const std::string& text, int padding = 2) {
	std::vector<std::string> lines;
	std::stringstream ss(text);
	std::string line;
	size_t maxVisual = 0;

	// Split text into lines & find longest (visually)
	while (std::getline(ss, line)) {
		lines.push_back(line);
		maxVisual = std::max(maxVisual, utf8_codepoints(line));
	}

	if (lines.empty()) {
		lines.push_back("");
		maxVisual = 0;
	}

	// Total inner width = content + (left/right padding)
	const size_t innerWidth = maxVisual + padding * 2;

	// Draw top border
	std::cout << u8"╔";
	for (size_t i = 0; i < innerWidth; ++i) std::cout << u8"═";
	std::cout << u8"╗\n";

	for (const auto& l : lines) {
		size_t visual = utf8_codepoints(l);
		// Total remaining space after accounting for padding
		size_t remaining = (innerWidth > visual) ? innerWidth - visual : 0;

		// Split remaining evenly for centering
		size_t leftPad = remaining / 2;
		size_t rightPad = remaining - leftPad;

		std::cout << u8"║"
			<< std::string(leftPad, ' ')
			<< l
			<< std::string(rightPad, ' ')
			<< u8"║\n";
	}

	// Draw bottom border
	std::cout << u8"╚";
	for (size_t i = 0; i < innerWidth; ++i) std::cout << u8"═";
	std::cout << u8"╝\n";
}

<<<<<<< Updated upstream
//================== Player Definition ==================//
//-----Player Class-----//
=======
static std::mt19937& rng() {
	static std::mt19937 g((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	return g;
}
int randint(int lo, int hi) { std::uniform_int_distribution<int> d(lo, hi); return d(rng()); }
double readDouble(const std::string& prompt, double minv, double maxv) {
	while (true) {
		std::cout << prompt;
		double x;
		if (!(std::cin >> x)) {
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			std::cout << "Invalid input. Enter a number.\n";
			continue;
		}
		std::cin.ignore(10000, '\n');
		if (x < minv || x > maxv) {
			std::cout << "Enter a number between " << minv << " and " << maxv << ".\n";
			continue;
		}
		return x;
	}
}
int readInt(const std::string& prompt, int minv, int maxv) {
	while (true) {
		std::cout << prompt;
		int x;
		if (!(std::cin >> x)) {
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			std::cout << "Invalid input. Enter an integer.\n";
			continue;
		}
		std::cin.ignore(10000, '\n');
		if (x < minv || x > maxv) {
			std::cout << "Enter a number between " << minv << " and " << maxv << ".\n";
			continue;
		}
		return x;
	}
}
void pauseEnter(const std::string& msg = "Press Enter to continue...") {
	std::cout << msg;
	std::string tmp;
	std::getline(std::cin, tmp);
}
std::string readLineTrimmed(const std::string& prompt = "") {
	if (!prompt.empty()) std::cout << prompt;
	std::string line;
	std::getline(std::cin, line);
	// trim
	auto l = line.find_first_not_of(" \t\r\n");
	if (l == std::string::npos) return "";
	auto r = line.find_last_not_of(" \t\r\n");
	return line.substr(l, r - l + 1);
}

//================== Player Definition ==================//
//------Class defining player attributes-------//
>>>>>>> Stashed changes
struct ActiveCurse {
	std::string name;
	int remainingRounds;
};
<<<<<<< Updated upstream

class Player {
private:
	std::string name;
	double balance = 500.0;
	double currentBet = 0.0;
	int mana;
	const int maxMana;
	std::vector<ActiveCurse> curses;

public: 
	Player(std::string nm = "Player", double startBalance = 500.0) : name(std::move(nm)), balance(startBalance), mana(50), maxMana(100) {}

	std::string getName() const {
		return name;
	}
	double getBalance() const {
		return balance;
	}
	int getMana() const {
		return mana;
	}

	//betting
	bool placeBet(double amount) {
		if (amount <= 0 || amount > balance) {
			drawAsciiBox("Invalid bet or Insufficient balance.");
			return false;
		}
		balance -= amount;
		currentBet = amount;
		return true;
	}
	void payWin(double multiplier = 2.0) {
		balance += static_cast<double>(currentBet) * multiplier; 
		currentBet = 0;
	}
	void refundBet() {
		balance += currentBet;
		currentBet = 0;
	}

	//mana & blessing
=======
class Player {
public:
	Player(std::string nm = "Player", double startBalance = 500.0)
		: name(std::move(nm)), balance(startBalance), mana(50), maxMana(100), currentBet(0.0), isFolded(false), isAllIn(false) {
	}

	std::string getName() const { return name; }
	double getBalance() const { return balance; }
	int getMana() const { return mana; }

	// ----- Betting helpers -----
	// commitBet: incrementally contribute to the current betting round.
	// Returns true if contribution successful.
	bool commitBet(double amount) {
		if (amount <= 0) return false;
		// If player doesn't have enough, they go all-in for the remaining balance.
		if (amount >= balance) {
			// all-in
			amount = balance;
			balance = 0.0;
			currentBet += amount;
			isAllIn = true;
			return true;
		}
		// normal deduct
		balance -= amount;
		currentBet += amount;
		return true;
	}

	// Legacy-compatible wrapper: behaves the same as commitBet now, so existing code calling placeBet(...) still works.
	bool placeBet(double amount) {
		return commitBet(amount);
	}

	// Payout: add chips to balance; multiplier is applied to the total contributed bet by player earlier.
	// This is a simplified payout function; full poker needs pot logic outside player.
	void payWin(double amount) { balance += amount; }

	// Refund only this player's current bet (used in some push logic)
	void refundCurrentBet() {
		balance += currentBet;
		currentBet = 0.0;
		isAllIn = false;
		isFolded = false;
	}

	// Clear current bet (move to pot externally)
	void clearCurrentBet() { currentBet = 0.0; }

	double getCurrentBet() const { return currentBet; }
	bool folded() const { return isFolded; }
	bool allIn() const { return isAllIn; }

	void fold() { isFolded = true; }
	void resetForNewHand() {
		currentBet = 0.0;
		isFolded = false;
		isAllIn = false;
		// keep balance and curses/mana across hands
	}

	bool canCover(double amount) const {
		return amount <= balance;
	}

	// Mana & Blessings (unchanged semantics)
>>>>>>> Stashed changes
	bool useMana(int cost) {
		if (mana < cost) return false;
		mana -= cost;
		return true;
	}
	void regenerateMana(int amount = 5) {
		mana += amount;
		if (mana > maxMana) mana = maxMana;
	}
<<<<<<< Updated upstream
	void castBlessing(const std::string& b) {
		if (b == "Fate's Glimpse") {
			if (useMana(15)) {
				fateGlimpse = true;
				drawAsciiBox("Fate's Glimpse active for this round.");
			}
			else drawAsciiBox("Not enough mana for Fate's Glimpse.");
		}
		else if (b == "Lucky Draw") {
			if (useMana(20)) {
				luckyDraw = true;
				drawAsciiBox("Lucky Draw active: next card you draw will be 10-valued.");
			}
			else drawAsciiBox("Not enough mana for Lucky Draw.");
		}
		else if (b == "Mana Shield") {
			if (useMana(10)) {
				manaShield = true;
				drawAsciiBox("Mana Shield Active: Will block one new curse this round.");
			}
			else drawAsciiBox("Not enough mana for Mana Shield.");
		}
		else {
			drawAsciiBox("Unknown Blessing.");
		}
	}
=======

	void castBlessing(const std::string& b) {
		if (b == "Fate's Glimpse") {
			if (useMana(15)) { fateGlimpse = true; drawAsciiBox("Fate's Glimpse active for this round."); }
			else drawAsciiBox("Not enough mana for Fate's Glimpse.");
		}
		else if (b == "Lucky Draw") {
			if (useMana(20)) { luckyDraw = true; drawAsciiBox("Lucky Draw active: next card for you will be 10-valued."); }
			else drawAsciiBox("Not enough mana for Lucky Draw.");
		}
		else if (b == "Mana Shield") {
			if (useMana(10)) { manaShield = true; drawAsciiBox("Mana Shield active: will block one new curse this round."); }
			else drawAsciiBox("Not enough mana for Mana Shield.");
		}
		else {
			drawAsciiBox("Unknown blessing.");
		}
	}

>>>>>>> Stashed changes
	void clearBlessings() {
		luckyDraw = false;
		fateGlimpse = false;
		manaShield = false;
	}

<<<<<<< Updated upstream
	//curses : stored by name and remaining rounds
	void applyCurse(const std::string& curseName, int duration) {
		if (manaShield) {
			manaShield = false;
			drawAsciiBox("Mana Shield absorde the curse:" + curseName);
=======
	// Curses: store by name + remaining rounds
	void applyCurse(const std::string& curseName, int duration) {
		if (manaShield) {
			// negate one curse application
			manaShield = false;
			drawAsciiBox("Mana Shield absorbed the curse: " + curseName);
>>>>>>> Stashed changes
			return;
		}
		for (auto& c : curses) {
			if (c.name == curseName) {
<<<<<<< Updated upstream
				c.remainingRounds = duration;
				drawAsciiBox("Curse Refreshed: " + curseName);
=======
				c.remainingRounds = duration; // refresh
				drawAsciiBox("Curse refreshed: " + curseName);
>>>>>>> Stashed changes
				return;
			}
		}
		curses.push_back({ curseName, duration });
<<<<<<< Updated upstream
		drawAsciiBox("You recieved a curse: " + curseName + " for (" + std::to_string(duration) + " rounds)");
	}
	bool hasCurse(const std::string& curseName) const {
		for (auto& c : curses) {
			if (c.name == curseName && c.remainingRounds > 0) {
				return true;
			}
		}
		return false;
	}
	void decayCurse() {
=======
		drawAsciiBox("You received a curse: " + curseName + " (" + std::to_string(duration) + " rounds)");
	}

	bool hasCurse(const std::string& curseName) const {
		for (auto& c : curses) if (c.name == curseName && c.remainingRounds > 0) return true;
		return false;
	}

	// Decrement curse durations after each round
	void decayCurses() {
>>>>>>> Stashed changes
		for (auto it = curses.begin(); it != curses.end();) {
			it->remainingRounds--;
			if (it->remainingRounds <= 0) {
				drawAsciiBox("Curse ended: " + it->name);
				it = curses.erase(it);
			}
			else ++it;
		}
	}

	void showStatus() const {
		std::ostringstream oss;
		oss << "PLAYER STATUS\n";
		oss << "Name: " << name << "\n";
<<<<<<< Updated upstream
		oss << "Balance: £" << balance << "\n";
		oss << "Mana: " << mana << "/" << maxMana << "\n";
		if (!curses.empty()) {
			oss << "Curses:\n";
			for (auto& c : curses) {
				oss << " - " << c.name << " (" << c.remainingRounds << ")\n";
			}
		}
		else oss << "Curses: No active curses.\n";
		oss << "Blessings active:\n";
		if (luckyDraw) {
			oss << " - Lucky Draw\n";
		}
		if (fateGlimpse) {
			oss << " - Fate Glimpse\n";
		}
		if (manaShield) {
			oss << " - Mana Shield\n";
		}
		if (!luckyDraw && !fateGlimpse && !manaShield) {
			oss << " - None active";
			drawAsciiBox(oss.str());
		}
	}

	bool luckyDraw = false; // force next card to be valued 10
	bool fateGlimpse = false; // reavel hidden card/opponent hand
	bool manaShield = false; // block one curse in a round
};

//================== Helper Functions ==================//
//-----Game Helpers for blessings/curses-----//

Card drawCardForPlayer(Deck& deck, Player& player) {
	if (player.luckyDraw) {
		Card found = deck.findAndRemove([] const Card & c) {
			int v = (int)c.rank();
		}
	}
=======
		oss << u8"Balance: £" << balance << "\n";
		oss << u8"Current Bet: £" << currentBet << "\n";
		oss << "Mana: " << mana << "/" << maxMana << "\n";
		if (!curses.empty()) {
			oss << "Curses:\n";
			for (auto& c : curses) oss << " - " << c.name << " (" << c.remainingRounds << ")\n";
		}
		else oss << "Curses: None\n";
		oss << "Blessings active:\n";
		if (luckyDraw) oss << " - Lucky Draw\n";
		if (fateGlimpse) oss << " - Fate's Glimpse\n";
		if (manaShield) oss << " - Mana Shield\n";
		if (!luckyDraw && !fateGlimpse && !manaShield) oss << " - None\n";
		drawAsciiBox(oss.str());
	}

	// public fields for easy checks (temporary flags)
	bool luckyDraw = false;      // next player draw forced to 10-value
	bool fateGlimpse = false;    // reveal a hidden card/opponent hand
	bool manaShield = false;     // block one curse application in this round

private:
	std::string name;
	double balance = 500.0;
	double currentBet;
	int mana;
	const int maxMana;
	bool isFolded;
	bool isAllIn;
	std::vector < ActiveCurse> curses;
};

class CasinoManager {
public:
	Player& player;
	double totalEarnings = 0;
	double totalLosses = 0;

	CasinoManager(Player& p) : player(p) {}

	bool placeBet(double& betAmount, double min = 10, double max = 1000) {
		betAmount = readDouble(u8"Enter your bet (£" + std::to_string(min) + u8"–£" + std::to_string(max) + "): ", min, max);
		if (player.getBalance() < betAmount) {
			drawAsciiBox(u8"Insufficient funds. Your balance: £" + std::to_string(player.getBalance()));
			return false;
		}
		player.placeBet(betAmount);
		drawAsciiBox(u8"Bet placed: £" + std::to_string(betAmount) + u8"\nRemaining balance: £" + std::to_string(player.getBalance()));
		return true;
	}

	void processWin(double betAmount, double multiplier = 2.0) {
		double win = betAmount * multiplier;
		player.payWin(win);
		totalEarnings += (win - betAmount);
		drawAsciiBox(u8"You won £" + std::to_string(win) + u8"!\nNew balance: £" + std::to_string(player.getBalance()));
	}

	void processLoss(double betAmount) {
		totalLosses += betAmount;
		drawAsciiBox(u8"You lost £" + std::to_string(betAmount) + u8"\nBalance: £" + std::to_string(player.getBalance()));
	}

	void showStats() {
		std::ostringstream oss;
		oss << "Session Summary\n";
		oss << u8"Balance: £" << player.getBalance() << "\n";
		oss << u8"Earnings: £" << totalEarnings << "\n";
		oss << u8"Losses: £" << totalLosses << "\n";
		drawAsciiBox(oss.str());
	}
};

//================== Helper Functions ==================//
//------Blessings and Curses-------//

Card drawCardForPlayer(Deck& deck, Player& player) {
	if (player.luckyDraw) {
		// search for 10-valued card (rank Ten or Jack or Queen or King)
		Card found = deck.findAndRemove([](const Card& c) {
			auto r = c.getRank();
			return r == Card::Ten || r == Card::Jack || r == Card::Queen || r == Card::King;
			});
		// consume flag
		const_cast<Player&>(player).luckyDraw = false;
		return found;
	}
	else {
		return deck.dealCard();
	}
}

// When player has "Unlucky Hand" curse in Poker: cap starting cards' ranks at 7
void applyUnluckyHandToStarting(std::vector<Card> &hand) {
	for (auto& c : hand) {
		if ((int)c.getRank() > (int)Card::Seven) {
			// lower rank to a random 2..7, keep suit
			int newRank = randint((int)Card::Two, (int)Card::Seven);
			c = Card(static_cast<Card::Rank>(newRank), c.getSuit());
		}
	}
}

// Muddled Sight: when asking Hit/Stand, 20% chance input is flipped
bool maybeFlipHSChoiceIfMuddled(Player& player, bool originalChoiceStand) {
	if (!player.hasCurse("Muddled Sight")) return originalChoiceStand;
	int roll = randint(1, 100);
	if (roll <= 20) {
		// flip
		return !originalChoiceStand;
	}
	return originalChoiceStand;
}

// Chance to apply a random curse after a loss
void maybeApplyRandomCurseAfterLoss(Player& player) {
	int roll = randint(1, 100);
	if (roll <= 25) { // 25% chance to get a curse
		int pick = randint(1, 2);
		if (pick == 1) const_cast<Player&>(player).applyCurse("Muddled Sight", 2);
		else const_cast<Player&>(player).applyCurse("Unlucky Hand", 3);
	}
}

// Show round summary: balance, mana, active curses
void showRoundSummary(const Player& player) {
	// player.showStatus already composes and draws a boxed status
	player.showStatus();
>>>>>>> Stashed changes
}