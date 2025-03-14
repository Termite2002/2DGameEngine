#ifndef  ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <set>
#include <deque>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include <iostream>


const unsigned int MAX_COMPONENTS = 32;

/// <summary>
/// Signature
/// We use bitset 1010101 to keep track of whichs components an entity has,
/// and also helps keep track of which entities a system is interested in
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
	protected :
		// Index in Signature
		static int nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component : public IComponent{
public:
	// Returns the unique id of Component<T>
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};


class Entity {
	private:
		int id;
	public:
		Entity(int id) : id(id) {};
		Entity(const Entity& entity) = default;
		void Kill();
		int GetId() const;

		// Manage entity tags and groups
		void Tag(const std::string& tag);
		bool HasTag(const std::string& tag) const;
		void Group(const std::string& group);
		bool BelongsToGroup(const std::string& group) const;

		Entity& operator =(const Entity& other) = default;
		bool operator ==(const Entity& other) const { return id == other.id; }
		bool operator !=(const Entity& other) const { return id != other.id; }
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }

		template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
		template <typename TComponent> void RemoveComponent();
		template <typename TComponent> bool HasComponent() const;
		template <typename TComponent> TComponent& GetComponent() const;

		// Hold a pointer to the entity's owner registry
		class Registry* registry;
};

/// <summary>
/// System
/// The System processes entities that contains a specific signature
/// </summary>
class System {
	private:
		// Which components an entity must have for the system to consider the entity
		Signature componentSignature;     // 01010110011010

		// List of all Entity that the system is interested in 
		std::vector<Entity> entities;     // tank, helicopter, pokemon..

	public:
		System() = default;
		~System() = default;

		void AddEntityToSystem(Entity entity);
		void RemoveEntityFromSystem(Entity entity);
		std::vector<Entity> GetSystemEntities() const;
		const Signature& GetComponentSignature() const;

		// Define the component type that entities must have to be considered by the system
		template <typename TComponent> void RequireComponent();
};


/// <summary>
/// Pool
/// A pool is just a vector (continous data) of objects of type T
/// </summary>

class IPool {
	public:
		virtual ~IPool() = default;
		virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool:public IPool {
	private:
		std::vector<T> data;
		int size;

		// Keep track of entity ids per index, so the vector is always packed
		std::unordered_map<int, int> entityIdToIndex;
		std::unordered_map<int, int> indexToEntityId;

	public:
		Pool(int capacity = 100) {
			size = 0;
			data.resize(capacity);
		}
		virtual ~Pool() = default;

		bool IsEmpty() const {
			return size == 0;
		}

		int GetSize() const {
			return size;
		}

		void Resize(int n) {
			data.resize(n);
		}

		void Clear() {
			data.clear();
			size = 0;
		}

		void Add(T object) {
			data.push_back(object);
		}

		void Set(int entityId, T object) {
			if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
				// if the element already exists, simply replace the componet object
				int index = entityIdToIndex[entityId];
				data[index] = object;
			}
			else {
				// Adding a new object 
				int index = size;
				entityIdToIndex.emplace(entityId, index);
				indexToEntityId.emplace(index, entityId);
				if (index >= data.capacity()) {
					// Resize data
					data.resize(size * 2);
				}
				data[index] = object;
				size++;
			}
		}

		void Remove(int entityId) {
			// copy the last element to the deleted position to keep the array packed
			int indexOfRemoved = entityIdToIndex[entityId];
			int indexOfLast = size - 1;
			data[indexOfRemoved] = data[indexOfLast];

			// Update the index-entity maps to point to the correct elements
			int entityIdOfLastElement = indexToEntityId[indexOfLast];
			entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
			indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

			entityIdToIndex.erase(entityId);
			indexToEntityId.erase(indexOfLast);

			size--;
		}

		void RemoveEntityFromPool(int entityId) override {
			if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
				Remove(entityId);
			}
		}

		T& Get(int entityId) {
			int index = entityIdToIndex[entityId];
			return static_cast<T&>(data[index]);
		}

		T& operator [](unsigned int index) {
			return data[index];
		}
};

/// <summary>
/// Registry 
/// The Registry manages the creation and destruction of entities
/// adding systems and adding components to entities
/// </summary>
class Registry {
private:
	// Keep track of how many entities were added to the scene
	int numEntities = 0;

	// Vector of component pools, 
	// each pool contains all the data for a certain component type
	// Vector index = component type id
	// Pool index == entity id
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which component is turned "on" for a given entity
	// [Vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// Map of active system [ index = system id ]
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities that are flagged to be added or removed in the next Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

	// Entity Tags (one tag per entity)
	std::unordered_map<std::string, Entity> entityPerTag;
	std::unordered_map<int, std::string> tagPerEntity;

	// Entity groups (a set of entities per group)
	std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<int, std::string> groupPerEntity;

	// List of free entity ids that were previously removed
	std::deque<int> freeIds;

public:
	Registry() {
		Logger::Log("Registry constructer called");
	}

	~Registry() {
		Logger::Log("Registry destructer called");
	}

	// The registry Update() finally processes the entities that are waiting to be added/ killed
	void Update();

	// Entity management
	Entity CreateEntity();
	void KillEntity(Entity entity);

	// Tag Management
	void TagEntity(Entity entity, const std::string& tag);
	bool EntityHasTag(Entity enitty, const std::string& tag) const;
	Entity GetEntityByTag(const std::string& tag) const;
	void RemoveEntityTag(Entity entity);

	// Group Management
	void GroupEntity(Entity entity, const std::string& group);
	bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
	std::vector<Entity> GetEntitiesByGroup(const std::string& tag) const;
	void RemoveEntityGroup(Entity entity);

	// Component management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity) const;
	template <typename TComponent> TComponent& GetComponent(Entity entity) const;

	// System management
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem();
	template <typename TSystem> bool HasSystem() const;
	template <typename TSystem> TSystem& GetSystem() const;


	// Add and remove entities from their systems
	void AddEntityToSystems(Entity entiy);
	void RemoveEntityFromSystems(Entity entity);

};

//// System //////////////////
template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> 
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem> 
bool Registry::HasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

//////////////////////////////////////


// Registry
///////////////////////////////////////////////////////
template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	entityComponentSignatures[entityId].set(componentId);

	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));

	std::cout << "Componet id = " << componentId << "--> POOL size: " << componentPool->GetSize() << std::endl;
}


template <typename TComponent> 
void Registry::RemoveComponent(Entity entity) {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	componentPool->Remove(entityId);

	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed to entity id " + std::to_string(entityId));
}


template <typename TComponent> 
bool Registry::HasComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}
////////////////////////////////////////////////////////////

// Entity /////////////////////////////////////////////////

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}


template <typename TComponent>
void Entity::RemoveComponent() {
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
	return registry->GetComponent<TComponent>(*this);
}


///////////////////////////////////////////////////////////
#endif 
