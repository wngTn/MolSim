#include <gtest/gtest.h>
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "physicsCalculator/LennardJones.h"


void ljforceNaive(Particle &p1, Particle &p2, double sigma, double eps){
    auto Fij = -(24*eps)/(pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 2)) *
               (pow((sigma / (ArrayUtils::L2Norm(p1.getX() - p2.getX()))),6) -
                2 * (pow((sigma / (ArrayUtils::L2Norm(p1.getX() - p2.getX()))),12))) * (p1.getX() - p2.getX());
    std::cout << "Fij: " << Fij << std::endl;
    p1.setF(p1.getF() + Fij);
    p2.setF(p2.getF() - Fij);
}

void ljnaive(ParticleContainer &particles, double sigma,  double eps){
    for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
        auto[p1, p2] = *it;
        ljforceNaive(p1, p2, sigma, eps);
    }
}

// Demonstrate some basic assertions.
// TODO insert tests
TEST(HelloTest, BasicAssertions) {
    double SIGMA = 1.;
    double EPS = 5.;

    ParticleContainer p;
    // TODO make good test case
    for (int i = 0; i < 10; ++i) {
        p.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                        std::array<double, 3>{0., 0., 0.},
                        static_cast<double>(i),
                        i);
    }

    ParticleContainer p2 = p;
    calculator::LennardJones lj{SIGMA, EPS};
    lj.calcF(p);
    ljnaive(p2, SIGMA, EPS);




    for(auto it1 = p.begin(), it2 = p2.begin(); it1 != p.end() && it2 != p2.end(); it1++, it2++){
        for(int i = 0; i < 3; i++){
            ASSERT_DOUBLE_EQ(it1->getX()[i], it2->getX()[i]);
        }
    }
}
