#include "gtest/gtest.h"

#include <Particle.h>
#include <physicsCalculator/LinkedCell.h>


/*
 * Tests whether the index method is correct
 */
TEST(LinkedCellTest, indexTest) {

    std::array<int, 3> dim = {3, 4, 1};
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{dim[0], dim[1], dim[2], 1.};
    std::array<int, 3> arr1 = {0, 0, 0};
    std::array<int, 3> arr2 = {0, 0, 1};
    std::array<int, 3> arr3 = {1, 2, 2};
    std::array<int, 3> arr4 = {1, 1, 1};
    std::array<int, 3> arr5 = {2, 3, 1};

    int res1 = linkedCellContainer.index(arr1);
    int res2 = linkedCellContainer.index(arr2);
    int res3 = linkedCellContainer.index(arr3);
    int res4 = linkedCellContainer.index(arr4);
    int res5 = linkedCellContainer.index(arr5);

    ASSERT_EQ(res1, 0);
    ASSERT_EQ(res2, 12);
    ASSERT_EQ(res3, 31);
    ASSERT_EQ(res4, 16);
    ASSERT_EQ(res5, 23);

    std::array<int, 3> dim1 = {7, 7, 1};
    std::array<int, 3> arr_1 = {3, 3, 0};

    linkedCellContainer = LinkedCellContainer{dim1[0], dim1[1], dim1[2], 1.};

    int res_1 = linkedCellContainer.index(arr_1);

    ASSERT_EQ(res_1, 24);
}

/**
 * This method is used to get a specific particle from the container
 * @param p the container
 * @param type the type of the particle
 * @return the first particle with the type
 */
Particle findParticle(const DirectSumParticleContainer &p, int type) {
    for (auto &e: p) {
        if (e.getType() == type) return e;
    }
    return Particle{};
}

/**
 * This method is used to get a specific particle from the container
 * @param p the container
 * @param type the type of the particle
 * @return the first particle with the type
 */
Particle findParticle(const LinkedCellContainer &p, int type) {
    for (auto &cell: p.getGrid()) {
        for (auto &e: cell) {
            if (e->getType() == type) return *e;
        }
    }
    return Particle{};
}


/**
 * Simple test to check whether the LinkedCellMethod works by comparing it with the lennard jones implementation
 * Populates the container with 4 particles each
 */
TEST(LinkedCellTest, LinkedCellMethodSimpleTest) {

    // The rounding error that we allow
    double EPSILON_VALUE = 0.01;
    double EPS = 5.;
    double SIGMA = 1.;

    DirectSumParticleContainer particleContainer = DirectSumParticleContainer{};
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{12, 12, 1, 3.};

    std::cout<<"Grid size: "<<linkedCellContainer.grid.size()<<std::endl;

    // Populate the normal container and the linkedCell container
    for (int i = 0; i < 4; ++i) {
        particleContainer.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                                       std::array<double, 3>{0., 0., 0.},
                                       static_cast<double>(i),
                                       i);
        linkedCellContainer.particles.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                              std::array<double, 3>{0., 0., 0.},
                              static_cast<double>(i),
                              i);
    }
    // Moves the Particles to the correct cell
    linkedCellContainer.setup();

    // calculate our result
    calculator::LennardJones lj{SIGMA, EPS};
    lj.calcF(particleContainer);

    calculator::LinkedCell lc{SIGMA, EPS, 3.};
    lc.calcF(linkedCellContainer);

    for (int i = 0; i < 4; ++i) {
        auto p1 = findParticle(particleContainer, i);
        auto p2 = findParticle(linkedCellContainer, i);
        EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
    }
}


/**
 * Tests whether the setup method correctly puts the particles into the correct cells
 */
