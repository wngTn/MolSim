#pragma once

#include "Particle.h"
#include "nlohmann/json.hpp"
#include "particleContainers/ParticleContainer.h"
#include "physicsCalculator/PhysicsCalc.h"

class CheckpointWriter {
public:
	static void writeCheckpoint(std::string &filename, ParticleContainer &container, PhysicsCalc &calc, int iteration);

};


