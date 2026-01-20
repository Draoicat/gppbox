#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
//#include <SFML/System/Vector2.hpp>

class Entity
{
public:

	enum EntityType
	{
		Player, Enemy, Projectile,
	};

	std::string get_type_name() const;

	EntityType type;

	sf::Shape* sprite;

	static int entityCount;
	int id{ 0 };

	// Case Coordinates
	int cx{ 0 };
	int cy{ 0 };

	// Case Displacement Coordinates
	float rx{ 0.5f };
	float ry{ 0.5f };

	// Scale
	int sx{ 1 };
	int sy{ 1 };

	// Movement
	float dx{ 0.0f };
	float dy{ 0.0f };

	// Gravity
	float const GRAVITY_RATE{ 100.0f };
	bool has_gravity{ true };

	// Actions
	static float const SPEED;
	bool goLeft{ true }; //enemy

	void jump();
	void stop_jump();
	bool is_jumping{ false };
	const float MIN_JUMP_FORCE{ 15.0f };
	const float MAX_JUMP_FORCE{ 40.0f };

	// Collisions
	bool hasCollidedThisFrame{ false };
	bool check_left_collision();
	bool check_right_collision();
	bool check_bottom_collision();
	bool check_top_collision();

	// Constructors
	Entity(EntityType type, sf::Vector2f position, sf::Vector2f size);
	Entity(EntityType type, sf::Vector2f position, sf::Vector2f size, sf::Shape* shape);
	
	void update(double deltaTime);
	void draw(sf::RenderWindow& win);

	void set_pixel_coordinates(int px, int py);
	void set_grid_coordinates(float xCoordinates, float yCoordinates);

	void synchronise_position();

	void im_gui();
};

#endif
