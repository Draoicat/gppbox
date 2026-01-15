#include <imgui.h>

#include "C.hpp"
#include "Entity.hpp"
#include "Game.hpp"

Entity::Entity() : sprite{ new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE * 2}) }
{
	sprite->setFillColor(sf::Color::White);
	sprite->setOutlineColor(sf::Color::Black);
	sprite->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2 });
	setGridCoord(3,3);
}

Entity::Entity(sf::Shape* sprite) : sprite{ sprite }
{
	sprite->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2 });
}

void Entity::update(double deltaTime)
{
	double rate = 1.0 / deltaTime;
	double dfr = 60.0f / rate;

	if (has_gravity) dy += GRAVITY_RATE * deltaTime;

	dx = dx * pow(frx, dfr);
	dy = dy * pow(fry, dfr);

	rx += dx * deltaTime;
	ry += dy * deltaTime;

	// collisions
	if (Game::instance->hasCollisions(cx + 1, cy) && rx >= 0.7f)
	{
		dx = 0;
		rx = 0.7f;
	}
	else if (Game::instance->hasCollisions(cx - 1, cy) && rx <= 0.3f)
	{
		dx = 0;
		rx = 0.3f;
	}

	if (Game::instance->hasCollisions(cx, cy+1)) 
	{
		dy = 0;
		ry = 0.98;
	}
	
	// movement
	if (rx > 1.0f)
	{
		--rx;
		++cx;
	}
	else if (rx < 0.0f)
	{
		++rx;
		--cx;
	}

	if (ry > 1.0f)
	{
		--ry;
		++cy;
	}

	syncPosition();
}

void Entity::draw(sf::RenderWindow& win)
{
	if (!sprite) return;
	win.draw(*sprite);
}

void Entity::setPixelCoord(int px, int py)
{
	cx = px / C::GRID_SIZE;
	cy = py / C::GRID_SIZE;

	rx = (px - (cx * C::GRID_SIZE)) / (float)C::GRID_SIZE;
	ry = (py - (cy * C::GRID_SIZE)) / (float)C::GRID_SIZE;

	syncPosition();
}

void Entity::setGridCoord(float coordX, float coordY)
{
	cx = (int)coordX;
	cy = (int)coordY;

	rx = coordX - cx;
	ry = coordY - cy;

	syncPosition();
}

void Entity::syncPosition()
{
	sprite->setPosition({ (cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE });
}

void Entity::imGui()
{
	if (!ImGui::TreeNode("Player")) return;

	ImGui::Value("cx", cx);
	ImGui::Value("cy", cy);

	ImGui::Value("rx", rx);
	ImGui::Value("ry", ry);

	if (ImGui::Button("Set Grid Position (5, 5)")) setGridCoord(5, 5);

	(ImGui::Checkbox("Enable Gravity", &has_gravity));

	ImGui::TreePop();
}
