#include "ParticleContainer.h"
#include "StoermerVerlet.h"

#include <iostream>


ParticleContainer::ParticleContainer() : particles{std::vector<Particle>{}} {

};

ParticleContainer::ParticleContainer(int DIM, double delta_t) :
                                            particles{std::vector<Particle>{}},
                                            DIM{DIM},
                                            delta_t{delta_t}  {}


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
    int i{};
    for (const auto & p : particles) {
        std::cout<<"Particle "<<i<<": "<<p.toString()<<std::endl;
        i++;
    }
}

void ParticleContainer::reserve(size_t size) {
    particles.reserve(size);
}








