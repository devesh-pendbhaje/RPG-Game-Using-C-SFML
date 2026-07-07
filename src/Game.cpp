#include "Game.h"
#include "EventManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <ctime>

// --- Particle Implementation ---
Particle::Particle(float x, float y, sf::Color col, float r, float vx, float vy, float life)
    : vel(vx, vy), lifetime(life), maxLife(life)
{
    shape.setRadius(r);
    shape.setOrigin(r, r);
    shape.setPosition(x, y);
    shape.setFillColor(col);
}

bool Particle::update(float dt)
{
    lifetime -= dt;
    shape.move(vel * dt);
    float alpha = std::max(0.f, lifetime / maxLife);
    sf::Color c = shape.getFillColor();
    c.a = (sf::Uint8)(alpha * 255);
    shape.setFillColor(c);
    return lifetime > 0.f;
}

// --- Game Implementation ---
Game::Game()
    : window(sf::VideoMode(1280, 720), "RPG Adventure - Story Mode", sf::Style::Titlebar | sf::Style::Close),
      camera(1280.f, 720.f)
{
    srand((unsigned)time(nullptr));
    window.setFramerateLimit(60);

    loadAssets();
    setupHUD();
    loadedRecipes = DataLoader::loadRecipes("recipes.json");
    loadLevel(currentLevel);
}

void Game::loadAssets()
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

void Game::setupHUD()
{
    playerHpBg.setSize({250.f, 25.f}); playerHpBg.setFillColor({50,50,50,200}); playerHpBg.setPosition(30, 30);
    playerHpFg.setSize({250.f, 25.f}); playerHpFg.setFillColor({80,255,80,220}); playerHpFg.setPosition(30,30);

    playerHungerBg.setSize({250.f, 20.f}); playerHungerBg.setFillColor({50,50,50,200}); playerHungerBg.setPosition(30, 60);
    playerHungerFg.setSize({250.f, 20.f}); playerHungerFg.setFillColor({255,165,0,220}); playerHungerFg.setPosition(30,60);

    playerStaminaBg.setSize({250.f, 20.f}); playerStaminaBg.setFillColor({50,50,50,200}); playerStaminaBg.setPosition(30, 85);
    playerStaminaFg.setSize({250.f, 20.f}); playerStaminaFg.setFillColor({100,100,255,220}); playerStaminaFg.setPosition(30,85);

    craftingOverlayBg.setSize({600.f, 400.f}); craftingOverlayBg.setFillColor({30,30,30,230}); craftingOverlayBg.setPosition(340, 160);
    craftingOverlayBg.setOutlineColor(sf::Color(180, 180, 180)); craftingOverlayBg.setOutlineThickness(3);

    auto setupText = [&](sf::Text& t, int size, sf::Color col, float y) {
        t.setFont(font); t.setCharacterSize(size); t.setFillColor(col); t.setPosition(30, y);
        t.setOutlineThickness(2); t.setOutlineColor(sf::Color::Black);
    };
    setupText(playerHpText,  16, sf::Color::White,         33);
    setupText(playerHungerText,  14, sf::Color::White,     61);
    setupText(playerStaminaText, 14, sf::Color::White,     86);
    setupText(waveText,      24, sf::Color::Cyan,          115);
    setupText(levelText,     20, sf::Color(255, 165, 0),   145);
    setupText(invText,       18, sf::Color(255,200,100),   180);
    setupText(controlsText,  13, sf::Color::White,         530);
    
    craftingHeaderText.setFont(font); craftingHeaderText.setCharacterSize(28); craftingHeaderText.setFillColor(sf::Color::White);
    craftingHeaderText.setString("CRAFTING MATRIX"); craftingHeaderText.setPosition(520, 180);
    controlsText.setString("WASD: Move | SPACE: Harvest/Attack | E: Eat | Q: Drink | F5: Save Game\nB: Build House | V: Build Farm | G: Upgrade Weapon | C: Craft Armor");

    setupText(gameOverText, 60, sf::Color::Red, 280);
    gameOverText.setString("ADVENTURE ENDED\nPress R to Restart");
    sf::FloatRect gb = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gb.width/2, gb.height/2);
    gameOverText.setPosition(640, 360);

    levelCompleteBg.setSize({1280.f, 720.f});
    levelCompleteBg.setFillColor({0, 0, 0, 170});
    levelCompleteBg.setPosition(0, 0);
    levelCompleteText.setFont(font);
    levelCompleteText.setCharacterSize(52);
    levelCompleteText.setFillColor(sf::Color(255, 220, 50));
    levelCompleteText.setOutlineThickness(4);
    levelCompleteText.setOutlineColor(sf::Color::Black);

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

    // Menu setup
    menuTitle.setFont(font); menuTitle.setCharacterSize(80); menuTitle.setFillColor(sf::Color(255, 200, 50));
    menuTitle.setString("PIXEL ADVENTURE");
    sf::FloatRect mtb = menuTitle.getLocalBounds(); menuTitle.setOrigin(mtb.width/2, mtb.height/2); menuTitle.setPosition(640, 150);

    const std::string opts[] = {"NEW GAME", "CONTINUE", "EXIT"};
    for(int i=0; i<3; i++) {
        menuOptions[i].setFont(font); menuOptions[i].setCharacterSize(40);
        menuOptions[i].setString(opts[i]);
        sf::FloatRect ob = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin(ob.width/2, ob.height/2);
        menuOptions[i].setPosition(640, 350 + i*80);
    }

    // Level Info Bar Setup
    levelInfoBarBg.setSize({1280.f, 40.f});
    levelInfoBarBg.setFillColor(sf::Color(20, 20, 30, 200));
    levelInfoBarBg.setOutlineColor(sf::Color(100, 100, 100, 150));
    levelInfoBarBg.setOutlineThickness(1.f);
    levelInfoBarBg.setPosition(0, 0);

    // Inventory Dashboard Setup
    inventoryDashboardBg.setSize({1280.f, 70.f});
    inventoryDashboardBg.setFillColor(sf::Color(20, 20, 30, 220));
    inventoryDashboardBg.setOutlineColor(sf::Color(100, 100, 100, 150));
    inventoryDashboardBg.setOutlineThickness(2.f);
    inventoryDashboardBg.setPosition(0, 650);
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event) )
        {
            if (event.type == sf::Event::Closed) window.close();
            
            if (currentState == GameState::MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) menuIndex = (menuIndex + 2) % 3;
                    if (event.key.code == sf::Keyboard::Down) menuIndex = (menuIndex + 1) % 3;
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (menuIndex == 0) { 
                            restartGame(); 
                            currentState = GameState::PLAYING; 
                        } else if (menuIndex == 1) { 
                            if (SaveManager::loadGame("savegame.json", currentLevel, maxLevelUnlocked, player, groundProps)) {
                                loadLevel(currentLevel, true);
                                currentState = GameState::PLAYING;
                            } else {
                                // Default to level 1 if no save exists, but stay in menu or handle gracefully
                                std::cout << "No save game found. Start a New Game instead." << std::endl;
                            }
                        }
                        else if (menuIndex == 2) window.close();
                    }
                }
            }
            else if (currentState == GameState::LEVEL_SELECT) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Down) currentLevel = std::min(MAX_LEVEL, currentLevel + 1);
                    if (event.key.code == sf::Keyboard::Up) currentLevel = std::max(1, currentLevel - 1);
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (currentLevel <= maxLevelUnlocked) {
                            loadLevel(currentLevel);
                            currentState = GameState::PLAYING;
                        }
                    }
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentState = GameState::MENU;
                    }
                }
            }
            else if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) restartGame();
            else if (currentState == GameState::DIALOGUE && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                dialogueManager.nextSentence();
                if (!dialogueManager.getIsActive()) currentState = GameState::PLAYING;
            }
        }
        float dt = clock.restart().asSeconds();
        update(dt);
        draw();
    }
}

