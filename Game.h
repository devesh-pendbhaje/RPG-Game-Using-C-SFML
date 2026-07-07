#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <iostream>

#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "TileMap.h"
#include "CombatSystem.h"

// ============================================================
//  Particle
// ============================================================
struct Particle
{
    sf::CircleShape shape;
    sf::Vector2f    vel;
    float           lifetime;
    float           maxLife;

    Particle(float x, float y, sf::Color col, float r, float vx, float vy, float life)
        : vel(vx, vy), lifetime(life), maxLife(life)
    {
        shape.setRadius(r);
        shape.setOrigin(r, r);
        shape.setPosition(x, y);
        shape.setFillColor(col);
    }

    bool update(float dt)
    {
        lifetime -= dt;
        shape.move(vel * dt);
        float alpha = std::max(0.f, lifetime / maxLife);
        sf::Color c = shape.getFillColor();
        c.a = (sf::Uint8)(alpha * 255);
        shape.setFillColor(c);
        return lifetime > 0.f;
    }
};

// ============================================================
//  Prop
// ============================================================
struct Prop
{
    sf::Sprite    sprite;
    sf::FloatRect hitbox;
    bool          solid = false;
    float         hp = 100.0f;
    bool          isTree = false, isRock = false, isStone = false, isWood = false, isFood = false, isWater = false, isArmor = false, isWeaponLevel = false, isFarm = false, isHouse = false, isWell = false, isBush = false, isFlower = false, isFence = false, isStation = false, isFurniture = false, isMetal = false, isStump = false;
    int           stationType = 0, variant = 0, growthStage = 0, animF = 0;
    float         growthTimer = 0.f, animT = 0.f;

    void setBaseOrigin(float w, float h, float bottomOffset = 16.f) {
        sprite.setOrigin(w / 2.f, h - bottomOffset);
    }
};

// ============================================================
//  Game
// ============================================================
class Game
{
private:
    sf::RenderWindow window;
    Player           player;
    std::vector<Enemy> enemies;
    Camera           camera;
    TileMap          mapGround;
    TileMap          mapWater;
    TileMap          mapRiversideWalls;

    // Textures --------------------------------------------
    sf::Texture texFloors, texWaterTiles, texTree, texRock, texVeg, texFarm, texRes, texWalls;
    sf::Texture texHouseWalls, texHouseRoofs, texAnvil, texFurnace, texSawmill, texWorkbench, texBonfire, texFurniture, texTools;
    
    // Props -----------------------------------------------
    std::vector<Prop>   groundProps;
    std::vector<Prop>   overheadProps;

    // Particles -------------------------------------------
    std::vector<Particle> particles;

    // HUD -------------------------------------------------
    sf::Font            font;
    sf::RectangleShape  playerHpBg, playerHpFg;
    sf::Text            playerHpText, waveText, controlsText, gameOverText, invText;
    std::vector<Prop>   pendingProps;
    
    // Game state ------------------------------------------
    bool  gameOver         = false;
    int   wave             = 1;   // wave within current level
    int   score            = 0;
    float enemyDmgTimer    = 0.f;

    // Level system ----------------------------------------
    int   currentLevel     = 1;   // 1=Village, 2=Forest, 3=Dungeon
    static constexpr int MAX_LEVEL = 3;
    int   wavesPerLevel[MAX_LEVEL + 1] = {0, 3, 3, 4}; // index 0 unused
    bool  levelComplete    = false;
    float levelCompleteTimer = 0.f;
    static constexpr float LEVEL_COMPLETE_DELAY = 3.5f;
    sf::Text levelCompleteText, levelText;
    sf::RectangleShape levelCompleteBg;

    // Task system -----------------------------------------
    struct Task {
        std::string label;      // Display name
        int         goal;       // Target value
        int         current;    // Current progress
        bool        done;       // Completed flag
        Task(const std::string& l, int g) : label(l), goal(g), current(0), done(false) {}
        void progress(int amount = 1) { if (!done) { current = std::min(current + amount, goal); done = (current >= goal); } }
        bool isDone() const { return done; }
    };
    std::vector<Task>  tasks;
    sf::Text           taskHeaderText;
    sf::RectangleShape taskPanelBg;
    sf::Text           taskItemText;   // reused for each task line

    // Persistent kill/event counters reset per-level
    int   killCount       = 0;
    bool  builtHouse      = false;
    bool  craftedArmor    = false;
    bool  smelted         = false;
    bool  upgradedWeapon  = false;
    
    // Map constants ---------------------------------------
    static constexpr int   MAP_COLS   = 80;
    static constexpr int   MAP_ROWS   = 80;
    static constexpr int   TILE_PX    = 16;
    static constexpr float TILE_SCALE = 4.0f;

    // Enhanced Tile Indices (Safest choices for floors.png)
    static constexpr int T_GRASS = 2;   
    static constexpr int T_DIRT  = 42;  
    static constexpr int T_WATER = 0; 
    static constexpr int T_ROCK  = 125; 
    
    // HQ Tree Constants (448x368 sheet: 4 variants, 2 rows)
    static constexpr int TREE_W = 112;
    static constexpr int TREE_H = 184;
    static constexpr float ASSET_SCALE = 2.5f; 

    // Map helper methods
    float T()    const { return TILE_PX * TILE_SCALE; }
    float mapW() const { return MAP_COLS * T(); }
    float mapH() const { return MAP_ROWS * T(); }

public:
    Game()
        : window(sf::VideoMode(1280, 720), "Survival RPG - Pixel Crawler Edition",
                 sf::Style::Titlebar | sf::Style::Close),
          camera(1280.f, 720.f)
    {
        srand((unsigned)time(nullptr));
        window.setFramerateLimit(60);

        loadAssets();
        setupHUD();
        loadLevel(currentLevel);
    }

