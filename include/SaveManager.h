#pragma once
#include <string>
#include <map>
#include <vector>
#include "Player.h"
#include "Components.h"

class SaveManager
{
public:
    static void saveGame(const std::string& filename, int level, int maxLevel, const Player& player, const std::vector<Prop>& props);
    static bool loadGame(const std::string& filename, int& level, int& maxLevel, Player& player, std::vector<Prop>& props);

private:
    static std::string serialize(const std::map<std::string, std::string>& data);
    static std::map<std::string, std::string> deserialize(const std::string& content);
};