void Game::restorePropsTextures()
{
    for (auto& p : groundProps) {
        if (p.isTree) {
            p.sprite.setTexture(texTree);
            if (p.isStump || p.hp <= 0) {
                p.solid = true;
                p.sprite.setTextureRect({p.variant * TREE_W, TREE_H - 64, TREE_W, 64});
                p.sprite.setOrigin(TREE_W / 2.f, 64 - 16.f);
                p.sprite.setColor(sf::Color::White);
                p.hitbox = {p.sprite.getPosition().x - 40, p.sprite.getPosition().y - 20, 80, 40};
            } else {
                p.sprite.setTextureRect({p.variant * TREE_W, 0, TREE_W, TREE_H});
                p.setBaseOrigin(TREE_W, TREE_H, 16.f); 
                p.solid = true;
                p.hitbox = {p.sprite.getPosition().x - 60, p.sprite.getPosition().y - 40, 120, 80};
            }
            p.sprite.setScale(2.5f, 2.5f);
        } else if (p.isRock) {
            p.sprite.setTexture(texRock); p.sprite.setTextureRect({0, 0, 32, 32}); 
            p.setBaseOrigin(32, 32, 8.f); p.sprite.setScale(ASSET_SCALE * 1.5f, ASSET_SCALE * 1.5f);
            if (p.hp <= 0) {
                p.solid = false;
                p.sprite.setColor({50,50,50,100});
                p.hitbox = {0,0,0,0};
            } else {
                p.solid = true;
                p.hitbox = {p.sprite.getPosition().x - 40, p.sprite.getPosition().y - 30, 80, 60};
            }
        } else if (p.isStone) {
            p.sprite.setTexture(texRock); p.sprite.setTextureRect({32, 0, 32, 32});
            p.sprite.setOrigin(16, 16); p.sprite.setScale(2.5f, 2.5f);
        } else if (p.isWood) {
            p.sprite.setTexture(texRes); p.sprite.setTextureRect({0, 48, 16, 16});
            p.sprite.setOrigin(8, 8); p.sprite.setScale(4.5f, 4.5f);
        } else if (p.isFood) {
            p.sprite.setTexture(texRes); p.sprite.setTextureRect({32, 48, 16, 16});
            p.sprite.setOrigin(8, 8); p.sprite.setScale(4.5f, 4.5f);
        } else if (p.isWater) {
            p.sprite.setTexture(texRes); p.sprite.setTextureRect({48, 48, 16, 16});
            p.sprite.setOrigin(8, 8); p.sprite.setScale(4.5f, 4.5f);
        } else if (p.isHouse) {
            p.sprite.setTexture(texHouseWalls); p.sprite.setTextureRect({0, 0, 64, 64});
            p.setBaseOrigin(32, 64, 0.f); p.sprite.setScale(4.f, 4.f);
            p.solid = true; p.hitbox = {p.sprite.getPosition().x-100, p.sprite.getPosition().y-50, 200, 100};
        } else if (p.isFarm) {
            p.sprite.setTexture(texFarm); p.sprite.setTextureRect({0, 0, 16, 16});
            p.setBaseOrigin(8, 16, 0.f); p.sprite.setScale(4.f, 4.f);
            p.solid = false; 
        } else if (p.isStation) {
            if (p.stationType == 0) { p.sprite.setTexture(texWorkbench); p.sprite.setTextureRect({0, 0, 32, 32}); }
            else if (p.stationType == 1) { p.sprite.setTexture(texFurnace); p.sprite.setTextureRect({0, 0, 32, 32}); }
            p.setBaseOrigin(16, 32, 8.f); p.sprite.setScale(3.0f, 3.0f);
            p.solid = true; p.hitbox = {p.sprite.getPosition().x-60, p.sprite.getPosition().y-40, 120, 80};
        }
    }
}

void Game::loadLevel(int level, bool isRestoringFromSave)
{
    if (!isRestoringFromSave) {
        groundProps.clear(); 
    }
    overheadProps.clear(); enemies.clear(); npcs.clear(); particles.clear();
    missionSystem.clear();
    wave = 1; levelComplete = false;

    if (level == 1) {
        buildMapLevel1(); 
        if (!isRestoringFromSave) player.setPosition(3500, 2400);
        if (!isRestoringFromSave) loadPropsLevel1(); 
        npcs.emplace_back("Old Man", sf::Vector2f(3400, 2300), std::vector<std::string>{"Welcome to our village.", "Can you help us build some shelter?", "Press [B] once you have enough Wood and Stone."});
        missionSystem.addMission(Mission("Construct Shelter", "Build a house [B] (50W, 30S)", 1));
        missionSystem.addMission(Mission("Gather Wood", "Collect 20 wood pieces", 20));
        missionSystem.addMission(Mission("Survive Waves", "Survive Enemy Waves", 1));
    } else if (level == 2) {
        buildMapLevel2(); 
        if (!isRestoringFromSave) player.setPosition(600, 600);
        if (!isRestoringFromSave) loadPropsLevel2(); 
        npcs.emplace_back("Ranger", sf::Vector2f(500, 500), std::vector<std::string>{"This land is barren.", "Clear 10 rocks and plant a farm using [V].", "Survive the incoming attacks."});
        missionSystem.addMission(Mission("Clear Area", "Mine 10 rocks to terraform", 10));
        missionSystem.addMission(Mission("Establish Farm", "Plant 3 farms [V] (20W, 10S)", 3));
        missionSystem.addMission(Mission("Survive Waves", "Survive the region's hordes", 1));
    } else if (level == 3) {
        buildMapLevel3(); 
        if (!isRestoringFromSave) player.setPosition(400, 400);
        if (!isRestoringFromSave) loadPropsLevel3(); 
        npcs.emplace_back("Smith", sf::Vector2f(500, 500), std::vector<std::string>{"This is the final assault.", "Refine metal at the furnace and master smithing [G].", "Wipe out the forces."});
        missionSystem.addMission(Mission("Refine Metal", "Furnace: 5 Stone -> 1 Metal", 2));
        missionSystem.addMission(Mission("Master Smithing", "Upgrade weapon [G] (60W, 40S)", 1));
        missionSystem.addMission(Mission("Assault Forces", "Defeat 15 enemies", 15));
    }

    if (isRestoringFromSave) {
        restorePropsTextures();
    }

    camera.setMapBounds(mapW(), mapH());
    camera.followPlayer(player);
    spawnWave(wave);
}

void Game::buildMapLevel1()
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
                groundData[x][y] = T_WATER; 
            } else if ((x >= riverX - 6 && x < riverX - 3) || (x > riverX + 1 && x <= riverX + 4)) {
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_DIRT;
            } else if (x < 3 || x >= MAP_COLS - 3 || y < 3 || y >= MAP_ROWS - 3) {
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_ROCK;
            } else {
                groundData[x][y] = T_GRASS;
            }
        }
    }
    mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
    mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
    mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
    mapGround.setScale(TILE_SCALE, TILE_SCALE); mapWater.setScale(TILE_SCALE, TILE_SCALE); mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
}

void Game::loadPropsLevel1()
{
    sf::Vector2f playerSpawn(3500, 2400);
    for (int i = 0; i < 150; i++) spawnTreeProcedural(playerSpawn);
    for (int i = 0; i < 200; i++) spawnRockProcedural(playerSpawn);
    for (int i = 0; i < 200; i++) spawnVeg();
    for (int i = 0; i < 150; i++) spawnStoneProcedural(playerSpawn);
    
    // Guaranteed cluster near starting point for easy collection
    for (int i = 0; i < 15; i++) {
        float rx = 3500.f + (rand() % 800 - 400);
        float ry = 2400.f + (rand() % 800 - 400);
        if (isBuildable(rx, ry)) {
            Prop p; p.sprite.setTexture(texRock); p.sprite.setTextureRect({0, 0, 32, 32}); 
            p.setBaseOrigin(32, 32, 8.f); p.sprite.setScale(ASSET_SCALE * 1.5f, ASSET_SCALE * 1.5f);
            p.sprite.setPosition(rx, ry); p.isRock = true; p.hp = 80; p.solid = true; 
            p.hitbox = {rx - 40, ry - 30, 80, 60};
            groundProps.push_back(p);
        }
    }
    for (int i = 0; i < 15; i++) {
        float rx = 3500.f + (rand() % 800 - 400);
        float ry = 2400.f + (rand() % 800 - 400);
        if (isBuildable(rx, ry)) {
            Prop p; p.sprite.setTexture(texRes); p.sprite.setTextureRect({16, 0, 16, 16}); 
            p.sprite.setOrigin(8, 8); p.sprite.setScale(4.5f, 4.5f);
            p.sprite.setPosition(rx, ry); p.isStone = true;
            groundProps.push_back(p);
        }
    }

    spawnWell({3500, 2400});
    spawnStation({3000, 2400}, 0); spawnStation({2900, 2400}, 1);
    
    // Debug: Force stones at player feet
    for (int i=0; i<10; i++) {
        Prop s; s.sprite.setTexture(texRock); s.sprite.setTextureRect({32, 0, 32, 32});
        s.sprite.setOrigin(16, 16); s.sprite.setScale(2.5f, 2.5f);
        s.sprite.setPosition(3500 + i*60 - 300, 2500); s.isStone = true;
        groundProps.push_back(s);
    }
}

