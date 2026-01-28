#include <imgui.h>
#include <array>
#include <vector>

#include "Game.hpp"

#include "Enemy.hpp"
#include "HotReloadShader.hpp"

#include "Player.hpp"
#include "Projectile.hpp"
#include "SaveSystem.h"

//todo : pet drone, read me

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
	
	loadPlayer(respawnPoint.x, respawnPoint.y);
	loadPet(2, 51);

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
	if (g_time - lastDeathRayTime < DEATH_RAY_TIME_ON_SCREEN_SECONDS) return;

	float x = Joystick::getAxisPosition(0, Joystick::Axis::X);
	bool jumpButton = (Joystick::isButtonPressed(0, 0));
	bool shootButton = Joystick::isButtonPressed(0,1);
	bool rayButton = Joystick::isButtonPressed(0,2);

	if (Keyboard::isKeyPressed(Keyboard::Key::Q) || x < -50) {
		player->go_left();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::D) || x > 50) {
		player->go_right();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::Tab) || shootButton) {
		shoot();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::A) || rayButton) {
		if (g_time - lastDeathRayTime < DEATH_RAY_TIMEOUT) return;
		death_ray();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::Space) || jumpButton) {
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

	if (isScreenshaking && g_time - screenshotStartTime > SCREENSHAKE_TIME_SECONDS)
		isScreenshaking = false;

	/*if (lastPetShortDeltaTime < (1 / petShootRatePerSeconds)) lastPetShortDeltaTime += dt;
	else petShoot();*/

	if (g_time - lastDeathRayTime > DEATH_RAY_TIME_ON_SCREEN_SECONDS)
		deathRaySprite = nullptr;

	for (Entity* e : entities)
	{
		if (e->shouldDelete)
			entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
		e->update(dt);
	}

	if (player->cy > 100) respawn();
	if (!isOtherEntityPresent("Enemy", player->cx, player->cy).empty()) respawn();

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
	
	pollInput(dt);
	petFollow(dt);
}

void Game::petFollow(double const dt)
{
	if (player->cx - pet->cx > petOffset.x) pet->go_right();
	else if (player->cx - pet->cx < petOffset.x) pet->go_left();

	if (player->cy - pet->cy > petOffset.y) pet->go_down();
	else if (player->cy - pet->cy < petOffset.y) pet->go_up();

	petOffset = Vector2i(petOffset.x, player->cy - pet->cy);
}


void Game::updateView(View* view, double const dt)
{
	//move view to player
	Vector2f origin = view->getCenter();
	Vector2f offset = {300, -300}; //I want the player in a different position on the screen
	Vector2f goal = Vector2f(player->cx * 16 , player->cy * 16  ) + offset;
	Vector2f displacement;
	if (isLevelEditorOn)
	{
		displacement = goal - origin;
	}
	else
	{
		displacement = Vector2f(Vector2f(goal.x * dt, goal.y * dt) - Vector2f(origin.x * dt, origin.y * dt));
		displacement = Vector2f(displacement.x * 6, displacement.y * 1.5f);
	}

	view->move(displacement);
	if (isLevelEditorOn)
	{
		int amountX = (int) view->getCenter().x % C::GRID_SIZE;
		int amountY = (int) view->getCenter().y % C::GRID_SIZE;
		view->setCenter(view->getCenter().x - amountX, view->getCenter().y - amountY);
	}

	// screenshake
	if (isScreenshaking)
	{
		if (g_time - screenshotStartTime > SCREENSHAKE_TIME_SECONDS / 3)
		{
			view->move(Vector2f(0, 1));
		}
		else if (g_time - screenshotStartTime < SCREENSHAKE_TIME_SECONDS / 3)
		{
			view->move(Vector2f(0, -1));
		}
	}
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
	Vector2i direction(player->facesLeft ? -1 : 1, 0);
	entities.emplace_back(new Projectile({(float) entities[0]->cx + (player->facesLeft ? -2 : 2), (float) entities[0]->cy + 0.5f }, {C::GRID_SIZE, C::GRID_SIZE}, direction));
}

