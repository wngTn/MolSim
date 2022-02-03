#pragma once

#include "particleContainers/ParticleContainer.h"

#include <cfloat>

class Thermostat {

public:
	Thermostat() = default;

	Thermostat(double initialTemperature, double targetTemperature, double maxDeltaTemperature, bool excludeY = false);

	void setupTemperature(ParticleContainer &particles) const;

	void applyTemperature(ParticleContainer &particles) const;

private:

	static double calculateKineticEnergy(ParticleContainer &particles, bool exclY);

	static double calculateCurrentTemp(ParticleContainer &particles, bool exclY);

	static void scaleVelocities(ParticleContainer &particles, double beta, bool exclY);

	static double calculateMeanYVelocity(ParticleContainer &container);

	double initialTemperature;
	double targetTemperature;
	double maxDeltaTemperature;

	bool excludeY;

};

