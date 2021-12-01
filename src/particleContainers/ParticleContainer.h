#pragma once

#include <vector>

#include "Particle.h"
#include "ParticleIterator.h"

class ParticleContainer {
public:

    /**
     * @brief Provides the iterator for single particles at the start of the collection
     * @return iterator
     */
    virtual ParticleIterator begin() = 0;

    /**
     * @brief Provides the iterator for single particles at the end of the collection
     * @return iterator
     */
    virtual ParticleIterator end() = 0;

    /**
     * @brief Provides the _const_ iterator for single particles at the start of the collection
     * @return const iterator
     */
    [[nodiscard]] virtual ParticleIterator begin() const = 0;

    /**
     * @brief Provides the _const_ iterator for single particles at the end of the collection
     * @return const iterator
     */
    [[nodiscard]] virtual ParticleIterator end() const = 0;


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