void Game::petShoot()
{
	if (!canPetShoot) return;
	/*std::vector<Entity*> enemies = */
	lastPetShortDeltaTime = 0.0f;
	Vector2i directionToEnemy(1, 1);
	entities.emplace_back(
		new Projectile({(float) pet->cx, (float) pet->cy }, {C::GRID_SIZE, C::GRID_SIZE}, directionToEnemy));
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
			for (Entity* enemyToDelete : touchedEnemies) 
				enemyToDelete->shouldDelete = true;
	}

	//prepare sprite
	deathRaySprite = new RectangleShape(Vector2f(C::GRID_SIZE * Entity::DEATH_RAY_LENGTH, C::GRID_SIZE * 1));
	deathRaySprite->setPosition(
		(player->facesLeft ? 
			C::GRID_SIZE * (goal.x) :
			C::GRID_SIZE * (player->cx + 1)
		),
		C::GRID_SIZE *  (player->cy - 0.5)
	);
	deathRaySprite->setFillColor(Color(0xffffffff));

	screenshake();
}

std::vector<Vector2i> Game::bresenham(Vector2i origin, Vector2i goal)
{
	std::vector<Vector2i> points;

	if (goal.x < origin.x)
	{
		Vector2i temp = origin;
		origin = goal;
		goal = temp;
	}
	
	int dx = goal.x - origin.x;
	int dy = goal.y - origin.y;

	//dy should be 0 if I keep a horizontal line, keep that in mind in case you wanna optimise that uwu
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

void Game::screenshake()
{
	if (isScreenshaking) return;
	isScreenshaking = true;
	screenshotStartTime = g_time;
}

bool Game::hasCollisions(const float posX, const float posY)
{
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

		Vector2i cameraAlignmentOffset{ PLAYER_POS_OFFSET_WITH_INITIAL_CAMERA.x - player->cx, PLAYER_POS_OFFSET_WITH_INITIAL_CAMERA.y - player->cy};
		Vector2f fakesPosition = Vector2f(
			 (((int) ImGui::GetMousePos().x / C::GRID_SIZE * C::GRID_SIZE) -  (cameraAlignmentOffset.x * C::GRID_SIZE)),
			 ((int) ImGui::GetMousePos().y / C::GRID_SIZE * C::GRID_SIZE) - (cameraAlignmentOffset.y * C::GRID_SIZE)
		);
		// Preview
		switch (levelEditorMode)
		{
		case WALL:
			fakeWall.setPosition(fakesPosition);
			fakeWall.setFillColor(Color(0x07ff0777));
			win.draw(fakeWall);
			break;
		case ENEMY:
			fakeEnemy.setFillColor(Color::Red);
			fakeEnemy.setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2 });
			fakeEnemy.setPosition(fakesPosition);
			win.draw(fakeEnemy);
			break;
		}

		// mouse click
		if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
		{
			placeWallMode = false;
			for (Vector2i& wall : walls)
				if (wall.x == (int) ImGui::GetMousePos().x / C::GRID_SIZE - cameraAlignmentOffset.x && wall.y == (int) ImGui::GetMousePos().y / C::GRID_SIZE- cameraAlignmentOffset.y) 
					placeWallMode = true;
			switch (levelEditorMode)
			{
			case ENEMY:
				tryAddEnemy(ImGui::GetMousePos().x / C::GRID_SIZE- cameraAlignmentOffset.x, ImGui::GetMousePos().y / C::GRID_SIZE- cameraAlignmentOffset.y);
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
					tryAddWall(ImGui::GetMousePos().x / C::GRID_SIZE- cameraAlignmentOffset.x, ImGui::GetMousePos().y / C::GRID_SIZE- cameraAlignmentOffset.y);
				else
					tryRemoveWall(ImGui::GetMousePos().x / C::GRID_SIZE- cameraAlignmentOffset.x, ImGui::GetMousePos().y / C::GRID_SIZE- cameraAlignmentOffset.y);
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

void Game::loadPet(float const x, float const y)
{
	if (pet != nullptr) return;
	pet = new Pet({ x, y }, { C::GRID_SIZE / 2, C::GRID_SIZE / 2 });
	entities.push_back(pet);
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
		if (wall.x == (int) x  && wall.y == (int) y ) //don't place two walls at the same spot 
			return;
	addWall(x , y);
}

void Game::addWall(int const x, int const y)
{
	walls.emplace_back(x, y);
	cacheWalls();
}

void Game::tryRemoveWall(float const x, float const y)
{
	for (Vector2i& wall : walls)
		if (wall.x == (int) x && wall.y == (int) y ) //don't remove a wall that doesn't exist
			removeWall(wall);
}

void Game::removeWall(Vector2i const& wall)
{
	walls.erase(std::remove(walls.begin(), walls.end(), wall), walls.end());
	cacheWalls();
}

void Game::respawn()
{
	player->cx = respawnPoint.x;
	player->cy = respawnPoint.y;
}
