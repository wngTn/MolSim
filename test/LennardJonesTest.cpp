#include <gtest/gtest.h>
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "physicsCalculator/LennardJones.h"


class ParameterizedLennardJonesTest : public testing::TestWithParam<std::tuple<size_t, double, double>> {
protected:
    ParticleContainer p_result;

    /**
     * Populates the ParticleContainer with size
     * @param size the size of the container
     */
    void setSize(size_t size) {
        for (int i = 0; i < size; ++i) {
            p_result.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                                  std::array<double, 3>{0., 0., 0.},
                                  static_cast<double>(i),
                                  i);
        }
    }

    /**
     * Implements the slow and naive way to calculate the Lennard Jones force between particles
     * @param p1 Particle one
     * @param p2 Particle two
     * @param sigma Sigma
     * @param eps Epsilon
     */
    static void ljforceNaive(Particle &p1, Particle &p2, double sigma, double eps) {
        auto Fij = -(24 * eps) / (pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 2)) *
                   (pow((sigma / (ArrayUtils::L2Norm(p1.getX() - p2.getX()))), 6) -
                    2 * (pow((sigma / (ArrayUtils::L2Norm(p1.getX() - p2.getX()))), 12))) * (p1.getX() - p2.getX());
        std::cout << "Fij: " << Fij << std::endl;
        p1.setF(p1.getF() + Fij);
        p2.setF(p2.getF() - Fij);
    }

    /**
     * Iterates through every distinct particle pair and calculates the Lennard Jones potential between the particles
     * @param particles The container with all our particles
     * @param sigma Sigma
     * @param eps Epsilon
     */
    static void ljnaive(ParticleContainer &particles, double sigma, double eps) {
        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto[p1, p2] = *it;
            ljforceNaive(p1, p2, sigma, eps);
        }
    }
};


/**
 * Tests our implemented Lennard Jones potential (p_result) vs a naive version (p_reference)
 * Uses different sizes, sigmas and epsilons
 */
TEST_P(ParameterizedLennardJonesTest, LennardJonesTest) {
    auto [size, sigma, eps] = GetParam();
    setSize(size);

    ParticleContainer p_reference = ParameterizedLennardJonesTest::p_result;
    calculator::LennardJones lj{sigma, eps};
    lj.calcF(p_result);
    ljnaive(p_reference, sigma, eps);


    for (auto it1 = p_result.begin(), it2 = p_reference.begin(); it1 != p_result.end() && it2 != p_reference.end(); it1++, it2++) {
        for (int i = 0; i < 3; i++) {
            // If one element does not match we can basically stop testing and get out of the loop
            ASSERT_DOUBLE_EQ(it1->getX()[i], it2->getX()[i]);
        }
    }
}

/**
 * Instantiates the parameterized tests with the to testing values
 */
INSTANTIATE_TEST_SUITE_P(
        ContainerSizeTest,
        ParameterizedLennardJonesTest,
        ::testing::Values(
                std::make_tuple(8, 1., 5.),
                std::make_tuple(16, 2., 6.),
                std::make_tuple(32, 1.5, 4.5),
                std::make_tuple(64, 3., 6.),
                std::make_tuple(128, 1., 8.),
                std::make_tuple(265, 1.8, 6.9)
        )
);