    void loadAssets()
    {
        if(!texFloors.loadFromFile("assets_new/floors.png")) std::cerr<<"Err Floors"<<std::endl;
        if(!texWaterTiles.loadFromFile("assets_new/water.png")) std::cerr<<"Err Water"<<std::endl;
        if(!texTree.loadFromFile("assets_new/tree_hq.png")) std::cerr<<"Err Tree"<<std::endl;
        if(!texRock.loadFromFile("assets_new/rocks.png")) std::cerr<<"Err Rock"<<std::endl;
        if(!texVeg.loadFromFile("assets_new/vegetation.png")) std::cerr<<"Err Veg"<<std::endl;
        if(!texFarm.loadFromFile("assets_new/farm.png")) std::cerr<<"Err Farm"<<std::endl;
        if(!texRes.loadFromFile("assets_new/resources.png")) std::cerr<<"Err Res"<<std::endl;
        if(!texHouseWalls.loadFromFile("assets_new/house_walls.png")) std::cerr<<"Err HouseWalls"<<std::endl;
        if(!texHouseRoofs.loadFromFile("assets_new/house_roofs.png")) std::cerr<<"Err HouseRoofs"<<std::endl;
        if(!texWalls.loadFromFile("assets_new/walls_tile.png")) std::cerr<<"Err WallsTile"<<std::endl;
        
        if(!texAnvil.loadFromFile("assets_new/anvil.png")) std::cerr<<"Err Anvil"<<std::endl;
        if(!texFurnace.loadFromFile("assets_new/furnace.png")) std::cerr<<"Err Furnace"<<std::endl;
        if(!texSawmill.loadFromFile("assets_new/sawmill.png")) std::cerr<<"Err Sawmill"<<std::endl;
        if(!texWorkbench.loadFromFile("assets_new/workbench.png")) std::cerr<<"Err Workbench"<<std::endl;
        if(!texBonfire.loadFromFile("assets_new/bonfire.png")) std::cerr<<"Err Bonfire"<<std::endl;
        if(!texFurniture.loadFromFile("assets_new/furniture.png")) std::cerr<<"Err Furniture"<<std::endl;
        if(!texTools.loadFromFile("assets_new/tools.png")) std::cerr<<"Err Tools"<<std::endl;
        
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }



    void spawnTree()
    {
        Prop p;
        p.sprite.setTexture(texTree);
        p.variant = rand() % 4;
        p.sprite.setTextureRect({p.variant * TREE_W, 0, TREE_W, TREE_H});
        p.sprite.setOrigin(TREE_W / 2, TREE_H - 16);
        p.sprite.setScale(2.0f, 2.0f);
        float x = (float)(300 + rand() % (int)(mapW() - 600));
        float y = (float)(300 + rand() % (int)(mapH() - 600));
        p.sprite.setPosition(x, y);
        p.isTree = true; p.hp = 100; p.solid = true; p.isStump = false;
        p.hitbox = {x - 40, y - 30, 80, 60};
        groundProps.push_back(p);
    }

    void spawnRock()
    {
        Prop p;
        p.sprite.setTexture(texRock);
        p.sprite.setTextureRect({0, 0, 32, 32}); // First rock
        p.sprite.setOrigin(16, 16);
        p.sprite.setScale(3.0f, 3.0f);
        float x = (float)(rand() % (int)mapW());
        float y = (float)(rand() % (int)mapH());
        p.sprite.setPosition(x, y);
        p.isRock = true; p.hp = 80; p.solid = true;
        p.hitbox = {x - 30, y - 20, 60, 40};
        groundProps.push_back(p);
    }

    void spawnVeg()
    {
        Prop p;
        p.sprite.setTexture(texVeg);
        int tx = (rand() % 8) * 16;
        int ty = (rand() % 8) * 16;
        p.sprite.setTextureRect({tx, ty, 16, 16});
        p.sprite.setOrigin(8, 8);
        p.sprite.setScale(TILE_SCALE, TILE_SCALE);
        float x = (float)(rand() % (int)mapW());
        float y = (float)(rand() % (int)mapH());
        p.sprite.setPosition(x, y);
        groundProps.push_back(p);
    }

    void spawnBush()
    {
        Prop p;
        p.sprite.setTexture(texVeg);
        p.sprite.setTextureRect({0, 128, 32, 32}); // Larger bush
        p.sprite.setOrigin(16, 24);
        p.sprite.setScale(2.5f, 2.5f);
        float x = (float)(rand() % (int)mapW());
        float y = (float)(rand() % (int)mapH());
        p.sprite.setPosition(x, y);
        p.isBush = true; p.hp = 30; p.solid = true;
        p.hitbox = {x - 30, y - 20, 60, 40};
        groundProps.push_back(p);
    }

    void spawnFlower()
    {
        Prop p;
        p.sprite.setTexture(texVeg);
        int type = rand() % 3;
        p.sprite.setTextureRect({128 + type*16, 0, 16, 16}); // Colorful flowers
        p.sprite.setOrigin(8, 8);
        p.sprite.setScale(3.0f, 3.0f);
        float x = (float)(rand() % (int)mapW());
        float y = (float)(rand() % (int)mapH());
        p.sprite.setPosition(x, y);
        p.isFlower = true;
        groundProps.push_back(p);
    }

    void spawnFence(sf::Vector2f pos)
    {
        Prop p;
        p.sprite.setTexture(texWalls);
        p.sprite.setTextureRect({16, 128, 16, 16}); // Wooden fence tile
        p.sprite.setOrigin(8, 8);
        p.sprite.setScale(TILE_SCALE, TILE_SCALE);
        p.sprite.setPosition(pos);
        p.isFence = true; p.solid = true;
        p.hitbox = {pos.x - 32, pos.y - 32, 64, 64};
        groundProps.push_back(p);
    }

    void spawnRuin(sf::Vector2f pos)
    {
        Prop p;
        p.sprite.setTexture(texWalls);
        p.sprite.setTextureRect({0, 0, 48, 48}); // Broken wall section
        p.sprite.setOrigin(24, 24);
        p.sprite.setScale(TILE_SCALE, TILE_SCALE);
        p.sprite.setPosition(pos);
        p.solid = true;
        p.hitbox = {pos.x - 80, pos.y - 80, 160, 160};
        groundProps.push_back(p);
    }

