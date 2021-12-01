#pragma once

#include "ParticleIterator.h"

class LinkedCellParticleIterator : public ParticleIterator {

public:
    reference operator*() override {
        return cells[cell_index].getParticles()[particle_index];
    }

    /**
         * @brief Prefix incrementation operator
         * @return the incremented PairIterator
         */
    ParticleIterator &operator++() override {
        if(particle_index < cells[cell_index].getParticles().size()){
            particle_index++;
        }else{

                cell_index++;
                if(cell_index < cell_count){
                    particle_index = 0;
                }
        }
        return *this;
    }

    // postfix
    // CLang tidy bug?
/*    ParticleIterator operator++(int) override {
        LinkedCellParticleIterator tmp = *this;
        ++(*this);
        return tmp;
    }*/

    bool operator==(const ParticleIterator &b) const override{
        auto b_casted = static_cast<const LinkedCellParticleIterator&>(b);
        // idk why?
        if(this->cell_index >= this->cell_count && b_casted.cell_index >= b_casted.cell_count){
            return true;
        }
        return this->cell_index == b_casted.cell_index && this->particle_index == b_casted.particle_index;

    }

    bool operator!=(const ParticleIterator &b) const override {
        return !(*this == b);
    }



    LinkedCellParticleIterator(std::vector<Cell> &vec, size_t cellI, size_t partI) :  cells{vec},
                                                                                      particle_index{partI},
                                                                                      cell_index{cellI},
                                                                                      cell_count{vec.size()} {}
private:
    std::vector<Cell> &cells;
    size_t particle_index;
    size_t cell_index;
    size_t cell_count;
};
