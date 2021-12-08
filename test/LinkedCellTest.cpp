#include "gtest/gtest.h"

#include <Particle.h>
#include <physicsCalculator/LinkedCell.h>


/*
 * Tests whether the index method is correct
 */
TEST(LinkedCellTest, indexTest) {

    std::array<int, 3> dim = {3, 4, 1};
    std::array<int, 3> arr1 = {0, 0, 0};
    std::array<int, 3> arr2 = {0, 0, 1};
    std::array<int, 3> arr3 = {1, 2, 2};
    std::array<int, 3> arr4 = {1, 1, 1};
    std::array<int, 3> arr5 = {2, 3, 1};

    int res1 = calculator::LinkedCell::index(arr1, dim);
    int res2 = calculator::LinkedCell::index(arr2, dim);
    int res3 = calculator::LinkedCell::index(arr3, dim);
    int res4 = calculator::LinkedCell::index(arr4, dim);
    int res5 = calculator::LinkedCell::index(arr5, dim);

    ASSERT_EQ(res1, 0);
    ASSERT_EQ(res2, 12);
    ASSERT_EQ(res3, 31);
    ASSERT_EQ(res4, 16);
    ASSERT_EQ(res5, 23);

    std::array<int, 3> dim1 = {7, 7, 1};
    std::array<int, 3> arr_1 = {3, 3, 0};

    int res_1 = calculator::LinkedCell::index(arr_1, dim1);

    ASSERT_EQ(res_1, 24);
}


Particle findParticle(const DirectSumParticleContainer &p, int type) {
    for (auto &e: p) {
        if (e.getType() == type) return e;
    }
    return Particle{};
}

Particle findParticle(const LinkedCellContainer &p, int type) {
    for (auto &cell: p.getGrid()) {
        for (auto &e: cell) {
            if (e->getType() == type) return *e;
        }
    }
    return Particle{};
}


/**
 * Simple test to check whether the LinkedCellMethod works
 * Every Particle is in the same grid in the LinkedCellMethodContainer
 */

TEST(LinkedCellTest, LinkedCellMethodSimpleTest) {

    // The rounding error that we allow
    double EPSILON_VALUE = 0.000001;
    double EPS = 5.;
    double SIGMA = 1.;

    DirectSumParticleContainer particleContainer = DirectSumParticleContainer{};
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{12, 12, 1, 3.};

    std::cout<<"Grid size: "<<linkedCellContainer.grid.size()<<std::endl;

    for (int i = 0; i < 4; ++i) {
        particleContainer.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                                       std::array<double, 3>{0., 0., 0.},
                                       static_cast<double>(i),
                                       i);
        linkedCellContainer.grid[calculator::LinkedCell::index(std::array<int, 3>{2, 1, 0}, linkedCellContainer.getDim())]
                .emplace_back(new Particle{std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                              std::array<double, 3>{0., 0., 0.},
                              static_cast<double>(i),
                              i});
    }

    // calculate our result
    calculator::LennardJones lj{SIGMA, EPS};
    lj.calcF(particleContainer);

    calculator::LinkedCell lc{SIGMA, EPS, 3., 0.1};
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
 * Tests whether the moveParticle method correctly moves the particles
 */
