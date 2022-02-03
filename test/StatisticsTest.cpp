#include <gtest/gtest.h>


#include <particleContainers/LinkedCellContainer.h>
#include <inputReader/XMLReader.h>
#include <utils/Config.h>
#include <utils/MainUtils.h>
#include <statistics/Thermodynamical.h>

class StatisticsTest : public testing::Test {
protected:
    /* The ParticleContainer we use in this test */
    LinkedCellContainer p;

    void SetUp() override {
        XMLReader::XMLInfo info = XMLReader::readFile("../../input/files/testing/statistics_test.xml");
        p = LinkedCellContainer{info.linkedCellSize[0], info.linkedCellSize[1], info.linkedCellSize[2], info.rCut,
                                std::array<LinkedCellContainer::Border,6>{LinkedCellContainer::periodic,
                                                                          LinkedCellContainer::periodic,
                                                                          LinkedCellContainer::periodic,
                                                                          LinkedCellContainer::periodic,
                                                                          LinkedCellContainer::periodic,
                                                                          LinkedCellContainer::periodic}};
        ParticleGenerator::generateParticles(p, info.generatorInfos);
    }

};

TEST_F(StatisticsTest, ThermodynamicalStatsTest){
    calculator::LinkedCell lc = calculator::LinkedCell(1.0,1.0,3.0);

    std::filesystem::create_directory(std::filesystem::path{"../statistics/"});

    statistics::Thermodynamical stats = statistics::Thermodynamical("testing_thermodynamical.csv", 1.0, 15);

    for(int i = 1; i <= 10; i++){
        lc.calcX(p);
        p.setup();
        lc.calcF(p);
        lc.calcV(p);
        stats.writeStatistics(p, i);
    }
    std::filesystem::path path_rdf {"../statistics/RDF_testing_thermodynamical.csv"};
    std::filesystem::path path_diffusion {"../statistics/diffusion_testing_thermodynamical.csv"};

    std::cout << "PATH: " << path_rdf << std::endl;
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::relative(path_rdf)));
    ASSERT_TRUE(std::filesystem::exists(path_diffusion));

    std::ifstream rdf_file{path_rdf};
    std::ifstream diff_file{path_diffusion};

    long rdf_line_count = std::count(std::istreambuf_iterator<char>(rdf_file),
                            std::istreambuf_iterator<char>(), '\n');
    long diff_line_count = std::count(std::istreambuf_iterator<char>(diff_file),
                                      std::istreambuf_iterator<char>(), '\n');
    ASSERT_EQ(1 + 10*15, rdf_line_count);
    ASSERT_EQ(1 + 10, diff_line_count);
}

TEST_F(StatisticsTest, DensityVelocityStatsTest){
    int NO_BINS = 10;

    calculator::LinkedCell lc = calculator::LinkedCell(1.0,1.0,3.0);

    std::filesystem::create_directory(std::filesystem::path{"../statistics/"});

    statistics::DensityVelocityProfile stats = statistics::DensityVelocityProfile("testing_densityVelocity.csv", NO_BINS);

    for(int i = 1; i <= 10; i++){
        lc.calcX(p);
        p.setup();
        lc.calcF(p);
        lc.calcV(p);
        stats.writeStatistics(p, i);
    }
    std::filesystem::path path {"../statistics/testing_densityVelocity.csv"};

    std::cout << "PATH: " << path << std::endl;
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::relative(path)));

    std::ifstream file{path};
    long line_count = std::count(std::istreambuf_iterator<char>(file),
                                      std::istreambuf_iterator<char>(), '\n');
    ASSERT_EQ(1 + 10*NO_BINS, line_count);
}