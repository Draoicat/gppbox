#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"

class Player :
    public Entity
{
public:
	Player(sf::Vector2f position, sf::Vector2f size);

	void jump();
	void stop_jump();
	bool is_jumping{ false };
	const float MIN_JUMP_FORCE{ 15.0f };
	const float MAX_JUMP_FORCE{ 40.0f };

	std::string get_type_name() const override  { return "Player"; }

	void im_gui() override;
protected:

	void calculateNextPosition(double deltaTime) override;

	bool check_bottom_collision() override;
};

#endif PLAYER_HPP
