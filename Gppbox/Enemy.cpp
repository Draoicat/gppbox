#include "Enemy.hpp"

#include "Game.hpp"

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
	facesLeft ? go_left() : go_right();
	Entity::update(deltaTime);
}

bool Enemy::die(Entity* projectile)
{
	if (projectile == nullptr) return false;
	shouldDelete = true;
	projectile->shouldDelete = true;
	return true;
}

bool Enemy::check_left_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		Entity* projectile = Game::instance->isOtherEntityPresent("Projectile", cx - 1, y);
		if (die(projectile)) return true;
	}
	bool const result = Entity::check_left_collision();
	if (result) facesLeft = false;
	return result;
}

bool Enemy::check_right_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		Entity* projectile = (Game::instance->isOtherEntityPresent("Projectile", cx + 1, y));
		if (die(projectile)) return true;
	}
	bool const result = Entity::check_right_collision();
	if (result) facesLeft = true;
	return result;
}

Entity* Enemy::checkForEntities(int x, int y)
{
	return Game::instance->isOtherEntityPresent("Projectile", x, y);
}

void Enemy::go_left()
{
	Entity::go_left();
	dx += SPEED /2;
}

void Enemy::go_right()
{
	Entity::go_right();
	dx += -SPEED / 2;
}
