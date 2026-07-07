#pragma once
#include <string>
#include <iostream>

class Character {
protected:
    std::string name;
    int health;
    int maxHealth;
    int level;

public:
    Character(std::string name, int health, int level)
        : name(name), health(health), maxHealth(health), level(level) {}

    virtual ~Character() {}

    // Pure virtual function making this an abstract class
    virtual void attack(Character& target) = 0;
    virtual void takeDamage(int damage) = 0;

    // Common getters
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getLevel() const { return level; }
    bool isAlive() const { return health > 0; }

    virtual void displayStats() const {
        std::cout << "--- " << name << " ---" << std::endl;
        std::cout << "Level: " << level << std::endl;
        std::cout << "Health: " << health << "/" << maxHealth << std::endl;
    }
};
