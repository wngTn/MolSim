#include "LennardJones.h"
#include "utils/ArrayUtils.h"

#include <iostream>

namespace calculator {

void LennardJones::calcF(ParticleContainer &container) {
	auto &particles = static_cast<DirectSumParticleContainer &>(container);
	for (auto &p : particles) {
		p.setOldF(p.getF());
		p.setF({0., 0., 0.});
	}

	for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
		auto[p1, p2] = *it;
		ljforce(p1, p2);
	}
}

void LennardJones::ljforce(Particle &p1, Particle &p2) {
	double sqrd_dist = 0;
	for (int i = 0; i < DIM; i++) {
		sqrd_dist += LennardJones::sqr(p2.getX()[i] - p1.getX()[i]);
	}
	double s = LennardJones::sqr(sigma) / sqrd_dist;
	s = s * s * s; // s = sqr(s) * s
	double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);

	auto force = f * (p2.getX() - p1.getX());

	p1.setF(p1.getF() + force);
	p2.setF(p2.getF() - force);
}

std::string LennardJones::toString() {
	return "LennardJones";
}
}