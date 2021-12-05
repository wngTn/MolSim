#pragma once

#include <vector>
#include <cmath> /* sqrt */
#include <functional>

#include "Particle.h"
#include "ParticleContainer.h"

class DirectSumParticleContainer : public ParticleContainer{
public:

    /**
     * @brief Default constructor
     */
    DirectSumParticleContainer();

    void setup() override {
        // TODO maybe set all forces to zero
    }

    void cleanup() override {

    }

    template<typename... Args>
    void emplace_back(Args &&... args);

    void emplace_back(Particle&& ) override;
    void emplace_back(Particle& ) override;
    void emplace_back( std::array<double, 3> x, std::array<double, 3> v, double m, int t) override;

    std::vector<Particle>::iterator begin() override;

    std::vector<Particle>::iterator end() override;

    [[nodiscard]] std::vector<Particle>::const_iterator begin() const override;

    [[nodiscard]] std::vector<Particle>::const_iterator end() const override;

    [[nodiscard]] size_t size() const noexcept override;

    /**
     * @brief Adds a particle to the container
     * @param p The particle that should be added
     */
    void push_back(const Particle &p) override;
    void push_back(const Particle&& p) override;

    /**
     * @brief Prints the content of the container
     */
    void print();


    void reserve(size_t size) override;


    /*
     * @brief Custom Iterator iterating over all distinct pairs in the Container
     */
    struct PairIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>>;
        using pointer = value_type *;
        using reference = value_type;

        /**
         * @brief Definition of the * operator
         * @return The particle pair
         */
        reference operator*() {
            return {vec[i], vec[j]};
        }

        /**
         * @brief Prefix incrementation operator
         * @return the incremented PairIterator
         */
        PairIterator &operator++() {

            if (j == vec.size() - 1) {
                i++;
                j = i + 1;
            } else {
                j++;
            }
            return *this;
        }

        // postfix
        // CLang tidy bug?
        PairIterator operator++(int) {
            PairIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const PairIterator &b) {
            const size_t max = vec.size();
            if ((this->i >= max || this->j >= max) && (b.i >= max || b.j >= max)) {
                return true;
            }
            return this->i == b.i && this->j == b.j;
        }

        bool operator!=(const PairIterator &b) {
            return !(*this == b);
        }


        PairIterator(std::vector<Particle> &vec, size_t i, size_t j) : vec{vec}, i{i}, j{j} {}

    private:

        std::vector<Particle> &vec;
        size_t i;
        size_t j;

        friend class ParticleContainer;
    };

    /**
     * @brief Provides the iterator for particle pairs at the start of the collection. The iterator iterates
     * through every distinct pair.
     * @return PairIterator
     */
    PairIterator pair_begin();

    /**
     * @brief Provides the iterator for particle pairs at the end of the collection.
     * @return PairIterator
     */
    PairIterator pair_end();

private:
    /**
     * Vector that contains the particles
     */
    std::vector<Particle> particles;

};

template<typename... Args>
void DirectSumParticleContainer::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}

