#include <iostream>
#include <ctime>
#include <cstdlib>
#include "GameEngine.h"

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    std::cout << "========================================" << std::endl;
    std::cout << "🎮 CONSOLE-BASED RPG GAME ENGINE 🎮" << std::endl;
    std::cout << "========================================" << std::endl;

    GameEngine engine;

    std::cout << "1. New Game" << std::endl;
    std::cout << "2. Load Game" << std::endl;
    std::cout << "Action: ";
    int startChoice;
    std::cin >> startChoice;

    if (startChoice == 2) {
        engine.loadGame();
    } else {
        engine.init();
    }

    engine.start();

    std::cout << "\nThank you for playing!" << std::endl;
    return 0;
}
