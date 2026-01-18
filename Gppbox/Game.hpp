#pragma once

#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"
#include "Entity.hpp"

#include "Particle.hpp"
#include "ParticleMan.hpp"

#include "C.hpp"

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

	std::vector<Entity*>			entities;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	bool isLevelEditorOn{ false };
	EditorMode levelEditorMode{ WALL };
	bool placeWallMode{ true };

	int enemyCount{ C::ENEMY_COUNT };

	Game(sf::RenderWindow * win);

	void cacheWalls();

	void processInput(sf::Event ev);
	bool wasPressed = false;
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);
	bool hasCollisions(float posX, float posY);

	void draw(sf::RenderWindow& win);

	bool isWall(int cx, int cy);
	void imGui(sf::RenderWindow& win);
	void addWall(float const x, float const y);
	void removeWall(float const x, float const y);
	void addEnemy(float const x, float const y);

	bool isGameOver{ false };
	void gameOver();
};
