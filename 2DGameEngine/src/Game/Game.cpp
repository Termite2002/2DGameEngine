#include "Game.h" 
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"


#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifeCycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/RenderGUISystem.h"


#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <fstream>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
	isRunning = false;
	isDebug = false;
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
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

	if (TTF_Init() != 0) {
		Logger::Err("Error init SDL TTF");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = 1280;//displayMode.w; // 800;
	windowHeight = 960;//displayMode.h; // 600;
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

	// Init Im GUI
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

	// TEMP
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	// Init Camarea view 
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;

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
		// Handle the event ImGui
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

		// Handle the event SDL
		switch (sdlEvent.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				if (sdlEvent.key.keysym.sym == SDLK_d) {
					isDebug = !isDebug;
				}
				eventBus->EmitEvent<KeyPressedEvent>(static_cast<SDL_KeyCode>(sdlEvent.key.keysym.sym));

				break;
		}
	}
}


void Game::LoadLevel(int level) {
	// Add the systems that need to be processed in game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifeCycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<RenderHealthBarSystem>();
	registry->AddSystem<RenderGUISystem>();

	// Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");

	assetStore->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 14);
	assetStore->AddFont("pico8-font-5", "./assets/fonts/pico-8.ttf", 5);
	assetStore->AddFont("pico8-font-10", "./assets/fonts/pico-8.ttf", 10);

	// Load the tilemap
	int tileSize = 32;
	double tileScale = 2.5;
	int mapNumCols = 25;
	int mapNumRows = 20;
	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = (ch - '0') * tileSize;
			mapFile.get(ch);
			int srcRectX = (ch - '0') * tileSize;
			mapFile.ignore();

			Logger::Log("SRC X = " + std::to_string(srcRectX) + " Y = " + std::to_string(srcRectY));

			Entity tile = registry->CreateEntity();
			tile.Group("tiles");
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileSize * tileScale), y * (tileSize * tileScale)), 
												  glm::vec2(tileScale, tileScale),	
												  0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	mapWidth = mapNumCols * tileSize * tileScale;
	mapHeight = mapNumRows * tileSize * tileScale;


	Entity chopper = registry->CreateEntity();
	chopper.Tag("player");
	chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 10, true);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(250.0, 250.0), 0, 10000, 10, true);
	chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -100), glm::vec2(100, 0), glm::vec2(0, 100), glm::vec2(-100, 0));
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth- 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.AddComponent<AnimationComponent>(8, 10, true);

	// Create an entity
	Entity tank = registry->CreateEntity();
	tank.Group("enemies");
	tank.AddComponent<TransformComponent>(glm::vec2(1000.0, 165.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 3000, 5000, 15, false);
	tank.AddComponent<HealthComponent>(100);

	Entity truck = registry->CreateEntity();
	truck.Group("enemies");
	truck.AddComponent<TransformComponent>(glm::vec2(150.0, 630.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0, -100), 2000, 5000, 10, false);
	truck.AddComponent<HealthComponent>(100);

	Entity label = registry->CreateEntity();
	SDL_Color green = { 0,255,0 };
	label.AddComponent<TextLabelComponent>(glm::vec2(windowWidth / 2 - 125 , 10), "GAME ENGINE 2D --- VER 1.0", "charriot-font", green, true);
	//tank.Kill();
}

// ~ Start in Unity
void Game::Setup() {
	LoadLevel(1);

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

	// Reset all event handlers for the current frame
	eventBus->Reset();

	// Perform the subcription of the events for all systems
	registry->GetSystem<DamageSystem>().SubcribeToEvents(eventBus);
	registry->GetSystem<KeyboardControlSystem>().SubcribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->Update();

	// Update the systems
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileLifeCycleSystem>().Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// Invoke all systems render
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);

	if (isDebug) {
		registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);

		registry->GetSystem<RenderGUISystem>().Update(registry, camera);
	}

	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
