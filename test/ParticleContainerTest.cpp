#include <gtest/gtest.h>
#include <set>
#include <iostream>

#include "DirectSumParticleContainer.h"
#include "utils/ParticleGenerator.h"

/**
 * Fixture test class
 * Before every TEST_F, the SetUp() override and TearDown() override test will be executed
 */
class ContainerTest : public testing::Test {
protected:
    /* The ParticleContainer we use in this test */
    DirectSumParticleContainer p;

    void SetUp() override {
        ContainerTest::init();
    }

    /**
     * Constructs the container
     */
    void init() {
        p = DirectSumParticleContainer{};
    }

    /**
     * Generates a small input dataset (JSON file) in the ./input/files directory
     * and reads it into the ParticleContainer
     */
    void readSmallData() {
        if (std::system("python ../../input/generate_json.py --size small --output small.json")) {
            FAIL()<<"There was a problem generating the input file in the tests!";
        }

        ParticleGenerator::generateParticles(p, "../../input/files/small.json");
    }

    /**
     * Generates a medium input dataset (JSON file) in the ./input/files directory
     * and reads it into the ParticleContainer
     */

    void readMediumData() {
        if (std::system("python ../../input/generate_json.py --size medium --output medium.json")) {
            FAIL()<<"There was a problem generating the input file in the tests!";
        }

        ParticleGenerator::generateParticles(p, "../../input/files/medium.json");
    }

    /*
     * Generates a large input dataset (JSON file) in the ./input/files directory
     * and reads it into the ParticleContainer
     */
    void readLargeData() {
        if (std::system("python ../../input/generate_json.py --size large --output large.json")) {
            FAIL()<<"There was a problem generating the input file in the tests!";
        }

        ParticleGenerator::generateParticles(p, "../../input/files/large.json");
    }
};

/**
 * Tests whether the default constructor does its job
 */
TEST_F(ContainerTest, DefaultConstructor) {
    EXPECT_EQ(0u, p.size());
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a small input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, SmallDataSetSize) {
    ContainerTest::readSmallData();
    ASSERT_EQ(p.size(), 120);
    if (std::remove("../../input/files/small.json")) {
        FAIL()<<"There was a problem deleting the generated (small dataset) JSON file";
    }
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a medium input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, MediumDataSetSize) {
    ContainerTest::readMediumData();
    EXPECT_EQ(p.size(), 432);
    if (std::remove("../../input/files/medium.json")) {
        FAIL()<<"There was a problem deleting the generated (medium dataset) JSON file";
    }
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a large input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, LargeDataSetSize) {
    ContainerTest::readLargeData();
    EXPECT_EQ(p.size(), 768);
    if (std::remove("../../input/files/large.json")) {
        FAIL()<<"There was a problem deleting the generated (large dataset) JSON file";
    }
}



/**
 * Parameterized Container Test testing with different sizes
 */
class ParameterizedContainerTest : public testing::TestWithParam<size_t> {
protected:
    DirectSumParticleContainer p;

    /**
     * Populates the ParticleContainer with size
     * Implicitly also tests whether emplace back does its job
     * Every particle can be identified by its mass
     * @param size the size of the container
     */
    void setSize(size_t size) {
        for (int i = 0; i < size; ++i) {
            p.emplace_back(std::array<double, 3>{0.1, 0.2, 0.3},
                           std::array<double, 3>{0.4, 0.5, 0.6},
                           static_cast<double>(i),
                           i);
        }
    }

    /**
     * A simple factorial function
     * @param x The number that should be factorialed
     * @return The factorial
     */
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
    EXPECT_EQ(p.size(), size);
    // ASSERT_EQ(p.size(), size + 1);
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
    for (auto it = p.pair_begin(); it != p.pair_end(); ++it) {
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





