#include "ECS.h"

int Entity::GetId() const {
	return id;
}
template <typename TComponent>
void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}