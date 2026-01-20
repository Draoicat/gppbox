#include <imgui.h>
#include <string>

#include "Entity.hpp"
#include "Game.hpp"

std::string Entity::get_type_name() const
{
	switch (this->type)
	{
	case Player:
		return "Player";
	case Enemy:
		return "Enemy";
	case Projectile:
		return "Projectile";
	}
	return "";
}

int Entity::entityCount = 0;
float const Entity::SPEED = 10.0f;

Entity::Entity(EntityType type, sf::Vector2f position, sf::Vector2f size)
{
	id = entityCount;
	++entityCount;
	sx = size.x / C::GRID_SIZE;
	sy = size.y / C::GRID_SIZE;
	this->type = type;

	sprite = new RectangleShape({size.x, size.y});
	sprite->setOrigin({size.x * 0.5f,  size.y});

	sprite->setFillColor(type != Enemy ? sf::Color::White : sf::Color::Red);
	sprite->setOutlineColor(sf::Color::Black);
	set_grid_coordinates(position.x, position.y);

	if (type == Projectile) has_gravity = false;
}

Entity::Entity(EntityType type, sf::Vector2f position, sf::Vector2f size, sf::Shape* sprite) : 
	sprite{sprite},
	type{type}
{
	id = entityCount;
	++entityCount;
	sx = size.x / C::GRID_SIZE;
	sy = size.y / C::GRID_SIZE;

	sprite->setOrigin({size.x * 0.5f,  size.y});
	set_grid_coordinates(position.x, position.y);
}

void Entity::jump()
{
	if (!check_bottom_collision() || is_jumping) return;
	dy -= MAX_JUMP_FORCE;
	is_jumping = true;
}

void Entity::stop_jump()
{
	if (!is_jumping || check_bottom_collision()) return;
	dy = 0;
}

bool Entity::check_left_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		if (Game::instance->hasCollisions(cx - 1, y) && rx <= 0.3f) return true;
	}
	return false;
}

bool Entity::check_right_collision()
{
	for (int y = cy; y > cy - sy; --y)
	{
		if (Game::instance->hasCollisions(cx + 1, y) && rx >= 0.7f) return true;
	}
	return false;
}

bool Entity::check_bottom_collision()
{
	return Game::instance->hasCollisions(cx, cy + 1);
}

bool Entity::check_top_collision()
{
	return Game::instance->hasCollisions(cx, cy - sy) && ry >= 0.98;
}

void Entity::update(double deltaTime)
{
	if (has_gravity) dy += GRAVITY_RATE * deltaTime;
	dy = clamp(dy, -150.0f, 150.0f);

	dx *= 0.70f;

	rx += dx * deltaTime;
	ry += dy * deltaTime;

	if (is_jumping && dy > 0) is_jumping = false;

	hasCollidedThisFrame = false;

	// collisions
	if (check_right_collision())
	{
		if (type == Enemy) goLeft = true;
		dx = 0.0f;
		rx = 0.7f;
		hasCollidedThisFrame = true;
	}
	else
	{
		if (rx > 1.0f)
		{
			--rx;
			++cx;
		}
	}

	if (check_left_collision())
	{
		if (type == Enemy) goLeft = false;
		dx = 0.0f;
		rx = 0.3f;
		hasCollidedThisFrame = true;
	}
	else
	{
		if (rx < 0.0f)
		{
			++rx;
			--cx;
		}
	}

	if (check_bottom_collision() && !is_jumping)
	{
		ry = 0.98f;
		dy = 0.0f;
		hasCollidedThisFrame = true;
	}
	else
	{
		if (ry > 1.0f)
		{
			--ry;
			++cy;
		}
	}

	if (check_top_collision())
	{
		ry = 0.98f;
		dy = 0.0f;
		hasCollidedThisFrame = true;
	}
	else
	{
		if (ry < 0.0f)
		{
			++ry;
			--cy;
		}
	}

	// update position
	synchronise_position();
}

void Entity::draw(sf::RenderWindow& win)
{
	if (!sprite) return;
	win.draw(*sprite);
}

void Entity::set_pixel_coordinates(int px, int py)
{
	cx = px / C::GRID_SIZE;
	cy = py / C::GRID_SIZE;

	rx = (px - (cx * C::GRID_SIZE)) / static_cast<float>(C::GRID_SIZE);
	ry = (py - (cy * C::GRID_SIZE)) / static_cast<float>(C::GRID_SIZE);

	synchronise_position();
}

void Entity::set_grid_coordinates(float xCoordinates, float yCoordinates)
{
	cx = static_cast<int>(xCoordinates);
	cy = static_cast<int>(yCoordinates);

	rx = xCoordinates - cx;
	ry = yCoordinates - cy;

	synchronise_position();
}

void Entity::synchronise_position()
{
	sprite->setPosition({(cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE});
}

void Entity::im_gui()
{
	if (ImGui::TreeNode((get_type_name() + " " + to_string(id)).c_str()))
	{
		ImGui::Value("cx", cx);
		ImGui::Value("cy", cy);

		ImGui::Value("rx", rx);
		ImGui::Value("ry", ry);

		ImGui::Value("dx", dx);
		ImGui::Value("dy", dy);

		ImGui::Value("Is Jumping", is_jumping);

		if (ImGui::Button("Set Grid Position (5, 5)")) set_grid_coordinates(5, 5);

		(ImGui::Checkbox("Enable Gravity", &has_gravity));

		ImGui::TreePop();
	}
}