void Game::buildMapLevel2()
{
    std::vector<int> groundTiles(MAP_COLS * MAP_ROWS, T_GRASS);
    std::vector<int> waterTiles(MAP_COLS * MAP_ROWS, -1);
    std::vector<int> riverWallTiles(MAP_COLS * MAP_ROWS, -1);

    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            // Re-implement the rivers for level 2 so they actually show up correctly!
            float r1 = 15 + 4 * std::sin(y * 0.12f);
            float r2 = 55 + 3 * std::cos(y * 0.18f);
            if ((x >= r1 - 2 && x <= r1 + 2) || (x >= r2 - 2 && x <= r2 + 2)) {
                waterTiles[y * MAP_COLS + x] = 16;
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_WATER;
            } else if ((x >= (int)r1 - 5 && x < (int)r1 - 2) || (x > (int)r1 + 2 && x <= (int)r1 + 5)) {
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_DIRT;
            } else if ((x >= (int)r2 - 5 && x < (int)r2 - 2) || (x > (int)r2 + 2 && x <= (int)r2 + 5)) {
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_DIRT;
            } else if ((x > 30 && x < 40 && y > 30 && y < 40) || (x > 55 && x < 65 && y > 50 && y < 60)) {
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_ROCK;
            } else if (x < 3 || x >= MAP_COLS - 3 || y < 3 || y >= MAP_ROWS - 3) {
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_ROCK;
            } else {
                groundData[x][y] = T_GRASS;
            }
        }
    }
    mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
    mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
    mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
    mapGround.setScale(TILE_SCALE, TILE_SCALE); mapWater.setScale(TILE_SCALE, TILE_SCALE); mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
    mapGround.setColor(sf::Color(160, 200, 160));
}

void Game::loadPropsLevel2()
{
    sf::Vector2f playerSpawn(600, 600);
    for (int i = 0; i < 300; i++) spawnTreeProcedural(playerSpawn);
    for (int i = 0; i < 150; i++) spawnRockProcedural(playerSpawn);
    for (int i = 0; i < 100; i++) spawnStoneProcedural(playerSpawn);
}

void Game::buildMapLevel3()
{
    std::vector<int> groundTiles(MAP_COLS * MAP_ROWS, T_ROCK);
    std::vector<int> waterTiles(MAP_COLS * MAP_ROWS, -1);
    std::vector<int> riverWallTiles(MAP_COLS * MAP_ROWS, -1);
    
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            bool inRoom = ((x / 10) % 2 == 0) && ((y / 10) % 2 == 0) && x > 5 && x < MAP_COLS - 5 && y > 5 && y < MAP_ROWS - 5;
            bool corridor = (x % 10 == 5 && y > 5 && y < MAP_ROWS - 5) || (y % 10 == 5 && x > 5 && x < MAP_COLS - 5);

            if (inRoom || corridor) {
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_DIRT;
            } else if (x > 35 && x < 40 && y > 35 && y < 40) {
                waterTiles[y * MAP_COLS + x] = 16;
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_WATER; // Treat lava as water to prevent spawning here
            } else if (x >= 33 && x <= 41 && y >= 33 && y <= 41 && !((x > 35 && x < 40) && (y > 35 && y < 40))) {
                groundTiles[y * MAP_COLS + x] = T_DIRT;
                groundData[x][y] = T_DIRT;
            } else if (x < 2 || x >= MAP_COLS - 2 || y < 2 || y >= MAP_ROWS - 2) {
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_ROCK;
            } else {
                groundTiles[y * MAP_COLS + x] = T_ROCK;
                groundData[x][y] = T_ROCK;
            }
        }
    }
    
    mapGround.load("assets_new/floors.png", {16, 16}, groundTiles, MAP_COLS, MAP_ROWS);
    mapWater.load("assets_new/water.png", {16, 16}, waterTiles, MAP_COLS, MAP_ROWS);
    mapRiversideWalls.load("assets_new/walls_tile.png", {16, 16}, riverWallTiles, MAP_COLS, MAP_ROWS);
    mapGround.setScale(TILE_SCALE, TILE_SCALE); mapWater.setScale(TILE_SCALE, TILE_SCALE); mapRiversideWalls.setScale(TILE_SCALE, TILE_SCALE);
    mapWater.setColor(sf::Color(255, 80, 0, 200)); mapGround.setColor(sf::Color(200, 190, 180));
}

void Game::loadPropsLevel3()
{
    sf::Vector2f playerSpawn(400, 400);
    for (int i = 0; i < 80; i++) spawnRockProcedural(playerSpawn);
    for (int i = 0; i < 50; i++) spawnStoneProcedural(playerSpawn);
    spawnStation({2000, 2000}, 1); spawnStation({2200, 2000}, 0);
}

void Game::spawnWave(int w)
{
    int baseCount = 10 + w * 5 + (currentLevel - 1) * 8;
    bool isBossWave = (currentLevel == 3 && w == wavesPerLevel[currentLevel]);
    if (isBossWave) baseCount *= 2;
    
    totalEnemiesInWave = baseCount;
    enemiesKilledInWave = 0;

    for (int i = 0; i < baseCount; i++) {
        float x = (rand() % 2 == 0) ? 100.f : mapW() - 100.f;
        float y = (float)(100 + rand() % (int)(mapH() - 200));
        Enemy::EType type = Enemy::ORC;
        if (currentLevel == 1) type = (rand() % 2 == 0) ? Enemy::ORC : Enemy::SKELETON;
        else if (currentLevel == 2) {
            int r = rand() % 5;
            if (r <= 1) type = Enemy::KNIGHT; else if (r == 2) type = Enemy::ROGUE; else type = Enemy::ORC;
        } else {
            if (isBossWave) {
                int r = rand() % 4;
                if (r == 0) type = Enemy::KNIGHT; else if (r == 1) type = Enemy::ROGUE; else if (r == 2) type = Enemy::WIZZARD; else type = Enemy::SKELETON;
            } else type = (rand() % 3 == 0) ? Enemy::WIZZARD : Enemy::SKELETON;
        }
        enemies.emplace_back(x, y, type);
    }
}

void Game::spawnTree() { spawnTreeProcedural(player.getPosition()); }
void Game::spawnTreeProcedural(sf::Vector2f playerSpawn)
{
    float x = 0.f, y = 0.f; bool found = false;
    for (int i = 0; i < 150; i++) {
        x = (float)(400 + rand() % ((int)mapW() - 800)); 
        y = (float)(400 + rand() % ((int)mapH() - 800));
        if (isValidSpawn(x, y, 100.f, playerSpawn)) { found = true; break; }
    }
    if (!found) {
        for (int i = 0; i < 50; i++) {
            x = (float)(400 + rand() % ((int)mapW() - 800)); 
            y = (float)(400 + rand() % ((int)mapH() - 800));
            if (isBuildable(x, y)) { found = true; break; }
        }
    }
    if (!found) return;

    Prop p; p.sprite.setTexture(texTree); p.variant = rand() % 4; p.sprite.setTextureRect({p.variant * TREE_W, 0, TREE_W, TREE_H});
    p.setBaseOrigin(TREE_W, TREE_H, 16.f); p.sprite.setScale(ASSET_SCALE, ASSET_SCALE);
    p.sprite.setPosition(x, y); p.isTree = true; p.hp = 100; p.solid = true; 
    p.hitbox = {x - 60, y - 40, 120, 80};
    groundProps.push_back(p);
}

