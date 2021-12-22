#pragma once

#include <vector>
#include "../Particle.h"

struct Cell {
    /**
     * Normal default constructor
     */
    Cell();

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
     * Deletes all the pointers in the particle list
     */
    void clear();

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

    /**
     * Prints a string for debugging purposes
     */
    void toString();

    [[nodiscard]] const std::array<int, 3> &getIndex() const;

    void setIndex(const std::array<int, 3> &indexV);

    [[nodiscard]] const std::vector<std::array<int, 3>> &getNeighbors2D() const;

    void setNeighbors2D(const std::vector<std::array<int, 3>> &neighbors2DV);

    [[nodiscard]] const std::vector<std::array<int, 3>> &getNeighbors3D() const;

    void setNeighbors3D(const std::vector<std::array<int, 3>> &neighbors3DV);

    void setNeighbors2D();

    void setNeighbors3D();



private:

    /**
     * The Particles in each cell
     */
    std::vector<Particle*> particles;

    /**
     * The index of the cell
     */
    std::array<int, 3> index{};

    /**
     * The 2D neighbors of this cell
     */
    std::vector<std::array<int, 3>> neighbors2D{};

    /**
     * The 3D neighbors of this cell
     */
    std::vector<std::array<int, 3>> neighbors3D{};

};

template<typename... Args>
void Cell::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}

