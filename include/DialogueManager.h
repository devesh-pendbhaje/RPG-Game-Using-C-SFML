#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <queue>

class DialogueManager
{
private:
    sf::RectangleShape textBox;
    sf::Text dialogueText;
    sf::Font font;
    std::queue<std::string> dialogueQueue;
    bool isActive;
    std::string currentMessage;
    int displayCount;
    sf::Clock typingClock;

public:
    DialogueManager();
    void startDialogue(const std::vector<std::string>& sentences);
    void update();
    void draw(sf::RenderWindow& window);
    void nextSentence();
    bool getIsActive() const { return isActive; }
};
