#include <gtest/gtest.h>
#include <math.h>

#include "physicsCalculator/Thermostat.h"
#include "physicsCalculator/LinkedCell.h"
#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"

class ThermostatTest : public testing::Test {
    public:

    Thermostat initThermostat(double initTemp, double targetTemp, double maxDTemp){
        return {initTemp, targetTemp, maxDTemp};
    }

    static double testingCalculateKineticEnergy(ParticleContainer &particles) {
        double totalEnergy = 0;
        for(auto &p: particles){
            totalEnergy += p.getM() * (ArrayUtils::dotProduct(p.getV(), p.getV()));
        }
        return totalEnergy * 0.5;
    }

    static double testingCalculateCurrentTemp(ParticleContainer &particles){
        return 2 * testingCalculateKineticEnergy(particles) / particles.size() / particles.dimensions();
    }

    static void moveParticles(ParticleContainer &particles, calculator::LinkedCell calc){
        calc.calcX(particles);
        particles.setup();
        calc.calcF(particles);
        calc.calcV(particles);
    }

};

TEST(ThermostatTest, IncreaseTemperatureTest){
    double initTemperature = 4.0;
    double targetTemperature = 20.0;
    double maxDeltaTemperature = 2.0;

    Thermostat thermostat = {initTemperature, targetTemperature, maxDeltaTemperature};
    calculator::LinkedCell calc = {1, 1, 1};

    LinkedCellContainer particles = LinkedCellContainer(3, 3, 3, 1, std::array<LinkedCellContainer::Border, 6>{
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective,
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective});
    //      {0.5, 0.5, 0.5}         {1, 1, 1}
    particles.particles.emplace_back(std::array<double, 3>{0.5, 0.5, 0.5},
                                     std::array<double, 3>{0, 1, 0},
                                     1);
    particles.particles.emplace_back(std::array<double, 3>{2.9, 2.9, 2.9},
                                     std::array<double, 3>{0, 1, 0},
                                     1);
    for (auto it = particles.particles.begin(); it != particles.particles.end(); ++it) {
        particles.grid[0].emplace_back(&(*it));
    }
    particles.setup();
    // tests if all particles are added to the domain
    EXPECT_EQ(particles.particles.size(), 2);

    thermostat.setupTemperature(particles);
    calc.calcX(particles);
    particles.setup();
    calc.calcF(particles);
    calc.calcV(particles);

    double startTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    thermostat.applyTemperature(particles);
    double endTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    // tests if the temperature has increased
    EXPECT_LE(abs(endTemp - startTemp), maxDeltaTemperature);
}

TEST(ThermostatTest, DecreaseTemperatureTest){
    double initTemperature = 20.0;
    double targetTemperature = 4.0;
    double maxDeltaTemperature = 2.0;

    Thermostat thermostat = {initTemperature, targetTemperature, maxDeltaTemperature};
    calculator::LinkedCell calc = {1, 1, 1};

    LinkedCellContainer particles = LinkedCellContainer(3, 3, 3, 1, std::array<LinkedCellContainer::Border, 6>{
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective,
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective});
    //      {0.5, 0.5, 0.5}         {1, 1, 1}
    particles.particles.emplace_back(std::array<double, 3>{0.5, 0.5, 0.5},
                                     std::array<double, 3>{0, 1, 0},
                                     1);
    particles.particles.emplace_back(std::array<double, 3>{2.9, 2.9, 2.9},
                                     std::array<double, 3>{0, 1, 0},
                                     1);
    for (auto it = particles.particles.begin(); it != particles.particles.end(); ++it) {
    particles.grid[0].emplace_back(&(*it));
    }
    particles.setup();
    // tests if all particles are added to the domain
    EXPECT_EQ(particles.particles.size(), 2);

    thermostat.setupTemperature(particles);
    calc.calcX(particles);
    particles.setup();
    calc.calcF(particles);
    calc.calcV(particles);

    double startTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    thermostat.applyTemperature(particles);
    double endTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    // tests if the temperature has increased
    EXPECT_LE(abs(endTemp - startTemp), maxDeltaTemperature);
}

TEST(ThermostatTest, HoldingTemperatureTest){
    double initTemperature = 20.0;
    double targetTemperature = 20.0;
    double maxDeltaTemperature = 2.0;

    Thermostat thermostat = {initTemperature, targetTemperature, maxDeltaTemperature};
    calculator::LinkedCell calc = {1, 1, 1};

    LinkedCellContainer particles = LinkedCellContainer(3, 3, 3, 1, std::array<LinkedCellContainer::Border, 6>{
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective,
            LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective});
    //      {0.5, 0.5, 0.5}         {1, 1, 1}
    particles.particles.emplace_back(std::array<double, 3>{0.5, 0.5, 0.5},
            std::array<double, 3>{0, 1, 0},
    1);
    particles.particles.emplace_back(std::array<double, 3>{2.9, 2.9, 2.9},
            std::array<double, 3>{0, 1, 0},
    1);
    for (auto it = particles.particles.begin(); it != particles.particles.end(); ++it) {
    particles.grid[0].emplace_back(&(*it));
    }
    particles.setup();
    // tests if all particles are added to the domain
    EXPECT_EQ(particles.particles.size(), 2);

    thermostat.setupTemperature(particles);
    calc.calcX(particles);
    particles.setup();
    calc.calcF(particles);
    calc.calcV(particles);

    double startTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    thermostat.applyTemperature(particles);
    double endTemp = ThermostatTest::testingCalculateCurrentTemp(particles);
    // tests if the temperature has increased
    EXPECT_EQ(startTemp, endTemp);
}

