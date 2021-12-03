#pragma once

#include <vector>

#include "Particle.h"

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
     * @brief Provides the _const_ iterator for single particles at the start of the collection
     * @return const iterator
     */
    // [[nodiscard]] virtual std::vector<Particle>::const_iterator begin() const = 0;

    /**
     * @brief Provides the _const_ iterator for single particles at the end of the collection
     * @return const iterator
     */
    // [[nodiscard]] virtual std::vector<Particle>::const_iterator end() const = 0;

};

