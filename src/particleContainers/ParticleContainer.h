#pragma once

#include <vector>

#include "Particle.h"
#include "PairIterator.h"

class ParticleContainer {
public:

    /**
     * @brief prepares the container for calcualtions. E.g. for LinkedCellContainer constructs the cells
     */
    virtual void setup() = 0;

    /**
     * @brief performs some sort of cleanup after calculations. Not guaranteed to be called after every iteration
     */
    virtual void cleanup() = 0;

    /**
    * @brief Allocates the vector size so it doesn't resize automatically
    * @param size The size of the vector
    */
    virtual void reserve(size_t) = 0;

    /**
     * @brief Adds a particle to the container
     * @param p The particle that should be added
     */
    virtual void push_back(const Particle &p) = 0;
    virtual void push_back(const Particle&& p) = 0;
    // i know this should be templated with a parameter pack, but templates and inheritance don't mix easily
    /**
   * @brief Moves a Particle to the collection
   * @param Particle The particle
   */
    virtual void emplace_back(Particle&&) = 0;

    /**
    * @brief Moves a Particle to the collection
    * @param Particle The particle
    */
    virtual void emplace_back(Particle&) = 0;

    /**
     * @brief Moves a Particle to the collection using Particle constructor
     * @param x position
     * @param v velocity
     * @param m mass
     * @param t type
     */
    virtual void emplace_back(const std::array<double, 3>& x, const std::array<double, 3>& v, double m, int t) = 0;


    /**
    * @brief Gives information about the size
    * @return The size of the container
    */
    [[nodiscard]] virtual size_t size() const noexcept = 0;

    /**
     * @brief Provides the iterator for single particles at the start of the collection
     * @return iterator
     */
    virtual std::vector<Particle>::iterator begin() = 0;

    /**
     * @brief Provides the iterator for single particles at the end of the collection
     * @return iterator
     */
    virtual std::vector<Particle>::iterator end() = 0;

    /**
     * @brief Provides the _const_ iterator for single particles at the start of the collection
     * @return const iterator
     */
    [[nodiscard]] virtual std::vector<Particle>::const_iterator begin() const = 0;

    /**
     * @brief Provides the _const_ iterator for single particles at the end of the collection
     * @return const iterator
     */
    [[nodiscard]] virtual std::vector<Particle>::const_iterator end() const = 0;

    /**
    * @brief Provides the iterator for particle pairs at the start of the collection. The iterator iterates
    * through every distinct pair.
    * @return PairIterator
    */
    virtual PairIterator pair_begin() = 0;

    /**
     * @brief Provides the iterator for particle pairs at the end of the collection.
     * @return PairIterator
     */
    virtual PairIterator pair_end() = 0;


    /**
     * @brief Returns the number of dimensions for this Container
     */
     virtual int dimensions() {
         return 3;
     };



    friend void to_json(nlohmann::json& j, const ParticleContainer& container);

    friend void from_json(const nlohmann::json&j, ParticleContainer& container);

};

using json = nlohmann::json;