void Game::spawnRock() { spawnRockProcedural(player.getPosition()); }
void Game::spawnRockProcedural(sf::Vector2f playerSpawn)
{
    float x = 0.f, y = 0.f; bool found = false;
    for (int i = 0; i < 150; i++) {
        x = (float)(200 + rand() % ((int)mapW() - 400)); 
        y = (float)(200 + rand() % ((int)mapH() - 400));
        if (isValidSpawn(x, y, 80.f, playerSpawn)) { found = true; break; }
    }
    if (!found) {
        for (int i = 0; i < 50; i++) {
            x = (float)(200 + rand() % ((int)mapW() - 400)); 
            y = (float)(200 + rand() % ((int)mapH() - 400));
            if (isBuildable(x, y)) { found = true; break; }
        }
    }
    if (!found) return;

    Prop p; p.sprite.setTexture(texRock); p.sprite.setTextureRect({0, 0, 32, 32}); 
    p.setBaseOrigin(32, 32, 8.f); p.sprite.setScale(ASSET_SCALE * 1.5f, ASSET_SCALE * 1.5f);
    p.sprite.setPosition(x, y); p.isRock = true; p.hp = 80; p.solid = true; 
    p.hitbox = {x - 40, y - 30, 80, 60};
    groundProps.push_back(p);
}

void Game::spawnVeg() { 
    float x = 0.f, y = 0.f; int maxAttempts = 20;
    while(maxAttempts--) { x = (float)(rand()%(int)mapW()); y = (float)(rand()%(int)mapH()); if (isBuildable(x, y)) break; }
    Prop p; p.sprite.setTexture(texVeg); p.sprite.setTextureRect({(rand()%8)*16,(rand()%8)*16,16,16}); p.sprite.setOrigin(8,8); p.sprite.setScale(TILE_SCALE, TILE_SCALE); p.sprite.setPosition(x, y); groundProps.push_back(p); 
}
void Game::spawnBush() { 
    float x = 0.f, y = 0.f; int maxAttempts = 20;
    while(maxAttempts--) { x = (float)(rand()%(int)mapW()); y = (float)(rand()%(int)mapH()); if (isBuildable(x, y)) break; }
    Prop p; p.sprite.setTexture(texVeg); p.sprite.setTextureRect({0,128,32,32}); p.sprite.setOrigin(16,24); p.sprite.setScale(2.5f,2.5f); p.sprite.setPosition(x,y); p.isBush=true; p.hp=30; p.solid=true; p.hitbox={x-30,y-20,60,40}; groundProps.push_back(p); 
}
void Game::spawnFlower() { 
    float x = 0.f, y = 0.f; int maxAttempts = 20;
    while(maxAttempts--) { x = (float)(rand()%(int)mapW()); y = (float)(rand()%(int)mapH()); if (isBuildable(x, y)) break; }
    Prop p; p.sprite.setTexture(texVeg); p.sprite.setTextureRect({128+(rand()%3)*16,0,16,16}); p.sprite.setOrigin(8,8); p.sprite.setScale(3.0f,3.0f); p.sprite.setPosition(x, y); groundProps.push_back(p); 
}
void Game::spawnStone() { spawnStoneProcedural(player.getPosition()); }
void Game::spawnStoneProcedural(sf::Vector2f playerSpawn) { 
    float x = 0.f, y = 0.f; bool found = false;
    for (int i = 0; i < 200; i++) {
        x = (float)(rand() % (int)mapW());
        y = (float)(rand() % (int)mapH());
        if (isValidSpawn(x, y, 30.f, playerSpawn)) { found = true; break; }
    }
    if (!found) {
        for (int i = 0; i < 100; i++) {
            x = (float)(rand() % (int)mapW());
            y = (float)(rand() % (int)mapH());
            if (isBuildable(x, y)) { found = true; break; }
        }
    }
    if (!found) return;

    Prop p; p.sprite.setTexture(texRock); p.sprite.setTextureRect({32, 0, 32, 32}); p.sprite.setOrigin(16, 16); p.sprite.setScale(2.5f, 2.5f); p.sprite.setPosition(x, y); p.isStone = true; groundProps.push_back(p); 
}
void Game::spawnFence(sf::Vector2f pos) { Prop p; p.sprite.setTexture(texWalls); p.sprite.setTextureRect({16,128,16,16}); p.sprite.setOrigin(8,8); p.sprite.setScale(TILE_SCALE,TILE_SCALE); p.sprite.setPosition(pos); p.isFence=true; p.solid=true; p.hitbox={pos.x-32,pos.y-32,64,64}; groundProps.push_back(p); }
void Game::spawnRuin(sf::Vector2f pos) { Prop p; p.sprite.setTexture(texWalls); p.sprite.setTextureRect({0,0,48,48}); p.sprite.setOrigin(24,24); p.sprite.setScale(TILE_SCALE,TILE_SCALE); p.sprite.setPosition(pos); p.solid=true; p.hitbox={pos.x-80,pos.y-80,160,160}; groundProps.push_back(p); }

void Game::onEnemyKilled()
{
    killCount++;
    enemiesKilledInWave++;
    if (currentLevel == 2) missionSystem.updateMission("Kill Enemies", 1);
    if (currentLevel == 3) missionSystem.updateMission("Assault Forces", 1);
}

void Game::onAllWavesCleared() { missionSystem.updateMission("Survive Waves", 1); }

void Game::update(float dt)
{
    if (currentState == GameState::MENU) return;
    if (gameOver) return;
    if (currentState == GameState::DIALOGUE) {
        dialogueManager.update();
        return;
    }
    if (levelComplete) {
        levelCompleteTimer -= dt;
        if (levelCompleteTimer <= 0.f) {
            levelComplete = false;
            if (currentLevel < MAX_LEVEL) { 
                currentLevel++; 
                maxLevelUnlocked = std::max(maxLevelUnlocked, currentLevel);
                loadLevel(currentLevel); 
                SaveManager::saveGame("savegame.json", currentLevel, maxLevelUnlocked, player, groundProps);
            }
            else { gameOver = true; gameOverText.setString("YOU WIN! All Levels Clear!\nPress R to Restart"); gameOverText.setFillColor(sf::Color::Yellow); }
        }
        return;
    }
    player.update(dt);
    player.stats.updateSurvival(dt);
    camera.update(dt);
    camera.followPlayer(player);
    sf::Vector2f pp = player.getPosition();
    pp.x = std::max(0.f, std::min(mapW(), pp.x)); pp.y = std::max(0.f, std::min(mapH(), pp.y));
    player.setPosition(pp.x, pp.y);

    updateFarms(dt); updateEnemies(dt); updateHarvesting(dt); updateCollisions(); updateInteractions(); updateParticles(dt); spawnAmbientParticles(dt);

    static float rockSpawnTimer = 0; rockSpawnTimer += dt;
    if(rockSpawnTimer > 30.0f) { spawnRock(); if(rand()%2==0) spawnStone(); rockSpawnTimer = 0; }

    if (!pendingProps.empty()) {
        groundProps.insert(groundProps.end(), pendingProps.begin(), pendingProps.end());
        pendingProps.clear();
    }

    if (player.stats.isDead()) gameOver = true;

    int liveEnemies = 0;
    for(auto& e : enemies) if(!e.isDead()) liveEnemies++;
    if (liveEnemies == 0 && !levelComplete) {
        if (wave >= wavesPerLevel[currentLevel]) {
            // Last wave cleared
            static bool waveMissionDone = false;
            if (enemiesKilledInWave >= totalEnemiesInWave && !waveMissionDone) {
                onAllWavesCleared();
                waveMissionDone = true;
            }
            
            if (missionSystem.allMissionsCompleted()) {
                levelComplete = true;
                levelCompleteTimer = LEVEL_COMPLETE_DELAY;
                waveMissionDone = false; // Reset for next level
            }
        } else {
            // Progress to next wave if current one is fully cleared
            if (enemiesKilledInWave >= totalEnemiesInWave) {
                wave++;
                spawnWave(wave);
            }
        }
    }

    // Dynamic Mission Description Update for Waves
    std::string waveInfo = "Wave " + std::to_string(wave) + "/" + std::to_string(wavesPerLevel[currentLevel]) + 
                          ": " + std::to_string(enemiesKilledInWave) + "/" + std::to_string(totalEnemiesInWave) + " defeated";
    missionSystem.updateDescription("Survive Waves", waveInfo);
}