    void setupHUD()
    {
        playerHpBg.setSize({250.f, 25.f}); playerHpBg.setFillColor({50,50,50,200}); playerHpBg.setPosition(30, 30);
        playerHpFg.setSize({250.f, 25.f}); playerHpFg.setFillColor({80,255,80,220}); playerHpFg.setPosition(30,30);

        auto setupText = [&](sf::Text& t, int size, sf::Color col, float y) {
            t.setFont(font); t.setCharacterSize(size); t.setFillColor(col); t.setPosition(30, y);
            t.setOutlineThickness(2); t.setOutlineColor(sf::Color::Black);
        };
        setupText(playerHpText,  16, sf::Color::White,         33);
        setupText(waveText,      24, sf::Color::Cyan,           65);
        setupText(levelText,     20, sf::Color(255, 165, 0),    95);
        setupText(invText,       18, sf::Color(255,200,100),   120);
        setupText(controlsText,  13, sf::Color::White,         530);
        controlsText.setString("WASD: Move | SPACE: Harvest/Mine | F: Plant | V: Farm (20W,10S)\n"
                               "E: Eat | Q: Drink | B: Build House (50W,30S) | C: Craft Armor (40S)\n"
                               "G: Upgrade Weapon (60W,40S) | Use Furnace/Anvil: SPACE near station");

        setupText(gameOverText, 60, sf::Color::Red, 280);
        gameOverText.setString("SURVIVAL ENDED\nPress R to Restart");
        sf::FloatRect gb = gameOverText.getLocalBounds();
        gameOverText.setOrigin(gb.width/2, gb.height/2);
        gameOverText.setPosition(640, 360);

        // Level complete overlay
        levelCompleteBg.setSize({1280.f, 720.f});
        levelCompleteBg.setFillColor({0, 0, 0, 170});
        levelCompleteBg.setPosition(0, 0);
        levelCompleteText.setFont(font);
        levelCompleteText.setCharacterSize(52);
        levelCompleteText.setFillColor(sf::Color(255, 220, 50));
        levelCompleteText.setOutlineThickness(4);
        levelCompleteText.setOutlineColor(sf::Color::Black);

        // Task panel (right side)
        taskPanelBg.setSize({310.f, 200.f});
        taskPanelBg.setFillColor(sf::Color(0, 0, 0, 160));
        taskPanelBg.setOutlineColor(sf::Color(255, 165, 0, 160));
        taskPanelBg.setOutlineThickness(2);
        taskPanelBg.setPosition(940, 20);

        taskHeaderText.setFont(font);
        taskHeaderText.setCharacterSize(17);
        taskHeaderText.setFillColor(sf::Color(255, 200, 50));
        taskHeaderText.setOutlineThickness(1);
        taskHeaderText.setOutlineColor(sf::Color::Black);
        taskHeaderText.setPosition(950, 28);

        taskItemText.setFont(font);
        taskItemText.setCharacterSize(15);
        taskItemText.setOutlineThickness(1);
        taskItemText.setOutlineColor(sf::Color::Black);
    }

    // ============================================================
    //  Task helpers
    // ============================================================
    void setupTasksForLevel(int level)
    {
        tasks.clear();
        killCount = 0; builtHouse = false; craftedArmor = false;
        smelted = false; upgradedWeapon = false;

        if (level == 1) {
            // Level 1 – The Village
            tasks.emplace_back("Survive all 3 waves",    1);   // "done" set by wave logic
            tasks.emplace_back("Collect 20 Wood",        20);
            tasks.emplace_back("Collect 15 Stone",       15);
            tasks.emplace_back("Build a House  [B]",     1);
        } else if (level == 2) {
            // Level 2 – Dark Forest
            tasks.emplace_back("Survive all 3 waves",    1);
            tasks.emplace_back("Collect 10 Food",        10);
            tasks.emplace_back("Kill 30 Enemies",        30);
            tasks.emplace_back("Upgrade Weapon  [G]",    1);
        } else {
            // Level 3 – The Dungeon
            tasks.emplace_back("Survive the Boss Wave",  1);
            tasks.emplace_back("Kill 50 Enemies",        50);
            tasks.emplace_back("Smelt 5 Metal  [Space+Furnace]", 5);
            tasks.emplace_back("Craft Armor  [C or Anvil]",      1);
        }
    }

    // Call whenever an enemy is killed
    void onEnemyKilled()
    {
        killCount++;
        // Task index 2 = kill task for levels 2 and 3
        if (currentLevel == 2 && tasks.size() > 2) tasks[2].progress();
        if (currentLevel == 3 && tasks.size() > 1) tasks[1].progress();
    }

    // Wave cleared – mark wave-survival task done when final wave completes
    void onAllWavesCleared()
    {
        if (!tasks.empty()) tasks[0].progress();
    }

    bool allTasksDone() const
    {
        for (auto& t : tasks) if (!t.isDone()) return false;
        return true;
    }

    // ============================================================
    //  Level Loading
    // ============================================================
    void loadLevel(int level)
    {
        groundProps.clear(); overheadProps.clear(); enemies.clear(); particles.clear();
        wave = 1;
        levelComplete = false;

        if (level == 1) {
            buildMapLevel1();
            loadPropsLevel1();
            player.setPosition(3500, 2400);
        } else if (level == 2) {
            buildMapLevel2();
            loadPropsLevel2();
            player.setPosition(600, 600);
        } else if (level == 3) {
            buildMapLevel3();
            loadPropsLevel3();
            player.setPosition(400, 400);
        }

        setupTasksForLevel(level);
        camera.setMapBounds(mapW(), mapH());
        camera.followPlayer(player);
        spawnWave(wave);
    }

    // ---- Level 1: The Village (original map) ----
    void buildMapLevel1()
    {
        std::vector<int> groundTiles(MAP_COLS * MAP_ROWS, T_GRASS);
        std::vector<int> waterTiles(MAP_COLS * MAP_ROWS, -1);
        std::vector<int> riverWallTiles(MAP_COLS * MAP_ROWS, -1);

        for (int y = 0; y < MAP_ROWS; y++) {
            for (int x = 0; x < MAP_COLS; x++) {
                float riverX = 10 + 3 * std::sin(y * 0.15f) + 2 * std::sin(y * 0.4f);
                if (x >= riverX - 3 && x <= riverX + 1) {
                    waterTiles[y * MAP_COLS + x] = 16;
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }
                // Create brown dirt border on left bank
                if (x >= (int)riverX - 6 && x < (int)riverX - 3) {
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }
                // Create brown dirt border on right bank
                if (x > (int)riverX + 1 && x <= (int)riverX + 4) {
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }
                if (x >= riverX + 2 && x <= riverX + 4)
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                if ((x >= 8 && x <= 45 && y == 8) || (x == 8 && y >= 8 && y <= 25) || (x == 35 && y >= 8 && y <= 30))
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                if (x > 30 && x < 45 && y > 20 && y < 35)
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                if (x < 15 && y > 45 && (x+y) > 65)
                    groundTiles[y * MAP_COLS + x] = 125;
                if (x < 3 || x >= MAP_COLS - 3 || y < 3 || y >= MAP_ROWS - 3)
                    groundTiles[y * MAP_COLS + x] = T_ROCK;
            }
        }
        mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
        mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
        mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
        mapGround.setScale(TILE_SCALE, TILE_SCALE);
        mapWater.setScale(TILE_SCALE, TILE_SCALE);
        mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
    }

