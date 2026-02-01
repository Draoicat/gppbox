#pragma once

#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"
#include "Entity.hpp"

#include "ParticleMan.hpp"

#include "C.hpp"
#include "Pet.h"
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
	Pet* pet;
	std::vector<Entity*>			entities;

	double playerShootRatePerSeconds{8};
	bool canPlayerShoot{ true };
	double lastShotDeltaTime{ 0.0f };

	double petShootRatePerSeconds{0.5f};
	bool canPetShoot{true};
	double lastPetShortDeltaTime{ 0.0f };

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
	void updateView(View* view, double const dt); //kinda like a late update? to update the cam after the player moves.

	bool allowScreenshake{ true };
	bool isScreenshaking{ false };
	double screenshotStartTime;
	float const SCREENSHAKE_TIME_SECONDS{ 0.1f };
	void screenshake();

	bool hasCollisions(float posX, float posY);
	std::vector<Entity*> isOtherEntityPresent(string typeName, int x, int y);

	void draw(RenderWindow& win);

	bool isWall(int cx, int cy);
	void imGui(RenderWindow& win);

	// data manip

	// alors
	// en gros
	// vu que la caméra scroll, les calculs pour les positions dans le level editor fonctionnent plus
	// pour pas se compliquer la vie, je me suis dit, vu que la caméra se déplace par rapport à la pos du joueur
	// gardons le calcul initial, et trouvons l'offset pour réaligner le level editor avec la position de la souris dans l'écran
	// (comme si l'écran collait toujours avec la grille des positions, mais on la décale après)
	// donc j'ai mit des valeurs dans imgui et j'ai tweaké petit à petit jusqu'à trouver ces magnifiques nombres magiques.
	Vector2i const PLAYER_POS_OFFSET_WITH_INITIAL_CAMERA{32, 38};

	void loadPlayer(float const x, float const y);
	void loadPet(float const x, float const y);
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
	void petShoot(Entity* target);
	void death_ray();
	static std::vector<Vector2i> bresenham(Vector2i origin, Vector2i goal);
	double const DEATH_RAY_TIME_ON_SCREEN_SECONDS{ 0.2 };
	double const DEATH_RAY_TIMEOUT{ 10.0f };
	double lastDeathRayTime{ -DEATH_RAY_TIMEOUT };

	Vector2f const respawnPoint{3, 53};
	void respawn();
	

	Vector2i petOffset{4, 3};
	void petFollow(double const dt);
};
