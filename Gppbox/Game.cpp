#include <imgui.h>
#include <array>
#include <vector>
#include <iostream>

#include "C.hpp"
#include "Game.hpp"

#include "HotReloadShader.hpp"

Game* Game::instance = 0;
static int cols = C::RES_X / C::GRID_SIZE;
static int lastLine = C::RES_Y / C::GRID_SIZE - 1;

Game::Game(sf::RenderWindow * win) {
	instance = this;

	this->win = win;
	bg = sf::RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));

	bool isOk = tex.loadFromFile("res/bg_stars.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	bg.setTexture(&tex);
	bg.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");
	
	for (int i = 0; i < C::RES_X / C::GRID_SIZE; ++i) 
		walls.push_back(Vector2i(i, lastLine) );
	
	cacheWalls();

	entities.push_back(new Entity);
}

void Game::cacheWalls()
{
	wallSprites.clear();
	for (Vector2i & w : walls) {
		sf::RectangleShape rect(Vector2f(16,16));
		rect.setPosition((float)w.x * C::GRID_SIZE, (float)w.y * C::GRID_SIZE);
		rect.setFillColor(sf::Color(0x07ff07ff));
		wallSprites.push_back(rect);
	}
}

void Game::processInput(sf::Event ev) {
	if (ev.type == sf::Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	if (ev.type == sf::Event::KeyReleased) {
		int here = 0;
		if (ev.key.code == Keyboard::K) {
			int there = 0;
			walls.clear();
			cacheWalls();
		}
	}
} 

static double g_time = 0.0;
static double g_tickTimer = 0.0;

float jump_time = 0.0f;

void Game::pollInput(double dt) {
	float lateralSpeed = 8.0;
	float maxSpeed = 40.0;

	Entity* player = entities[0];

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		player->dx -= player->SPEED;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		player->dx += player->SPEED;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {

	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		if (!wasPressed) { // started
			onSpacePressed();
			wasPressed = true;
		}
	}
	else { // release =
		if (!wasPressed) return;
		wasPressed = false;
		entities[0]->stop_jump();
	}
}

static sf::VertexArray va;
static RenderStates vaRs;
static std::vector<sf::RectangleShape> rects;

int blendModeIndex(sf::BlendMode bm) {
	if (bm == sf::BlendAlpha) return 0;
	if (bm == sf::BlendAdd) return 1;
	if (bm == sf::BlendNone) return 2;
	if (bm == sf::BlendMultiply) return 3;
	return 4;
};

void Game::update(double dt) {

	g_time += dt;

	for (Entity* e : entities)
		e->update(dt);

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
	
	pollInput(dt);
}

 void Game::draw(sf::RenderWindow & win) {
	if (closing) return;

	sf::RenderStates states = sf::RenderStates::Default;
	sf::Shader * sh = &bgShader->sh;
	states.blendMode = sf::BlendAdd;
	states.shader = sh;
	states.texture = &tex;
	sh->setUniform("texture", tex);
	//sh->setUniform("time", g_time);
	win.draw(bg, states);

	beforeParts.draw(win);

	for (sf::RectangleShape & r : wallSprites)
		win.draw(r);

	for (sf::RectangleShape& r : rects) 
		win.draw(r);

	for (Entity* e : entities)
		e->draw(win);

	afterParts.draw(win);
}

void Game::onSpacePressed() {
	entities[0]->jump();
}

bool Game::isWall(int cx, int cy)
{
	for (Vector2i& w : walls) 
	{
		if (w.x == cx && w.y == cy)
			return true;
	}
	return false;
}

bool Game::hasCollisions(const float posX, const float posY)
{
	if (posX < 1.5f) return true;

	int wallRightX = (C::RES_X / C::GRID_SIZE) - 1;
	if (posX >= wallRightX) return true;

	return isWall(static_cast<int>(posX), static_cast<int>(posY));
}

void Game::imGui()
{
	if (levelEditorMode)
	{
		for (int x = 0; x < C::RES_X / C::GRID_SIZE; ++x)
		{
			ImGui::GetBackgroundDrawList()->AddLine(
				{ (float)(0 + x * C::GRID_SIZE), 0 }, { (float)(0 + x * C::GRID_SIZE), C::RES_Y }, IM_COL32(200, 200, 200, 150), 0.5f
			);
		}

		for (int y = 0; y < C::RES_Y / C::GRID_SIZE; ++y)
		{
			ImGui::GetBackgroundDrawList()->AddLine(
				{ 0, (float)(0 + y * C::GRID_SIZE) }, { C::RES_X, (float)(0 + y * C::GRID_SIZE) }, IM_COL32(200, 200, 200, 150), 0.5f
			);
		}

		/*ImGui::GetBackgroundDrawList()->AddRect(
			ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y),  
			ImVec2((ImGui::GetMousePos().x + C::GRID_SIZE), (ImGui::GetMousePos().y + C::GRID_SIZE)),
			IM_COL32(07, 255, 07, 255)
		);*/

		if (ImGui::IsMouseDown(0))
		{
			addWall();
		}
	}

	if (ImGui::Button("Level Editor"))
	{
		levelEditorMode = !levelEditorMode;
	}

	if (ImGui::CollapsingHeader("Walls"))
	{
		for (Vector2i const& wall : walls)
		{
			ImGui::Value("x", wall.x);
			ImGui::Value("y", wall.y);
		}
	}

	if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (Entity* e : entities) e->imGui();
	}
}

void Game::addWall()
{
	int x = ImGui::GetMousePos().x / C::GRID_SIZE;
	int y = ImGui::GetMousePos().y / C::GRID_SIZE;
	for (Vector2i& wall : walls)
	{
		if (wall.x == x && wall.y == y) 
			return;
	}
	printf("Putting Wall");
	walls.push_back(Vector2i(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE));
	cacheWalls();
}