TEST(LinkedCellTest, SetupMethodTest) {
    /**********************Test with 1 particle**********************************/
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{16, 14, 13, 3.};

    linkedCellContainer.particles.emplace_back(std::array<double, 3>{10, 4, 5},
                                             std::array<double, 3>{0., 0., 0.},
                                             static_cast<double>(1),
                                             0);

    linkedCellContainer.setup();

    EXPECT_EQ(linkedCellContainer.grid[0].getParticles().size(), 0);
    EXPECT_EQ(linkedCellContainer.grid[linkedCellContainer.index(std::array<int, 3>{3, 1, 1})]
    .getParticles().size(), 1);

    /**********************Test with 3 particles**********************************/
    LinkedCellContainer linkedCellContainer1 = LinkedCellContainer{120, 80, 100, 3.};

    linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.1, 2.8, 5.6},
                                              std::array<double, 3>{0.1, 0.1, 0.1},
                                              .05,
                                              7);
    linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.4, 3.2, 6.4},
                                             std::array<double, 3>{0.1, 0.1, 0.1},
                                             .05,
                                             8);
    linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.7, 3.6, 7.2},
                                             std::array<double, 3>{0.1, 0.1, 0.1},
                                             .05,
                                             9);


    linkedCellContainer1.setup();

    EXPECT_EQ(linkedCellContainer1.grid[0].getParticles().size(), 0);
    EXPECT_EQ(linkedCellContainer1.grid[linkedCellContainer1.index(std::array<int, 3>{0, 1, 2})]
    .getParticles().size(), 2);
    EXPECT_EQ(linkedCellContainer1.grid[linkedCellContainer1.index(std::array<int, 3>{0, 0, 1})]
    .getParticles().size(), 1);
}


/**
 * Tests whether the outflow and cyclic boundaries work
 */
TEST(LinkedCellTest, OutflowAndCyclicTest) {
    // Check whether outflow works
    LinkedCellContainer cellContainer = LinkedCellContainer{16, 14, 13, 3.};

    cellContainer.particles.emplace_back(std::array<double, 3>{-1, 2, 2},
                                         std::array<double, 3>{0., 0., 0.},
                                         static_cast<double>(1), 0);
    cellContainer.particles.emplace_back(std::array<double, 3>{1, -2, 2},
                                         std::array<double, 3>{0., 0., 0.},
                                         static_cast<double>(1),
                                         0);
    cellContainer.particles.emplace_back(std::array<double, 3>{1, 2, -2},
                                         std::array<double, 3>{0., 0., 0.},
                                         static_cast<double>(1),
                                         0);
    for (auto it = cellContainer.particles.begin(); it != cellContainer.particles.end(); ++it) {
        cellContainer.grid[0].emplace_back(&(*it));
    }

    ASSERT_EQ(cellContainer.grid[0].getParticles().size(), 3);
    calculator::LinkedCell::moveParticles(cellContainer);
    cellContainer.setup();
    EXPECT_EQ(cellContainer.grid[0].getParticles().size(), 0);

    // Check whether cyclic works
    LinkedCellContainer cellContainer1 = LinkedCellContainer{12, 12, 12, 3.,
                                                             std::array<LinkedCellContainer::Border, 6>{
        LinkedCellContainer::cyclic, LinkedCellContainer::cyclic, LinkedCellContainer::cyclic, LinkedCellContainer::cyclic
        ,LinkedCellContainer::cyclic, LinkedCellContainer::cyclic}};

    cellContainer1.particles.emplace_back(std::array<double, 3>{-1, 2, 2},
                                          std::array<double, 3>{0., 0., 0.},
                                          static_cast<double>(1), 0);
    cellContainer1.particles.emplace_back(std::array<double, 3>{1, -2, 2},
                                          std::array<double, 3>{0., 0., 0.},
                                          static_cast<double>(1),
                                          0);
    cellContainer1.particles.emplace_back(std::array<double, 3>{1, 2, -2},
                                          std::array<double, 3>{0., 0., 0.},
                                          static_cast<double>(1),
                                          0);
    for (auto it = cellContainer1.particles.begin(); it != cellContainer1.particles.end(); ++it) {
        cellContainer1.grid[0].emplace_back(&(*it));
    }

    ASSERT_EQ(cellContainer1.grid[0].getParticles().size(), 3);
    calculator::LinkedCell::moveParticles(cellContainer1);
    cellContainer1.setup();
    EXPECT_EQ(cellContainer1.grid[cellContainer1.index( std::array<int, 3>{3, 0, 0})].getParticles().size(), 1);
    EXPECT_EQ(cellContainer1.grid[cellContainer1.index( std::array<int, 3>{0, 3, 0})].getParticles().size(), 1);
    EXPECT_EQ(cellContainer1.grid[cellContainer1.index( std::array<int, 3>{0, 0, 3})].getParticles().size(), 1);
}

