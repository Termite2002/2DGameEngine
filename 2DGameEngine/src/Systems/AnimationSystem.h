#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "SDL.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

class AnimationSystem : public System {
	public:
		AnimationSystem() {
			RequireComponent<SpriteComponent>();
			RequireComponent<AnimationComponent>();
		}

		void Update() {
			for (auto entity : GetSystemEntities()) {
				auto& animation = entity.GetComponent<AnimationComponent>();
				auto& sprite = entity.GetComponent<SpriteComponent>();

				// Change the current frame
				// Change the source rectangle of the sprite
				animation.currentFrame = 
					((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % 
						animation.numFrames;
				sprite.srcRect.x = animation.currentFrame * sprite.width;
			}
		}
};


#endif // !ANIMATIONSYSTEM_H
