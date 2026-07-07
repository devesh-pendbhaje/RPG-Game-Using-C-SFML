#pragma once
#include "Character.h"
#include <iostream>

class Enemy : public Character {
protected:
    int damage;
    int xpValue;

public:
    Enemy(std::string name, int health, int level, int damage, int xpValue)
        : Character(name, health, level), damage(damage), xpValue(xpValue) {}

    virtual void attack(Character& target) override {
        std::cout << name << " attacks for " << damage << " damage!" << std::endl;
        target.takeDamage(damage);
    }

    virtual void takeDamage(int damage) override {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << " took " << damage << " damage! (Health: " << health << ")" << std::endl;
    }

    int getXPValue() const { return xpValue; }
};

class Goblin : public Enemy {
public:
    Goblin(int level) : Enemy("Goblin Scout", 30 + (level * 10), level, 8 + (level * 2), 15 * level) {}

    void attack(Character& target) override {
        std::cout << name << " stabs with a rusty dagger!" << std::endl;
        Enemy::attack(target);
    }
};

class Dragon : public Enemy {
public:
    Dragon(int level) : Enemy("High Dragon", 150 + (level * 50), level, 20 + (level * 5), 100 * level) {}

    void attack(Character& target) override {
        if (rand() % 4 == 0) { // 25% chance for special attack
            std::cout << name << " breathes FIRE!" << std::endl;
            target.takeDamage(damage * 1.5);
        } else {
            std::cout << name << " claws at you!" << std::endl;
            Enemy::attack(target);
        }
    }
};

class Boss : public Enemy {
private:
    std::string title;
public:
    Boss(std::string name, std::string title, int level)
        : Enemy(name, 500 + (level * 100), level, 40 + (level * 10), 1000 * level), title(title) {}

    void attack(Character& target) override {
        std::cout << title << " " << name << " unleashes a DEVASTATING strike!" << std::endl;
        Enemy::attack(target);
        if (rand() % 5 == 0) {
            std::cout << name << " is ENRAGED! Strength increased!" << std::endl;
            damage += 5;
        }
    }

    void displayStats() const override {
        std::cout << "\n--- LEVEL BOSS ---" << std::endl;
        Enemy::displayStats();
        std::cout << "Title: " << title << std::endl;
    }
};
