#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"

class Enemy :
    public Entity
{
public:
	bool goLeft{ true }; //enemy

	Enemy(sf::Vector2f position, sf::Vector2f size);

	std::string get_type_name() const override { return "Enemy"; }

	void update(double deltaTime) override;

	bool die(Entity* projectile);

protected:
	bool check_left_collision() override;
	bool check_right_collision() override;
	bool check_bottom_collision() override;

	Entity* checkForEntities(int x, int y);

public:
	void go_left() override;
	void go_right() override;
};

#endif