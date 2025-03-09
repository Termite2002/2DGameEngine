#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

#include <algorithm> 

class MovementSystem : public System {
	public:
		MovementSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();
		}

		void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubcribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
		}

		void OnCollision(CollisionEvent& event) {
			Entity a = event.a;
			Entity b = event.b;
			Logger::Log("Damage system : " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

			if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
				OnEnemyHitsObstacle(a, b);
			}
			if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
				OnEnemyHitsObstacle(b, a);
			}
		}

		void OnEnemyHitsObstacle(Entity enemy, Entity obstacle) {
			if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
				auto& enemyRigidbody = enemy.GetComponent<RigidBodyComponent>();
				auto& enemySprite = enemy.GetComponent<SpriteComponent>();

				if (enemyRigidbody.velocity.x != 0) {
					enemyRigidbody.velocity.x *= -1;
					enemySprite.flip = (enemySprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				}
				if (enemyRigidbody.velocity.y != 0) {
					enemyRigidbody.velocity.y *= -1;
					enemySprite.flip = (enemySprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
				}

			}

		}

		void Update(double deltaTime) {
			// Loop all entities that the system is interested in 
			for (auto entity : GetSystemEntities()) {

				// Update entity position based on its velocity every frame of the game loop 
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

				transform.position.x += rigidbody.velocity.x * deltaTime;
				transform.position.y += rigidbody.velocity.y * deltaTime;

				bool isEntityOutsideMap = (
					transform.position.x < 0 || transform.position.x > Game::mapWidth ||
					transform.position.y < 0 || transform.position.y > Game::mapHeight
					);

				// Prevent the main player from going outside the map
				if (entity.HasTag("player")) {
					transform.position.x = std::clamp(transform.position.x, 10.0f, Game::mapWidth - 50.0f);
					transform.position.y = std::clamp(transform.position.y, 10.0f, Game::mapHeight - 50.0f);
				}

				// Kill entity if it is outside the map
				if (isEntityOutsideMap && !entity.HasTag("player")) {
					entity.Kill();
				}
			}

		}
};

#endif // !MOVEMENTSYSTEM_H
