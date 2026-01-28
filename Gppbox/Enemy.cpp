#include "Enemy.hpp"

#include "Game.hpp"

Enemy::Enemy(Vector2f position, Vector2f size) :
	Entity(position, size)
{
	sprite = new RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(Color::Red);
	set_grid_coordinates(position.x, position.y);
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

bool Enemy::check_bottom_collision()
{
	bool collides = Entity::check_bottom_collision();
	if (!Game::instance->hasCollisions(cx + (facesLeft ? -1 : 1), cy + 1))
	{
		facesLeft = !facesLeft;
	}
	return collides;
}

bool Enemy::check_left_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		std::vector<Entity*> projectiles = Game::instance->isOtherEntityPresent("Projectile", cx - 1, y);
		if (!projectiles.empty() && die(projectiles.at(0))) return true;
	}
	bool const result = Entity::check_left_collision();
	if (result) facesLeft = false;
	return result;
}

bool Enemy::check_right_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		std::vector<Entity*> projectiles = (Game::instance->isOtherEntityPresent("Projectile", cx + 1, y));
		if (!projectiles.empty() && die(projectiles.at(0))) return true;
	}
	bool const result = Entity::check_right_collision();
	if (result) facesLeft = true;
	return result;
}

Entity* Enemy::checkForEntities(int x, int y)
{
	//only one player in the gayme, so let's just return the first index
	return Game::instance->isOtherEntityPresent("Player", x, y).at(0);
}

void Enemy::go_left()
{
	Entity::go_left();
	dx += SPEED / 2;
}

void Enemy::go_right()
{
	Entity::go_right();
	dx += -SPEED / 2;
}
