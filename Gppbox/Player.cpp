#include "Player.hpp"

#include "imgui.h"

Player::Player(sf::Vector2f position, sf::Vector2f size) :
	Entity(position, size)
{
	sprite = new sf::RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});
	sprite->setFillColor(sf::Color::White);
	set_grid_coordinates(position.x, position.y);
}

void Player::jump()
{
	if (!check_bottom_collision() || is_jumping) return;
	dy -= MAX_JUMP_FORCE;
	is_jumping = true;
}

void Player::stop_jump()
{
	if (!is_jumping || check_bottom_collision()) return;
	dy = 0;
}

void Player::calculateNextPosition(double deltaTime)
{
	Entity::calculateNextPosition(deltaTime);
	if (is_jumping && dy > 0) is_jumping = false;
}

bool Player::check_bottom_collision()
{
	if (is_jumping) return false;
	return Entity::check_bottom_collision();
}

void Player::im_gui()
{
	Entity::im_gui();
	
	if (ImGui::TreeNode((get_type_name() + " " + std::to_string(id)).c_str()))
	{
		ImGui::Value("Is Jumping", is_jumping);
		ImGui::TreePop();
	}
}