void Game::updateFarms(float dt)
{
    for (auto& p : groundProps) {
        if (!p.isStump && (p.isTree || p.isRock || p.isBush)) {
            if (p.sprite.getColor().r == 200) {
                p.animT += dt;
                if (p.animT > 0.15f) {
                    p.sprite.setColor(sf::Color::White);
                    p.animT = 0.f;
                }
            }
        }
        if (p.isFarm && p.growthStage < 2) {
            p.growthTimer += dt;
            if (p.growthTimer > 8.0f) {
                p.growthStage++; p.growthTimer = 0.f;
                if (p.growthStage == 1) p.sprite.setTextureRect({16, 16, 16, 16}); else p.sprite.setTextureRect({48, 16, 16, 16});
            }
        }
        if (p.isStation) {
            p.animT += dt;
            float animSpeed = 0.12f; int maxF = 4, size = 32;
            if (p.stationType == 2) { size = 64; maxF = 8; } else if (p.stationType == 1 || p.stationType == 0) { size = 32; maxF = 6; }
            if(p.animT > animSpeed) { p.animF = (p.animF + 1) % maxF; p.animT = 0.f; p.sprite.setTextureRect({p.animF * size, 0, size, size}); }
        }
    }
}

void Game::updateEnemies(float dt)
{
    enemyDmgTimer += dt;
    auto it = enemies.begin();
    while(it != enemies.end()) {
        it->setTarget(player.getPosition()); it->update(dt);
        if (it->isAnimFinished()) { it = enemies.erase(it); continue; }
        
        if (!it->isDead()) {
            float dx = player.getPosition().x - it->getPosition().x;
            float dy = player.getPosition().y - it->getPosition().y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (it->popShootRequest()) {
                // Wizzard fired a projectile
                float projSpeed = 400.f;
                sf::Vector2f vel(dx / dist * projSpeed, dy / dist * projSpeed);
                projectiles.emplace_back(it->getPosition().x, it->getPosition().y - 20.f, vel, 2.5f, 15);
            }

            // Synchronization of damage with attack animation
            if (it->getState() == Enemy::ATTACK && it->getAnimFrame() == 3 && dist < 60.f && enemyDmgTimer > 0.8f) {
                int damage = 12; if (player.inventory.getArmorLevel() > 0) damage -= 4; if(damage < 4) damage = 4;
                player.stats.modifyHealth(-damage); enemyDmgTimer = 0.f; camera.shake(12, 0.25f);
            }

            int playerDmg = player.getAttackPower() * player.inventory.getWeaponLevel();
            if (player.getState() == Player::ATTACKING && dist < 100.f) {
                bool wasDead = it->isDead(); it->takeDamage(playerDmg); spawnBlood(it->getPosition());
                if (!wasDead && it->isDead()) { 
                    onEnemyKilled(); 
                    EventManager::getInstance().trigger(EventType::OnEnemyKilled);
                    if(rand()%8 == 0) spawnDrop(it->getPosition(), false, false, false, false, (rand()%2==0), (rand()%2==0)); 
                }
            }
        }
        it++;
    }
}

void Game::updateHarvesting(float dt)
{
    int attackDmg = player.getAttackPower();
    if (attackDmg <= 0) return;

    sf::FloatRect hitArea = player.getSprite().getGlobalBounds();
    hitArea.left -= 80; hitArea.top -= 80; hitArea.width += 160; hitArea.height += 160;

    Prop* targetProp = nullptr;
    float minDist = 10000.f;

    for (auto& p : groundProps) {
        if ((p.isTree || p.isRock || p.isBush) && !p.isStump && p.hp > 0) {
            float dist = std::sqrt(std::pow(p.sprite.getPosition().x - player.getPosition().x, 2) + 
                                   std::pow(p.sprite.getPosition().y - player.getPosition().y, 2));
            if (hitArea.intersects(p.sprite.getGlobalBounds()) && dist < minDist) {
                minDist = dist;
                targetProp = &p;
            }
        }
    }

    if (targetProp) {
        if (targetProp->isTree) {
            player.triggerHarvest(); targetProp->hp -= 20; spawnLeaf(targetProp->sprite.getPosition());
            targetProp->sprite.setColor(sf::Color(200, 100, 100)); targetProp->animT = 0.f;
            if(targetProp->hp <= 0) {
                player.inventory.addItem("wood", 5);
                EventManager::getInstance().trigger(EventType::OnTreeCut, (void*)5);
                targetProp->isStump = true; targetProp->solid = true;  
                // Represent stump as bottom 64px of the tree variant
                targetProp->sprite.setTextureRect({targetProp->variant * TREE_W, TREE_H - 64, TREE_W, 64});
                targetProp->sprite.setOrigin(TREE_W / 2.f, 64 - 16.f);
                targetProp->sprite.setColor(sf::Color::White);
                targetProp->hitbox = {targetProp->sprite.getPosition().x - 40, targetProp->sprite.getPosition().y - 20, 80, 40};
            }
        } else if (targetProp->isRock) {
            player.triggerCrush(); targetProp->hp -= 20; targetProp->sprite.setColor(sf::Color(200, 100, 100)); targetProp->animT = 0.f;
            if(targetProp->hp <= 0) {
                player.inventory.addItem("stone", 5);
                targetProp->isRock = false; targetProp->solid = false; targetProp->sprite.setColor({50,50,50,100});
                
                // Triggers an event indicating collection instead of specifically updating Mission directly
                EventManager::getInstance().trigger(EventType::OnItemCollected);
            }
        }
        camera.shake(10, 0.15f);
    }
}

void Game::updateCollisions()
{
    // 1. Map Boundary / Water Collision (Strict Tile Validation)
    sf::Vector2f pPos = player.getPosition();
    sf::FloatRect pBounds = player.getSprite().getGlobalBounds();
    sf::FloatRect pFeet = {pBounds.left + 20, pBounds.top + pBounds.height - 30, pBounds.width - 40, 20};

    // Check corners of feet hitbox against map tiles
    float cornersX[] = {pFeet.left, pFeet.left + pFeet.width};
    float cornersY[] = {pFeet.top, pFeet.top + pFeet.height};
    
    bool stuck = false;
    for (float cx : cornersX) {
        for (float cy : cornersY) {
            if (!isBuildable(cx, cy)) stuck = true;
        }
    }
    if (stuck) {
        // Push back towards map center (quick & dirty fix for tile collision)
        sf::Vector2f center(mapW()/2.f, mapH()/2.f);
        sf::Vector2f dir = center - pPos;
        float dist = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (dist > 0) player.setPosition(pPos.x + (dir.x/dist)*5.f, pPos.y + (dir.y/dist)*5.f);
    }

    // 2. Prop Collision (Player)
    for (auto& g : groundProps) {
        if (g.solid) {
            if (pFeet.intersects(g.hitbox)) {
                sf::Vector2f gCenter(g.hitbox.left + g.hitbox.width/2.f, g.hitbox.top + g.hitbox.height/2.f);
                sf::Vector2f pCenter(pFeet.left + pFeet.width/2.f, pFeet.top + pFeet.height/2.f);
                sf::Vector2f diff = pCenter - gCenter;
                
                // Normalizing push out
                float d = std::sqrt(diff.x*diff.x + diff.y*diff.y);
                if (d > 0) {
                    player.setPosition(player.getPosition().x + (diff.x/d)*6.f, player.getPosition().y + (diff.y/d)*6.f);
                }
            }
        }
    }

    // 3. Prop Collision (Enemies)
    for (auto& e : enemies) {
        if (e.isDead()) continue;
        sf::FloatRect eBounds = e.getSprite().getGlobalBounds();
        sf::FloatRect eFeet = {eBounds.left + 15, eBounds.top + eBounds.height - 25, eBounds.width - 30, 15};
        
        for (auto& g : groundProps) {
            if (g.solid && eFeet.intersects(g.hitbox)) {
                 sf::Vector2f gCenter(g.hitbox.left + g.hitbox.width/2.f, g.hitbox.top + g.hitbox.height/2.f);
                 sf::Vector2f eCenter(eFeet.left + eFeet.width/2.f, eFeet.top + eFeet.height/2.f);
                 sf::Vector2f diff = eCenter - gCenter;
                 float d = std::sqrt(diff.x*diff.x + diff.y*diff.y);
                 if (d > 0) e.setPosition(e.getPosition().x + (diff.x/d)*4.f, e.getPosition().y + (diff.y/d)*4.f);
            }
        }
    }
}

