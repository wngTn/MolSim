#include <iostream>
#include "Cell.h"


std::vector<Particle*>::iterator Cell::begin() {
    return particles.begin();
}

std::vector<Particle*>::iterator Cell::end() {
    return particles.end();
}

std::vector<Particle*>::const_iterator Cell::begin() const {
    return particles.begin();
}

std::vector<Particle*>::const_iterator Cell::end() const {
    return particles.end();
}

std::vector<Particle*>::iterator Cell::erase(std::vector<Particle*>::const_iterator position) {
    return particles.erase(position);
}

void Cell::add_particle(Particle& p){
    Particle* pp = &p;
    particles.emplace_back(pp);
}

void Cell::toString() {
    std::cout<<"Cell with: {";
    for (const auto & p : particles) {
        std::cout<<"Particle-"<<p->getType()<<", ";
    }
    std::cout<<"}"<<std::endl;
}

template<typename... Args>
void Cell::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}