#include "Game.h"


Game* game = nullptr;

int main(int argc, char* argv[])
{
	// handle any user input
	// update all objects eg. positions etc.
	// render changes to the display

	const int FPS = 60; // normal refresh rate
	const int frameDelay = 1000 / FPS; // max time between frames

	Uint32 frameStart;
	int frameTime;

	std::string version = "0.0.1";
	std::string title = "ScafeEngine " + version;

	game = new Game();
	game->init(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, false);

	while (game->running()) 
	{

		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}

	game->clean();

	return 0;
}