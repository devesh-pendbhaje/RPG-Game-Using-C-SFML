#pragma once
#include <functional>
#include <vector>
#include <unordered_map>

enum class EventType {
    OnTreeCut,
    OnEnemyKilled,
    OnItemCollected,
    OnRecipeUnlocked,
    OnHouseBuilt,
    OnFarmBuilt,
    OnMetalSmelted,
    OnWeaponUpgraded
};

class EventManager {
public:
    using EventCallback = std::function<void(void*)>; // Using void* to pass variant event data

    // Singleton pattern for global event bus
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }

    void subscribe(EventType type, EventCallback callback) {
        subscribers[type].push_back(callback);
    }

    void trigger(EventType type, void* data = nullptr) {
        if (subscribers.find(type) != subscribers.end()) {
            for (auto& callback : subscribers[type]) {
                callback(data);
            }
        }
    }

private:
    EventManager() {}
    std::unordered_map<EventType, std::vector<EventCallback>> subscribers;
};
