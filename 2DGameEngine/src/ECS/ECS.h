#ifndef  ECS_H
#define ECS_H

#include <bitset>
#include <vector>

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
		int GetId() const;
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
		Signature& GetComponentSignature() const;

		// Define the component type that entities must have to be considered by the system
		template <typename TComponent> void RequireComponent();
};

class Registry {

};


#endif // ! ECS_H
