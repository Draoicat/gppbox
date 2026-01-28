#include "PetProjectile.h"

PetProjectile::PetProjectile(sf::Vector2f position, sf::Vector2f size, bool left, sf::Vector2i directionToEnemy)
	: Projectile(position, size, left),
	directionToEnemy{directionToEnemy}
{

}

void PetProjectile::update(double deltaTime)
{
	dy += 2 * directionToEnemy.y * SPEED;
	Projectile::update(deltaTime);
}