/**
 * Tests the LinkedCell Method more detailedly against the Lennard Jones implementation
 * Creates two container with 10 particles each and compares the values
*/
TEST(LinkedCellTest, LinkedCellMethodIntermediateTest) {
    // The rounding error that we allow
    double EPSILON_VALUE = 0.009 ;
    double EPS = 5.;
    double SIGMA = 1.;
    double delta_t = 0.0005;
    double rCut = 3.0;

    DirectSumParticleContainer particleContainer = DirectSumParticleContainer{};
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{120, 80, 100, rCut};

    std::cout<<"Grid size: "<<linkedCellContainer.grid.size()<<std::endl;

    for (int i = 0; i < 10; ++i) {
        particleContainer.emplace_back(std::array<double, 3>{(0.1 * i) + 0.8 * i, (0.2 * i) + 0.8 * i, (0.6 * i) + 0.8 * i},
                                       std::array<double, 3>{0.1, 0.1, 0.1},
                                       .05,
                                       i);
        linkedCellContainer.particles.emplace_back(std::array<double, 3>{(0.1 * i) + 0.8 * i, (0.2 * i) + 0.8 * i, (0.6 * i) + 0.8 * i},
                              std::array<double, 3>{0.1, 0.1, 0.1},
                              .05,
                              i);
    }

    linkedCellContainer.setup();

    // calculate our result
    calculator::LennardJones lj{SIGMA, EPS};
    lj.setDim(3);
    lj.setDeltaT(delta_t);

    lj.calcF(particleContainer);

    calculator::LinkedCell lc{SIGMA, EPS, rCut};
    lc.setDim(3);
    lc.setDeltaT(delta_t);
    lc.calcF(linkedCellContainer);


    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer, i);
        auto p2 = findParticle(linkedCellContainer, i);
        std::cout<<"F1 of P1 is: "<<p1.getF()[0]<<" F1 of P2 is: "<<p2.getF()[0]<<std::endl;
        std::cout<<"F2 of P1 is: "<<p1.getF()[1]<<" F2 of P2 is: "<<p2.getF()[1]<<std::endl;
        std::cout<<"F3 of P1 is: "<<p1.getF()[2]<<" F3 of P2 is: "<<p2.getF()[2]<<std::endl;
        EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
    }

    lj.calcX(particleContainer);
    lc.calcX(linkedCellContainer);

    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer, i);
        auto p2 = findParticle(linkedCellContainer, i);
        std::cout<<"X of P1 is: "<<p1.getX()[0]<<" X of P2 is: "<<p2.getX()[0]<<std::endl;
        std::cout<<"Y of P1 is: "<<p1.getX()[1]<<" Y of P2 is: "<<p2.getX()[1]<<std::endl;
        std::cout<<"Z of P1 is: "<<p1.getX()[2]<<" Z of P2 is: "<<p2.getX()[2]<<std::endl;
        EXPECT_LE(fabs(p1.getX()[0] - p2.getX()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getX()[1] - p2.getX()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getX()[2] - p2.getX()[2]), EPSILON_VALUE);
    }

    lj.calcV(particleContainer);
    lc.calcV(linkedCellContainer);

    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer, i);
        auto p2 = findParticle(linkedCellContainer, i);
        std::cout<<"V1 of P1 is: "<<p1.getV()[0]<<" V1 of P2 is: "<<p2.getV()[0]<<std::endl;
        std::cout<<"V2 of P1 is: "<<p1.getV()[1]<<" V2 of P2 is: "<<p2.getV()[1]<<std::endl;
        std::cout<<"V3 of P1 is: "<<p1.getV()[2]<<" V3 of P2 is: "<<p2.getV()[2]<<std::endl;
         EXPECT_LE(fabs(p1.getV()[0] - p2.getV()[0]), EPSILON_VALUE);
         EXPECT_LE(fabs(p1.getV()[1] - p2.getV()[1]), EPSILON_VALUE);
         EXPECT_LE(fabs(p1.getV()[2] - p2.getV()[2]), EPSILON_VALUE);
    }

    /************************************* Second Container Test *************************************/

    DirectSumParticleContainer particleContainer2 = DirectSumParticleContainer{};
    LinkedCellContainer linkedCellContainer2 = LinkedCellContainer{120, 80, 100, rCut};

    std::cout<<"Grid size: "<<linkedCellContainer2.grid.size()<<std::endl;

    for (int i = 0; i < 10; ++i) {
        particleContainer2.emplace_back(std::array<double, 3>{(0.4 * i) + 0.8 * i, (0.6 * i) + 0.8 * i, (1.2 * i) + 0.8 * i},
                                       std::array<double, 3>{0.1, 0.1, 0.1},
                                       .05,
                                       i);
        linkedCellContainer2.particles.emplace_back(std::array<double, 3>{(0.4 * i) + 0.8 * i, (0.6 * i) + 0.8 * i, (1.2 * i) + 0.8 * i},
                              std::array<double, 3>{0.1, 0.1, 0.1},
                              .05,
                              i);
    }

    linkedCellContainer2.setup();

    // calculate our result
    calculator::LennardJones lj2{SIGMA, EPS};
    lj2.setDim(3);
    lj2.setDeltaT(delta_t);

    lj2.calcF(particleContainer2);

    calculator::LinkedCell lc2{SIGMA, EPS, rCut};
    lc2.setDim(3);
    lc2.setDeltaT(delta_t);
    lc2.calcF(linkedCellContainer2);

    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer2, i);
        auto p2 = findParticle(linkedCellContainer2, i);
        std::cout<<"F1 of P1 is: "<<p1.getF()[0]<<" F1 of P2 is: "<<p2.getF()[0]<<std::endl;
        std::cout<<"F2 of P1 is: "<<p1.getF()[1]<<" F2 of P2 is: "<<p2.getF()[1]<<std::endl;
        std::cout<<"F3 of P1 is: "<<p1.getF()[2]<<" F3 of P2 is: "<<p2.getF()[2]<<std::endl;
        EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
    }

    lj.calcX(particleContainer2);
    lc.calcX(linkedCellContainer2);

    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer2, i);
        auto p2 = findParticle(linkedCellContainer2, i);
        std::cout<<"X of P1 is: "<<p1.getX()[0]<<" X of P2 is: "<<p2.getX()[0]<<std::endl;
        std::cout<<"Y of P1 is: "<<p1.getX()[1]<<" Y of P2 is: "<<p2.getX()[1]<<std::endl;
        std::cout<<"Z of P1 is: "<<p1.getX()[2]<<" Z of P2 is: "<<p2.getX()[2]<<std::endl;
        EXPECT_LE(fabs(p1.getX()[0] - p2.getX()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getX()[1] - p2.getX()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getX()[2] - p2.getX()[2]), EPSILON_VALUE);
    }

    lj.calcV(particleContainer2);
    lc.calcV(linkedCellContainer2);

    for (int i = 0; i < 10; ++i) {
        auto p1 = findParticle(particleContainer2, i);
        auto p2 = findParticle(linkedCellContainer2, i);
        std::cout<<"V1 of P1 is: "<<p1.getV()[0]<<" V1 of P2 is: "<<p2.getV()[0]<<std::endl;
        std::cout<<"V2 of P1 is: "<<p1.getV()[1]<<" V2 of P2 is: "<<p2.getV()[1]<<std::endl;
        std::cout<<"V3 of P1 is: "<<p1.getV()[2]<<" V3 of P2 is: "<<p2.getV()[2]<<std::endl;
        EXPECT_LE(fabs(p1.getV()[0] - p2.getV()[0]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getV()[1] - p2.getV()[1]), EPSILON_VALUE);
        EXPECT_LE(fabs(p1.getV()[2] - p2.getV()[2]), EPSILON_VALUE);
    }
}