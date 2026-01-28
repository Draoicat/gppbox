#include "Projectile.hpp"

#include "Game.hpp"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f size, Vector2i givenDirection) :
	Entity(position, size)
{
	direction = givenDirection;
	facesLeft = false; //unused here
	has_gravity = false;
	sprite = new sf::CircleShape(size.x);
	sprite->setOrigin({size.x * 1.0f,  size.y * 1.5f});
	sprite->setFillColor(sf::Color::White);
	set_grid_coordinates(position.x, position.y);
	initialCx = cx;
}

void Projectile::update(double deltaTime)
{
	dx += 2 * direction.x * SPEED;
	dy += 2 * direction.y * SPEED;
	Entity::update(deltaTime);
	if (shouldDelete) return;
	shouldDelete = abs(cx - initialCx) > MAX_DISTANCE;
	if (hasRectangleShapeBeenInitialized) return;
	if (isProjectileFirstFrame)
	{
		isProjectileFirstFrame = false;
	}
	else
	{
		sprite = new RectangleShape({(float) C::GRID_SIZE * sx, (float) C::GRID_SIZE * sy});
		sprite->setOrigin({(float)sx * C::GRID_SIZE * 0.5f,  (float)sy*C::GRID_SIZE});
		sprite->setFillColor(sf::Color::Yellow);
		hasRectangleShapeBeenInitialized = true;
	}
}

bool Projectile::check_right_collision()
{
	if (Entity::check_right_collision()) 
		shouldDelete = true;
	return shouldDelete;
}

bool Projectile::check_left_collision()
{
	if (Entity::check_left_collision()) 
		shouldDelete = true;
	return shouldDelete;
}

bool Projectile::check_bottom_collision()
{
	return false;
}
