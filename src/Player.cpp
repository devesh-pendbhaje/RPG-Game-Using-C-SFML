#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

Player::Player()
{
    health = 100;
    maxHealth = 100;
    speed = 3.5f;
    attackPower = 20;
    currentState = IDLE;
    currentDir = DOWN;
    facingLeft = false;
    facingLeft = false;

    // Load new assets
    // Load all directional sheets
    tIdleDown.loadFromFile("assets_new/Idle_Down-Sheet.png");
    tIdleUp.loadFromFile("assets_new/Idle_Up-Sheet.png");
    tIdleSide.loadFromFile("assets_new/Idle_Side-Sheet.png");

    tWalkDown.loadFromFile("assets_new/Walk_Down-Sheet.png");
    tWalkUp.loadFromFile("assets_new/Walk_Up-Sheet.png");
    tWalkSide.loadFromFile("assets_new/Walk_Side-Sheet.png");

    tSliceDown.loadFromFile("assets_new/Slice_Down-Sheet.png");
    tSliceUp.loadFromFile("assets_new/Slice_Up-Sheet.png");
    tSliceSide.loadFromFile("assets_new/Slice_Side-Sheet.png");

    tCollectDown.loadFromFile("assets_new/Collect_Down-Sheet.png");
    tCollectUp.loadFromFile("assets_new/Collect_Up-Sheet.png");
    tCollectSide.loadFromFile("assets_new/Collect_Side-Sheet.png");

    tCrushDown.loadFromFile("assets_new/Crush_Down-Sheet.png");
    tCrushUp.loadFromFile("assets_new/Crush_Up-Sheet.png");
    tCrushSide.loadFromFile("assets_new/Crush_Side-Sheet.png");

    anim.addAnimation("idle_down", &tIdleDown, 6, 0.15f);
    anim.addAnimation("idle_up", &tIdleUp, 6, 0.15f);
    anim.addAnimation("idle_side", &tIdleSide, 6, 0.15f);

    anim.addAnimation("walk_down", &tWalkDown, 6, 0.1f);
    anim.addAnimation("walk_up", &tWalkUp, 6, 0.1f);
    anim.addAnimation("walk_side", &tWalkSide, 6, 0.1f);

    anim.addAnimation("slice_down", &tSliceDown, 6, 0.08f, false);
    anim.addAnimation("slice_up", &tSliceUp, 6, 0.08f, false);
    anim.addAnimation("slice_side", &tSliceSide, 6, 0.08f, false);

    anim.addAnimation("collect_down", &tCollectDown, 8, 0.1f, false);
    anim.addAnimation("collect_up", &tCollectUp, 8, 0.1f, false);
    anim.addAnimation("collect_side", &tCollectSide, 8, 0.1f, false);

    anim.addAnimation("crush_down", &tCrushDown, 8, 0.1f, false);
    anim.addAnimation("crush_up", &tCrushUp, 8, 0.1f, false);
    anim.addAnimation("crush_side", &tCrushSide, 8, 0.1f, false);

    sprite.setOrigin(24, 48); // Set proper base of character center
    sprite.setScale(3.5f, 3.5f);
    sprite.setPosition(400, 300);
    anim.play("idle_down");
}

void Player::triggerAttack()
{
    if (currentState != ATTACKING && currentState != EATING && currentState != DRINKING) {
        currentState = ATTACKING;
    }
}

void Player::triggerEat()
{
    if (currentState != ATTACKING && currentState != EATING && currentState != DRINKING) {
        currentState = EATING;
    }
}

void Player::triggerDrink()
{
    if (currentState == IDLE || currentState == WALKING) {
        currentState = DRINKING;
    }
}

void Player::triggerHarvest()
{
    if (currentState == IDLE || currentState == WALKING) {
        currentState = HARVESTING;
    }
}

void Player::triggerCrush()
{
    if (currentState == IDLE || currentState == WALKING) {
        currentState = CRUSHING;
    }
}

void Player::handleInput()
{
    if (currentState != IDLE && currentState != WALKING) return; 

    sf::Vector2f move(0, 0);
    bool moving = false;
// ... (W, S, A, D logic omitted for brevity here, but included in the tool call)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { move.y -= speed; currentDir = UP; moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { move.y += speed; currentDir = DOWN; moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { move.x -= speed; currentDir = SIDE; moving = true; facingLeft = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { move.x += speed; currentDir = SIDE; moving = true; facingLeft = false; }

    if (moving)
    {
        sprite.move(move);
        currentState = WALKING;
    }
    else
    {
        currentState = IDLE;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && currentState != ATTACKING)
    {
        triggerAttack();
    }
}

void Player::update(float dt)
{
    handleInput();

    std::string prefix = "";
    if (currentState == IDLE) prefix = "idle_";
    else if (currentState == WALKING) prefix = "walk_";
    else if (currentState == ATTACKING) prefix = "slice_";
    else if (currentState == HARVESTING) prefix = "collect_";
    else if (currentState == CRUSHING || currentState == EATING || currentState == DRINKING) prefix = "crush_";

    std::string dirStr = (currentDir == DOWN) ? "down" : (currentDir == UP) ? "up" : "side";
    
    anim.play(prefix + dirStr);
    anim.update(dt, sprite, 64, 64);

    if (anim.isFinished()) {
        currentState = IDLE;
        anim.play("idle_" + dirStr);
    }

    float baseScale = 3.5f;
    if (currentDir == SIDE)
        sprite.setScale(facingLeft ? -baseScale : baseScale, baseScale);
    else
        sprite.setScale(baseScale, baseScale);

    sprite.setColor(sf::Color::White);
    stats.updateSurvival(dt);
}

int Player::getAttackPower()
{
    if (currentState == ATTACKING && anim.getCurrentFrame() == 3) return attackPower;
    if (currentState == HARVESTING && anim.getCurrentFrame() == 4) return attackPower / 2;
    if (currentState == CRUSHING && anim.getCurrentFrame() == 4) return attackPower / 2;
    return 0;
}
