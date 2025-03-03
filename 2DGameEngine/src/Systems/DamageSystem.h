#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System {
	public:
		DamageSystem() {
			RequireComponent<BoxColliderComponent>();
		}

		void SubcribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubcribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
		}

		void onCollision(CollisionEvent& event) {
			Entity a = event.a;
			Entity b = event.b;
			Logger::Log("Damage system : " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

			if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
				OnProjectileHitsPlayer(a, b);
			}
			if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
				OnProjectileHitsPlayer(b, a);
			}
			if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
				OnProjectileHitsEnemy(a, b);
			}
			if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
				OnProjectileHitsEnemy(b, a);
			}
		}

		void OnProjectileHitsPlayer(Entity projectile, Entity player) {
			auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

			if (!projectileComponent.isFriendly) {
				auto& health = player.GetComponent<HealthComponent>();

				// Subtract health player
				health.heathPercentage -= projectileComponent.hitPercentDamage;

				// Die
				if (health.heathPercentage <= 0) {
					player.Kill();
				}

				projectile.Kill();
			}
		}

		void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
			const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

			if (projectileComponent.isFriendly) {
				auto& health = enemy.GetComponent<HealthComponent>();

				// Subtract health player
				health.heathPercentage -= projectileComponent.hitPercentDamage;

				// Die
				if (health.heathPercentage <= 0) {
					enemy.Kill();
				}

				projectile.Kill();
			}
		}

		void Update() {

		}
};

#endif // !DAMAGESYSTEM_h
