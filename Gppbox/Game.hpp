#pragma once

#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"
#include "Entity.hpp"

#include "ParticleMan.hpp"

#include "C.hpp"
#include "Player.hpp"

enum EditorMode 
{
	WALL = 0,
	ENEMY = 1
};

class HotReloadShader;
class Game {
public:
	static Game* instance;

	sf::RenderWindow*				win = nullptr;

	sf::RectangleShape				bg;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;
	
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::RectangleShape> wallSprites;

	Player* player;
	std::vector<Entity*>			entities;

	double playerShootRatePerSeconds{ 4 };
	bool canPlayerShoot{ true };
	double lastShotDeltaTime{ 0.0f };

	ParticleMan beforeParts;
	ParticleMan afterParts;

	bool isLevelEditorOn{ false };
	EditorMode levelEditorMode{ WALL };
	bool placeWallMode{ true };

	int enemyCount{ C::ENEMY_COUNT };

	Game(sf::RenderWindow * win);

	void cacheWalls();

	bool wasPressed = false;
	void processInput(sf::Event ev);
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);
	bool hasCollisions(float posX, float posY);
	Entity* isOtherEntityPresent(string typeName, int x, int y);

	void draw(sf::RenderWindow& win);

	bool isWall(int cx, int cy);
	void imGui(sf::RenderWindow& win);
	void addWall(float const x, float const y);
	void removeWall(float const x, float const y);
	void addEnemy(float const x, float const y);


	void shoot();
	bool isGameOver{ false };
	void gameOver();

	//Sprites
};
