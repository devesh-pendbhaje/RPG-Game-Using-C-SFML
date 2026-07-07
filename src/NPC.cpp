#include "../include/NPC.h"
#include <iostream>

NPC::NPC(std::string name, sf::Vector2f pos, const std::vector<std::string>& dialogues)
    : name(name), dialogues(dialogues), currentDialogueIndex(0)
{
    sprite.setPosition(pos);
    // Placeholder texture logic for NPC
    if (!texture.loadFromFile("assets_new/Idle_Down-Sheet.png")) {
        // Fallback or silent fail if asset missing
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 64, 64)); 
    sprite.setOrigin(32, 48); 
    sprite.setScale(3.0f, 3.0f);
}

void NPC::update(float dt)
{
    // Idle logic
}

void NPC::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

std::string NPC::getNextDialogue()
{
    if (dialogues.empty()) return "...";
    if (currentDialogueIndex >= (int)dialogues.size()) {
        currentDialogueIndex = 0; 
    }
    return dialogues[currentDialogueIndex++];
}
