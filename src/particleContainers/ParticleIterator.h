#pragma once

#include "../Particle.h"
#include "Cell.h"

 struct ParticleIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Particle;
    using pointer = value_type *;
    using reference = value_type&;

    /**
     * @brief Definition of the * operator
     * @return The particle
     */
    virtual reference operator*() = 0;


    /**
     * @brief Prefix incrementation operator
     * @return the incremented PairIterator
     */
    virtual ParticleIterator &operator++() = 0;

    // postfix
    // CLang tidy bug?
    // virtual ParticleIterator operator++(int) = 0;

    virtual bool operator==(const ParticleIterator &b) const = 0;

    virtual bool operator!=(const ParticleIterator &b) const = 0;


private:

};

