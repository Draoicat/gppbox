#include "Pet.h"

#include "Game.hpp"

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
	dy += (-SPEED / 3);
}

void Pet::go_down()
{
	dy += (SPEED / 3);
}


Entity* Pet::scan_for_enemies()
{
	std::vector<Entity*> enemies;
	for (int x = cx -ENEMY_RANGE; x < cx + ENEMY_RANGE; ++x)
	{
		for (int y = cy - ENEMY_RANGE; y < cy + ENEMY_RANGE; ++y)
		{
			enemies = Game::instance->isOtherEntityPresent("Enemy", x, y);
			if (!enemies.empty()) return enemies.at(0);
		}
	}
	return nullptr;
}
