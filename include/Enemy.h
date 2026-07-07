#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "AnimationManager.h"

class Enemy : public Entity
{
public:
    enum EType { ORC, SKELETON, KNIGHT, ROGUE, WIZZARD };
    enum EState { IDLE, PATROL, CHASE, ATTACK, STAGGER, DEAD };

private:
    EType mobType;
    sf::Vector2f targetPos;
    sf::Vector2f patrolPoint;
    float aiTimer;
    float stateTimer;

    sf::Texture tIdle, tRun, tHit, tDeath;
    EState currentState;

    AnimationManager anim;
    bool isDeadFinished;
    bool wShootFlag;

public:
    Enemy(float x = 500.f, float y = 500.f, EType type = ORC);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    void takeDamage(int dmg);
    bool isDead() const;
    bool isAnimFinished() const { return isDeadFinished; }
    
    bool popShootRequest() { if (wShootFlag) { wShootFlag = false; return true; } return false; }

    void setTarget(sf::Vector2f target);
    sf::Vector2f getPosition() const;
    EType getType() const { return mobType; }
    EState getState() const { return currentState; }
    int getAnimFrame() const { return anim.getCurrentFrame(); }
};