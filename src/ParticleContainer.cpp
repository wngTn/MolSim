#include "ParticleContainer.h"

#include <iostream>


ParticleContainer::ParticleContainer() : particles{std::vector<Particle>{}} {}

ParticleContainer::ParticleContainer(int DIM, double delta_t) : DIM{DIM}, delta_t{delta_t},
    particles{std::vector<Particle>{}}  {}

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

void ParticleContainer::calculateF() {
    // set Force for all particles to zero
    for (auto &p : particles) {
        for (int i = 0; i < DIM; ++i) {
            p.setF(i, 0);
        }
    }
    // calculates new force
    for (auto &p1 : particles) {
        for (auto &p2 : particles) {
            if (p1 != p2) {
                grav_force(p1, p2);
            }
        }
    }
}

void ParticleContainer::grav_force(Particle &p1, const Particle &p2) {
    double sqrd_dist = 0;
    double result;
    // calculate the squared distance
    for (int i = 0; i < DIM; ++i) {
        sqrd_dist += ParticleContainer::sqr(p2.getX().at(i) - p1.getX().at(i));
    }
    // left side of the term
    double var = p1.getM() * p2.getM() / (sqrt(sqrd_dist) * sqrd_dist);
    // multiplying with (p2 - p1) and setting the force
    for (int i = 0; i < DIM; ++i) {
        result = p1.getF().at(i) + (var * (p2.getX().at(i) - p1.getX().at(i)));
        p1.setF(i, result);
    }
}

void ParticleContainer::calculateX() {
    double new_X;
    // calculate new positions
    for (auto &p : particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new position
            new_X = p.getX().at(i) + (delta_t * (p.getV().at(i) + delta_t * .5 / p.getM() * p.getF().at(i)));
            // set new position
            p.setX(i, new_X);
        }
        // set old force
        p.setOldF(p.getF());
    }
}

void ParticleContainer::calculateV() {
    double new_V;
    // calculate new velocities
    for (auto &p : particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new velocity
            new_V = p.getV().at(i) + (delta_t * 0.5 / p.getM() * (p.getF().at(i) + p.getOldF().at(i)));
            p.setV(i, new_V);
        }
    }
}