void Game::updateInteractions()
{
    static bool fP=0,bP=0,cP=0,gP=0,eP=0,qP=0,sP=0,vP=0, tabP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) { if(!tabP){ craftingMenuOpen = !craftingMenuOpen; tabP=1; } } else tabP=0;
    if (craftingMenuOpen) return; // Freeze interactions when UI is open

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) { if(!fP){ plantCrop();fP=1; } } else fP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { if(!eP && player.inventory.getItemCount("food") > 0){ player.stats.modifyHealth(20); player.inventory.removeItem("food"); player.triggerEat(); spawnHealParticles(player.getPosition()); eP=1; } } else eP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { if(!qP && player.inventory.getItemCount("water") > 0){ player.inventory.removeItem("water"); player.triggerDrink(); spawnHealParticles(player.getPosition()); qP=1; } } else qP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { if(!sP){ interactWithWorld();sP=1; } } else sP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) { if(!bP && player.inventory.getItemCount("wood") >= 50 && player.inventory.getItemCount("stone") >= 30){ spawnHouse(player.getPosition()); player.inventory.removeItem("wood", 50); player.inventory.removeItem("stone", 30); builtHouse=true; EventManager::getInstance().trigger(EventType::OnHouseBuilt); EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); bP=1; } } else bP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { if(!vP && player.inventory.getItemCount("wood") >= 20 && player.inventory.getItemCount("stone") >= 10){ buildFarm(); player.inventory.removeItem("wood", 20); player.inventory.removeItem("stone", 10); vP=1; } } else vP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) { if(!cP && player.inventory.getItemCount("stone") >= 40){ player.inventory.upgradeArmor(); player.inventory.removeItem("stone", 40); craftedArmor=true; EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); spawnHealParticles(player.getPosition()); cP=1; } } else cP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) { if(!gP && player.inventory.getItemCount("wood") >= 60 && player.inventory.getItemCount("stone") >= 40){ player.inventory.upgradeWeapon(); player.inventory.removeItem("wood", 60); player.inventory.removeItem("stone", 40); upgradedWeapon=true; EventManager::getInstance().trigger(EventType::OnWeaponUpgraded); EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); spawnHealParticles(player.getPosition()); gP=1; } } else gP=0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) { SaveManager::saveGame("savegame.json", currentLevel, maxLevelUnlocked, player, groundProps); }

    auto it = groundProps.begin();
    while (it != groundProps.end()) {
        if (it->isWood || it->isStone || it->isFood || it->isWater || it->isArmor || it->isWeaponLevel || it->isMetal) {
            if (player.getSprite().getGlobalBounds().intersects(it->sprite.getGlobalBounds())) {
                if (it->isWood) { player.inventory.addItem("wood"); EventManager::getInstance().trigger(EventType::OnTreeCut, (void*)1); }
                if (it->isStone) { player.inventory.addItem("stone"); EventManager::getInstance().trigger(EventType::OnItemCollected); }
                if (it->isFood) { player.inventory.addItem("food"); EventManager::getInstance().trigger(EventType::OnItemCollected); }
                if (it->isWater) player.inventory.addItem("water"); if (it->isArmor) player.inventory.upgradeArmor(); if (it->isWeaponLevel) player.inventory.upgradeWeapon();
                if (it->isMetal) { player.inventory.addItem("metal"); smelted=true; EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); }
                it = groundProps.erase(it); continue;
            }
        }
        it++;
    }
}

void Game::interactWithWorld()
{
    if (currentState == GameState::DIALOGUE) return;

    sf::FloatRect interactRange = player.getSprite().getGlobalBounds(); interactRange.left -= 150; interactRange.top -= 150; interactRange.width += 300; interactRange.height += 300;
    
    // NPC interactions
    for (auto& n : npcs) {
        if (interactRange.intersects(n.getSprite().getGlobalBounds())) {
            std::vector<std::string> sentences;
            sentences.push_back(n.getNextDialogue());
            dialogueManager.startDialogue(sentences);
            currentState = GameState::DIALOGUE;
            return;
        }
    }

    for (auto& p : groundProps) {
        if (interactRange.intersects(p.sprite.getGlobalBounds())) {
            if (p.isFarm && p.growthStage == 2) { spawnDrop(p.sprite.getPosition(), false, false, true, false, false, false, (rand()%2==0)); p.growthStage = 0; p.growthTimer = 0; p.sprite.setTextureRect({0, 16, 16, 16}); }
            if (p.isWell) { if(rand()%3==0) player.inventory.addItem("water"); }
            if (p.isStation) {
                if (p.stationType == 1 && player.inventory.getItemCount("stone") >= 5) { player.inventory.removeItem("stone", 5); player.inventory.addItem("metal"); smelted = true; EventManager::getInstance().trigger(EventType::OnMetalSmelted); EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); spawnHealParticles(p.sprite.getPosition()); }
                if (p.stationType == 0 && player.inventory.getItemCount("metal") >= 2) { player.inventory.removeItem("metal", 2); player.inventory.upgradeArmor(); craftedArmor = true; EventManager::getInstance().trigger(EventType::OnRecipeUnlocked); spawnHealParticles(p.sprite.getPosition()); }
            }
        }
    }
}

void Game::spawnHouse(sf::Vector2f pos)
{
    Prop wall, roof;
    wall.sprite.setTexture(texHouseWalls); wall.sprite.setPosition(pos); wall.sprite.setOrigin(48, 48); wall.sprite.setScale(4, 4); wall.solid = true; wall.isHouse = true; wall.hitbox = {pos.x-100, pos.y-50, 200, 100};
    roof.sprite.setTexture(texHouseRoofs); roof.sprite.setPosition(pos.x, pos.y - 120); roof.sprite.setOrigin(48, 48); roof.sprite.setScale(4.5f, 4.5f);
    groundProps.push_back(wall); overheadProps.push_back(roof);
}

void Game::spawnWell(sf::Vector2f pos)
{
    Prop w; w.sprite.setTexture(texRock); w.sprite.setTextureRect({64, 0, 32, 32}); w.sprite.setScale(4,4); w.sprite.setOrigin(16,16); w.sprite.setPosition(pos); w.solid = true; w.isWell = true; w.hitbox = {pos.x-60, pos.y-60, 120, 120};
    groundProps.push_back(w);
}

void Game::buildFarm()
{
    sf::Vector2f pos = player.getPosition();
    for(int y=-1; y<=1; y++) {
        for(int x=-1; x<=1; x++) {
            Prop f; f.sprite.setTexture(texFarm); f.sprite.setTextureRect({0, 16, 16, 16}); f.sprite.setScale(TILE_SCALE, TILE_SCALE); f.sprite.setOrigin(8, 8); f.sprite.setPosition(pos.x + x*TILE_PX*TILE_SCALE, pos.y + y*TILE_PX*TILE_SCALE);
            f.isFarm = true; f.growthStage = 0; f.growthTimer = 0;
            groundProps.push_back(f);
        }
    }
    if (currentLevel == 2) {
        missionSystem.updateMission("Establish Farm", 1);
        EventManager::getInstance().trigger(EventType::OnFarmBuilt);
    }
}

