#pragma once

struct HealthComponent {
	int heathPercentage;

	HealthComponent(int healthPercentage = 0) {
		this->heathPercentage = healthPercentage;
	}
};