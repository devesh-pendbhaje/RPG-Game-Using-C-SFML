#include "MissionSystem.h"
#include "EventManager.h"
#include <iostream>

MissionSystem::MissionSystem() {
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        // Fallback for different environments if necessary
        std::cerr << "Font Not Found: arial.ttf" << std::endl;
    }

    panelBg.setSize({320, 220});
    panelBg.setFillColor(sf::Color(0, 0, 0, 180)); // Slightly darker for contrast
    panelBg.setOutlineThickness(2.f);
    panelBg.setOutlineColor(sf::Color(255, 165, 0, 180)); // Orange border to match theme
    panelBg.setPosition(940, 20);

    headerText.setFont(font);
    headerText.setCharacterSize(18);
    headerText.setStyle(sf::Text::Bold);
    headerText.setFillColor(sf::Color(255, 220, 50));
    headerText.setString("ACTIVE MISSIONS");
    headerText.setPosition(955, 30);

    missionText.setFont(font);
    missionText.setCharacterSize(14);

    // Observer Hooks
    EventManager::getInstance().subscribe(EventType::OnTreeCut, [this](void* data) {
        int amount = (data == nullptr) ? 1 : (int)(size_t)data;
        this->updateMission("Gather Wood", amount);
    });

    EventManager::getInstance().subscribe(EventType::OnEnemyKilled, [this](void* data) {
        this->updateMission("Defeat Enemies", 1);
    });

    EventManager::getInstance().subscribe(EventType::OnItemCollected, [this](void* data) {
        this->updateMission("Clear Area", 1);
    });

    EventManager::getInstance().subscribe(EventType::OnHouseBuilt, [this](void* data) {
        this->updateMission("Construct Shelter", 1);
    });

    EventManager::getInstance().subscribe(EventType::OnFarmBuilt, [this](void* data) {
        this->updateMission("Establish Farm", 1);
    });

    EventManager::getInstance().subscribe(EventType::OnMetalSmelted, [this](void* data) {
        this->updateMission("Refine Metal", 1);
    });

    EventManager::getInstance().subscribe(EventType::OnWeaponUpgraded, [this](void* data) {
        this->updateMission("Master Smithing", 1);
    });
}

void MissionSystem::addMission(const Mission& m) {
    activeMissions.push_back(m);
}

void MissionSystem::updateMission(const std::string& title, int progress) {
    for (auto& m : activeMissions) {
        if (m.title == title) {
            m.progress(progress);
        }
    }
}

void MissionSystem::updateDescription(const std::string& title, const std::string& desc) {
    for (auto& m : activeMissions) {
        if (m.title == title) {
            m.description = desc;
        }
    }
}

void MissionSystem::draw(sf::RenderWindow& window) {
    window.draw(panelBg);
    window.draw(headerText);
    
    float yOffset = 55.f;
    for (const auto& m : activeMissions) {
        std::string prefix = m.isDone() ? "[X] " : "[ ] ";
        std::string progressStr = "(" + std::to_string(m.current) + "/" + std::to_string(m.goal) + ")";
        
        // Draw Title
        missionText.setCharacterSize(14);
        missionText.setStyle(sf::Text::Bold);
        missionText.setString(prefix + m.title + " " + progressStr);
        missionText.setPosition(955, yOffset);
        missionText.setFillColor(m.isDone() ? sf::Color(100, 255, 100) : sf::Color::White);
        window.draw(missionText);
        yOffset += 18.f;

        // Draw Description (where the costs are stored)
        missionText.setCharacterSize(11);
        missionText.setStyle(sf::Text::Regular);
        missionText.setString("  > " + m.description);
        missionText.setPosition(955, yOffset);
        missionText.setFillColor(sf::Color(200, 200, 200));
        window.draw(missionText);
        
        yOffset += 32.f;
    }
}

bool MissionSystem::allMissionsCompleted() const {
    for (const auto& m : activeMissions) {
        if (!m.isDone()) return false;
    }
    return true;
}

void MissionSystem::clear() {
    activeMissions.clear();
}
