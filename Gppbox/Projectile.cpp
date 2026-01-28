#include "Projectile.hpp"

#include "Game.hpp"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f size, bool left) :
	Entity(position, size)
{
	facesLeft = left;
	has_gravity = false;
	sprite = new sf::CircleShape(25.0f);
	sprite->setOrigin({size.x * 2.0f,  size.y * 2.0f});
	sprite->setFillColor(sf::Color::White);
	set_grid_coordinates(position.x, position.y);
	initialCx = cx;
}

void Projectile::update(double deltaTime)
{
	dx += 2 * (facesLeft ? -SPEED : SPEED);
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

void Projectile::go_left()
{
	Entity::go_left();
	dx += -SPEED;
}

void Projectile::go_right()
{
	Entity::go_right();
	dx += SPEED;
}
