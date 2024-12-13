#ifndef  ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>

const unsigned int MAX_COMPONENTS = 32;

/// <summary>
/// Signature
/// We use bitset 1010101 to keep track of whichs components an entity has,
/// and also helps keep track of which entities a system is interested in
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
	protected :
		static int nextId;
};

template <typename T>
class Component : public IComponent{
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
		int GetId() const;

		Entity& operator =(const Entity& other) = default;
		bool operator ==(const Entity& other) const { return id == other.id; }
		bool operator !=(const Entity& other) const { return id != other.id; }
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }
};

/// <summary>
/// System
/// The System processes entities that contains a specific signature
/// </summary>
class System {
	private:
		Signature componentSignature;     // 01010110011010
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
		virtual ~IPool() {}
};

template <typename T>
class Pool:public IPool {
	private:
		std::vector<T> data;

	public:
		Pool(int size = 100) {
			data.resize(size);
		}
		 virtual Pool() = default;

		bool isEmpty() const {
			return data.empty();
		}

		int GetSize() const {
			return data.size();
		}

		void Resize(int n) {
			data.resize(n);
		}

		void Clear() {
			data.clear();
		}

		void Add(T object) {
			data.push_back(object);
		}

		void Set(int index, T object) {
			data[index] = object;
		}

		T& Get(int index) {
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
	std::vector<IPool*> componentPools;

	// Vector of component signatures per entity, saying which component is turned "on" for a given entity
	// [Vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// Map of active system [ index = system id ]
	std::unordered_map<std::type_index, System*> systems;

	// Set of entities that are flagged to be added or removed in the next Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

public:
	Registry() = default;

	void Update();

	Entity CreateEntity();
	// Entity Management
	void AddEntityToSystem(Entity entity);

	// Component management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);


	// TODO:
	// Kill
	// removeComponent

	// GetComponent

	// add system()

	// update: add/remove

};

template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const auto componentId = Component<TComponent>:GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		Pool<TComponent>* newComponentPool = new Pool<TComponent>();
		componentPools[componentId] = newComponentPool;
	}

	Pool<TComponent>* componentPool = componentPools[componentId];
	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities)
	}

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId;)
}


#endif 
