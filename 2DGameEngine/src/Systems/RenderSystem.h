#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL.h>

class RenderSystem : public System {
public:
	RenderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer) {
		// Loop all entities that the system is interested in 
		for (auto entity : GetSystemEntities()) {

			// Update entity position based on its velocity every frame of the game loop 
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect obj = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width,
				sprite.height
			};

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &obj);
		}

	}
};

#endif // !MOVEMENTSYSTEM_H