    void loadPropsLevel1()
    {
        for (int i = 0; i < 200; i++) spawnTree();
        for (int i = 0; i < 150; i++) spawnRock();
        for (int i = 0; i < 300; i++) spawnVeg();
        for (int i = 0; i < 100; i++) spawnBush();
        for (int i = 0; i < 150; i++) spawnFlower();
        spawnHouse({3200, 2000}); spawnHouse({3800, 2000}); spawnHouse({3200, 2800});
        spawnWell({3500, 2400});
        spawnStation({3000, 2400}, 0); spawnStation({2900, 2400}, 1);
        spawnStation({3100, 2400}, 3); spawnStation({3000, 2600}, 2);
        for(int x=2800; x<4200; x+=64) spawnFence({(float)x, 1800});
        for(int y=1800; y<3200; y+=64) spawnFence({2800.f, (float)y});
        spawnRuin({6000, 1000}); spawnRuin({500, 5000});
        for(int i=0; i<10; i++) spawnFurniture({(float)(3000+rand()%800), (float)(2000+rand()%600)});
    }

    // ---- Level 2: The Dark Forest ----
    void buildMapLevel2()
    {
        std::vector<int> groundTiles(MAP_COLS * MAP_ROWS, T_GRASS);
        std::vector<int> waterTiles(MAP_COLS * MAP_ROWS, -1);
        std::vector<int> riverWallTiles(MAP_COLS * MAP_ROWS, -1);

        for (int y = 0; y < MAP_ROWS; y++) {
            for (int x = 0; x < MAP_COLS; x++) {
                // Swampy patches
                float swampNoise = std::sin(x * 0.3f) * std::cos(y * 0.25f);
                if (swampNoise > 0.5f && x > 10 && y > 10)
                    groundTiles[y * MAP_COLS + x] = T_DIRT;

                // Two winding rivers
                float r1 = 15 + 4 * std::sin(y * 0.12f);
                float r2 = 55 + 3 * std::cos(y * 0.18f);
                if ((x >= r1 - 2 && x <= r1 + 2) || (x >= r2 - 2 && x <= r2 + 2)) {
                    waterTiles[y * MAP_COLS + x] = 16;
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }
                // Create brown dirt borders around both rivers
                if ((x >= (int)r1 - 5 && x < (int)r1 - 2) || (x > (int)r1 + 2 && x <= (int)r1 + 5)) {
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }
                if ((x >= (int)r2 - 5 && x < (int)r2 - 2) || (x > (int)r2 + 2 && x <= (int)r2 + 5)) {
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }

                // Rocky clearings
                if ((x > 30 && x < 40 && y > 30 && y < 40) ||
                    (x > 55 && x < 65 && y > 50 && y < 60))
                    groundTiles[y * MAP_COLS + x] = T_ROCK;

                // Border
                if (x < 3 || x >= MAP_COLS - 3 || y < 3 || y >= MAP_ROWS - 3)
                    groundTiles[y * MAP_COLS + x] = T_ROCK;
            }
        }
        mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
        mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
        mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
        mapGround.setScale(TILE_SCALE, TILE_SCALE);
        mapWater.setScale(TILE_SCALE, TILE_SCALE);
        mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
        mapGround.setColor(sf::Color(160, 200, 160)); // Darker forest tint
    }

    void loadPropsLevel2()
    {
        // Very dense forest
        for (int i = 0; i < 400; i++) spawnTree();
        for (int i = 0; i < 250; i++) spawnRock();
        for (int i = 0; i < 200; i++) spawnVeg();
        for (int i = 0; i < 200; i++) spawnBush();
        for (int i = 0; i < 80; i++) spawnFlower();
        // A lone ruined outpost
        spawnRuin({3000, 1500}); spawnRuin({4500, 3000}); spawnRuin({1500, 4500});
        // An old campfire / bonfire station
        spawnStation({3500, 3500}, 0); // Anvil deep in the forest
        // Some fences (old abandoned village)
        for(int x=3200; x<3800; x+=64) spawnFence({(float)x, 3200});
        for(int y=3200; y<3700; y+=64) spawnFence({3200.f, (float)y});
    }

    // ---- Level 3: The Dungeon ----
    void buildMapLevel3()
    {
        std::vector<int> groundTiles(MAP_COLS * MAP_ROWS, T_ROCK); // Stone floor
        std::vector<int> waterTiles(MAP_COLS * MAP_ROWS, -1);
        std::vector<int> riverWallTiles(MAP_COLS * MAP_ROWS, -1);

        for (int y = 0; y < MAP_ROWS; y++) {
            for (int x = 0; x < MAP_COLS; x++) {
                // Room-like clear areas using a grid pattern
                bool inRoom = ((x / 10) % 2 == 0) && ((y / 10) % 2 == 0) &&
                              x > 5 && x < MAP_COLS - 5 && y > 5 && y < MAP_ROWS - 5;
                // Corridors
                bool corridor = (x % 10 == 5 && y > 5 && y < MAP_ROWS - 5) ||
                                (y % 10 == 5 && x > 5 && x < MAP_COLS - 5);

                if (inRoom || corridor)
                    groundTiles[y * MAP_COLS + x] = T_DIRT; // Paths use dirt
                else
                    groundTiles[y * MAP_COLS + x] = T_ROCK;

                // Lava pools (pure aesthetic via water layer with distinct tile)
                if (x > 35 && x < 40 && y > 35 && y < 40) {
                    waterTiles[y * MAP_COLS + x] = 16; // Reuse water tile (orange tinted)
                    groundTiles[y * MAP_COLS + x] = T_ROCK;
                }
                // Create brown dirt border around lava
                if ((x >= 33 && x <= 41 && y >= 33 && y <= 41) && 
                    !((x > 35 && x < 40) && (y > 35 && y < 40))) {
                    groundTiles[y * MAP_COLS + x] = T_DIRT;
                }

                // Borders are solid rock
                if (x < 2 || x >= MAP_COLS - 2 || y < 2 || y >= MAP_ROWS - 2)
                    groundTiles[y * MAP_COLS + x] = T_ROCK;
            }
        }
        mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
        mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
        mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
        mapGround.setScale(TILE_SCALE, TILE_SCALE);
        mapWater.setScale(TILE_SCALE, TILE_SCALE);
        mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
        mapWater.setColor(sf::Color(255, 80, 0, 200)); // Lava-orange tint
        mapGround.setColor(sf::Color(200, 190, 180)); // Dungeon stone tint
    }

