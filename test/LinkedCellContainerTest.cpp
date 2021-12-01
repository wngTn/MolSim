#include "gtest/gtest.h"

#include <Particle.h>
#include <particleContainers/LinkedCellContainer.h>
#include <physicsCalculator/LinkedCell.h>

/**
* Simple test to check whether a particle will be added to the container
*/

TEST(LinkedCellContainerTest, ConstructorTest) {
    int X = 7;
    int Y = 7;
    int Z = 1;
    double rCut = 1;


    LinkedCellContainer linkedCellContainer = LinkedCellContainer(X, Y, Z, rCut);

    // 48 elements will be reserved, however, the size is still 0 - this says the grid was constructed though
    ASSERT_EQ(linkedCellContainer.getGrid().size(), 7*7);

    linkedCellContainer.grid[24].emplace_back(std::array<double, 3>{0, 0, 0},
                                                   std::array<double, 3>{0, 0, 0},
                                                   .5,
                                                   0);
    linkedCellContainer.grid[11].emplace_back(std::array<double, 3>{0.6, 1.6, 0},
                                                   std::array<double, 3>{1, 2, 3},
                                                   1,
                                                   1);

    ASSERT_EQ(linkedCellContainer.getGrid()[24].getParticles().size(), 1);
    ASSERT_EQ(linkedCellContainer.getGrid()[11].getParticles().size(), 1);

    auto xRef = std::array<double, 3>{0, 0, 0};
    auto xRef2 = std::array<double, 3>{0.6, 1.6, 0};

    EXPECT_EQ(linkedCellContainer.getGrid()[24].getParticles()[0].getX(), xRef);
    EXPECT_EQ(linkedCellContainer.getGrid()[11].getParticles()[0].getX(), xRef2);
}

/**
 * More sophisticated Test to check whether the Container saves the particles
 */

class ParameterizedLinkedCellTest : public testing::TestWithParam<std::tuple<int, int, int>> {
protected:
    LinkedCellContainer p_result;

    /**
     * Populates the ParticleContainer with size
     * @param size the size of the container
     */
    void setSize(size_t size) {
        for (int i = 0; i < size; ++i) {
            p_result.grid[i].emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
                                            std::array<double, 3>{0., 0., 0.},
                                            static_cast<double>(i),
                                            i);
        }
    }

};

TEST_P(ParameterizedLinkedCellTest, CorrectDimensionsTest) {
    auto [X, Y, Z] = GetParam();
    p_result = LinkedCellContainer(X, Y, Z, 1.);

    setSize(X*Y*Z);

    ASSERT_EQ(p_result.getGrid().size(), X*Y*Z);

    for (int i = 0; i < p_result.getGrid().size(); ++i) {
        ASSERT_EQ(p_result.getGrid()[i].getParticles()[0].getType(), i);
    }
}

INSTANTIATE_TEST_SUITE_P(
        CorrectDimensionsTest,
        ParameterizedLinkedCellTest,
        ::testing::Values(
                std::make_tuple(1, 1, 1),
                std::make_tuple(1, 2, 3),
                std::make_tuple(2, 3, 4),
                std::make_tuple(4, 3, 2),
                std::make_tuple(5, 6, 7),
                std::make_tuple(5, 7, 8)
        )
);

/**
 * Tests whether the getNeighbor function really gets all neighbors
 */
//TEST(LinkedCellContainer, getNeighbor) {
//    int X = 3;
//    int Y = 6;
//    int Z = 1;
//    double rCut = 1;
//
//    LinkedCellContainer linkedCellContainer1 = LinkedCellContainer(3, 4, 1, 1.);
//    LinkedCellContainer linkedCellContainer2 = LinkedCellContainer(3, 4, 5, 1.);
//
//    for (int i = 0; i < 12; ++i) {
//        std::cout<<"I will be executed"<<std::endl;
//        linkedCellContainer1.addParticle(Particle(i), i);
//    }
//
//    for (int i = 0; i < 60; ++i) {
//        linkedCellContainer2.addParticle(Particle(i), i);
//    }
//
//    std::vector<int> n1_ref = {0, 1, 2, 3, 5, 6, 7, 8};
//    std::vector<int> n2_ref = {24, 25, 26, 12, 13, 14, 0, 1, 2, 27, 28, 29, 15, 17, 3, 4, 5, 30, 31, 32,
//                           18, 19, 20, 6, 7, 8};
//
//
//    auto n1_impl = linkedCellContainer1.getNeighbors<4>();
//    auto n2_impl = linkedCellContainer2.getNeighbors<16>();
//
//    ASSERT_EQ(n1_impl.size(), 8);
//    ASSERT_EQ(n2_impl.size(), 26);
//
//    for (int i = 0; i < 8; ++i) {
//        EXPECT_EQ(n1_impl[i].getParticles()[0].getType(), n1_ref[i]);
//    }
//
//    for (int i = 0; i < 26; ++i) {
//        EXPECT_EQ(n2_impl[i].getParticles()[0].getType(), n2_ref[i]);
//    }
// }