#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components//BoxColliderComponent.h"
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
			Logger::Log("Damage system : " + std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));
			//event.a.Kill();
			//event.b.Kill();
		}

		void Update() {

		}
};

#endif // !DAMAGESYSTEM_h