    void loadPropsLevel3()
    {
        // Only rocks and ruins in the dungeon
        for (int i = 0; i < 80; i++) spawnRock();
        // Wall segments in corridors
        for (int i = 0; i < 60; i++) {
            float x = (float)(200 + rand() % (int)(mapW() - 400));
            float y = (float)(200 + rand() % (int)(mapH() - 400));
            spawnRuin({x, y});
        }
        // A few crafting stations in the dungeon
        spawnStation({2000, 2000}, 1); // Furnace
        spawnStation({2200, 2000}, 0); // Anvil
    }

    void spawnWave(int w)
    {
        // Count = more per level (higher base, steeper scaling)
        int baseCount = 10 + w * 5 + (currentLevel - 1) * 8;

        // Boss wave in dungeon: double the enemy count
        bool isBossWave = (currentLevel == 3 && w == wavesPerLevel[currentLevel]);
        if (isBossWave) baseCount *= 2;

        for (int i = 0; i < baseCount; i++) {
            float x = (rand() % 2 == 0) ? 100.f : mapW() - 100.f;
            float y = (float)(100 + rand() % (int)(mapH() - 200));
            Enemy::EType type;

            if (currentLevel == 1) {
                // Village: Orcs & Skeletons
                int r = rand() % 2;
                type = (r == 0) ? Enemy::ORC : Enemy::SKELETON;

            } else if (currentLevel == 2) {
                // Dark Forest: Knight & Rogue ambushers, some Orcs
                int r = rand() % 5;
                if (r <= 1) type = Enemy::KNIGHT;
                else if (r == 2) type = Enemy::ROGUE;
                else type = Enemy::ORC;

            } else {
                // Dungeon: Wizzards & Skeletons; boss wave adds Knights+Rogues
                if (isBossWave) {
                    int r = rand() % 4;
                    if (r == 0) type = Enemy::KNIGHT;
                    else if (r == 1) type = Enemy::ROGUE;
                    else if (r == 2) type = Enemy::WIZZARD;
                    else type = Enemy::SKELETON;
                } else {
                    int r = rand() % 3;
                    if (r == 0) type = Enemy::WIZZARD;
                    else type = Enemy::SKELETON;
                }
            }

            enemies.emplace_back(x, y, type);
        }
    }

    void update(float dt)
    {
        if (gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) restartGame();
            return;
        }

        // Level-complete transition
        if (levelComplete) {
            levelCompleteTimer -= dt;
            if (levelCompleteTimer <= 0.f) {
                levelComplete = false;
                if (currentLevel < MAX_LEVEL) {
                    currentLevel++;
                    loadLevel(currentLevel);
                } else {
                    // All levels beaten — victory = game over with win flag
                    gameOver = true;
                    gameOverText.setString("YOU WIN! All Levels Clear!\nPress R to Restart");
                    gameOverText.setFillColor(sf::Color::Yellow);
                }
            }
            return;
        }

        player.update();
        camera.update(dt);
        camera.followPlayer(player);

        // Movement bounds
        sf::Vector2f pp = player.getPosition();
        pp.x = std::max(0.f, std::min(mapW(), pp.x));
        pp.y = std::max(0.f, std::min(mapH(), pp.y));
        player.setPosition(pp.x, pp.y);

        updateFarms(dt);
        updateEnemies(dt);
        updateHarvesting(dt);
        updateCollisions();
        updateInteractions();
        updateParticles(dt);
        spawnAmbientParticles(dt);

        if (player.health <= 0) gameOver = true;

        // Wave / level progression logic
        int liveEnemies = 0;
        for(auto& e : enemies) if(!e.isDead()) liveEnemies++;

