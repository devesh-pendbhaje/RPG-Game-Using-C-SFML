#pragma once
#include "Player.h"
#include "Enemy.h"

// Pure utility class – demonstrates static utility (OOP: abstraction)
class CombatSystem
{
public:
    static void playerAttack(Player& player, Enemy& enemy)
    {
        int dmg = player.getAttackPower();
        if (dmg > 0)
            enemy.takeDamage(dmg);
    }
};
