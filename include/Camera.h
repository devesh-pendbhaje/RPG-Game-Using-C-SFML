#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class Camera
{
private:
    sf::View view;
    float shakeTime;
    float shakeMag;
    float mapW;
    float mapH;

public:
    Camera(float width, float height);

    void setMapBounds(float mw, float mh);
    void shake(float magnitude, float duration);
    void followPlayer(const Player& player);
    void update(float dt);
    void apply(sf::RenderWindow& window);

    sf::Vector2f getCenter() const;
};
