#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

class Entity
{
public:
	sf::Shape* sprite;

	// Case Coordinates
	int cx{ 0 };
	int cy{ 0 };

	// Case Displacement Coordinates
	float rx{ 0.5f };
	float ry{ 0.5f };

	// Movement
	float dx{ 0.0f };
	float dy{ 0.0f };

	// Gravity
	float const GRAVITY_RATE{ 100.0f };
	bool has_gravity{ true };

	// Actions
	float const SPEED{ 10.0f };

	void jump();
	void stop_jump();
	bool is_jumping{ false };
	const float MIN_JUMP_FORCE{ 15.0f };
	const float MAX_JUMP_FORCE{ 40.0f };

	// Collisions
	bool checkLeftCollision();
	bool checkRightCollision();
	bool checkBottomCollision();
	bool checkTopCollision();

	Entity();
	Entity(sf::Shape* shape);
	
	void update(double deltaTime);
	void draw(sf::RenderWindow& win);

	void setPixelCoord(int px, int py);
	void setGridCoord(float coordX, float coordY);

	void syncPosition();

	void imGui();
};

#endif
