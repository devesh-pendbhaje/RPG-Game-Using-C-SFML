#pragma once
#include <string>

class Weapon {
private:
    std::string name;
    int damage;
    int upgradeLevel;

public:
    Weapon(std::string name = "Rusty Sword", int damage = 5)
        : name(name), damage(damage), upgradeLevel(0) {}

    void upgrade() {
        upgradeLevel++;
        damage += 3;
    }

    std::string getName() const { return name; }
    int getDamage() const { return damage; }
    int getUpgradeLevel() const { return upgradeLevel; }

    // Operator overloading for weapon comparison (Bonus OOP usage)
    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }
};
