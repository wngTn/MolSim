#pragma once

#include <vector>
#include <cmath> /* sqrt */
#include <functional>
#include "Particle.h"

class ParticleContainer {
public:

    /**
     * @brief Default constructor
     */
    ParticleContainer();

/**
     * @brief Constructor
     * @param DIM the dimension we are in
     * @param delta_t the delta_t given over the command line
     */
    ParticleContainer(int DIM, double delta_t);

    /**
     * @brief Moves an object to the collection, i.e. uses one of the constructor of the Particle class
     * and creates an object directly
     * @tparam Args Parameter pack
     * @param args The parameter for the constructor
     */
    template<typename... Args>
    void emplace_back(Args &&... args);

    /**
     * @brief Provides the iterator for single particles at the start of the collection
     * @return iterator
     */
    std::vector<Particle>::iterator begin();

    /**
     * @brief Provides the iterator for single particles at the end of the collection
     * @return iterator
     */
    std::vector<Particle>::iterator end();

    /**
     * @brief Provides the _const_ iterator for single particles at the start of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle>::const_iterator begin() const;

    /**
     * @brief Provides the _const_ iterator for single particles at the end of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Particle>::const_iterator end() const;

    /**
     * @brief Gives information about the size
     * @return The size of the container
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * @brief Adds a particle to the container
     * @param p The particle that should be added
     */
    void push_back(const Particle &p);

    /**
     * @brief Prints the content of the container
     */
    void print();

    /**
     * @brief Allocates the vector size so it doesn't resize automatically
     * @param size The size of the vector
     */
    void reserve(size_t size);


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
            return std::make_pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>>(vec[i], vec[j]);
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

    private:
        PairIterator(std::vector<Particle> &vec, size_t i, size_t j) : vec{vec}, i{i}, j{j} {}

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
void ParticleContainer::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}

