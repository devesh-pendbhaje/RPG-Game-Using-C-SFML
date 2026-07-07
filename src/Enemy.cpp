#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Enemy::Enemy(float x, float y, EType type) : mobType(type), aiTimer(0.f), stateTimer(0.f), isDeadFinished(false), wShootFlag(false)
{
    currentState = PATROL;
    patrolPoint = sf::Vector2f(x + (rand() % 200 - 100), y + (rand() % 200 - 100));

    int fw = 64, fh = 64;
    int idleF = 4, runF = 6, deathF = 4;
    float idleS = 0.15f, runS = 0.1f, deathS = 0.12f;

    switch (mobType)
    {
    case SKELETON:
        health = 60; maxHealth = 60; speed = 2.0f;
        tIdle.loadFromFile("assets_new/skel_idle.png");
        tRun.loadFromFile("assets_new/skel_run.png");
        tDeath.loadFromFile("assets_new/skel_death.png");
        fw = 64; fh = 64; idleF = 6; runF = 6; deathF = 8;
        break;

    case KNIGHT:
        health = 120; maxHealth = 120; speed = 2.4f;
        tIdle.loadFromFile("assets_new/knight_idle.png");
        tRun.loadFromFile("assets_new/knight_run.png");
        tDeath.loadFromFile("assets_new/knight_death.png");
        fw = 32; fh = 32; idleF = 4; runF = 6; deathF = 9;
        break;

    case ROGUE:
        health = 70; maxHealth = 70; speed = 3.8f;
        tIdle.loadFromFile("assets_new/rogue_idle.png");
        tRun.loadFromFile("assets_new/rogue_run.png");
        tDeath.loadFromFile("assets_new/rogue_death.png");
        fw = 32; fh = 32; idleF = 4; runF = 6; deathF = 12;
        break;

    case WIZZARD:
        health = 50; maxHealth = 50; speed = 1.8f;
        tIdle.loadFromFile("assets_new/wizzard_idle.png");
        tRun.loadFromFile("assets_new/wizzard_run.png");
        tDeath.loadFromFile("assets_new/wizzard_death.png");
        fw = 32; fh = 32; idleF = 4; runF = 6; deathF = 12;
        break;

    default: // ORC
        health = 80; maxHealth = 80; speed = 2.8f;
        tIdle.loadFromFile("assets_new/orc_idle.png");
        tRun.loadFromFile("assets_new/orc_run.png");
        tDeath.loadFromFile("assets_new/enemy_run.png");
        fw = 64; fh = 64; idleF = 4; runF = 6; deathF = 4;
        break;
    }

    anim.addAnimation("idle", &tIdle, idleF, idleS);
    anim.addAnimation("run", &tRun, runF, runS);
    anim.addAnimation("death", &tDeath, deathF, deathS, false);

    sprite.setOrigin(fw / 2.f, fh * 0.85f);
    sprite.setScale(3.5f, 3.5f);
    sprite.setPosition(x, y);
    anim.play("idle");
}

