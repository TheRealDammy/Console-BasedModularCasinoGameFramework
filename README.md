# Console-Based Modular Casino Game Framework (C++)

A **C++ console application** featuring multiple casino-style mini-games built around reusable systems:
betting flow, player state, deck/card utilities, and a central manager to keep logic modular and scalable.

> This project is intentionally **text-based** to emphasize **clean architecture**, testable logic, and systems design.

---

## ✨ Features
- **Multiple mini-games** in one application (see list below)
- Centralized **betting flow** with validation (min/max, balance checks)
- Player state tracking (balance + additional status systems)
- Reusable **Card / Deck** utilities
- Console UI helpers (boxed status/messages)

---

## 🎮 Games Included
- Blackjack
- Poker
- Baccarat
- Slots
- High-Low

---

## 🧱 Architecture (high-level)

### Modules
- **Main / Menu Loop**  
  Handles navigation between games and calls into the manager.
- **CasinoManager**  
  Orchestrates the round flow (place bet → play game → win/loss → summary).
- **Player**  
  Stores persistent player state (balance + other status data).
- **Deck / Card**  
  Shared card and deck logic for card-based games.
- **Game Modules**  
  Each mini-game runs its own rules while using shared systems.

### Tiny architecture diagram

        +----------------------+
        |   Main / Menu Loop   |
        +----------+-----------+
                   |
                   v
        +----------------------+
        |     CasinoManager    |
        | (bet -> run -> pay)  |
        +----+----------+------+
             |          |
             |          v
             |   +-------------+
             |   |   Player    |
             |   | balance/etc |
             |   +-------------+
             |
             v
  +-------------------------+
  |     Game Modules        |
  | Blackjack / Poker / ... |
  +-----------+-------------+
              |
              v
      +---------------+
      |  Deck / Card  |
      +---------------+

