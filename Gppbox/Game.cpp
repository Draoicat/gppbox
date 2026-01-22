#include <imgui.h>
#include <array>
#include <vector>

#include "Game.hpp"

#include "Enemy.hpp"
#include "HotReloadShader.hpp"
#include "Player.hpp"

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

	//player
	sf::RectangleShape playerSprite({C::GRID_SIZE, C::GRID_SIZE * 2});
	playerSprite.setFillColor( sf::Color::White );
	playerSprite.setOutlineColor(sf::Color::Black);
	
	player = new Player({ 3, 50 }, { C::GRID_SIZE, C::GRID_SIZE * 2 });
	entities.emplace_back(player);

	//entities
	for (int i = 0; i < enemyCount; ++i)
		entities.emplace_back(new Enemy({80, 50}, {C::GRID_SIZE, C::GRID_SIZE * 2}));
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
	if (isGameOver) return;

	float lateralSpeed = 8.0;
	float maxSpeed = 40.0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		player->dx -= Entity::SPEED;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		player->dx += Entity::SPEED;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab)) {
		shoot();
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
		player->stop_jump();
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

void Game::update(double dt)
{
	if (isLevelEditorOn) return;

	g_time += dt;

	for (Entity* e : entities)
	{
		e->update(dt);

	}

	//checkwin
	for (int i = 1; i <= enemyCount; ++i)
	{
		if (entities[0]->cx == entities[i]->cx && entities[0]->cy == entities[i]->cy)
			gameOver();
	}

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
	
	// INPUTS
	if (lastShotDT < 1 / playerShootRatePerSeconds) lastShotDT += dt;
	else
	{
		lastShotDT = 0.0f;
		canPlayerShoot = true;
	}
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
	player->jump();
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

void Game::shoot()
{
	if (!canPlayerShoot) return;
	canPlayerShoot = false;
	entities.emplace_back(new Projectile({(float) entities[0]->cx + (player->facesLeft ? -1 : 1), (float) entities[0]->cy}, {C::GRID_SIZE, C::GRID_SIZE}, player->facesLeft)); 
}

bool Game::hasCollisions(const float posX, const float posY)
{
	if (posX < 1.5f) return true;

	int wallRightX = (C::RES_X / C::GRID_SIZE) - 1;
	if (posX >= wallRightX) return true;

	return isWall(static_cast<int>(posX), static_cast<int>(posY));
}

void Game::gameOver()
{
	//entities.erase(std::remove(entities.begin(), entities.end(), entities[0]), entities.end());
	isGameOver = true;
}

void Game::imGui(sf::RenderWindow& win)
{
	// level editor button
	if (ImGui::Button("Level Editor"))
	{
		isLevelEditorOn = !isLevelEditorOn;
	}

	if (isLevelEditorOn)
	{
		// draw editor grid
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

		sf::RectangleShape fakeWall(Vector2f(C::GRID_SIZE, C::GRID_SIZE));
		sf::RectangleShape fakeEnemy(Vector2f(C::GRID_SIZE, C::GRID_SIZE * 2));
		// Preview
		switch (levelEditorMode)
		{
		case WALL:
			fakeWall.setPosition((int) (ImGui::GetMousePos().x / C::GRID_SIZE) * C::GRID_SIZE, (int) (ImGui::GetMousePos().y / C::GRID_SIZE) * C::GRID_SIZE);
			fakeWall.setFillColor(sf::Color(0x07ff0777));
			win.draw(fakeWall);
			break;
		case ENEMY:
			fakeEnemy.setFillColor(sf::Color::Red);
			fakeEnemy.setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2 });
			fakeEnemy.setPosition((int) (ImGui::GetMousePos().x / C::GRID_SIZE) * C::GRID_SIZE, (int) (ImGui::GetMousePos().y / C::GRID_SIZE) * C::GRID_SIZE);
			win.draw(fakeEnemy);
			break;
		}

		// mouse click
		if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			placeWallMode = false;
			for (Vector2i& wall : walls)
				if (wall.x == (int) ImGui::GetMousePos().x / C::GRID_SIZE && wall.y == (int) ImGui::GetMousePos().y / C::GRID_SIZE) 
					placeWallMode = true;
			switch (levelEditorMode)
			{
			case ENEMY:
				addEnemy(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
				break;
			default:
				break;
			}
		}

		// mouse hold
		if (ImGui::IsMouseDown(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			switch (levelEditorMode)
			{
			case WALL:
				if (!placeWallMode)
					addWall(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
				else
					removeWall(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
				break;
			default:
				break;
			}
		}
	}
	
	// level editor menu
	if (isLevelEditorOn)
	{
		int levelEditorModeInt = (int) levelEditorMode;
        ImGui::RadioButton("Wall", &levelEditorModeInt, 0); ImGui::SameLine();
        ImGui::RadioButton("Enemy", &levelEditorModeInt, 1);
		levelEditorMode = (EditorMode) levelEditorModeInt;
	}

	if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (Entity* e : entities) 
			e->im_gui();
	}
}

void Game::addEnemy(float const x, float const y)
{
	for (Entity* e : entities)
		if (e->cx == (int) x && e->cy == (int) y) 
			return;
	printf("Putting Enemy at : (%f, %f)\n", x, y);
	entities.emplace_back(new Enemy({x, y}, {C::GRID_SIZE, C::GRID_SIZE * 2}));
	++enemyCount;
	cacheWalls();
}


void Game::addWall(float const x, float const y)
{
	for (Vector2i& wall : walls)
		if (wall.x == (int) x && wall.y == (int) y) //don't place two walls at the same spot 
			return;
	printf("Putting Wall at : (%f, %f)\n", x, y);
	walls.push_back(Vector2i(x, y));
	cacheWalls();
}

void Game::removeWall(float const x, float const y)
{
	for (Vector2i& wall : walls)
		if (wall.x == (int) x && wall.y == (int) y) //don't remove a wall that doesn't exist
		{
			printf("Removing Wall at : (%f, %f)\n", x, y);
			walls.erase(std::remove(walls.begin(), walls.end(), wall), walls.end());
		}
	cacheWalls();
}
