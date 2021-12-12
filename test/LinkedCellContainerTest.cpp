#include "gtest/gtest.h"

#include <Particle.h>
#include <particleContainers/LinkedCellContainer.h>
#include "utils/ArrayUtils.h"

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

    linkedCellContainer.grid[24].emplace_back(new Particle{std::array<double, 3>{0, 0, 0},
                                                   std::array<double, 3>{0, 0, 0},
                                                   .5,
                                                   0});
    linkedCellContainer.grid[11].emplace_back(new Particle{std::array<double, 3>{0.6, 1.6, 0},
                                                   std::array<double, 3>{1, 2, 3},
                                                   1,
                                                   1});

    ASSERT_EQ(linkedCellContainer.getGrid()[24].getParticles().size(), 1);
    ASSERT_EQ(linkedCellContainer.getGrid()[11].getParticles().size(), 1);

    auto xRef = std::array<double, 3>{0, 0, 0};
    auto xRef2 = std::array<double, 3>{0.6, 1.6, 0};

    EXPECT_EQ(linkedCellContainer.getGrid()[24].getParticles()[0]->getX(), xRef);
    EXPECT_EQ(linkedCellContainer.getGrid()[11].getParticles()[0]->getX(), xRef2);

    linkedCellContainer.grid[24].clear();
    linkedCellContainer.grid[11].clear();
}


/**
 * Uses the Particle Vector and creates each cell new with setup
 */
TEST(LinkedCellContainerTest, ParticleConstructorTest) {
    int X = 7;
    int Y = 7;
    int Z = 1;
    double rCut = 1;


    LinkedCellContainer linkedCellContainer = LinkedCellContainer(X, Y, Z, rCut);

    // 48 elements will be reserved, however, the size is still 0 - this says the grid was constructed though
    ASSERT_EQ(linkedCellContainer.getGrid().size(), 7*7);

    linkedCellContainer.particles.emplace_back(std::array<double, 3>{0, 0, 0},
                                               std::array<double, 3>{0, 0, 0},
                                               .5,
                                               0);

    linkedCellContainer.particles.emplace_back(std::array<double, 3>{0.6, 1.6, 0},
                                                             std::array<double, 3>{1, 2, 3},
                                                             1,
                                                             1);

    linkedCellContainer.setup();

    ASSERT_EQ(linkedCellContainer.getGrid()[0].getParticles().size(), 1);
    ASSERT_EQ(linkedCellContainer.getGrid()[7].getParticles().size(), 1);

    auto xRef = std::array<double, 3>{0, 0, 0};
    auto xRef2 = std::array<double, 3>{0.6, 1.6, 0};

    EXPECT_EQ(linkedCellContainer.getGrid()[0].getParticles()[0]->getX(), xRef);
    EXPECT_EQ(linkedCellContainer.getGrid()[7].getParticles()[0]->getX(), xRef2);

    linkedCellContainer.grid.clear();
}

/**
 * More sophisticated Test to check whether the Container saves the particles in the ParticleContainer
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
            p_result.particles.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
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

    for (int i = 0; i < p_result.particles.size(); ++i) {
        ASSERT_EQ(p_result.particles[i].getType(), i);
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


/*
 * Tests whether the getPerNeighbor function returns the correct neighbors
 */

TEST(LinkedCellContainer, GetPerNeighborFunctionTest) {
    LinkedCellContainer linkedCellContainer = LinkedCellContainer{10, 10, 0, 1.};
    std::array<int, 3> ci = {0, 0, 0};
    std::array<int, 3> ci_n_1 = {9, 0, 0};
    std::array<int, 3> ci_n_2 = {9, 1, 0};
    // Left border
    auto result = linkedCellContainer.getPerNeighbors(ci, 0);
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], ci_n_1);
    ASSERT_EQ(result[1], ci_n_2);

    LinkedCellContainer linkedCellContainer3D = LinkedCellContainer{10, 10, 10, 1.};
    std::array<int, 3> ci_3D = {0, 0, 0};
    auto result3D = linkedCellContainer3D.getPerNeighbors(ci_3D, 0);
    ASSERT_EQ(result3D.size(), 4);
    std::cout<<result3D<<std::endl;

    std::array<int, 3> ci_3D_2 = {0, 5, 5};
    auto result3D_2 = linkedCellContainer3D.getPerNeighbors(ci_3D_2, 0);
    ASSERT_EQ(result3D_2.size(), 9);
    std::cout<<result3D_2<<std::endl;
}