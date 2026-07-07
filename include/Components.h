#pragma once
#include <map>
#include <string>
#include <algorithm>

#include <SFML/Graphics.hpp>

// --- Prop Component / Entity Descriptor ---
struct Prop
{
    sf::Sprite    sprite;
    sf::FloatRect hitbox;
    bool          solid = false;
    float         hp = 100.0f;
    bool          isTree = false, isRock = false, isStone = false, isWood = false, isFood = false, isWater = false, isArmor = false, isWeaponLevel = false, isFarm = false, isHouse = false, isWell = false, isBush = false, isFlower = false, isFence = false, isStation = false, isFurniture = false, isMetal = false, isStump = false;
    int           stationType = 0, variant = 0, growthStage = 0, animF = 0;
    float         growthTimer = 0.f, animT = 0.f;

    void setBaseOrigin(float w, float h, float bottomOffset = 16.f) {
        sprite.setOrigin(w / 2.f, h - bottomOffset);
    }
};

// --- Stats Component ---
class Stats {
private:
    float health;
    float maxHealth;
    float hunger;
    float maxHunger;
    float stamina;
    float maxStamina;

public:
    Stats(float hp = 100.f, float st = 100.f, float hu = 100.f)
        : health(hp), maxHealth(hp), hunger(hu), maxHunger(hu), stamina(st), maxStamina(st) {}

    // Getters
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    float getHunger() const { return hunger; }
    float getMaxHunger() const { return maxHunger; }
    float getStamina() const { return stamina; }
    float getMaxStamina() const { return maxStamina; }

    // Setters & Modifiers
    void modifyHealth(float amt) { health = std::max(0.f, std::min(maxHealth, health + amt)); }
    void modifyHunger(float amt) { hunger = std::max(0.f, std::min(maxHunger, hunger + amt)); }
    void modifyStamina(float amt) { stamina = std::max(0.f, std::min(maxStamina, stamina + amt)); }

    void setHealth(float amt) { health = std::max(0.f, std::min(maxHealth, amt)); }
    void setHunger(float amt) { hunger = std::max(0.f, std::min(maxHunger, amt)); }
    void setStamina(float amt) { stamina = std::max(0.f, std::min(maxStamina, amt)); }

    bool isDead() const { return health <= 0; }
    bool isStarving() const { return hunger <= 0; }
    bool isExhausted() const { return stamina <= 0; }
    
    // Survival specific loop progression
    void updateSurvival(float dt) {
        // Natural hunger decay (Slowed down)
        modifyHunger(-0.1f * dt); 
        
        if (isStarving()) {
            modifyHealth(-1.f * dt); // Starving deducts health
        } else if (hunger > maxHunger * 0.75f) {
            modifyHealth(2.f * dt); // High hunger naturally regenerates health
        }
        
        // Natural stamina regen
        modifyStamina(5.f * dt); 
    }
};

// --- Inventory Component ---
class Inventory {
private:
    std::map<std::string, int> items;
    int armorLevel = 0;
    int weaponLevel = 1;

public:
    Inventory() {
        items["wood"] = 0;
        items["stone"] = 0;
        items["food"] = 10;
        items["water"] = 10;
        items["metal"] = 0;
    }

    void addItem(const std::string& name, int amt = 1) { items[name] += amt; }
    bool removeItem(const std::string& name, int amt = 1) { 
        if(items[name] >= amt) { items[name] -= amt; return true; } 
        return false; 
    }
    
    int getItemCount(const std::string& name) const { 
        auto it = items.find(name);
        return it != items.end() ? it->second : 0; 
    }

    int getArmorLevel() const { return armorLevel; }
    int getWeaponLevel() const { return weaponLevel; }
    void upgradeArmor() { armorLevel++; }
    void upgradeWeapon() { weaponLevel++; }

    void setItem(const std::string& name, int amt) { items[name] = amt; }
    void setArmorLevel(int val) { armorLevel = val; }
    void setWeaponLevel(int val) { weaponLevel = val; }
};
