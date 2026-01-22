#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

class Entity
{
public:
	virtual std::string get_type_name() const = 0;

	sf::Shape* sprite;

	static int entityCount;
	int id{ 0 };

	// Case Coordinates
	int cx{ 0 };
	int cy{ 0 };

	// Case Displacement Coordinates
	float rx{ 0.5f };
	float ry{ 0.5f };

	// Scale in case units
	int sx{ 1 };
	int sy{ 1 };

	// Movement
	float dx{ 0.0f };
	float dy{ 0.0f };
	bool facesLeft{ false };

	// Gravity
	float static constexpr GRAVITY_RATE{ 100.0f };
	bool has_gravity{ true };

	// Actions
	float static constexpr SPEED{ 15.0f };

	// Constructors & Destructors
	Entity(sf::Vector2f position, sf::Vector2f size);
	Entity();
	virtual ~Entity() = default;

	// Game Loop
	virtual void update(double deltaTime);
	void draw(sf::RenderWindow& win);
	virtual void im_gui();

protected:
	// Movement
	virtual void calculateNextPosition(double deltaTime);
	void handleCollisions();
	void set_pixel_coordinates(int px, int py);
	void set_grid_coordinates(float xCoordinates, float yCoordinates);

	void synchronise_position();

	// Collisions
	virtual bool check_left_collision();
	virtual bool check_right_collision();
	virtual bool check_bottom_collision();
	bool check_top_collision();
};

#endif
