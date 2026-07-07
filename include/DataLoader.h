#pragma once
#include <string>
#include <vector>
#include <map>
#include "json.hpp"

struct EnemyData {
    std::string id;
    int health;
    int speed;
    int damage;
    int detectionRadius;
    std::string behavior;
};

struct RecipeData {
    std::string id;
    std::string name;
    std::map<std::string, int> requirements;
    std::string dependency;
};

class DataLoader {
public:
    static std::vector<EnemyData> loadEnemies(const std::string& path);
    static std::vector<RecipeData> loadRecipes(const std::string& path);
};
