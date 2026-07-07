#include "DataLoader.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::vector<EnemyData> DataLoader::loadEnemies(const std::string& path) {
    std::vector<EnemyData> enemies;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << path << std::endl;
        return enemies;
    }
    
    json j;
    try {
        file >> j;
        if (j.contains("enemies")) {
            for (const auto& item : j["enemies"]) {
                EnemyData ed;
                ed.id = item.value("id", "unknown");
                ed.health = item.value("health", 100);
                ed.speed = item.value("speed", 50);
                ed.damage = item.value("damage", 10);
                ed.detectionRadius = item.value("detectionRadius", 200);
                ed.behavior = item.value("behavior", "idle");
                enemies.push_back(ed);
            }
        }
    } catch(json::parse_error& ex) {
        std::cerr << "Parse error in " << path << " at byte " << ex.byte << std::endl;
    }
    return enemies;
}

std::vector<RecipeData> DataLoader::loadRecipes(const std::string& path) {
    std::vector<RecipeData> recipes;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << path << std::endl;
        return recipes;
    }
    
    json j;
    try {
        file >> j;
        if (j.contains("recipes")) {
            for (const auto& item : j["recipes"]) {
                RecipeData rd;
                rd.id = item.value("id", "unknown");
                rd.name = item.value("name", "Unknown Recipe");
                rd.dependency = item.value("dependency", "none");
                
                if (item.contains("requirements")) {
                    for (auto& [key, val] : item["requirements"].items()) {
                        rd.requirements[key] = val.get<int>();
                    }
                }
                recipes.push_back(rd);
            }
        }
    } catch(json::parse_error& ex) {
        std::cerr << "Parse error in " << path << " at byte " << ex.byte << std::endl;
    }
    return recipes;
}
