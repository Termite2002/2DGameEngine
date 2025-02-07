#include "Game.h" 
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"


#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>


Game::Game() {
	isRunning = false;
	registry = std::make_unique<Registry>();
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


// ~ Start in Unity
void Game::Setup() {
	// Add the systems that need to be processed in game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();

	// Create an entity
	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 20.0));
	tank.AddComponent<SpriteComponent>(10, 10);

	Entity tank2 = registry->CreateEntity();
	tank2.AddComponent<TransformComponent>(glm::vec2(400.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	tank2.AddComponent<RigidBodyComponent>(glm::vec2(1.0, 5.0));
	tank2.AddComponent<SpriteComponent>(20, 50);

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

	// Update the systems
	registry->GetSystem<MovementSystem>().Update(deltaTime);

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// Invoke all systems render
	registry->GetSystem<RenderSystem>().Update(renderer);

	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
