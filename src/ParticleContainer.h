#pragma once

#include <vector>
#include "Particle.h"

//TODO: describe the wrapper pattern (Adapter Pattern?)

class ParticleContainer {
public:
    /**
     * Constructor
     */
    ParticleContainer();

    /**
     * Moves an object to the collection, i.e. uses one of the constructor of the Particle class
     * and creates an object directly
     * @tparam Args
     * @param args The parameter for the constructor
     */
    template<typename... Args>
    void emplace_back(Args &&... args);

    /**
     * Provides the iterator for the start of the collection
     * @return iterator
     */
    std::vector<Particle>::iterator begin();

    /**
     * Provides the iterator for the end of the collection
     * @return iterator
     */
    std::vector<Particle>::iterator end();

    //TODO Severin, iterator für Particle Pairs

    /**
     * Provides the const iterator for the start of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle>::const_iterator begin() const;

    /**
     * Provides the const iterator for the end of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle>::const_iterator end() const;

    /**
     * Gives information about the size
     * @return The size of the container
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * Adds a particle to the container
     * @param p The particle that should be added
     */
    void push_back(const Particle& p);

    /**
     * Prints the content of the container
     */
    void print();

    /**
     * Allocates the vector size so it doesn't resize automatically
     * @param size The size of the vector
     */
    void reserve(size_t size);

private:
    /**
     * Vector that contains the particles
     */
    std::vector<Particle> particles;
};

template<typename... Args>
void ParticleContainer::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}


