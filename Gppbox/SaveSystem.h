#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

class Game;
class SaveSystem
{
public:
	enum LoadState //sooo what are we loading rn? :3
	{
		WALLS, PLAYER, ENEMIES, PET
	};

	static void save_level(Game const& gameState);
	static void load_level(Game& outGameState);
	static LoadState loadState;
	
	SaveSystem() = delete;
};

#endif //SAVE_SYSTEM_H
