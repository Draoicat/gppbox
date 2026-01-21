#include "Projectile.hpp"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f size) :
	Entity(position, size)
{
	has_gravity = false;
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(sf::Color::Yellow);
	set_grid_coordinates(position.x, position.y);
	initialCx = cx;
}

std::string Projectile::get_type_name() const
{
	return "Projectile";
}

void Projectile::update(double deltaTime)
{
	dx += SPEED * 2;
	Entity::update(deltaTime);
	if (abs(cx - initialCx) > 20);
		//delete projectile! how!;
}

bool Projectile::check_bottom_collision()
{
	return false;
}
