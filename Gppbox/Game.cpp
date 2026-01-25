#include <imgui.h>
#include <array>
#include <vector>

#include "Game.hpp"

#include "Enemy.hpp"
#include "HotReloadShader.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "SaveSystem.h"

Game* Game::instance = 0;
static int cols = C::RES_X / C::GRID_SIZE;
static int lastLine = C::RES_Y / C::GRID_SIZE - 1;

Game::Game(RenderWindow * win) {
	instance = this;

	this->win = win;
	bg = RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));

	bool isOk = tex.loadFromFile("res/bg_stars.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	bg.setTexture(&tex);
	bg.setSize(Vector2f(C::RES_X, C::RES_Y));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");
	
	for (int i = 0; i < C::RES_X / C::GRID_SIZE; ++i)
		addWall(i, lastLine);
	
	cacheWalls();

	//player
	RectangleShape playerSprite({C::GRID_SIZE, C::GRID_SIZE * 2});
	playerSprite.setFillColor( Color::White );
	playerSprite.setOutlineColor(Color::Black);
	
	loadPlayer(3, 50);

	// enemy creation
	entities.push_back(new Enemy({80, 50}, {C::GRID_SIZE, C::GRID_SIZE * 2}));
}

void Game::cacheWalls()
{
	wallSprites.clear();
	for (Vector2i & w : walls) {
		RectangleShape rect(Vector2f(16,16));
		rect.setPosition((float)w.x * C::GRID_SIZE, (float)w.y * C::GRID_SIZE);
		rect.setFillColor(Color(0x07ff07ff));
		wallSprites.push_back(rect);
	}
}

