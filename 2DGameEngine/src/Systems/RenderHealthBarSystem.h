#pragma once

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/HealthComponent.h"

#include <SDL.h>
#include <string>

class RenderHealthBarSystem : public System {
	public:

		RenderHealthBarSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
			RequireComponent<HealthComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
			for (auto entity : GetSystemEntities()) {
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto sprite = entity.GetComponent<SpriteComponent>();
				const auto health = entity.GetComponent<HealthComponent>();

				// Draw a health bar with HP
				SDL_Color healthBarColor = { 255,255,255 };

				if (health.heathPercentage >= 0 && health.heathPercentage < 40) {
					// RED
					healthBarColor = { 255, 0 ,0 };
				}
				if (health.heathPercentage >= 40 && health.heathPercentage < 80) {
					// YELLOW
					healthBarColor = { 255, 255 ,0 };
				}
				if (health.heathPercentage >= 80 && health.heathPercentage <= 100) {
					// GREEN
					healthBarColor = { 0, 255 ,0 };
				}

				// Positon of the health bar 
				int healthBarWidth = 15;
				int healthBarHeight = 3;
				double healthBarPosX = (transform.position.x + (sprite.width * transform.scale.x)) - camera.x;
				double healthBarPosY = (transform.position.y) - camera.y;

				SDL_Rect healthBarRectangle = {
					static_cast<int>(healthBarPosX),
					static_cast<int>(healthBarPosY),
					static_cast<int>(healthBarWidth * (health.heathPercentage / 100.0)),
					static_cast<int>(healthBarHeight),
				};
				SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
				SDL_RenderFillRect(renderer, &healthBarRectangle);

				// Render Health Text
				std::string healthText = std::to_string(health.heathPercentage);
				SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont("pico8-font-5"), healthText.c_str(), healthBarColor);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_FreeSurface(surface);

				int labelWidth = 0;
				int labelHeight = 0;
				
		

				SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

				SDL_Rect healthBarTextRectangle = {
					static_cast<int>(healthBarPosX),
					static_cast<int>(healthBarPosY) + 5,
					labelWidth,
					labelHeight
				};


				SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRectangle);

				SDL_DestroyTexture(texture);
			}
		}
};