        if (liveEnemies == 0 && !levelComplete) {
            if (wave >= wavesPerLevel[currentLevel]) {
                // Final wave cleared
                onAllWavesCleared();

                // Also auto-complete resource/kill tasks based on current state
                // (some players may have done them already; this syncs)
                if (currentLevel == 1) {
                    if (player.inv.wood  >= 20 && tasks.size() > 1) tasks[1].done = (tasks[1].current = std::min(player.inv.wood,  20)) >= 20;
                    if (player.inv.stone >= 15 && tasks.size() > 2) tasks[2].done = (tasks[2].current = std::min(player.inv.stone, 15)) >= 15;
                    if (builtHouse      && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                } else if (currentLevel == 2) {
                    if (upgradedWeapon && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                } else {
                    if (smelted        && tasks.size() > 2) tasks[2].done = (tasks[2].current >= 5);
                    if (craftedArmor   && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                }

                if (allTasksDone()) {
                    // All tasks AND waves done — level complete!
                    levelComplete = true;
                    levelCompleteTimer = LEVEL_COMPLETE_DELAY;
                    if (currentLevel < MAX_LEVEL)
                        levelCompleteText.setString("LEVEL " + std::to_string(currentLevel) + " COMPLETE!\n\nEntering Level " + std::to_string(currentLevel + 1) + "...");
                    else
                        levelCompleteText.setString("ALL LEVELS COMPLETE!\n\nYOU ARE A LEGEND!");
                } else {
                    // Tasks not done: spawn a bonus wave so player has time
                    spawnWave(wave); // replay final wave as bonus pressure
                }
            } else {
                wave++;
                spawnWave(wave);
            }
        }
    }

    void updateHarvesting(float dt)
    {
        int attackDmg = player.getAttackPower();
        if (attackDmg <= 0) return;

        sf::FloatRect hitArea = player.getSprite().getGlobalBounds();
        hitArea.left -= 60; hitArea.top -= 60; hitArea.width += 120; hitArea.height += 120;

        for (auto& p : groundProps) {
            if (hitArea.intersects(p.sprite.getGlobalBounds())) {
                if (p.isTree && !p.isStump) {
                    player.triggerHarvest();
                    p.hp -= 20; spawnLeaf(p.sprite.getPosition());
                    p.sprite.setColor(sf::Color(200, 100, 100)); // Hit flash
                    if(p.hp <= 0) {
                        spawnDrop(p.sprite.getPosition(), true, false, false, false);
                        p.isStump = true; p.solid = false; 
                        p.sprite.setTextureRect({p.variant * TREE_W, TREE_H, TREE_W, TREE_H});
                        p.sprite.setColor(sf::Color::White);
                    }
                }
                if (p.isRock) {
                    player.triggerCrush();
                    p.hp -= 20; p.sprite.setColor(sf::Color(200, 100, 100));
                    if(p.hp <= 0) {
                        spawnDrop(p.sprite.getPosition(), false, true, false, false);
                        p.isRock = false; p.solid = false; p.sprite.setColor({50,50,50,100});
                    }
                }
                if (p.isBush) {
                    player.triggerHarvest();
                    p.hp -= 15; spawnLeaf(p.sprite.getPosition());
                    if(p.hp <= 0) {
                        spawnDrop(p.sprite.getPosition(), false, false, true, false, false, false, true);
                        p.isBush = false; p.solid = false; p.sprite.setColor({80,120,80,100});
                    }
                }
            }
        }
        
        // Push pending items safely
        for(auto& dp : pendingProps) groundProps.push_back(dp);
        pendingProps.clear();
    }

    void updateFarms(float dt)
    {
        for (auto& p : groundProps) {
            if (p.isFarm && p.growthStage < 2) {
                p.growthTimer += dt;
                if (p.growthTimer > 8.0f) {
                    p.growthStage++; p.growthTimer = 0.f;
                    if (p.growthStage == 1) p.sprite.setTextureRect({16, 16, 16, 16}); 
                    else p.sprite.setTextureRect({48, 16, 16, 16}); 
                }
            }
            if (p.isStation) {
                p.animT += dt;
                float animSpeed = 0.12f;
                int maxF = 4;
                int size = 32;
                if (p.stationType == 2) { size = 64; maxF = 8; } // Sawmill
                if (p.stationType == 1) { size = 32; maxF = 6; } // Furnace
                if (p.stationType == 0) { size = 32; maxF = 6; } // Anvil

                if(p.animT > animSpeed) {
                    p.animF = (p.animF + 1) % maxF;
                    p.animT = 0.f;
                    p.sprite.setTextureRect({p.animF * size, 0, size, size});
                }
            }
        }
    }

    void updateEnemies(float dt)
    {
        enemyDmgTimer += dt;
        auto it = enemies.begin();
        while(it != enemies.end())
        {
            it->setTarget(player.getPosition());
            it->update();
            
            if (it->isAnimFinished()) {
                it = enemies.erase(it);
                continue;
            }

            if (!it->isDead()) {
                float dist = std::sqrt(std::pow(it->getPosition().x - player.getPosition().x, 2) + 
                                        std::pow(it->getPosition().y - player.getPosition().y, 2));
                if (dist < 50.f && enemyDmgTimer > 1.2f) {
                    int damage = 12;
                    if (player.inv.armor > 0) damage -= 4;
                    if(damage < 4) damage = 4;
                    player.health -= damage;
                    enemyDmgTimer = 0.f;
                    camera.shake(12, 0.25f);
                }

                int playerDmg = player.getAttackPower() * player.inv.weaponLevel;
                if (playerDmg > 0 && dist < 85.f) {
                    bool wasDead = it->isDead();
                    it->takeDamage(playerDmg);
                    spawnBlood(it->getPosition());
                    if (!wasDead && it->isDead()) {
                        onEnemyKilled(); // <-- track kill for tasks
                        if(rand()%8 == 0) {
                            spawnDrop(it->getPosition(), false, false, false, false, (rand()%2==0), (rand()%2==0));
                        }
                    }
                }
            }
            it++;
        }
    }

    void updateCollisions()
    {
        sf::Vector2f p = player.getPosition();
        for (auto& g : groundProps) {
            if (g.solid && g.hitbox.intersects(player.getSprite().getGlobalBounds())) {
                // simple bounce back
                player.setPosition(p.x, p.y); 
            }
        }
    }

    void updateInteractions()
    {
        static bool fPressed = false;
        static bool bPressed = false;
        static bool cPressed = false;
        static bool gPressed = false;
        static bool ePressed = false;
        static bool qPressed = false;
        static bool spacePressed = false;

        // Plant
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
            if (!fPressed) { plantCrop(); fPressed = true; }
        } else fPressed = false;

        // Eat
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if (!ePressed && player.inv.food > 0) {
                player.health = std::min(100, player.health + 20);
                player.inv.food--;
                player.triggerEat();
                spawnHealParticles(player.getPosition());
                ePressed = true;
            }
        } else ePressed = false;

        // Drink
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            if (!qPressed && player.inv.water > 0) {
                player.inv.water--;
                player.triggerDrink();
                spawnHealParticles(player.getPosition());
                qPressed = true;
            }
        } else qPressed = false;

