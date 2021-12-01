#pragma once

#include "ParticleIterator.h"

class DirectSumParticleIterator : public ParticleIterator {

public:
    /**
         * @brief Definition of the * operator
         * @return The particle
         */
    reference operator*() override{
        return particles[particle_index];
    }


    /**
         * @brief Prefix incrementation operator
         * @return the incremented PairIterator
         */
    ParticleIterator &operator++() override{
        if(particle_index < current_size){
            particle_index++;
        }
        return *this;
    }

    bool operator==(const ParticleIterator &b) const override{
        auto b_casted = static_cast<const DirectSumParticleIterator&>(b);

        return this->particle_index == b_casted.particle_index;

    }

    bool operator!=(const ParticleIterator &b) const override{
        return !(*this == b);
    }


    DirectSumParticleIterator(std::vector<Particle> &vec, size_t partI) :   particles{vec},
                                                                            current_size{vec.size()},
                                                                            particle_index{partI} {}
private:
    std::vector<Particle> &particles;
    size_t current_size;
    size_t particle_index;
};
