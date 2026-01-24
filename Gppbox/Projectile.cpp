#include "Projectile.hpp"

#include "Game.hpp"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f size, bool left) :
	Entity(position, size)
{
	facesLeft = left;
	has_gravity = false;
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(sf::Color::Yellow);
	set_grid_coordinates(position.x, position.y);
	initialCx = cx;
}

void Projectile::update(double deltaTime)
{
	dx += 2 * (facesLeft ? -SPEED : SPEED);
	Entity::update(deltaTime);
	if (shouldDelete) return;
	shouldDelete = abs(cx - initialCx) > MAX_DISTANCE;
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
