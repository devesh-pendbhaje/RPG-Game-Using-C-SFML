#pragma once
#include <SFML/Graphics.hpp>

class Entity
{
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

public:
    int health;
    int maxHealth;

    Entity();
    virtual ~Entity();

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow &window);

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    sf::Sprite& getSprite();
    int getHealth() const;
};