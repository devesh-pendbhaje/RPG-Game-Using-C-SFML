#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Weapon.h"

class Inventory {
private:
    std::vector<std::string> items;
    Weapon currentWeapon;
    int potionCount;

public:
    Inventory() : currentWeapon("Rusty Sword", 10), potionCount(3) {}

    void addItem(std::string item) {
        items.push_back(item);
    }

    void usePotion() {
        if (potionCount > 0) potionCount--;
    }

    void equipWeapon(Weapon newWeapon) {
        currentWeapon = newWeapon;
    }

    Weapon& getWeapon() { return currentWeapon; }
    const Weapon& getWeapon() const { return currentWeapon; }
    int getPotionCount() const { return potionCount; }
    void addPotions(int count) { potionCount += count; }

    void displayInventory() const {
        std::cout << "\n--- Inventory ---" << std::endl;
        std::cout << "Weapon: " << currentWeapon.getName() << " (Damage: " << currentWeapon.getDamage() << ")" << std::endl;
        std::cout << "Potions: " << potionCount << std::endl;
        std::cout << "Items: ";
        if (items.empty()) std::cout << "Empty";
        else {
            for (const auto& item : items) std::cout << item << ", ";
        }
        std::cout << std::endl;
    }
};
