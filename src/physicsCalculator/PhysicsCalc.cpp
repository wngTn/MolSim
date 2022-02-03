#include "PhysicsCalc.h"
#include "utils/ArrayUtils.h"

void PhysicsCalc::calcV(ParticleContainer &particles) const {
	// calculate new velocities
	for (auto &p : particles) { // loop over every particle
		auto newV = p.getV() + delta_t * 0.5 / p.getM() * (p.getF() + p.getOldF());
		p.setV(newV);
	}
}

void PhysicsCalc::calcX(ParticleContainer &particles) const {
	// calculate new positions
	for (auto &p : particles) { // loop over every particle
		// go through all three dimensions

		auto newX = p.getX() + delta_t * (p.getV() + delta_t * 0.5 / p.getM() * p.getF());
		p.setX(newX);
	}
}

void PhysicsCalc::setDeltaT(double dt) {
	delta_t = dt;
}

void PhysicsCalc::setDim(int d) {
	DIM = d;
}

double PhysicsCalc::getDeltaT() const {
	return delta_t;
}

int PhysicsCalc::getDim() const {
	return DIM;
}

