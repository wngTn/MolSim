#include <gtest/gtest.h>
#include <ctime>
#include <set>
#include <iostream>

#include "ParticleContainer.h"

/**
 * Fixture test
 * Before every TEST_F, the SetUp() override and TearDown() override test will be executed
 */
class ContainerTest : public testing::Test {
protected:
    /* For default constructor */
    ParticleContainer p1;

    void SetUp() override {
        ContainerTest::init();
    }

    /**
     * Constructs the container
     */
    void init() {
        p1 = ParticleContainer{};
    }
};

/**
 * Tests whether the default constructor does its job
 */
TEST_F(ContainerTest, DefaultConstructor) {
    EXPECT_EQ(0u, p1.size());
}


/**
 * Parameterized Container Test testing with different sizes
 */
class ParameterizedContainerTest : public testing::TestWithParam<size_t> {
protected:
    ParticleContainer p1;

    /**
     * Implictly also tests whether emplace back does its job
     * Every particle can be identified by its mass
     * @param size the size of the container
     */
    void setSize(size_t size) {
        for (int i = 0; i < size; ++i) {
            p1.emplace_back(std::array<double, 3>{0.1, 0.2, 0.3},
                            std::array<double, 3>{0.4, 0.5, 0.6},
                            static_cast<double>(i),
                            i);
        }
    }

    static unsigned int fact(unsigned int x) {
        unsigned int result{1};
        for (unsigned int i = 2; i <= x; ++i) {
            result *= i;
        }
        return result;
    }
};

/**
 * Tests whether the size of the container is correct after the emplace backs
 */
TEST_P(ParameterizedContainerTest, ContainerSize) {
    size_t size = GetParam();
    setSize(size);
    // Here it does not matter whether we use ASSERT or EQUAL, since the test ends here anyway
    EXPECT_EQ(p1.size(), size);
    // ASSERT_EQ(p1.size(), size + 1);
}

/**
 * Tests whether the ParticlePairIterator correctly parses over every distinct particle pair
 */
TEST_P(ParameterizedContainerTest, ParticlePairIterator) {
    size_t size = GetParam();
    setSize(size);
    // Set of mass-pairs (masses identify the particles)
    std::set<std::pair<double, double>> s;
    // Inserts every particle pair
    for (auto it = p1.pair_begin(); it != p1.pair_end(); ++it) {
        s.insert({(*it).first.get().getM(), (*it).second.get().getM()});
    }
    // The reference size which is combination(size, 2)
    size_t refSize = (size == 1) ? 0 : fact(size)/(fact(2)*fact(size-2));
    // Theoretically it doesn't matter again whether we use ASSERT or EXPECT, since the test ends here
    ASSERT_EQ(s.size(), refSize);
}

/**
 * Instantiates the parameterized tests with the to testing values
 */
INSTANTIATE_TEST_SUITE_P(
        ContainerSizeTest,
        ParameterizedContainerTest,
        ::testing::Values(
                1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u
        )
);