void Enemy::update(float dt)
{
    if (health <= 0)
    {
        if (currentState != DEAD) {
            currentState = DEAD;
            anim.play("death");
        }
        
        int fw = (mobType == SKELETON || mobType == ORC) ? 64 : 32;
        anim.update(dt, sprite, fw, fw);
        
        if (anim.isFinished()) isDeadFinished = true;
        return;
    }

    aiTimer += dt;
    stateTimer += dt;

    sf::Vector2f pos = sprite.getPosition();
    float dx = targetPos.x - pos.x;
    float dy = targetPos.y - pos.y;
    float dist = std::sqrt(dx*dx + dy*dy);

    bool moving = false;

    // FSM Logic
    switch (currentState)
    {
    case PATROL:
        {
            float pdx = patrolPoint.x - pos.x;
            float pdy = patrolPoint.y - pos.y;
            float pdist = std::sqrt(pdx*pdx + pdy*pdy);
            if (pdist > 5.f) {
                sprite.move(pdx / pdist * speed * 0.5f, pdy / pdist * speed * 0.5f);
                moving = true;
                if (pdx != 0) sprite.setScale(pdx < 0 ? -3.5f : 3.5f, 3.5f);
            }
            else if (aiTimer > 2.0f) {
                patrolPoint = sf::Vector2f(pos.x + (rand() % 300 - 150), pos.y + (rand() % 300 - 150));
                aiTimer = 0.f;
            }
            if (dist < 400.f) currentState = CHASE;
        }
        break;

    case CHASE:
        {
            moving = true;
            if (dx != 0) sprite.setScale(dx < 0 ? -3.5f : 3.5f, 3.5f);

            if (mobType == WIZZARD) {
                  if (dist < 200.f) {
                      sprite.move(-dx / dist * speed * 60.f * dt, -dy / dist * speed * 60.f * dt);
                  } else if (dist > 300.f) {
                      sprite.move(dx / dist * speed * 60.f * dt, dy / dist * speed * 60.f * dt);
                  }
                  if (dist < 400.f && stateTimer > 2.0f) {
                      currentState = ATTACK; stateTimer = 0.f;
                  }
            } else if (mobType == ROGUE) {
                  if (dist < 150.f && stateTimer > 2.0f) {
                      sprite.move(dx / dist * speed * 300.f * dt, dy / dist * speed * 300.f * dt);
                      if (dist < 50.f) { currentState = ATTACK; stateTimer = 0.f; }
                  } else if (dist < 300.f && stateTimer <= 2.0f) {
                      sprite.move((dy / dist) * speed * 80.f * dt, (-dx / dist) * speed * 80.f * dt);
                  } else {
                      sprite.move(dx / dist * speed * 60.f * dt, dy / dist * speed * 60.f * dt);
                  }
                  if (stateTimer > 2.5f) stateTimer = 0.f;
                  if (dist < 60.f) { currentState = ATTACK; stateTimer = 0.f; }
            } else {
                  sprite.move(dx / dist * speed * 60.f * dt, dy / dist * speed * 60.f * dt);
                  if (dist < 60.f) { currentState = ATTACK; stateTimer = 0.f; }
            }
            if (dist > 800.f) { currentState = PATROL; stateTimer = 0.f; }
        }
        break;

    case ATTACK:
        if (mobType == WIZZARD && anim.getCurrentFrame() == 2 && stateTimer > 0.3f && stateTimer < 0.4f) {
            wShootFlag = true;
        }
        if (stateTimer >= 0.6f) {
            currentState = CHASE;
            stateTimer = 0.f;
            wShootFlag = false;
        }
        break;

    case STAGGER:
        {
            sprite.setColor(sf::Color(255, 100, 100));
            sf::Vector2f dir = pos - targetPos;
            float d = std::sqrt(dir.x*dir.x + dir.y*dir.y);
            if (d > 0.1f) sprite.move(dir.x / d * speed * 2.0f * dt, dir.y / d * speed * 2.0f * dt);
            if (stateTimer > 0.25f) { currentState = CHASE; sprite.setColor(sf::Color::White); }
        }
        break;
    }

    // Animation logic
    if (currentState == CHASE || (currentState == PATROL && moving)) {
        anim.play("run");
    } else if (currentState == ATTACK) {
        // Attack uses run or idle depending on mob, ideally a separate 'slice' anim exists
        anim.play("run"); 
    } else {
        anim.play("idle");
    }

    int fw = (mobType == SKELETON || mobType == ORC) ? 64 : 32;
    anim.update(dt, sprite, fw, fw);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
    if (health <= 0) return;
    sf::Color barColour;
    switch (mobType) {
        case KNIGHT: barColour = {80, 160, 255}; break;
        case ROGUE: barColour = {180, 50, 255}; break;
        case WIZZARD: barColour = {50, 220, 220}; break;
        case SKELETON: barColour = {220, 220, 200}; break;
        default: barColour = {255, 80, 50}; break;
    }
    float barWidth = 50.f;
    sf::RectangleShape bg({barWidth, 5}); bg.setFillColor({30, 30, 30, 180}); bg.setPosition(sprite.getPosition().x - barWidth/2.f, sprite.getPosition().y - 55);
    sf::RectangleShape fg({barWidth * ((float)health / maxHealth), 5}); fg.setFillColor(barColour); fg.setPosition(sprite.getPosition().x - barWidth/2.f, sprite.getPosition().y - 55);
    window.draw(bg); window.draw(fg);
}

void Enemy::takeDamage(int dmg) { 
    if (health <= 0) return;
    health -= dmg; 
    if (health <= 0) {
        currentState = DEAD;
    } else {
        currentState = STAGGER; 
        stateTimer = 0.f; 
    }
}
bool Enemy::isDead() const { return health <= 0; }
void Enemy::setTarget(sf::Vector2f target) { targetPos = target; }
sf::Vector2f Enemy::getPosition() const { return sprite.getPosition(); }
