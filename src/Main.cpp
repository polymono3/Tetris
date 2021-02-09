#include "Game.h"
#include "Constants.h"

int main(int argc, char* argv[])
{
	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

	game.Run();

	return 0;
}