void Game::spawnStation(sf::Vector2f pos, int type)
{
    Prop s; s.isStation = true; s.stationType = type; s.solid = true; s.sprite.setPosition(pos); s.sprite.setScale(TILE_SCALE, TILE_SCALE);
    if(type==0) { s.sprite.setTexture(texAnvil); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
    if(type==1) { s.sprite.setTexture(texFurnace); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
    if(type==2) { s.sprite.setTexture(texSawmill); s.sprite.setTextureRect({0,0,64,64}); s.sprite.setOrigin(32,48); }
    if(type==3) { s.sprite.setTexture(texWorkbench); s.sprite.setTextureRect({0,0,32,32}); s.sprite.setOrigin(16,24); }
    s.hitbox = {pos.x-60, pos.y-40, 120, 80};
    groundProps.push_back(s);
}

void Game::spawnFurniture(sf::Vector2f pos)
{
    Prop f; f.isFurniture = true; f.sprite.setTexture(texFurniture); f.sprite.setPosition(pos); f.sprite.setScale(TILE_SCALE, TILE_SCALE);
    int tx = (rand()%4)*32; int ty = (rand()%4)*32; f.sprite.setTextureRect({tx, ty, 32, 32}); f.sprite.setOrigin(16, 24); f.solid = true; f.hitbox = {pos.x-40, pos.y-30, 80, 60};
    groundProps.push_back(f);
}

void Game::plantCrop()
{
    Prop f; f.sprite.setTexture(texFarm); f.sprite.setTextureRect({0, 16, 16, 16}); f.sprite.setScale(TILE_SCALE, TILE_SCALE); f.sprite.setOrigin(8, 8); f.sprite.setPosition(player.getPosition());
    f.isFarm = true; f.growthStage = 0; f.growthTimer = 0;
    groundProps.push_back(f);
}

void Game::spawnDrop(sf::Vector2f pos, bool iw, bool is, bool ifo, bool iwa, bool ia, bool iwl, bool isFruit, bool im)
{
    Prop d; d.sprite.setTexture(texRes); d.sprite.setPosition(pos.x + rand()%40-20, pos.y + rand()%40-20); d.sprite.setScale(3,3); d.sprite.setOrigin(8,8);
    if(iw) { d.isWood = true; d.sprite.setTextureRect({0,48,16,16}); }
    if(is) { 
        d.isStone = true; d.sprite.setTexture(texRock); d.sprite.setTextureRect({32,0,32,32}); d.sprite.setOrigin(16,16); d.sprite.setScale(1.5f, 1.5f);
    }
    if(ifo) { d.isFood = true; d.sprite.setTextureRect({32,48,16,16}); }
    if(iwa) { d.isWater = true; d.sprite.setTextureRect({48,48,16,16}); }
    if(ia) { d.isArmor = true; d.sprite.setTextureRect({64,0,16,16}); d.sprite.setColor(sf::Color::Cyan); }
    if(iwl) { d.isWeaponLevel = true; d.sprite.setTextureRect({80,0,16,16}); d.sprite.setColor(sf::Color::Yellow); }
    if(isFruit) { d.isFood = true; d.sprite.setTextureRect({32, 16, 16, 16}); d.sprite.setColor(sf::Color::Red); } 
    if(im) { d.isMetal = true; d.sprite.setTextureRect({64, 16, 16, 16}); d.sprite.setColor(sf::Color(200,200,200)); }
    pendingProps.push_back(d);
}

void Game::updateParticles(float dt)
{
    auto it = particles.begin();
    while(it != particles.end()) {
        if(!(*it)->update(dt)) it = particles.erase(it);
        else it++;
    }

    auto projIt = projectiles.begin();
    while(projIt != projectiles.end()) {
        projIt->life -= dt;
        projIt->shape.move(projIt->vel * dt);

        float dx = projIt->shape.getPosition().x - player.getPosition().x;
        float dy = projIt->shape.getPosition().y - player.getPosition().y;
        if (std::sqrt(dx*dx + dy*dy) < 30.f && projIt->active) {
            int damage = projIt->dmg; if (player.inventory.getArmorLevel() > 0) damage -= 4; if(damage < 4) damage = 4;
            player.stats.modifyHealth(-damage); camera.shake(8, 0.2f);
            projIt->active = false;
        }

        if(projIt->life <= 0 || !projIt->active) projIt = projectiles.erase(projIt);
        else projIt++;
    }
}

void Game::spawnBlood(sf::Vector2f pos) { for(int i=0; i<5; i++) particles.push_back(std::make_shared<Particle>(pos.x, pos.y, sf::Color::Red, 3, (rand()%100-50), (rand()%100-50), 0.5f)); }
void Game::spawnLeaf(sf::Vector2f pos) { for(int i=0; i<3; i++) particles.push_back(std::make_shared<Particle>(pos.x, pos.y, sf::Color(50, 200, 50), 4, (rand()%60-30), (rand()%60-30), 1.0f)); }
void Game::spawnHealParticles(sf::Vector2f pos) { for(int i=0; i<10; i++) particles.push_back(std::make_shared<Particle>(pos.x, pos.y, sf::Color::Green, 4, (rand()%100-50), (rand()%100-50), 0.8f)); }

void Game::spawnAmbientParticles(float dt)
{
    static float timer = 0; timer += dt;
    if(timer > 0.1f) {
        float rx = camera.getCenter().x + (rand()%1280 - 640); float ry = camera.getCenter().y + (rand()%720 - 360);
        particles.push_back(std::make_shared<Particle>(rx, ry, sf::Color(255,255,200,100), 2, (rand()%20-10), (rand()%20-10), 3.0f));
        timer = 0;
    }
}

void Game::restartGame()
{
    gameOver = false;
    maxLevelUnlocked = 1; // Reset progression for a true "New Game"
    player.stats.setHealth(player.stats.getMaxHealth()); 
    player.inventory.setItem("food", 10);
    player.inventory.setItem("water", 10);
    player.inventory.setItem("wood", 0);
    player.inventory.setItem("stone", 0);
    player.inventory.setItem("metal", 0);
    player.inventory.setArmorLevel(0);
    player.inventory.setWeaponLevel(1);
    currentLevel = 1;
    gameOverText.setString("ADVENTURE ENDED\nPress R to Restart");
    gameOverText.setFillColor(sf::Color::Red);
    loadLevel(1);
}

void Game::draw()
{
    if (currentState == GameState::MENU || currentState == GameState::LEVEL_SELECT) {
        drawMenu();
        return;
    }
    window.clear(sf::Color(30, 80, 40));
    camera.apply(window);
    window.draw(mapGround); window.draw(mapWater); window.draw(mapRiversideWalls);

    std::vector<sf::Drawable*> drawList;
    struct YSort { sf::Drawable* d; float y; };
    std::vector<YSort> sorted;
    for(auto& p : groundProps) sorted.push_back({&p.sprite, p.sprite.getPosition().y});
    for(auto& e : enemies) sorted.push_back({&e.getSprite(), e.getPosition().y});
    for(auto& n : npcs) sorted.push_back({&n.getSprite(), n.getPosition().y});
    sorted.push_back({&player.getSprite(), player.getPosition().y});

    std::sort(sorted.begin(), sorted.end(), [](const YSort& a, const YSort& b){ return a.y < b.y; });
    for(auto& s : sorted) window.draw(*s.d);
    for(auto& n : npcs) n.draw(window);

    for(auto& e : enemies) e.draw(window);
    for(auto& p : overheadProps) window.draw(p.sprite);
    for(auto& p : particles) window.draw(p->shape);
    for(auto& pr : projectiles) window.draw(pr.shape);

    drawHUD();
    drawMiniMap();
    if (currentState == GameState::DIALOGUE) dialogueManager.draw(window);
    window.display();
}

void Game::drawHUD()
{
    window.setView(window.getDefaultView());
    
    // Health Bar
    playerHpFg.setSize({250.f * (player.stats.getHealth() / player.stats.getMaxHealth()), 25.f});
    window.draw(playerHpBg); window.draw(playerHpFg);
    playerHpText.setString("HP: " + std::to_string(static_cast<int>(player.stats.getHealth())));
    window.draw(playerHpText);

    // Hunger Bar
    playerHungerFg.setSize({250.f * (player.stats.getHunger() / player.stats.getMaxHunger()), 20.f});
    window.draw(playerHungerBg); window.draw(playerHungerFg);
    playerHungerText.setString("Hunger: " + std::to_string(static_cast<int>(player.stats.getHunger())));
    window.draw(playerHungerText);

    // Stamina Bar
    playerStaminaFg.setSize({250.f * (player.stats.getStamina() / player.stats.getMaxStamina()), 20.f});
    window.draw(playerStaminaBg); window.draw(playerStaminaFg);
    playerStaminaText.setString("Stamina: " + std::to_string(static_cast<int>(player.stats.getStamina())));
    window.draw(playerStaminaText);

    // Top Level Info Bar
    window.draw(levelInfoBarBg);
    std::string levelName = (currentLevel == 1) ? "The Village" : (currentLevel == 2 ? "Dark Forest" : "The Dungeon");
    levelText.setCharacterSize(18);
    levelText.setString("LEVEL " + std::to_string(currentLevel) + ": " + levelName);
    levelText.setPosition(20, 8);
    window.draw(levelText);

    // Wave text is now handled inside the Mission Log panel


    // Bottom Inventory Dashboard
    window.draw(inventoryDashboardBg);
    invText.setCharacterSize(16);
    invText.setString("RESOURCES:  Wood: " + std::to_string(player.inventory.getItemCount("wood")) + 
                      " | Stone: " + std::to_string(player.inventory.getItemCount("stone")) + 
                      " | Metal: " + std::to_string(player.inventory.getItemCount("metal")) +
                      " | Food: " + std::to_string(player.inventory.getItemCount("food")) + 
                      " | Water: " + std::to_string(player.inventory.getItemCount("water")) +
                      " | Armor: Lvl " + std::to_string(player.inventory.getArmorLevel()) + 
                      " | Weapon: Lvl " + std::to_string(player.inventory.getWeaponLevel()));
    
    sf::FloatRect itb = invText.getLocalBounds();
    invText.setOrigin(itb.width/2, itb.height/2);
    invText.setPosition(640, 685);
    window.draw(invText);

    window.draw(controlsText);
    if (gameOver) window.draw(gameOverText);

    missionSystem.draw(window);

    if (levelComplete) {
        std::string lvlNames[] = {"", "The Village", "Dark Forest", "The Dungeon"};
        if (currentLevel < MAX_LEVEL) levelCompleteText.setString("LEVEL " + std::to_string(currentLevel) + " COMPLETE!\n\nEntering: Level " + std::to_string(currentLevel + 1) + " - " + lvlNames[currentLevel + 1]);
        else levelCompleteText.setString("ALL LEVELS COMPLETE!\n\nYOU ARE A LEGEND!");
        sf::FloatRect tb = levelCompleteText.getLocalBounds(); levelCompleteText.setOrigin(tb.width / 2, tb.height / 2); levelCompleteText.setPosition(640, 320);
        window.draw(levelCompleteBg); window.draw(levelCompleteText);
    }

    if (craftingMenuOpen) {
        window.draw(craftingOverlayBg);
        window.draw(craftingHeaderText);
        
        float yPos = 240.f;
        for (const auto& recipe : loadedRecipes) {
            sf::Text rx; rx.setFont(font); rx.setCharacterSize(18); rx.setFillColor(sf::Color(200, 255, 200));
            std::string reqs = recipe.name + " -> ";
            for (auto& [reqName, reqAmt] : recipe.requirements) reqs += reqName + ":" + std::to_string(reqAmt) + " ";
            rx.setString(reqs); rx.setPosition(370, yPos);
            window.draw(rx);
            yPos += 30.f;
        }
    }
}

void Game::drawMiniMap()
{
    // Mini-map setup
    sf::View miniView;
    miniView.setViewport(sf::FloatRect(0.75f, 0.75f, 0.2f, 0.2f)); // Bottom right
    miniView.setCenter(player.getPosition());
    miniView.setSize(2000, 2000); // Zoomed out view
    window.setView(miniView);

    // Draw simplified map elements
    window.draw(mapGround);
    
    // Draw player as a yellow dot
    sf::CircleShape playerDot(40.f);
    playerDot.setFillColor(sf::Color::Yellow);
    playerDot.setOrigin(40.f, 40.f);
    playerDot.setPosition(player.getPosition());
    window.draw(playerDot);

    // Draw enemies as red dots
    for (const auto& e : enemies) {
        if (!e.isDead()) {
            sf::CircleShape enemyDot(30.f);
            enemyDot.setFillColor(sf::Color::Red);
            enemyDot.setOrigin(30.f, 30.f);
            enemyDot.setPosition(e.getPosition());
            window.draw(enemyDot);
        }
    }

    window.setView(window.getDefaultView()); // Reset
}

void Game::drawMenu()
{
    window.clear(sf::Color(20,20,30));
    window.draw(menuTitle);

    if (currentState == GameState::MENU) {
        for(int i=0; i<3; i++) {
            if(i == menuIndex) {
                menuOptions[i].setFillColor(sf::Color::Yellow);
                menuOptions[i].setScale(1.2f, 1.2f);
            } else {
                menuOptions[i].setFillColor(sf::Color::White);
                menuOptions[i].setScale(1.0f, 1.0f);
            }
            window.draw(menuOptions[i]);
        }
    } else if (currentState == GameState::LEVEL_SELECT) {
        for (int i=1; i<=MAX_LEVEL; i++) {
            sf::Text lvlTxt = menuOptions[0];
            lvlTxt.setString("Level " + std::to_string(i));
            lvlTxt.setPosition(640, 350 + (i-1)*80);
            
            if (i == currentLevel) {
                lvlTxt.setFillColor(sf::Color::Yellow);
                lvlTxt.setScale(1.2f, 1.2f);
            } else if (i > maxLevelUnlocked) {
                lvlTxt.setFillColor(sf::Color(100, 100, 100)); // Locked
                lvlTxt.setScale(1.0f, 1.0f);
            } else if (i < maxLevelUnlocked) {
                lvlTxt.setFillColor(sf::Color::Green); // Completed
                lvlTxt.setScale(1.0f, 1.0f);
            } else {
                lvlTxt.setFillColor(sf::Color::White); // Current highest but not hovered
                lvlTxt.setScale(1.0f, 1.0f);
            }
            window.draw(lvlTxt);
        }
        
        sf::Text helpTxt = menuOptions[0];
        helpTxt.setCharacterSize(20);
        helpTxt.setString("Up/Down to Select | Enter to Play | Esc to Back");
        helpTxt.setPosition(640, 650);
        helpTxt.setFillColor(sf::Color::Cyan);
        helpTxt.setScale(1.0f, 1.0f);
        window.draw(helpTxt);

        // Dashboard in Level Select
        window.draw(inventoryDashboardBg);
        invText.setString("CURRENT INVENTORY:  Wood: " + std::to_string(player.inventory.getItemCount("wood")) + 
                          " | Stone: " + std::to_string(player.inventory.getItemCount("stone")) + 
                          " | Metal: " + std::to_string(player.inventory.getItemCount("metal")) +
                          " | Food: " + std::to_string(player.inventory.getItemCount("food")) + 
                          " | Water: " + std::to_string(player.inventory.getItemCount("water")));
        sf::FloatRect itb = invText.getLocalBounds();
        invText.setOrigin(itb.width/2, itb.height/2);
        invText.setPosition(640, 685);
        window.draw(invText);
    }
    
    window.display();
}

bool Game::isValidSpawn(float x, float y, float minDist, sf::Vector2f playerSpawn) const {
    if (!isBuildable(x, y)) return false;

    // Check distance from player spawn - Reduced to 250px
    float dx = x - playerSpawn.x;
    float dy = y - playerSpawn.y;
    if (std::sqrt(dx*dx + dy*dy) < 250.f) return false;

    // Check against existing props
    for (auto& p : groundProps) {
        if (!p.solid && !p.isStone && !p.isTree && !p.isRock) continue;
        float px = p.sprite.getPosition().x;
        float py = p.sprite.getPosition().y;
        float pdx = x - px;
        float pdy = y - py;
        if (std::sqrt(pdx*pdx + pdy*pdy) < minDist) return false;
    }
    
    // Check against pending props
    for (auto& p : pendingProps) {
        if (!p.solid && !p.isStone && !p.isTree && !p.isRock) continue;
        float px = p.sprite.getPosition().x;
        float py = p.sprite.getPosition().y;
        float pdx = x - px;
        float pdy = y - py;
        if (std::sqrt(pdx*pdx + pdy*pdy) < minDist) return false;
    }

    return true;
}

bool Game::isBuildable(float x, float y) const {
    int tx = (int)(x / T());
    int ty = (int)(y / T());
    if (tx >= 0 && tx < MAP_COLS && ty >= 0 && ty < MAP_ROWS) {
        int tile = groundData[tx][ty];
        return (tile == T_GRASS || tile == T_DIRT);
    }
    return false;
}
