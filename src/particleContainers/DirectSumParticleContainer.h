#pragma once

#include <vector>
#include <cmath> /* sqrt */

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
    void emplace_back(const std::array<double, 3>& x, const std::array<double, 3>& v, double m, int t) override;

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

    /**
     * @brief Provides the iterator for particle pairs at the start of the collection. The iterator iterates
     * through every distinct pair.
     * @return PairIterator
     */
    PairIterator pair_begin() override;

    /**
     * @brief Provides the iterator for particle pairs at the end of the collection.
     * @return PairIterator
     */
    PairIterator pair_end() override;

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

