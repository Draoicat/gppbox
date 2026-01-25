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

	RenderWindow*				win = nullptr;

	RectangleShape				bg;
	RectangleShape* deathRaySprite;
	HotReloadShader *				bgShader = nullptr;

	Texture						tex;

	bool							closing = false;
	
	std::vector<Vector2i>		walls;
	std::vector<RectangleShape> wallSprites;

	Player* player;
	std::vector<Entity*>			entities;

	double playerShootRatePerSeconds{8};
	bool canPlayerShoot{ true };
	double lastShotDeltaTime{ 0.0f };

	ParticleMan beforeParts;
	ParticleMan afterParts;

	bool isLevelEditorOn{ false };
	EditorMode levelEditorMode{ WALL };
	bool placeWallMode{ true };

	Game(RenderWindow * win);

	bool wasPressed = false;
	void processInput(Event ev);
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);
	bool hasCollisions(float posX, float posY);
	std::vector<Entity*> isOtherEntityPresent(string typeName, int x, int y);

	void draw(RenderWindow& win);

	bool isWall(int cx, int cy);
	void imGui(RenderWindow& win);

	// data manip
	void loadPlayer(float const x, float const y);
	void tryAddWall(float const x, float const y);
	void addWall(int const x, int const y);
	void tryRemoveWall(float const x, float const y);
	void removeWall(Vector2i const& wall);
	void cacheWalls();
	void tryAddEnemy(float const x, float const y);
	void addEnemy(float const x, float const y);

	void save() const;
	void load();

	//actions
	void shoot();
	void death_ray();
	static std::vector<Vector2i> bresenham(Vector2i origin, Vector2i goal);
	double const DEATH_RAY_TIME_ON_SCREEN_SECONDS{ 0.2 };
	double lastDeathRayTime{ 0.0 };
	bool isGameOver{ false };
	void gameOver();
};
