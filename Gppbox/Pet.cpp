#include "Pet.h"

Pet::Pet(sf::Vector2f position, sf::Vector2f size) :
	Entity(position, size)
{
	has_gravity = false;
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x,  size.y});
	sprite->setFillColor(sf::Color::White);
	set_grid_coordinates(position.x, position.y);
}

std::string Pet::get_type_name() const
{
	return "Pet";
}

void Pet::go_up()
{
	dy += (-SPEED / 16);
}

void Pet::go_down()
{
	dy += (SPEED / 16);
}