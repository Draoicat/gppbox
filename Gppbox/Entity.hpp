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
	const float GRAVITY_RATE{ 0.05f };
	bool has_gravity{ false };

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
