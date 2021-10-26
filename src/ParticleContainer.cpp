//
// Created by tony on 8/22/21.
//

#include "ParticleContainer.h"

#include <iostream>


ParticleContainer::ParticleContainer() : particles{std::vector<Particle>{}} {}

std::vector<Particle>::iterator ParticleContainer::begin() {
    return particles.begin();
}

std::vector<Particle>::iterator ParticleContainer::end() {
    return particles.end();
}

std::vector<Particle>::const_iterator ParticleContainer::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator ParticleContainer::end() const {
    return particles.end();
}


void ParticleContainer::push_back(const Particle& p) {
    particles.push_back(p);
}


size_t ParticleContainer::size() const noexcept {
    return particles.size();
}

void ParticleContainer::print() {
    for (const auto & p : particles) {
        std::cout<<p.toString()<<std::endl;
    }
}

void ParticleContainer::reserve(size_t size) {
    particles.reserve(size);
}



