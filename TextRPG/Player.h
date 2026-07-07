#pragma once
#include "Character.h"
#include "Inventory.h"
#include <iostream>

class Player : public Character {
private:
    Inventory inventory;
    int xp;
    int xpToNextLevel;

public:
    Player(std::string name)
        : Character(name, 100, 1), xp(0), xpToNextLevel(10) {}

    void attack(Character& target) override {
        int damage = inventory.getWeapon().getDamage() + (level * 2);
        std::cout << name << " swings their " << inventory.getWeapon().getName() << " for " << damage << " damage!" << std::endl;
        target.takeDamage(damage);
    }

    void takeDamage(int damage) override {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << " took " << damage << " damage! (Health: " << health << ")" << std::endl;
    }

    void gainXP(int amount) {
        xp += amount;
        std::cout << "\nYou gained " << amount << " XP!" << std::endl;
        if (xp >= xpToNextLevel) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        xp -= xpToNextLevel;
        xpToNextLevel *= 1.5;
        maxHealth += 20;
        health = maxHealth;
        std::cout << "\nLEVEL UP! You are now level " << level << "!" << std::endl;
        inventory.getWeapon().upgrade();
    }

    Inventory& getInventory() { return inventory; }
    const Inventory& getInventory() const { return inventory; }

    void heal() {
        if (inventory.getPotionCount() > 0) {
            inventory.usePotion();
            int healAmt = 40 + (level * 5);
            health = std::min(maxHealth, health + healAmt);
            std::cout << name << " used a potion and healed for " << healAmt << "!" << std::endl;
        } else {
            std::cout << "Out of potions!" << std::endl;
        }
    }

    void displayStats() const override {
        Character::displayStats();
        std::cout << "XP: " << xp << "/" << xpToNextLevel << std::endl;
        std::cout << "Current Weapon: " << inventory.getWeapon().getName() << " (Level " << inventory.getWeapon().getUpgradeLevel() << ")" << std::endl;
    }

    // Save/Load helpers (simple approach for now)
    std::string serialize() const {
        return name + "," + std::to_string(health) + "," + std::to_string(level) + "," + std::to_string(xp);
    }
};
