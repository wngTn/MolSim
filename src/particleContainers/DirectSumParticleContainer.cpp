#include <iostream>

#include "DirectSumParticleContainer.h"


DirectSumParticleContainer::DirectSumParticleContainer() : particles{std::vector<Particle>{}} {}

std::vector<Particle>::iterator DirectSumParticleContainer::begin() {
    return particles.begin();
}

std::vector<Particle>::iterator DirectSumParticleContainer::end() {
    return particles.end();
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::end() const {
    return particles.end();
}

/*
std::vector<Particle>::const_iterator DirectSumParticleContainer::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::end() const {
    return particles.end();
}*/

void DirectSumParticleContainer::emplace_back(Particle&& part) {
    particles.emplace_back(part);
}

void DirectSumParticleContainer::emplace_back(Particle& part) {
    particles.emplace_back(part);
}

void DirectSumParticleContainer::emplace_back(std::array<double, 3> x, std::array<double, 3> v, double m, int t) {
    particles.emplace_back(x,v,m,t);
}

void DirectSumParticleContainer::push_back(const Particle&& p) {
    particles.push_back(p);
}

void DirectSumParticleContainer::push_back(const Particle& p) {
    particles.push_back(p);
}


size_t DirectSumParticleContainer::size() const noexcept {
    return particles.size();
}

void DirectSumParticleContainer::print() {
    int i{};
    for (const auto &p: particles) {
        std::cout << "Particle " << i << ": " << p.toString() << std::endl;
        i++;
    }
}

void DirectSumParticleContainer::reserve(size_t size) {
    particles.reserve(size);
}

DirectSumParticleContainer::PairIterator DirectSumParticleContainer::pair_begin() {
    // ++ to skip pair (0,0)
    return {particles, 0, 1};
}

DirectSumParticleContainer::PairIterator DirectSumParticleContainer::pair_end() {
    auto pit = DirectSumParticleContainer::PairIterator{particles, particles.size(), particles.size()};
    return pit;
}
