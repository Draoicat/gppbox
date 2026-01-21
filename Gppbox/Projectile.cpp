#include "Projectile.hpp"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f size) :
	Entity(position, size)
{
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(sf::Color::Yellow);
	set_grid_coordinates(position.x, position.y);
}

std::string Projectile::get_type_name() const
{
	return "Projectile";
}
