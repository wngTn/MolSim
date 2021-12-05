#pragma once

#include <vector>
#include "../Particle.h"

struct Cell {
    /**
     * Normal default constructor
     */
    Cell() : particles{std::vector<Particle*>{}} {};

    void setParticles(const std::vector<Particle*> &particlesV) {
        Cell::particles = particlesV;
    }

    std::vector<Particle*> &getParticles() {
        return particles;
    }

    [[nodiscard]] const std::vector<Particle*> &getParticles() const {
        return particles;
    }

    /**
     * @brief Provides the iterator for single particles at the start of the collection
     * @return iterator
     */
    std::vector<Particle*>::iterator begin();

    /**
     * @brief Provides the iterator for single particles at the end of the collection
     * @return iterator
     */
    std::vector<Particle*>::iterator end();

    /**
     * @brief Provides the _const_ iterator for single particles at the start of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle*>::const_iterator begin() const;

    /**
     * @brief Provides the _const_ iterator for single particles at the end of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle*>::const_iterator end() const;

    void add_particle(Particle&);

    /**
     * @brief Moves an object to the collection, i.e. uses one of the constructor of the Particle class
     * and creates an object directly
     * @tparam Args Parameter pack
     * @param args The parameter for the constructor
     */
    template<typename... Args>
    void emplace_back(Args &&... args);

    /*
     * Removes particle at the position
     */
    std::vector<Particle*>::iterator erase (std::vector<Particle*>::const_iterator position);

    void toString();

private:
    /**
     * The Particles in each cell
     */
    std::vector<Particle*> particles;

};

