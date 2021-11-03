#pragma once

#include <vector>
#include <cmath> /* sqrt */
#include <functional>
#include "Particle.h"


//TODO: describe the wrapper pattern (Adapter Pattern?)

class ParticleContainer {
public:
    /**
     * Default constructor
     */
    ParticleContainer();

/**
     * Constructor
     * @param DIM the dimension we are in
     * @param delta_t the delta_t given over the command line
     */
    ParticleContainer(int DIM, double delta_t);

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
    void push_back(const Particle &p);

    /**
     * Prints the content of the container
     */
    void print();

    /**
     * Allocates the vector size so it doesn't resize automatically
     * @param size The size of the vector
     */
    void reserve(size_t size);

    /**
     * Calculates the forces between the particles
     */
    void calculateF();

    /**
     * Calculates the positions of the particles
     */
    void calculateX();

    /**
     * Calculates the velocities of the particles
     */
     void calculateV();

     // custom Iterator iterating over all distinct pairs in the Container
    struct PairIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>>;
        using pointer = value_type*;
        using reference = value_type;

        reference operator*() {
            return std::make_pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>>(vec[i], vec[j]);
        }

        // prefix
        PairIterator& operator++() {

            if(j == vec.size()-1){
                i++;
                j = i+1;
            }else{
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

        bool operator== (const PairIterator& b) {
            const size_t max = vec.size();
            if((this->i >= max || this->j >= max) && (b.i >= max || b.j >= max)){
                return true;
            }
            return this->i == b.i && this->j == b.j;
        }

        bool operator!= (const PairIterator& b) {
            return !(*this == b);
        }

    private:
        PairIterator(std::vector<Particle>& vec, size_t i, size_t j) : vec(vec), i(i), j(j) {}

        std::vector<Particle>& vec;
        size_t i;
        size_t j;
        friend class ParticleContainer;
    };

    PairIterator pair_begin(){
        // ++ to skip pair (0,0)
        return {particles, 0, 1};
    }

    PairIterator pair_end(){
        return {particles, particles.size(), particles.size()};
    }

    /**
* Returns the square of a number
* @param x: the number
*/
    template<typename T>
    T sqr(T x);

private:
    /**
     * Vector that contains the particles
     */
    std::vector<Particle> particles;

    /** Computes the gravitational force between two particles for the first particle
     *
     * @param p1 first particle
     * @param p2 second particle
     */
    void grav_force(Particle & p1, Particle & p2);

    // default dimensions
    int DIM = 3;

    double delta_t{};
};


template<typename... Args>
void ParticleContainer::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}

template<typename T>
T ParticleContainer::sqr(T x) {
    return x * x;
}

