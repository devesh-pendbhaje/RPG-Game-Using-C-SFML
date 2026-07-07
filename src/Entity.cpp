#include "../include/Entity.h"

Entity::Entity()
{
    speed = 2.0f;
    health = 100;
    maxHealth = 100;
}

Entity::~Entity()
{
}

void Entity::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

void Entity::setPosition(float x, float y)
{
    sprite.setPosition(x,y);
}

sf::Vector2f Entity::getPosition() const
{
    return sprite.getPosition();
}

sf::Sprite& Entity::getSprite()
{
    return sprite;
}

int Entity::getHealth() const
{
    return health;
}
