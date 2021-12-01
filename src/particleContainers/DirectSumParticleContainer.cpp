#include <iostream>

#include "DirectSumParticleContainer.h"
#include "DirectSumParticleIterator.h"


DirectSumParticleContainer::DirectSumParticleContainer() : particles{std::vector<Particle>{}} {}

// TODO
std::vector<Particle>::iterator DirectSumParticleContainer::begin() {
    return std::vector<Particle>::iterator{};
}

std::vector<Particle>::iterator DirectSumParticleContainer::end() {
    return std::vector<Particle>::iterator{};
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::begin() const {
    return std::vector<Particle>::const_iterator{};
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::end() const {
    return std::vector<Particle>::const_iterator{};
}

/*
std::vector<Particle>::const_iterator DirectSumParticleContainer::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator DirectSumParticleContainer::end() const {
    return particles.end();
}*/

void DirectSumParticleContainer::push_back(const Particle &p) {
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








