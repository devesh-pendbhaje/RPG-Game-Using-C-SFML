#include "Camera.h"
#include <cmath>
#include <algorithm>
#include <ctime>

Camera::Camera(float width, float height)
    : shakeTime(0.f), shakeMag(0.f), mapW(0.f), mapH(0.f)
{
    view.setSize(width, height);
    view.setCenter(width / 2.f, height / 2.f);
}

void Camera::setMapBounds(float mw, float mh)
{
    mapW = mw;
    mapH = mh;
}

void Camera::shake(float magnitude, float duration)
{
    shakeMag = magnitude;
    shakeTime = duration;
}

void Camera::followPlayer(const Player& player)
{
    sf::Vector2f target = player.getPosition();
    sf::Vector2f curr = view.getCenter();
    sf::Vector2f next = curr + (target - curr) * 0.12f;

    float hw = view.getSize().x / 2.f;
    float hh = view.getSize().y / 2.f;
    next.x = std::max(hw, std::min(mapW - hw, next.x));
    next.y = std::max(hh, std::min(mapH - hh, next.y));

    view.setCenter(next);
}

void Camera::update(float dt)
{
    if (shakeTime > 0.f)
    {
        shakeTime -= dt;
        float ox = (rand() % 100 / 100.f - 0.5f) * 2.f * shakeMag;
        float oy = (rand() % 100 / 100.f - 0.5f) * 2.f * shakeMag;
        sf::Vector2f c = view.getCenter();
        view.setCenter(c.x + ox, c.y + oy);
        if (shakeTime <= 0.f) shakeTime = 0.f;
    }
}

void Camera::apply(sf::RenderWindow& window)
{
    window.setView(view);
}

sf::Vector2f Camera::getCenter() const
{
    return view.getCenter();
}
