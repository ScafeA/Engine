#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>


Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

// TODO: CHANGE THESE TO DYNAMICALLY PROCESS
//The dimensions of the level
const int LEVEL_WIDTH = 2400;
const int LEVEL_HEIGHT = 1900;

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Rect Game::camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

SDL_Color white = { 255,255,255,255 };

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;

auto& player(manager.addEntity());
auto& label(manager.addEntity());

Game::Game()
{}
Game::~Game()
{}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		isRunning = true;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	assets->AddTexture("terrain", "assets/terrain_ss.png");
	assets->AddTexture("player", "assets/player_anims.png");
	assets->AddTexture("projectile", "assets/proj.png");

	assets->AddFont("arial", "assets/arial.ttf", 16);

	map = new Map("terrain", 3, 32);
	//ecs implementation

	map->LoadMap("assets/map.map", 25, 20);

	player.addComponent<TransformComponent>(4, LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayers);

	white = { 255,255,255,255 };
	label.addComponent<UILabel>(10, 10, "TestString", "arial", white);

	//assets->CreateProjectile(Vector2D(600, 600), Vector2D(2, 0), 200, 2, "projectile");
}

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;

	default:
		break;
	}
}

void Game::update()
{

	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;

	std::stringstream ss;
	ss << "Player position: " << playerPos;
	label.getComponent<UILabel>().SetLabelText(ss.str(), "arial");

	manager.refresh();
	manager.update();

	for (auto& c : colliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			player.getComponent<TransformComponent>().position = playerPos;
		}
	}

	for (auto& p : projectiles)
	{
		if (Collision::AABB(player.getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Hit player!" << std::endl;
			p->destroy();
		}
	}

	//std::cout << "X:" << player.getComponent<TransformComponent>().position.x << "Y:" << player.getComponent<TransformComponent>().position.y << std::endl;

	/*camera.x = player.getComponent<TransformComponent>().position.x - 960;
	camera.y = player.getComponent<TransformComponent>().position.y - 540;

	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > 25 * 96 - camera.w)
		camera.x = 25 * 96 - camera.w;
	if (camera.y > 20 * 96 - camera.h)
		camera.y = 20 * 96 - camera.h;*/

	camera.x = (player.getComponent<TransformComponent>().position.x + player.getComponent<TransformComponent>().width / 2) - SCREEN_WIDTH / 2;
	camera.y = (player.getComponent<TransformComponent>().position.y + player.getComponent<TransformComponent>().height / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Game::render()
{
	SDL_RenderClear(renderer);
	for (auto& t : tiles)
	{
		t->draw();
	}
	for (auto& c : colliders)
	{
		c->draw();
	}
	for (auto& p : players)
	{
		p->draw();
	}
	for (auto& p : projectiles)
	{
		p->draw();
	}

	label.draw();
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}