//TEST(LinkedCellTest, MoveMethodTest) {
//    LinkedCellContainer linkedCellContainer = LinkedCellContainer{16, 14, 13, 3.};
//
//    linkedCellContainer.grid[0].emplace_back(new Particle{std::array<double, 3>{10, 4, 5},
//                                             std::array<double, 3>{0., 0., 0.},
//                                             static_cast<double>(1),
//                                             0});
//
//    calculator::LinkedCell::moveParticles(linkedCellContainer);
//
//    EXPECT_EQ(linkedCellContainer.grid[0].getParticles().size(), 0);
//    EXPECT_EQ(linkedCellContainer.grid[calculator::LinkedCell::index(std::array<int, 3>{3, 1, 1}, linkedCellContainer.getDim())].getParticles().size(), 1);
//
//    LinkedCellContainer linkedCellContainer1 = LinkedCellContainer{120, 80, 100, 3.};
//
//    linkedCellContainer1.grid[0].emplace_back(new Particle{std::array<double, 3>{2.1, 2.8, 5.6},
//                                              std::array<double, 3>{0.1, 0.1, 0.1},
//                                              .05,
//                                              7});
//    linkedCellContainer1.grid[0].emplace_back(new Particle{std::array<double, 3>{2.4, 3.2, 6.4},
//                                             std::array<double, 3>{0.1, 0.1, 0.1},
//                                             .05,
//                                             8});
//    linkedCellContainer1.grid[0].emplace_back(new Particle{std::array<double, 3>{2.7, 3.6, 7.2},
//                                             std::array<double, 3>{0.1, 0.1, 0.1},
//                                             .05,
//                                             9});
//
//
//    calculator::LinkedCell::moveParticles(linkedCellContainer1);
//
//    EXPECT_EQ(linkedCellContainer1.grid[0].getParticles().size(), 0);
//    EXPECT_EQ(linkedCellContainer1.grid[calculator::LinkedCell::index(std::array<int, 3>{0, 1, 2}, linkedCellContainer1.getDim())].getParticles().size(), 2);
//    EXPECT_EQ(linkedCellContainer1.grid[calculator::LinkedCell::index(std::array<int, 3>{0, 0, 1}, linkedCellContainer1.getDim())].getParticles().size(), 1);
//
//
//    // Check whether outflow works
//    LinkedCellContainer linkedCellContainer2 = LinkedCellContainer{16, 14, 13, 3.};
//
//    linkedCellContainer2.grid[0].emplace_back(new Particle{std::array<double, 3>{-1, 2, 2},
//                                             std::array<double, 3>{0., 0., 0.},
//                                             static_cast<double>(1),
//                                             0});
//    linkedCellContainer2.grid[0].emplace_back(new Particle{std::array<double, 3>{1, -2, 2},
//                                              std::array<double, 3>{0., 0., 0.},
//                                              static_cast<double>(1),
//                                              0});
//    linkedCellContainer2.grid[0].emplace_back(new Particle{std::array<double, 3>{1, 2, -2},
//                                              std::array<double, 3>{0., 0., 0.},
//                                              static_cast<double>(1),
//                                              0});
//
//    EXPECT_EQ(linkedCellContainer2.grid[0].getParticles().size(), 3);
//    calculator::LinkedCell::moveParticles(linkedCellContainer2);
//
//    EXPECT_EQ(linkedCellContainer2.grid[0].getParticles().size(), 0);
//
//    // Check whether cyclic works
//    LinkedCellContainer linkedCellContainer3 = LinkedCellContainer{12, 12, 12, 3.,
//                                                                   std::array<LinkedCellContainer::Border, 6>{
//        LinkedCellContainer::cyclic, LinkedCellContainer::cyclic, LinkedCellContainer::cyclic, LinkedCellContainer::cyclic
//    , LinkedCellContainer::cyclic, LinkedCellContainer::cyclic}};
//
//    linkedCellContainer3.grid[0].emplace_back(new Particle{std::array<double, 3>{-1, 2, 2},
//                                              std::array<double, 3>{0., 0., 0.},
//                                              static_cast<double>(1),
//                                              0});
//    linkedCellContainer3.grid[0].emplace_back(new Particle{std::array<double, 3>{1, -2, 2},
//                                              std::array<double, 3>{0., 0., 0.},
//                                              static_cast<double>(1),
//                                              0});
//    linkedCellContainer3.grid[0].emplace_back(new Particle{std::array<double, 3>{1, 2, -2},
//                                              std::array<double, 3>{0., 0., 0.},
//                                              static_cast<double>(1),
//                                              0});
//
//    EXPECT_EQ(linkedCellContainer3.grid[0].getParticles().size(), 3);
//    calculator::LinkedCell::moveParticles(linkedCellContainer3);
//    EXPECT_EQ(linkedCellContainer3.grid[calculator::LinkedCell::index(
//            std::array<int, 3>{3, 0, 0},std::array<int, 3>{4, 4, 4})].getParticles().size(), 1);
//    EXPECT_EQ(linkedCellContainer3.grid[calculator::LinkedCell::index(
//            std::array<int, 3>{0, 3, 0},std::array<int, 3>{4, 4, 4})].getParticles().size(), 1);
//    EXPECT_EQ(linkedCellContainer3.grid[calculator::LinkedCell::index(
//            std::array<int, 3>{0, 0, 3},std::array<int, 3>{4, 4, 4})].getParticles().size(), 1);
//}

/**
* Tests the LinkedCell Method more detailedly against the Lennard Jones implementation
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
        linkedCellContainer.grid[calculator::LinkedCell::index(std::array<int, 3>{0, 0, 0}, linkedCellContainer.getDim())]
                .emplace_back(new Particle{std::array<double, 3>{(0.1 * i) + 0.8 * i, (0.2 * i) + 0.8 * i, (0.6 * i) + 0.8 * i},
                              std::array<double, 3>{0.1, 0.1, 0.1},
                              .05,
                              i});
    }

    calculator::LinkedCell::moveParticles(linkedCellContainer);

    // calculate our result
    calculator::LennardJones lj{SIGMA, EPS};
    lj.setDim(3);
    lj.setDeltaT(delta_t);

    lj.calcF(particleContainer);

    calculator::LinkedCell lc{SIGMA, EPS, rCut, 0.0005};
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
        linkedCellContainer2.grid[calculator::LinkedCell::index(std::array<int, 3>{0, 0, 0}, linkedCellContainer.getDim())]
                .emplace_back(new Particle{std::array<double, 3>{(0.4 * i) + 0.8 * i, (0.6 * i) + 0.8 * i, (1.2 * i) + 0.8 * i},
                              std::array<double, 3>{0.1, 0.1, 0.1},
                              .05,
                              i});
    }

    calculator::LinkedCell::moveParticles(linkedCellContainer2);

    // calculate our result
    calculator::LennardJones lj2{SIGMA, EPS};
    lj2.setDim(3);
    lj2.setDeltaT(delta_t);

    lj2.calcF(particleContainer2);

    calculator::LinkedCell lc2{SIGMA, EPS, rCut, delta_t};
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