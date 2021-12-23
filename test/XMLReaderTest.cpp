#include <gtest/gtest.h>
#include <set>
#include <iostream>

#include "inputReader/XMLReader.h"
#include "utils/ParticleGenerator.h"
#include "physicsCalculator/PhysicsCalc.h"

/**
 * Fixture test class
 * Before every TEST_F, the SetUp() override and TearDown() override test will be executed
 */
class XMLReaderTest : public testing::Test {
protected:
    /* The ParticleContainer we use in this test */
    XMLReader::XMLInfo info;

    void SetUp() override {
        XMLReaderTest::init();
    }

    void init() {

    }


    void readGeneratorInfoFromXML() {
        info = XMLReader::readFile("../../input/files/assignment_3/collision_of_two_bodies.xml");
    }

    void readFilesFromXML(){
        info = XMLReader::readFile("../../input/files/assignment_3/test.xml");
    }


};

/**
 * Tests correct XML parsing
 */
TEST_F(XMLReaderTest, XMLInfoTest){
    XMLReaderTest::readGeneratorInfoFromXML();


    EXPECT_EQ(info.epsilon, 5);
    EXPECT_EQ(info.sigma, 1);
    EXPECT_EQ(info.t_end, 20);
    EXPECT_EQ(info.delta_t, 0.0005);
    EXPECT_EQ(info.writeFrequency, 50);
    EXPECT_EQ(info.brownianMotionMean, 0.0);
    EXPECT_FALSE(info.random);
    EXPECT_EQ(info.outputWriterType, IOWriter::vtk);
    EXPECT_EQ(info.calculatorType, PhysicsCalc::lennardJones);
    EXPECT_EQ(info.inputFiles.size(), 0);
    EXPECT_EQ(info.outputfile, "output");

    EXPECT_EQ(info.generatorInfos.size(), 2);
    EXPECT_EQ(info.generatorInfos[0].type, ParticleGenerator::cuboid);
    EXPECT_EQ(info.generatorInfos[0].DIM, 2);
    EXPECT_EQ(info.generatorInfos[0].mass, 1.0);
    EXPECT_EQ(info.generatorInfos[0].distance, 1.1225);
    EXPECT_EQ(info.generatorInfos[0].brownianFactor, 0.1);
    EXPECT_EQ(info.generatorInfos[0].N[0], 100);
    EXPECT_EQ(info.generatorInfos[0].N[1], 20);
    EXPECT_EQ(info.generatorInfos[0].N[2], 1);
    EXPECT_EQ(info.generatorInfos[0].pos[0], 20.0);
    EXPECT_EQ(info.generatorInfos[0].pos[1], 20.0);
    EXPECT_EQ(info.generatorInfos[0].pos[2], 0.0);
    EXPECT_EQ(info.generatorInfos[1].vel[0], 00.0);
    EXPECT_EQ(info.generatorInfos[1].vel[1], -10.0);
    EXPECT_EQ(info.generatorInfos[1].vel[2], 00.0);

    EXPECT_EQ(info.generatorInputFiles.size(), 0);
    EXPECT_TRUE(info.linkedcell);
    EXPECT_EQ(info.linkedCellSize[0], 180);
    EXPECT_EQ(info.linkedCellSize[1], 90);
    EXPECT_EQ(info.linkedCellSize[2], 1);
    EXPECT_EQ(info.rCut, 3.0);
    EXPECT_EQ(info.boundaryConditions[0], LinkedCellContainer::outflow);
    EXPECT_EQ(info.boundaryConditions[1], LinkedCellContainer::outflow);
    EXPECT_EQ(info.boundaryConditions[2], LinkedCellContainer::outflow);
    EXPECT_EQ(info.boundaryConditions[3], LinkedCellContainer::outflow);
    EXPECT_EQ(info.boundaryConditions[4], LinkedCellContainer::outflow);
    EXPECT_EQ(info.boundaryConditions[5], LinkedCellContainer::outflow);
}

/**
 * Tests more correct XML parsing with additional input files
 */
TEST_F(XMLReaderTest, XMLInfoTest2){
    XMLReaderTest::readFilesFromXML();

    EXPECT_EQ(info.writeFrequency, 10);
    EXPECT_EQ(info.epsilon, 10);
    EXPECT_EQ(info.sigma, 0.1);
    EXPECT_EQ(info.t_end, 10);
    EXPECT_EQ(info.delta_t, 0.005);
    EXPECT_EQ(info.calculatorType, PhysicsCalc::lennardJones);
    EXPECT_EQ(info.linkedcell, true);
    EXPECT_EQ(info.random, true);
    EXPECT_EQ(info.brownianMotionMean, 0.0);
    EXPECT_EQ(info.outputWriterType, IOWriter::xyz);
    EXPECT_EQ(info.linkedCellSize[0], 9);
    EXPECT_EQ(info.linkedCellSize[1], 9);
    EXPECT_EQ(info.linkedCellSize[2], 9);

    //main part of test is correctly reading multiple input/generator files
    EXPECT_EQ(info.generatorInputFiles.size(), 2);
    EXPECT_EQ(info.generatorInputFiles[0], "../input/files/genFile.json");
    EXPECT_EQ(info.generatorInputFiles[1], "../input/files/genFile2.json");

    EXPECT_EQ(info.inputFiles.size(), 2);
    EXPECT_EQ(info.inputFiles[0], "../input/files/assignment_1/eingabe-sonne.txt");
    EXPECT_EQ(info.inputFiles[1], "../input/files/someInput.txt");

}