#pragma once
#include <thread>
#include <chrono>
#include "Main.h"

void openSplashScreen(const std::string& title) {
    system("cls");
    drawAsciiBox(title);
    drawAsciiBox("Loading");
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // pause 1.5s
    system("cls");
}
void exitSplashScreen(const std::string& title) {
    system("cls");
    drawAsciiBox(title);
    drawAsciiBox("Thank you for playing! Goodbye!\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // pause 1.5s
    system("cls");
}

static void slowPrint(const std::string& text, int delay = 20) {
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

static void animateDice() {
    std::vector<std::string> frames = {
        u8"●     ",
        u8"● ●   ",
        u8"● ● ● ",
        u8" ● ● ●",
        u8"   ● ●",
        u8"     ●"
    };

    for (int i = 0; i < 10; i++) {
        std::cout << "\rDice rolling: " << frames[i % frames.size()] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }
}

static void playSplashScreen(const std::string& title) {
    system("cls");
    drawAsciiBox(title);
    drawAsciiBox("Loading");

    slowPrint(u8R"(



         ♠   ♦   ♣   ♥               ♣   ♠   ♥   ♦  


          ┌─────────┐   ┌─────────┐   ┌─────────┐
          │10       │   │ Q       │   │ A       │
          │         │   │         │   │         │
          │    ♥    │   │    ♠    │   │    ♦    │
          │         │   │         │   │         │
          │       10│   │       Q │   │       A │
          └─────────┘   └─────────┘   └─────────┘


                  ● Dice rolling ●
                  £ Chips stacking £
                  ★ Cards shuffling ★


        ♦   ♣   ♠   ♥               ♥   ♦   ♣   ♠ 
    )");

    animateDice();
    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

void animateRouletteWheel() {
    std::vector<std::string> frames = {
        "  [ 0 ]  32  15  19  4  21  2  25  17  ",
        "  17  [ 0 ]  32  15  19  4  21  2  25  ",
        "  25  17  [ 0 ]  32  15  19  4  21  2  ",
        "  2   25  17  [ 0 ]  32  15  19  4  21 ",
        "  21  2   25  17  [ 0 ]  32  15  19  4 ",
        "  4   21  2   25  17  [ 0 ]  32  15  19",
        "  19  4   21  2   25  17  [ 0 ]  32  15",
        "  15  19  4   21  2   25  17  [ 0 ]  32"
    };

    for (int i = 0; i < 16; i++) {
        std::cout << "\rSpinning wheel: " << frames[i % frames.size()] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }

    std::cout << "\n";
}

void animateRouletteBall() {
    std::vector<std::string> ball = {
        "Ball: ○-------------",
        "Ball: ----○---------",
        "Ball: --------○-----",
        "Ball: ------------○--",
        "Ball: -------------○-",
        "Ball: --------------○",
        "Ball: -----○---------",
        "Ball: -----------○---"
    };

    for (int i = 0; i < 12; i++) {
        std::cout << "\r" << ball[i % ball.size()] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(140));
    }

    std::cout << "\n";
}

void playRouletteSplash(const std::string& title) {
    system("cls");

    slowPrint("====================================\n", 2);
    slowPrint("           " + title + "\n", 5);
    slowPrint("====================================\n\n", 2);

    slowPrint("         W H E E L   S P I N N I N G\n\n", 10);

    // Wheel animation
    animateRouletteWheel();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Ball animation
    animateRouletteBall();
    std::cout << "\n";

    // Fancy roulette ASCII
    slowPrint(R"(
              ┌──────────────────────────┐
              │   0  32  15  19  4  21   │
              │  2  25  17  34  6  27    │
              │   13  36  11  30  8  23  │
              └──────────────────────────┘
                  Place your bets...
)", 1);

    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    system("cls");
}