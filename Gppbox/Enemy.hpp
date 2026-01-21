#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"

class Enemy :
    public Entity
{
public:
	bool goLeft{ true }; //enemy

	Enemy(sf::Vector2f position, sf::Vector2f size);

	std::string get_type_name() const override;

	void update(double deltaTime) override;

protected:
	bool check_left_collision() override;
	bool check_right_collision() override;

};

#endif