        // Space interaction (Harvest, Mine, Cut)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (!spacePressed) {
                interactWithWorld();
                spacePressed = true;
            }
        } else spacePressed = false;

        // Build house
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
            if (!bPressed && player.inv.wood >= 50 && player.inv.stone >= 30) {
                spawnHouse(player.getPosition());
                player.inv.wood -= 50; player.inv.stone -= 30;
                builtHouse = true;
                if (currentLevel == 1 && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                bPressed = true;
            }
        } else bPressed = false;

        // Build farm area
        static bool vPressed = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
            if (!vPressed && player.inv.wood >= 20 && player.inv.stone >= 10) {
                buildFarm();
                player.inv.wood -= 20; player.inv.stone -= 10;
                vPressed = true;
            }
        } else vPressed = false;

        // Craft armor
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            if (!cPressed && player.inv.stone >= 40) {
                 player.inv.armor++;
                 player.inv.stone -= 40;
                 craftedArmor = true;
                 if (currentLevel == 3 && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                 spawnHealParticles(player.getPosition());
                 cPressed = true;
            }
        } else cPressed = false;

        // Upgrade weapon
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            if (!gPressed && player.inv.wood >= 60 && player.inv.stone >= 40) {
                 player.inv.weaponLevel++;
                 player.inv.wood -= 60; player.inv.stone -= 40;
                 upgradedWeapon = true;
                 if (currentLevel == 2 && tasks.size() > 3) tasks[3].progress(tasks[3].goal);
                 spawnHealParticles(player.getPosition());
                 gPressed = true;
            }
        } else gPressed = false;

        // Collect items  (also update resource tasks live)
        auto it = groundProps.begin();
        while (it != groundProps.end()) {
            if (it->isWood || it->isStone || it->isFood || it->isWater || it->isArmor || it->isWeaponLevel || it->isMetal) {
                if (player.getSprite().getGlobalBounds().intersects(it->sprite.getGlobalBounds())) {
                    if (it->isWood)  { player.inv.wood++;  if (currentLevel==1 && tasks.size()>1) tasks[1].progress(); }
                    if (it->isStone) { player.inv.stone++; if (currentLevel==1 && tasks.size()>2) tasks[2].progress(); }
                    if (it->isFood)  { player.inv.food++;  if (currentLevel==2 && tasks.size()>1) tasks[1].progress(); }
                    if (it->isWater)  player.inv.water++;
                    if (it->isArmor)  player.inv.armor++;
                    if (it->isWeaponLevel) player.inv.weaponLevel++;
                    if (it->isMetal) {
                        player.inv.metal++;
                        smelted = true;
                        if (currentLevel==3 && tasks.size()>2) tasks[2].progress();
                    }
                    it = groundProps.erase(it);
                    continue;
                }
            }
            it++;
        }
    }

    void interactWithWorld()
    {
        sf::FloatRect interactRange = player.getSprite().getGlobalBounds();
        interactRange.left -= 150; interactRange.top -= 150;
        interactRange.width += 300; interactRange.height += 300;

        for (auto& p : groundProps) {
            if (interactRange.intersects(p.sprite.getGlobalBounds())) {
                if (p.isFarm && p.growthStage == 2) { 
                    spawnDrop(p.sprite.getPosition(), false, false, true, false, false, false, (rand()%2==0)); 
                    p.growthStage = 0; p.growthTimer = 0; p.sprite.setTextureRect({0, 16, 16, 16}); 
                }
                if (p.isWell) { if(rand()%3==0) player.inv.water++; }
                
                // Station Interactions
                if (p.isStation) {
                    if (p.stationType == 1 && player.inv.stone >= 5) { // Furnace: Stone -> Metal
                        player.inv.stone -= 5; player.inv.metal++;
                        smelted = true;
                        if (currentLevel==3 && tasks.size()>2) tasks[2].progress();
                        spawnHealParticles(p.sprite.getPosition());
                    }
                    if (p.stationType == 0 && player.inv.metal >= 2) { // Anvil: Metal -> Armor
                        player.inv.metal -= 2; player.inv.armor++;
                        craftedArmor = true;
                        if (currentLevel==3 && tasks.size()>3) tasks[3].progress(tasks[3].goal);
                        spawnHealParticles(p.sprite.getPosition());
                    }
                }
            }
        }
        
        // Push pending items safely
        for(auto& dp : pendingProps) groundProps.push_back(dp);
        pendingProps.clear();
    }

    void spawnHouse(sf::Vector2f pos)
    {
        Prop wall, roof;
        wall.sprite.setTexture(texHouseWalls); wall.sprite.setPosition(pos); wall.sprite.setOrigin(48, 48); wall.sprite.setScale(4, 4); wall.solid = true; wall.isHouse = true; wall.hitbox = {pos.x-100, pos.y-50, 200, 100};
        roof.sprite.setTexture(texHouseRoofs); roof.sprite.setPosition(pos.x, pos.y - 120); roof.sprite.setOrigin(48, 48); roof.sprite.setScale(4.5f, 4.5f);
        groundProps.push_back(wall);
        overheadProps.push_back(roof);
    }

    void spawnWell(sf::Vector2f pos)
    {
        Prop w; w.sprite.setTexture(texRock); w.sprite.setTextureRect({64, 0, 32, 32}); w.sprite.setScale(4,4); w.sprite.setOrigin(16,16); w.sprite.setPosition(pos); w.solid = true; w.isWell = true; w.hitbox = {pos.x-60, pos.y-60, 120, 120};
        groundProps.push_back(w);
    }

    void buildFarm()
    {
        sf::Vector2f pos = player.getPosition();
        for(int y=-1; y<=1; y++) {
            for(int x=-1; x<=1; x++) {
                Prop f; f.sprite.setTexture(texFarm); f.sprite.setTextureRect({0, 16, 16, 16}); f.sprite.setScale(TILE_SCALE, TILE_SCALE); f.sprite.setOrigin(8, 8); f.sprite.setPosition(pos.x + x*TILE_PX*TILE_SCALE, pos.y + y*TILE_PX*TILE_SCALE);
                f.isFarm = true; f.growthStage = 0; f.growthTimer = 0;
                groundProps.push_back(f);
            }
        }
    }

    void spawnStation(sf::Vector2f pos, int type)
    {
        Prop s; s.isStation = true; s.stationType = type; s.solid = true; s.sprite.setPosition(pos); s.sprite.setScale(TILE_SCALE, TILE_SCALE);
        if(type==0) { s.sprite.setTexture(texAnvil); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
        if(type==1) { s.sprite.setTexture(texFurnace); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
        if(type==2) { s.sprite.setTexture(texSawmill); s.sprite.setTextureRect({0,0,64,64}); s.sprite.setOrigin(32,48); }
        if(type==3) { s.sprite.setTexture(texWorkbench); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
        s.hitbox = {pos.x-60, pos.y-40, 120, 80};
        groundProps.push_back(s);
    }

    void spawnFurniture(sf::Vector2f pos)
    {
        Prop f; f.isFurniture = true; f.sprite.setTexture(texFurniture); f.sprite.setPosition(pos); f.sprite.setScale(TILE_SCALE, TILE_SCALE);
        int tx = (rand()%4)*32; int ty = (rand()%4)*32; f.sprite.setTextureRect({tx, ty, 32, 32}); f.sprite.setOrigin(16, 24); f.solid = true; f.hitbox = {pos.x-40, pos.y-30, 80, 60};
        groundProps.push_back(f);
    }

    void plantCrop()
    {
        Prop f; f.sprite.setTexture(texFarm); f.sprite.setTextureRect({0, 16, 16, 16}); f.sprite.setScale(TILE_SCALE, TILE_SCALE); f.sprite.setOrigin(8, 8); f.sprite.setPosition(player.getPosition());
        f.isFarm = true; f.growthStage = 0; f.growthTimer = 0;
        groundProps.push_back(f);
    }

    void spawnDrop(sf::Vector2f pos, bool iw, bool is, bool ifo, bool iwa, bool ia = false, bool iwl = false, bool isFruit = false, bool im = false)
    {
        Prop d; d.sprite.setTexture(texRes); d.sprite.setPosition(pos.x + rand()%40-20, pos.y + rand()%40-20); d.sprite.setScale(3,3); d.sprite.setOrigin(8,8);
        if(iw) { d.isWood = true; d.sprite.setTextureRect({0,0,16,16}); }
        if(is) { d.isStone = true; d.sprite.setTextureRect({16,0,16,16}); }
        if(ifo) { d.isFood = true; d.sprite.setTextureRect({32,0,16,16}); }
        if(iwa) { d.isWater = true; d.sprite.setTextureRect({48,0,16,16}); }
        if(ia) { d.isArmor = true; d.sprite.setTextureRect({64,0,16,16}); d.sprite.setColor(sf::Color::Cyan); }
        if(iwl) { d.isWeaponLevel = true; d.sprite.setTextureRect({80,0,16,16}); d.sprite.setColor(sf::Color::Yellow); }
        if(isFruit) { d.isFood = true; d.sprite.setTextureRect({32, 16, 16, 16}); d.sprite.setColor(sf::Color::Red); } 
        if(im) { d.isMetal = true; d.sprite.setTextureRect({64, 16, 16, 16}); d.sprite.setColor(sf::Color(200,200,200)); }
        pendingProps.push_back(d);
    }

    void updateParticles(float dt)
    {
        auto it = particles.begin();
        while(it != particles.end()) {
            if(!it->update(dt)) it = particles.erase(it);
            else it++;
        }
    }

    void spawnBlood(sf::Vector2f pos) { for(int i=0; i<5; i++) particles.emplace_back(pos.x, pos.y, sf::Color::Red, 3, (rand()%100-50), (rand()%100-50), 0.5f); }
    void spawnLeaf(sf::Vector2f pos) { for(int i=0; i<3; i++) particles.emplace_back(pos.x, pos.y, sf::Color(50, 200, 50), 4, (rand()%60-30), (rand()%60-30), 1.0f); }
    void spawnHealParticles(sf::Vector2f pos) { for(int i=0; i<10; i++) particles.emplace_back(pos.x, pos.y, sf::Color::Green, 4, (rand()%100-50), (rand()%100-50), 0.8f); }

    void spawnAmbientParticles(float dt)
    {
        static float timer = 0;
        timer += dt;
        if(timer > 0.1f) {
            float rx = camera.getCenter().x + (rand()%1280 - 640);
            float ry = camera.getCenter().y + (rand()%720 - 360);
            particles.emplace_back(rx, ry, sf::Color(255,255,200,100), 2, (rand()%20-10), (rand()%20-10), 3.0f);
            timer = 0;
        }
    }

    void restartGame()
    {
        gameOver = false; score = 0; player.health = 100; player.inv = {0, 10, 10, 0, 1};
        currentLevel = 1;
        gameOverText.setString("SURVIVAL ENDED\nPress R to Restart");
        gameOverText.setFillColor(sf::Color::Red);
        loadLevel(1);
    }

    void draw()
    {
        window.clear(sf::Color(30, 80, 40));
        camera.apply(window);

        window.draw(mapGround);
        window.draw(mapWater);
        window.draw(mapRiversideWalls);

        // Sort props + player + enemies by Y
        std::vector<sf::Drawable*> drawList;
        struct YSort { 
            sf::Drawable* d; float y; 
            bool operator<(const YSort& other) const { return y < other.y; } 
        };
        std::vector<YSort> sorted;
        for(auto& p : groundProps) sorted.push_back({&p.sprite, p.sprite.getPosition().y});
        for(auto& e : enemies) if(!e.isDead()) sorted.push_back({&e.getSprite(), e.getPosition().y});
        sorted.push_back({&player.getSprite(), player.getPosition().y});
        
        std::sort(sorted.begin(), sorted.end());
        for(auto& s : sorted) window.draw(*s.d);

        for(auto& e : enemies) e.draw(window); // Draw health bars on top
        for(auto& p : overheadProps) window.draw(p.sprite);
        for(auto& p : particles) window.draw(p.shape);

        drawHUD();
        window.display();
    }

    void drawHUD()
    {
        window.setView(window.getDefaultView());
        playerHpFg.setSize({250.f * (player.health / 100.f), 25.f});
        window.draw(playerHpBg);
        window.draw(playerHpFg);
        playerHpText.setString("HP: " + std::to_string(player.health));
        window.draw(playerHpText);

        std::string levelName;
        if (currentLevel == 1) levelName = "The Village";
        else if (currentLevel == 2) levelName = "Dark Forest";
        else levelName = "The Dungeon";

        waveText.setString("Wave " + std::to_string(wave) + "/" + std::to_string(wavesPerLevel[currentLevel]));
        window.draw(waveText);
        levelText.setString("LEVEL " + std::to_string(currentLevel) + ": " + levelName);
        window.draw(levelText);

        invText.setString("Wood:" + std::to_string(player.inv.wood) +
                          "  Stone:" + std::to_string(player.inv.stone) +
                          "  Metal:" + std::to_string(player.inv.metal) +
                          "\nFood:" + std::to_string(player.inv.food) +
                          "  Water:" + std::to_string(player.inv.water) +
                          "\nArmor:" + std::to_string(player.inv.armor) +
                          "  Wep Lvl:" + std::to_string(player.inv.weaponLevel));
        window.draw(invText);
        window.draw(controlsText);
        if (gameOver) window.draw(gameOverText);

        // ---- Task Panel (right side) ----
        // Resize panel to fit tasks
        float panelH = 38.f + tasks.size() * 28.f;
        taskPanelBg.setSize({310.f, panelH});
        window.draw(taskPanelBg);

        taskHeaderText.setString(" OBJECTIVES");
        window.draw(taskHeaderText);

        float ty = 50.f;
        for (auto& t : tasks) {
            std::string prefix = t.isDone() ? "[+] " : "[ ] ";
            std::string bar;
            if (!t.isDone() && t.goal > 1) {
                // Show progress bar as text fraction
                bar = "  (" + std::to_string(t.current) + "/" + std::to_string(t.goal) + ")";
            }
            taskItemText.setString(prefix + t.label + bar);
            taskItemText.setFillColor(t.isDone() ? sf::Color(100, 255, 120) : sf::Color(220, 220, 220));
            taskItemText.setPosition(950, ty);
            window.draw(taskItemText);
            ty += 28.f;
        }

        // ---- Level complete overlay ----
        if (levelComplete) {
            std::string lvlNames[] = {"", "The Village", "Dark Forest", "The Dungeon"};
            if (currentLevel < MAX_LEVEL)
                levelCompleteText.setString("LEVEL " + std::to_string(currentLevel) + " COMPLETE!\n\nEntering: Level " + std::to_string(currentLevel + 1) + " - " + lvlNames[currentLevel + 1]);
            else
                levelCompleteText.setString("ALL LEVELS COMPLETE!\n\nYOU ARE A LEGEND!");
            sf::FloatRect tb = levelCompleteText.getLocalBounds();
            levelCompleteText.setOrigin(tb.width / 2, tb.height / 2);
            levelCompleteText.setPosition(640, 320);
            window.draw(levelCompleteBg);
            window.draw(levelCompleteText);
        }
    }

    void run()
    {
        sf::Clock clock;
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) window.close();
            }
            float dt = clock.restart().asSeconds();
            update(dt);
            draw();
        }
    }
};
