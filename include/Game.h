#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "TileMap.h"
#include "CombatSystem.h"
#include "NPC.h"
#include "DialogueManager.h"
#include "SaveManager.h"
#include "MissionSystem.h"
#include "DataLoader.h"

// Forward declarations
struct Particle;
struct Prop;
struct Projectile;

class Game
{
public:
    enum class GameState { MENU, LEVEL_SELECT, INTRO, PLAYING, DIALOGUE, PAUSE, GAMEOVER, VICTORY };

private:
    sf::RenderWindow window;
    std::vector<Enemy> enemies;
    std::vector<NPC> npcs;
    Player           player;
    Camera           camera;
    TileMap          mapGround;
    TileMap          mapWater;
    TileMap          mapRiversideWalls;
    MissionSystem    missionSystem;
    DialogueManager  dialogueManager;

    // Textures
    sf::Texture texFloors, texWaterTiles, texTree, texRock, texVeg, texFarm, texRes, texWalls;
    sf::Texture texHouseWalls, texHouseRoofs, texAnvil, texFurnace, texSawmill, texWorkbench, texBonfire, texFurniture, texTools;
    
    // Props
    std::vector<Prop>   groundProps;
    std::vector<Prop>   overheadProps;
    std::vector<Prop>   pendingProps;

    // Particles
    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<Projectile> projectiles;

    // HUD
    sf::Font            font;
    sf::RectangleShape  playerHpBg, playerHpFg, taskPanelBg, inventoryDashboardBg, levelInfoBarBg;
    sf::RectangleShape  playerHungerBg, playerHungerFg;
    sf::RectangleShape  playerStaminaBg, playerStaminaFg;
    sf::Text            playerHpText, playerHungerText, playerStaminaText, waveText, controlsText, gameOverText, invText;
    sf::Text            taskHeaderText, taskItemText;
    sf::Text            menuTitle, menuOptions[3];
    int                 menuIndex = 0;
    
    // CRAFTING UI
    bool  craftingMenuOpen = false;
    sf::RectangleShape  craftingOverlayBg;
    sf::Text            craftingHeaderText;
    std::vector<RecipeData> loadedRecipes;
    
    // Game state
    GameState currentState = GameState::MENU; // Start at MENU
    bool  gameOver         = false;
    int   wave             = 1;
    int   score            = 0;
    float enemyDmgTimer    = 0.f;

    // Level system
    int   currentLevel       = 1;
    int   maxLevelUnlocked   = 1;
    static constexpr int MAX_LEVEL = 3;
    int   wavesPerLevel[MAX_LEVEL + 1] = {0, 3, 3, 4};
    int   totalEnemiesInWave = 0;
    int   enemiesKilledInWave = 0;
    bool  levelComplete    = false;
    float levelCompleteTimer = 0.f;
    static constexpr float LEVEL_COMPLETE_DELAY = 3.5f;
    sf::Text levelCompleteText, levelText;
    sf::RectangleShape levelCompleteBg;

    // Mission System is now used instead of the legacy task vector

    // Persistent kill/event counters
    int   killCount       = 0;
    bool  builtHouse      = false;
    bool  craftedArmor    = false;
    bool  smelted         = false;
    bool  upgradedWeapon  = false;
    
    // Map constants
    static constexpr int   MAP_COLS   = 80;
    static constexpr int   MAP_ROWS   = 80;
    static constexpr int   TILE_PX    = 16;
    static constexpr float TILE_SCALE = 4.0f;

    static constexpr int T_GRASS = 2;   
    static constexpr int T_DIRT  = 42;  
    static constexpr int T_WATER = 0; 
    static constexpr int T_ROCK  = 125; 
    
    int groundData[MAP_COLS][MAP_ROWS];
    
    static constexpr int TREE_W = 112;
    static constexpr int TREE_H = 184;
    static constexpr float ASSET_SCALE = 2.5f; 

    // Private methods
    void loadAssets();
    void setupHUD();
    void loadLevel(int level, bool isRestoringFromSave = false);
    
    void buildMapLevel1();
    void loadPropsLevel1();
    void buildMapLevel2();
    void loadPropsLevel2();
    void buildMapLevel3();
    void loadPropsLevel3();

    void restorePropsTextures();

    void spawnWave(int w);
    void spawnTree();
    void spawnTreeProcedural(sf::Vector2f playerSpawn);
    void spawnRock();
    void spawnRockProcedural(sf::Vector2f playerSpawn);
    void spawnVeg();
    void spawnBush();
    void spawnFlower();
    void spawnStone();
    void spawnStoneProcedural(sf::Vector2f playerSpawn);
    void spawnFence(sf::Vector2f pos);
    void spawnRuin(sf::Vector2f pos);
    void spawnHouse(sf::Vector2f pos);
    void spawnWell(sf::Vector2f pos);
    void buildFarm();
    void spawnStation(sf::Vector2f pos, int type);
    void spawnFurniture(sf::Vector2f pos);
    void plantCrop();
    void spawnDrop(sf::Vector2f pos, bool iw, bool is, bool ifo, bool iwa, bool ia = false, bool iwl = false, bool isFruit = false, bool im = false);
    
    void onEnemyKilled();
    void onAllWavesCleared();

    void update(float dt);
    void updateFarms(float dt);
    void updateEnemies(float dt);
    void updateHarvesting(float dt);
    void updateCollisions();
    void updateInteractions();
    void interactWithWorld();
    void updateParticles(float dt);
    void spawnAmbientParticles(float dt);
    
    void spawnBlood(sf::Vector2f pos);
    void spawnLeaf(sf::Vector2f pos);
    void spawnHealParticles(sf::Vector2f pos);

    void restartGame();
    void draw();
    void drawHUD();
    void drawMenu();
    void drawMiniMap();

    float T()    const { return TILE_PX * TILE_SCALE; }
    float mapW() const { return MAP_COLS * T(); }
    float mapH() const { return MAP_ROWS * T(); }
    bool  isValidSpawn(float x, float y, float minDist, sf::Vector2f playerSpawn) const;
    bool  isBuildable(float x, float y) const;

public:
    Game();
    void run();
};

// Moving Particle/Prop to their own structs (optionally in separate files later)
struct Particle
{
    sf::CircleShape shape;
    sf::Vector2f    vel;
    float           lifetime;
    float           maxLife;

    Particle(float x, float y, sf::Color col, float r, float vx, float vy, float life);
    bool update(float dt);
};

struct Projectile {
    sf::CircleShape shape;
    sf::Vector2f vel;
    float life;
    int dmg;
    bool active;
    
    Projectile(float x, float y, sf::Vector2f v, float l, int d) 
      : vel(v), life(l), dmg(d), active(true) {
          shape.setRadius(6.f);
          shape.setOrigin(6.f, 6.f);
          shape.setPosition(x, y);
          shape.setFillColor(sf::Color(100, 255, 255));
    }
};


