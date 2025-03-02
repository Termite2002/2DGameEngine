#pragma once

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileLifeCycleSystem : public System {
	public:
		ProjectileLifeCycleSystem() {
			RequireComponent<ProjectileComponent>();
		}

		void Update() {
			for (auto entity : GetSystemEntities()) {
				auto projectile = entity.GetComponent<ProjectileComponent>();

				// Kill projectiles after thay reach their duration limit
				if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
					entity.Kill(); 
				}
			}
		}
};