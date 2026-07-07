#include "../include/DialogueManager.h"
#include <iostream>

DialogueManager::DialogueManager() : isActive(false), displayCount(0)
{
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load dialogue font" << std::endl;
    }

    textBox.setSize(sf::Vector2f(800.f, 200.f));
    textBox.setFillColor(sf::Color(0, 0, 0, 200));
    textBox.setOutlineColor(sf::Color::White);
    textBox.setOutlineThickness(2.f);
    textBox.setPosition(240.f, 500.f);

    dialogueText.setFont(font);
    dialogueText.setCharacterSize(24);
    dialogueText.setFillColor(sf::Color::White);
    dialogueText.setPosition(260.f, 520.f);
}

void DialogueManager::startDialogue(const std::vector<std::string>& sentences)
{
    while (!dialogueQueue.empty()) dialogueQueue.pop();
    for (const auto& s : sentences) dialogueQueue.push(s);
    isActive = true;
    nextSentence();
}

void DialogueManager::update()
{
    if (!isActive) return;

    if (displayCount < currentMessage.length()) {
        if (typingClock.getElapsedTime().asMilliseconds() > 50) {
            displayCount++;
            typingClock.restart();
        }
    }
    
    dialogueText.setString(currentMessage.substr(0, displayCount));
}

void DialogueManager::draw(sf::RenderWindow& window)
{
    if (!isActive) return;
    window.draw(textBox);
    window.draw(dialogueText);
}

void DialogueManager::nextSentence()
{
    if (dialogueQueue.empty()) {
        isActive = false;
        return;
    }

    currentMessage = dialogueQueue.front();
    dialogueQueue.pop();
    displayCount = 0;
    typingClock.restart();
}
