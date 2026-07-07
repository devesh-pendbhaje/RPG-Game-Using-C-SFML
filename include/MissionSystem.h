#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

struct Mission {
    std::string title;
    std::string description;
    int current;
    int goal;
    bool completed;

    Mission(std::string t, std::string d, int g) 
        : title(t), description(d), current(0), goal(g), completed(false) {}

    void progress(int amount = 1) {
        if (completed) return;
        current += amount;
        if (current >= goal) {
            current = goal;
            completed = true;
        }
    }

    bool isDone() const { return completed; }
};

class MissionSystem {
private:
    std::vector<Mission> activeMissions;
    sf::Font font;
    sf::RectangleShape panelBg;
    sf::Text headerText;
    sf::Text missionText;

public:
    MissionSystem();
    void addMission(const Mission& m);
    void updateMission(const std::string& title, int progress);
    void updateDescription(const std::string& title, const std::string& desc);
    void draw(sf::RenderWindow& window);
    bool allMissionsCompleted() const;
    void clear();
};
