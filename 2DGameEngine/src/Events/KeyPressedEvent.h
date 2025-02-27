#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include <SDL.h>

class KeyPressedEvent : public Event {
	public: 
		SDL_KeyCode symbol;
		KeyPressedEvent(SDL_KeyCode symbol) : symbol(symbol) {}
};

#endif // !KEYPRESSEDEVENT_H
