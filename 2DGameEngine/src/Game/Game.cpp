#include "Game.h" 
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>


Game::Game() {
	isRunning = false;
	Logger::Log("Game constructor called!");
}

Game::~Game() {
	Logger::Log("Game destructor called!");
}

void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("Error init SDL");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = 800; //displayMode.w;
	windowHeight = 600; //displayMode.h;
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_RESIZABLE
	);
	if (!window) {
		Logger::Err("Error creating SDL window");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		Logger::Err("Error creating SDL renderer");
		return;
	}
	// TEMP
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	isRunning = true;
}

void Game::Run() {
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				break;
		}
	}
}


 
void Game::Setup() {
	// TODO:
	// Entity tank = regitry.CreateEntity();
	// tank.AddComponent<TransformComponent>();
	// tank.AddComponent<BoxColliderComponent>();
	// tank.AddComponent<SpriteComponent>("./assets/images/tank.png");
}

void Game::Update() {
	// If we too fast, waste some time until we reach the MILISECS_PER_FRAME
	int timeToWait = MILISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// The diffrence in ticks since the last time, converted to sec
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	// Store the current frame time
	millisecsPreviousFrame = SDL_GetTicks();

	// TODO: 
	// MovementSystem.Update();
	// CollisionSystem.Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// TODO: render system

	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
