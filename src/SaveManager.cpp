#include "SaveManager.h"
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

void SaveManager::saveGame(const std::string& filename, int level, int maxLevel, const Player& player, const std::vector<Prop>& props)
{
    json j;
    j["level"] = level;
    j["maxLevel"] = maxLevel;
    j["health"] = player.stats.getHealth();
    j["posX"] = player.getPosition().x;
    j["posY"] = player.getPosition().y;
    
    j["inventory"] = {
        {"wood", player.inventory.getItemCount("wood")},
        {"stone", player.inventory.getItemCount("stone")},
        {"metal", player.inventory.getItemCount("metal")},
        {"food", player.inventory.getItemCount("food")},
        {"water", player.inventory.getItemCount("water")},
        {"armor", player.inventory.getArmorLevel()},
        {"weaponLevel", player.inventory.getWeaponLevel()}
    };

    json propsArray = json::array();
    for (const auto& p : props) {
        json pJson;
        pJson["x"] = p.sprite.getPosition().x;
        pJson["y"] = p.sprite.getPosition().y;
        pJson["hp"] = p.hp;
        pJson["isTree"] = p.isTree;
        pJson["isRock"] = p.isRock;
        pJson["isStump"] = p.isStump;
        pJson["isFarm"] = p.isFarm;
        pJson["isHouse"] = p.isHouse;
        pJson["stationType"] = p.stationType;
        pJson["isStation"] = p.isStation;
        propsArray.push_back(pJson);
    }
    j["props"] = propsArray;

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

bool SaveManager::loadGame(const std::string& filename, int& level, int& maxLevel, Player& player, std::vector<Prop>& props)
{
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
        if (j.contains("level")) level = j["level"].get<int>();
        if (j.contains("maxLevel")) maxLevel = j["maxLevel"].get<int>();

        if (j.contains("health")) player.stats.setHealth(j["health"].get<float>());
        if (j.contains("posX") && j.contains("posY")) {
            player.setPosition(j["posX"].get<float>(), j["posY"].get<float>());
        }

        if (j.contains("inventory")) {
            auto& inv = j["inventory"];
            player.inventory.setItem("wood", inv.value("wood", 0));
            player.inventory.setItem("stone", inv.value("stone", 0));
            player.inventory.setItem("metal", inv.value("metal", 0));
            player.inventory.setItem("food", inv.value("food", 0));
            player.inventory.setItem("water", inv.value("water", 0));
            player.inventory.setArmorLevel(inv.value("armor", 0));
            player.inventory.setWeaponLevel(inv.value("weaponLevel", 1));
        }

        // NOTE: Restoring the exact textures for the props during load process will happen via the Game's loader interpreting these variables!
        if (j.contains("props")) {
            props.clear();
            for (auto& item : j["props"]) {
                Prop p;
                p.sprite.setPosition(item.value("x", 0.f), item.value("y", 0.f));
                p.hp = item.value("hp", 100.f);
                p.solid = true;
                p.isTree = item.value("isTree", false);
                p.isRock = item.value("isRock", false);
                p.isStump = item.value("isStump", false);
                p.isFarm = item.value("isFarm", false);
                p.isHouse = item.value("isHouse", false);
                p.isStation = item.value("isStation", false);
                p.stationType = item.value("stationType", 0);
                props.push_back(p);
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}
