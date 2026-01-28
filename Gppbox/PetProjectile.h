#pragma once
#include "Projectile.hpp"

class PetProjectile :
    public Projectile
{
	sf::Vector2i directionToEnemy;

public:
    PetProjectile(sf::Vector2f position, sf::Vector2f size, bool left, sf::Vector2i directionToEnemy );
    void update(double deltaTime) override;
};

