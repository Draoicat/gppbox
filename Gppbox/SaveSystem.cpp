#include "SaveSystem.h"

#include "Game.hpp"

#include <string>
#include <fstream>
#include <iostream>

char const* const FILE_PATH = "save.txt";
SaveSystem::LoadState SaveSystem::loadState = WALLS;

void SaveSystem::save_level(Game const& gameState)
{
	std::ofstream outFile{FILE_PATH}; 
	//outFile constructor opens the stream

	if (!outFile)
	{
		printf("could not be opened for writing.\n");
		return;
	}

	outFile << "Walls\n";
	for (Vector2i const& wall : gameState.walls)
		outFile << wall.x << "," << wall.y << '\n';

	outFile << "Player\n";
	outFile << gameState.player->cx << "," << gameState.player->cy << '\n';

	outFile << "Pet\n";
	outFile << gameState.pet->cx << "," << gameState.pet->cy << '\n';

	outFile << "Entities\n";
	auto entityIterator = gameState.entities.begin();
	++entityIterator; //skip player
	++entityIterator; //skip pet
	for (; entityIterator != gameState.entities.end(); ++entityIterator)
		outFile << (*entityIterator)->cx << "," << (*entityIterator)->cy << '\n';

	//outFile destructor closes the stream when it is out of scope.
}

void SaveSystem::load_level(Game& outGameState)
{


	std::ifstream inFile{FILE_PATH};
	if (!inFile)
	{
		printf("could not be opened for reading.\n");
		return;
	}

	outGameState.player = nullptr;
	outGameState.entities.erase(outGameState.entities.begin(), outGameState.entities.end());
	outGameState.walls.erase(outGameState.walls.begin(), outGameState.walls.end());

	std::string line{};
	while (std::getline(inFile, line))
	{
		if (line == "Walls") loadState = LoadState::WALLS;
		else if (line == "Pet") loadState = LoadState::PET;
		else if (line == "Player") loadState = LoadState::PLAYER;
		else if (line == "Entities") loadState = LoadState::ENEMIES;
		else
		{
			string del = ",";
			size_t pos = line.find(del);
			string xS = line.substr(0, pos);
			line.erase(0, pos + del.length());
			string yS = line;

			switch (loadState)
			{
			case LoadState::WALLS:
				outGameState.addWall(stoi(xS), stoi(yS));
				break;
			case LoadState::PLAYER:
				outGameState.loadPlayer(stoi(xS), stoi(yS));
				outGameState.player->isKnockback = false;
				outGameState.player->is_jumping = false;
				break;
			case LoadState::ENEMIES:
				outGameState.addEnemy(stoi(xS), stoi(yS));
				break;
			case LoadState::PET:
				outGameState.loadPet(stoi(xS), stoi(yS));
				break;
			}
		}
	}
}
