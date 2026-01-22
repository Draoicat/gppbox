#include <imgui.h>
#include <string>

#include "Entity.hpp"
#include "Game.hpp"

int Entity::entityCount = 0;
int Entity::totalEntityCount = 0;

Entity::Entity(sf::Vector2f position, sf::Vector2f size)
{
	id = entityCount;
	++entityCount;
	sx = size.x / C::GRID_SIZE;
	sy = size.y / C::GRID_SIZE;
}

Entity::Entity() : Entity({0.0, 0.0}, {1.0f, 1.0f}) { }

Entity::~Entity()
{
	--entityCount;
	delete sprite;
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

void Entity::calculateNextPosition(double deltaTime)
{
	if (has_gravity) dy += GRAVITY_RATE * deltaTime;
	dy = clamp(dy, -150.0f, 150.0f);

	dx *= 0.70f;

	rx += dx * deltaTime;
	ry += dy * deltaTime;
}

void Entity::handleCollisions()
{
	if (check_right_collision())
	{
		dx = 0.0f;
		rx = 0.7f;
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
		dx = 0.0f;
		rx = 0.3f;
	}
	else
	{
		if (rx < 0.0f)
		{
			++rx;
			--cx;
		}
	}

	if (check_bottom_collision())
	{
		ry = 0.98f;
		dy = 0.0f;
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
	}
	else
	{
		if (ry < 0.0f)
		{
			++ry;
			--cy;
		}
	}
}

void Entity::update(double deltaTime)
{
	calculateNextPosition(deltaTime);
	facesLeft = dx < 0;

	handleCollisions();

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

		ImGui::Value("facesLeft", facesLeft);

		//ImGui::Value("shouldDelete", shouldDelete);

		ImGui::TreePop();
	}
}
