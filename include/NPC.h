#pragma once
#include "Entity.h"
#include <string>
#include <vector>

class NPC : public Entity
{
private:
    std::string name;
    std::vector<std::string> dialogues;
    int currentDialogueIndex;

public:
    NPC(std::string name, sf::Vector2f pos, const std::vector<std::string>& dialogues);
    
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;
    
    std::string getNextDialogue();
    std::string getName() const { return name; }
    void resetDialogue() { currentDialogueIndex = 0; }
};