void Game::processInput(Event ev) {
	if (ev.type == Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	if (ev.type == Event::KeyReleased) {
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

	float x = Joystick::getAxisPosition(0, Joystick::Axis::X);
	bool jumpButton = (Joystick::isButtonPressed(0, 0));
	bool shootButton = Joystick::isButtonPressed(0,1);
	bool rayButton = Joystick::isButtonPressed(0,2);

	if (Keyboard::isKeyPressed(Keyboard::Key::Q) || x < -50) {
		if (deathRaySprite) return;
		player->go_left();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::D) || x > 50) {
		if (deathRaySprite) return;
		player->go_right();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::Tab) || shootButton) {
		if (deathRaySprite) return;
		shoot();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::A) || rayButton) {
		if (g_time - lastDeathRayTime < DEATH_RAY_TIMEOUT) return;
		death_ray();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::Space) || jumpButton) {
		if (deathRaySprite) return;
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

static VertexArray va;
static RenderStates vaRs;
static std::vector<RectangleShape> rects;

int blendModeIndex(BlendMode bm) {

	if (bm == BlendAlpha) return 0;
	if (bm == BlendAdd) return 1;
	if (bm == BlendNone) return 2;
	if (bm == BlendMultiply) return 3;
	return 4;
}

void Game::update(double dt)
{
	if (isLevelEditorOn) return;
	g_time += dt;

	if (lastShotDeltaTime < (1 / playerShootRatePerSeconds)) lastShotDeltaTime += dt;
	else canPlayerShoot = true;

	if (g_time - lastDeathRayTime > DEATH_RAY_TIME_ON_SCREEN_SECONDS)
		deathRaySprite = nullptr;
	
	for (Entity* e : entities)
	{
		if (e->shouldDelete)
			entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
		e->update(dt);
	}

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
	
	pollInput(dt);
}

 void Game::draw(RenderWindow & win) {
	if (closing) return;

	RenderStates states = RenderStates::Default;
	Shader * sh = &bgShader->sh;
	states.blendMode = BlendAdd;
	states.shader = sh;
	states.texture = &tex;
	sh->setUniform("texture", tex);
	//sh->setUniform("time", g_time);
	win.draw(bg, states);

	beforeParts.draw(win);

	for (RectangleShape & r : wallSprites) win.draw(r);
	for (RectangleShape& r : rects) win.draw(r);
	for (Entity* e : entities) e->draw(win);
	if (deathRaySprite != nullptr) win.draw(*deathRaySprite);

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
	lastShotDeltaTime = 0.0f;
	canPlayerShoot = false;
	player->isKnockback = true;
	entities.emplace_back(new Projectile({(float) entities[0]->cx + (player->facesLeft ? -2 : 2), (float) entities[0]->cy + 0.5f }, {C::GRID_SIZE, C::GRID_SIZE}, player->facesLeft));
}

void Game::death_ray()
{
	//logic
	lastDeathRayTime = g_time;
	Vector2i goal { 
		(player->facesLeft ? player->cx - Entity::DEATH_RAY_LENGTH : player->cx + Entity::DEATH_RAY_LENGTH),
		player->cy
	};
	std::vector<Vector2i> points = bresenham({player->cx, player->cy}, goal);
	std::vector<Vector2i> pointsUpOffset = bresenham({player->cx, player->cy + 1}, goal);
	std::vector<Vector2i> pointsDownOffset = bresenham({player->cx, player->cy - 1}, goal);
	points.insert( points.end(), pointsUpOffset.begin(), pointsUpOffset.end() );
	points.insert( points.end(), pointsDownOffset.begin(), pointsDownOffset.end() );
	for (Vector2i const& point : points)
	{
		std::vector<Entity*> touchedEnemies = isOtherEntityPresent("Enemy", point.x, point.y);
		if (!touchedEnemies.empty())
			for (Entity* enemyToDelete : touchedEnemies) enemyToDelete->shouldDelete = true;
	}

	//draw
	deathRaySprite = new RectangleShape(Vector2f(C::GRID_SIZE * Entity::DEATH_RAY_LENGTH, C::GRID_SIZE * 1));
	deathRaySprite->setPosition(
		(player->facesLeft ? 
			C::GRID_SIZE * (goal.x) :
			C::GRID_SIZE * (player->cx + 1)
		),
		C::GRID_SIZE *  (player->cy - 0.5)
	);
	deathRaySprite->setFillColor(Color(0xffffffff));
}

std::vector<Vector2i> Game::bresenham(Vector2i origin, Vector2i goal)
{
	std::vector<Vector2i> points;

	int dx = goal.x - origin.x;
	int dy = goal.y - origin.y;
	int D = 2*dy - dx;
	int y = origin.y;

	for (int x = origin.x; x <= goal.x; ++x)
	{
		points.emplace_back(x, y);
		if (D > 0)
		{
			y += 1;
			D += 2 * (dy - dx);
		}
		else
			D += 2*dy;
	}
	return points;
}

bool Game::hasCollisions(const float posX, const float posY)
{
	if (posX < 1.0f) return true;

	int wallRightX = (C::RES_X / C::GRID_SIZE) - 1;
	if (posX >= wallRightX) return true;

	return isWall(static_cast<int>(posX), static_cast<int>(posY));
}

std::vector<Entity*> Game::isOtherEntityPresent(string typeName, int x, int y)
{
	std::vector<Entity*> results;
	for (Entity* e : entities)
		if (e->cx == x && e->cy == y && typeName == e->get_type_name())
			results.push_back(e);
	return results;
}

void Game::gameOver()
{
	isGameOver = true;
}

void Game::save() const
{
	SaveSystem::save_level(*this);
}

void Game::load()
{	

	SaveSystem::load_level(*this);
	Entity::entityCount = 0;
	Entity::totalEntityCount = 0;
}

void Game::imGui(RenderWindow& win)
{
	ImGui::Value("Can player shoot", canPlayerShoot);
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

		RectangleShape fakeWall(Vector2f(C::GRID_SIZE, C::GRID_SIZE));
		RectangleShape fakeEnemy(Vector2f(C::GRID_SIZE, C::GRID_SIZE * 2));
		// Preview
		switch (levelEditorMode)
		{
		case WALL:
			fakeWall.setPosition((int) (ImGui::GetMousePos().x / C::GRID_SIZE) * C::GRID_SIZE, (int) (ImGui::GetMousePos().y / C::GRID_SIZE) * C::GRID_SIZE);
			fakeWall.setFillColor(Color(0x07ff0777));
			win.draw(fakeWall);
			break;
		case ENEMY:
			fakeEnemy.setFillColor(Color::Red);
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
				tryAddEnemy(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
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
					tryAddWall(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
				else
					tryRemoveWall(ImGui::GetMousePos().x / C::GRID_SIZE, ImGui::GetMousePos().y / C::GRID_SIZE);
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

	if (ImGui::Button("Save level"))
	{
		save();
	}
	if (ImGui::Button("Load saved level"))
	{
		load();
	}

	if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (Entity* e : entities) 
			e->im_gui();
	}
}

void Game::loadPlayer(float const x, float const y)
{
	if (player != nullptr) return;
	player = new Player({ x, y }, { C::GRID_SIZE, C::GRID_SIZE * 2 });
	entities.push_back(player);
}

void Game::tryAddEnemy(float const x, float const y)
{
	for (Entity* e : entities)
		if (e->cx == (int) x && e->cy == (int) y) 
			return;
	addEnemy(x, y);
}

void Game::addEnemy(float const x, float const y)
{
	entities.emplace_back(new Enemy({x, y}, {C::GRID_SIZE, C::GRID_SIZE * 2}));
}

void Game::tryAddWall(float const x, float const y)
{
	for (Vector2i& wall : walls)
		if (wall.x == (int) x && wall.y == (int) y) //don't place two walls at the same spot 
			return;
	addWall(x, y);
}

void Game::addWall(int const x, int const y)
{
	walls.emplace_back(x, y);
	cacheWalls();
}

void Game::tryRemoveWall(float const x, float const y)
{
	for (Vector2i& wall : walls)
		if (wall.x == (int) x && wall.y == (int) y) //don't remove a wall that doesn't exist
			removeWall(wall);
}

void Game::removeWall(Vector2i const& wall)
{
	walls.erase(std::remove(walls.begin(), walls.end(), wall), walls.end());
	cacheWalls();
}
