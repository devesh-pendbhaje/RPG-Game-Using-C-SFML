#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include "Player.h"
#include "Enemy.h"

class GameEngine {
private:
    std::unique_ptr<Player> player;
    int currentFloor;
    bool isRunning;

public:
    GameEngine() : currentFloor(1), isRunning(true) {}

    void init() {
        std::string name;
        std::cout << "\nWelcome, Hero! Enter your name: ";
        std::cin >> name;
        player = std::make_unique<Player>(name);
    }

    void start() {
        while (isRunning && player->isAlive()) {
            std::cout << "\n--- Floor " << currentFloor << " ---" << std::endl;
            std::cout << "1. Fight Enemy" << std::endl;
            std::cout << "2. View Stats" << std::endl;
            std::cout << "3. Inventory" << std::endl;
            std::cout << "4. Save Game" << std::endl;
            std::cout << "5. Exit" << std::endl;
            std::cout << "Choose Action: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: fight(); break;
                case 2: player->displayStats(); break;
                case 3: player->getInventory().displayInventory(); break;
                case 4: saveGame(); break;
                case 5: isRunning = false; break;
                default: std::cout << "Invalid choice!" << std::endl;
            }
        }

        if (!player->isAlive()) {
            std::cout << "\nGAME OVER! Legend has it " << player->getName() << " died on floor " << currentFloor << "." << std::endl;
        }
    }

    void fight() {
        // Spawn appropriate enemy based on floor
        Enemy* enemy = nullptr;
        if (currentFloor % 5 == 0) {
            enemy = new Boss("Morgoth", "Dark Lord", currentFloor);
        } else if (currentFloor % 3 == 0) {
            enemy = new Dragon(currentFloor);
        } else {
            enemy = new Goblin(currentFloor);
        }

        std::cout << "\nA " << enemy->getName() << " appears!" << std::endl;
        enemy->displayStats();

        while (player->isAlive() && enemy->isAlive()) {
            std::cout << "\n-- Combat --" << std::endl;
            std::cout << "1. Attack" << std::endl;
            std::cout << "2. Heal (Pots: " << player->getInventory().getPotionCount() << ")" << std::endl;
            std::cout << "3. Flee" << std::endl;
            std::cout << "Action: ";
            int action;
            std::cin >> action;

            if (action == 1) {
                player->attack(*enemy);
                if (enemy->isAlive()) {
                    enemy->attack(*player);
                }
            } else if (action == 2) {
                player->heal();
                enemy->attack(*player);
            } else if (action == 3) {
                if (rand() % 2 == 0) {
                    std::cout << "Successfully fled!" << std::endl;
                    delete enemy;
                    return;
                } else {
                    std::cout << "Failed to flee!" << std::endl;
                    enemy->attack(*player);
                }
            }
        }

        if (enemy->isAlive() == false) {
            std::cout << "\nYou defeated the " << enemy->getName() << "!" << std::endl;
            player->gainXP(enemy->getXPValue());
            player->getInventory().addPotions(rand() % 2); // Random potion drop
            currentFloor++;
        }

        delete enemy;
    }

    void saveGame() {
        std::ofstream file("save.txt");
        if (file.is_open()) {
            file << player->serialize() << "," << currentFloor;
            file.close();
            std::cout << "Game saved!" << std::endl;
        } else {
            std::cout << "Could not open save file!" << std::endl;
        }
    }

    void loadGame() {
        std::ifstream file("save.txt");
        if (file.is_open()) {
            std::string data;
            std::getline(file, data);
            std::cout << "Loading state: " << data << std::endl;
            // Simple parsing (can be much more complex)
            // Name, Health, Level, XP, Floor
            // For now, let's just create a new player for demo
            init(); 
            file.close();
            std::cout << "Loaded save data into " << player->getName() << std::endl;
        } else {
            std::cout << "No save file found!" << std::endl;
            init();
        }
    }
};
