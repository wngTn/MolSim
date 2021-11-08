#include <gtest/gtest.h>
#include <ctime>
#include "ParticleContainer.h"

/**
 * Fixture test
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

    void setSize(size_t size) {
        for (int i = 0; i < size; ++i) {
            p1.emplace_back(std::array<double, 3>{0.1, 0.2, 0.3},
                            std::array<double, 3>{0.4, 0.5, 0.6},
                            static_cast<double>(i),
                            i);
        }
    }
};

TEST_P(ParameterizedContainerTest, ContainerSize) {
    size_t size = GetParam();
    setSize(size);
    ASSERT_EQ(p1.size(), size);
}

INSTANTIATE_TEST_SUITE_P(
        ContainerSizeTest,
        ParameterizedContainerTest,
        ::testing::Values(
            1u, 10u, 20u, 100u, 200u
                )
        );


