#pragma once

#include <SDL.h>

struct ProjectileComponent {
	bool isFriendly;
	int hitPercentDamage;
	int duration;
	int startTime;

	ProjectileComponent(bool isFirendly = false, int hitPercentDamage = 0, int duration = 0) {
		this->isFriendly = isFirendly;
		this->hitPercentDamage = hitPercentDamage;
		this->duration = duration;
		this->startTime = SDL_GetTicks();
	}
};
