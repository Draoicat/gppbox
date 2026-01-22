#include "Enemy.hpp"

Enemy::Enemy(sf::Vector2f position, sf::Vector2f size) :
	Entity(position, size)
{
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(sf::Color::Red);
	set_grid_coordinates(position.x, position.y);
}

std::string Enemy::get_type_name() const
{
	return "Enemy";
}

void Enemy::update(double deltaTime)
{
	dx += (goLeft) ? -SPEED / 2 : SPEED / 2;
	Entity::update(deltaTime);
}

void Enemy::die()
{
	shouldDelete = true;
}

bool Enemy::check_left_collision()
{
	bool const result = Entity::check_left_collision();
	if (result) goLeft = false;
	return result;
}

bool Enemy::check_right_collision()
{
	bool const result = Entity::check_right_collision();
	if (result) goLeft = true;
	return result;
}
