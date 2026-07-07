#pragma once
#include "Entity.h"
#include "Components.h"
#include "AnimationManager.h"

class Player : public Entity
{
public:
    enum State { IDLE, WALKING, ATTACKING, DRINKING, EATING, HARVESTING, CRUSHING };
    enum Direction { DOWN, UP, SIDE };

    // Inventory and Stats instances managed efficiently by composition
    Inventory inventory;
    Stats stats;

private:
    int attackPower;

    sf::Texture tWalkDown, tWalkUp, tWalkSide;
    sf::Texture tIdleDown, tIdleUp, tIdleSide;
    sf::Texture tSliceDown, tSliceUp, tSliceSide;
    sf::Texture tCollectDown, tCollectUp, tCollectSide;
    sf::Texture tCrushDown, tCrushUp, tCrushSide;

    State currentState;
    Direction currentDir;

    AnimationManager anim;

    bool facingLeft;

public:
    Player();

    void handleInput();
    void update(float dt) override;

    int getAttackPower();

    void triggerAttack();
    void triggerEat();
    void triggerDrink();
    void triggerHarvest();
    void triggerCrush();

    State getState() const { return currentState; }
    Direction getDir() const { return currentDir; }
};