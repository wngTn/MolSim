#include "MainUtils.h"

void MainUtils::get_arguments(int argc, char **argv, Config& config) {
    const std::string help = "Usage: ./MolSim [-x <xml_file>] [-i <input_file>] [-g <generator_input>] [-e <end_time>] [-d <delta_t>] [-w <writer>] [-c <calc>] [-b <brownian_motion_velocity_mean>] [-r]\n"
                             "\tuse -x to specify an XML input file. Overwrites command line options (for options where multiple inputs are allowed, e.g. input files, command line argument is added)\n"
                             "\tuse -i to specify an input file\n"
                             "\tuse -g to specify a generator input file\n"
                             //"\tuse -t to specify an input type: 'g'/'generate' to generate based on values from input_file, 'r'/'random' for random input (-i discarded for random)\n"
                             "\tuse -w to choose an output writer: v/vtk for VTKWriter (default) or x/xyz for XYZWriter\n"
                             "\tuse -c to choose a physics calculator: g/grav/gravitation for Gravitation, lj/lennardjones for LennardJonesPotential (default)\n"
                             // the situation with the generator is not very nice, but I want BrownianMotion optional for the generator but optional flag arguments are not supported
                             "\tuse -b to initialize particle movement with Brownian Motion (MaxwellBoltzmann) (not used for Generator generated particles)\n"
                             "\tuse -r for randomly generated particles\n"
                             "\tuse -m to change execution mode: benchmark to disable logging and IO operations, debug to write logs, normal to only write output files\n"
                             "\twhen leaving out -e/-d default values are used (1000/0.014)\n"
                             "\tcall with flag -h to display this message\n";
    int opt;
    if (argc == 1) {
        std::cout << help;
        exit(0);
    }
    while ((opt = getopt(argc, argv, "x:hi:g:e:d:w:c:rb:m:")) != -1) {
        switch (opt) {
            case 'h':
                std::cout << help;
                exit(0);
            case 'x':
                config.xml_file = optarg;
                break;
            case 'i':
                config.filename.emplace_back(optarg);
                break;
            case 'g':
                config.generator_files.emplace_back(optarg);
                break;
            case 'e':
                config.end_time = std::stod(optarg);
                break;
            case 'd':
                config.delta_t = std::stod(optarg);
                break;
            case 'w':
                if (std::string("vtk") == optarg || std::string("v") == optarg) {
                    config.io_type = IOWriter::vtk;
                } else if (std::string("xyz") == optarg || std::string("x") == optarg) {
                    config.io_type = IOWriter::xyz;
                }
                break;
            case 'c':
                if (std::string("lj") == optarg || std::string("lennardjones") == optarg) {
                    config.calc_type = PhysicsCalc::lennardJones;
                } else if (std::string("g") == optarg || std::string("grav") == optarg
                           || std::string("gravitation") == optarg) {
                    config.calc_type = PhysicsCalc::gravitation;
                }
                break;
            case 'b':
                config.brownianMotion = true;
                config.brownianMotionMean = std::stod(optarg);
                break;
            case 'r':
                config.randomGen = true;
                break;
            case 'm':
                if(std::string("benchmark") == optarg){
                    spdlog::set_level(spdlog::level::off);
                    config.benchmarking = true;
                }else if(std::string("debug") == optarg){
                    spdlog::set_level(spdlog::level::debug);
                }
                break;
            case '?':
                std::cerr << "Unknown option: " << static_cast<char>(optopt) << "\n";
                std::cerr << help;
                break;
            default:
                break;
        }
    }
}

std::unique_ptr<IOWriter> MainUtils::get_io_type(Config& config) {
    switch (config.io_type) {
        case IOWriter::xyz:
            return std::make_unique<outputWriter::XYZWriter>();
        case IOWriter::vtk:
        case IOWriter::unknown:
        default:
            return std::make_unique<outputWriter::VTKWriter>();
    }
}

std::unique_ptr<PhysicsCalc> MainUtils::get_calculator(Config& config) {
    switch (config.calc_type) {
        case PhysicsCalc::gravitation:
            return std::make_unique<calculator::Gravitation>();
        case PhysicsCalc::lennardJones:
        case PhysicsCalc::unknown:
        default:
            if(config.linkedCell){
                auto c = std::make_unique<calculator::LinkedCell>(config.sigma, config.eps, config.rCut);
                c->setSigmaTable(sigmaTable);
                c->setEpsilonTable(epsilonTable);
                return c;
            }
            return std::make_unique<calculator::LennardJones>(config.sigma, config.eps);
    }
}

std::unique_ptr<ParticleContainer> MainUtils::get_container(Config& config) {
    if(config.linkedCell){
        return std::make_unique<LinkedCellContainer>(config.linkedCellSize[0], config.linkedCellSize[1], config.linkedCellSize[2], config.rCut, config.boundaryConditions, config.grav);
    }
    return std::make_unique<DirectSumParticleContainer>();
}

Thermostat MainUtils::get_thermostat(Config& config) {
    if(!config.useThermostat){
        return Thermostat{};
    }
    return Thermostat{config.initialTemperature, config.targetTemperature, config.maxDeltaTemperature};
}

void MainUtils::initializeParticles(ParticleContainer &particles, Config& config) {
    // read normal input file
    for(auto& file : config.filename){
        FileReader::readFile(particles, file);
    }

    // if -r flag is set, generate random input using python script
    if (config.randomGen) {
        // maybe change particle amount
        if (std::system("python ../input/generate_input.py -n 24 -o input.txt")) {
            spdlog::critical("Error while generating random input.");
            exit(-1);
        }
        FileReader::readFile(particles, "../input/files/input.txt");
    }
    // apply brownian motion for everything except the particles generated by the ParticleGenerator
    if (config.brownianMotion) {
        initializeBrownianMotion(particles, config.brownianMotionMean);
    }
    // if there are already generated particles, those have type 0
    auto typeToSE = std::vector<std::pair<int, std::pair<double,double>>>{};
    if((!config.filename.empty() || config.randomGen) && config.calc_type == PhysicsCalc::lennardJones){
        typeToSE.push_back({0, {config.sigma, config.eps}});
    }
    // generate Particles from generator input JSONs
    for(auto& genFile : config.generator_files){
        // TODO handle se index
        auto newTypeToSe = ParticleGenerator::generateParticles(particles, genFile, typeToSE.size());
        typeToSE.insert(typeToSE.begin(), newTypeToSe.begin(), newTypeToSe.end());
    }
    // generate Particles directly specified in XML
    // TODO handle se index
    auto newTypeToSe = ParticleGenerator::generateParticles(particles, config.generatorInfos);
    typeToSE.insert(typeToSE.begin(), newTypeToSe.begin(), newTypeToSe.end());
    if(config.calc_type == PhysicsCalc::lennardJones && config.linkedCell){
        buildSETable(typeToSE);
    }

}

void MainUtils::buildSETable(std::vector<std::pair<int, std::pair<double, double>>> &mapping){
    auto temp = std::vector<double>{};
    // fill 2D vectors with zeroes
    temp.assign(mapping.size(), 0.0);
    sigmaTable.assign(mapping.size(), temp);
    epsilonTable.assign(mapping.size(), temp);

    for(auto& [t1, p1] : mapping){
        for(auto& [t2, p2] : mapping){
            double sigma_mixed = (p1.second + p2.second) / 2;
            double eps_mixed = sqrt(p1.first * p2.first);

            sigmaTable[t1][t2] = sigma_mixed;
            sigmaTable[t2][t1] = sigma_mixed;
            epsilonTable[t1][t2] = eps_mixed;
            epsilonTable[t2][t1] = eps_mixed;
        }
    }
    /*for_each(sigmaTable.begin(),
             sigmaTable.end(),
             [](const auto & row ) {
                 for_each(row.begin(), row.end(),
                          [](const auto & elem){
                              std::cout<<elem<<", ";
                          });
                 std::cout<<std::endl;
             });
    std::cout<<std::endl;
    for_each(epsilonTable.begin(),
             epsilonTable.end(),
             [](const auto & row ) {
                 for_each(row.begin(), row.end(),
                          [](const auto & elem){
                              std::cout<<elem<<", ";
                          });
                 std::cout<<std::endl;
             });
    std::cout<<std::endl;*/
}

void MainUtils::logParticle(ParticleContainer &particles) {
    spdlog::info("-----------------------------------------------------------------------------------------------"
                 "---------------------------------------------------------------");
    for(auto &p : particles){
        auto x = p.getX()[0];
        auto y = p.getX()[1];
        auto z = p.getX()[2];
        auto v0 = p.getV()[0];
        auto v1 = p.getV()[1];
        auto v2 = p.getV()[2];
        auto f0 = p.getF()[0];
        auto f1 = p.getF()[1];
        auto f2 = p.getF()[2];
        spdlog::info("Moved particle ({} {} {}) with velocity ({} {} {}) and force ({} {} {})", x, y, z, v0, v1, v2, f0, f1, f2);
    }
}

void MainUtils::parseXML(Config& config) {
    spdlog::info("Starting XML parsing!");
    if(config.xml_file.empty()) return;

    XMLReader::XMLInfo info = XMLReader::readFile(config.xml_file);

    if(!config.filename.empty()){
        info.inputFiles.push_back(config.filename.front());
    }
    config.filename = info.inputFiles;
    config.io_type = info.outputWriterType;
    config.output_file = info.outputfile;
    config.end_time = info.t_end;
    config.delta_t = info.delta_t;
    config.writeFrequency = info.writeFrequency;
    config.randomGen = info.random;

    if(!config.generator_files.empty()){
        info.generatorInputFiles.push_back(config.generator_files.front());
    }
    config.generator_files = info.generatorInputFiles;
    config.generatorInfos = info.generatorInfos;

    config.calc_type = info.calculatorType;
    if(config.calc_type == PhysicsCalc::lennardJones){
        config.eps = info.epsilon;
        config.sigma = info.sigma;
        config.brownianMotionMean = info.brownianMotionMean;
        if(info.linkedcell){
            config.linkedCell = info.linkedcell;
            config.rCut = info.rCut;
            config.linkedCellSize = info.linkedCellSize;
            config.boundaryConditions = info.boundaryConditions;
        }
        config.grav = info.gravityFactor;
    }
    config.useThermostat = info.useThermostat;
    if(info.useThermostat){
        config.initialTemperature = info.t_init;
        config.targetTemperature = info.t_target;
        config.maxDeltaTemperature = info.delta_temp;
    }
    spdlog::info("Finished XML parsing!");
}

void MainUtils::printConfig(Config& config) {
    std::string message = "Your configurations are:\n";
    if(!config.xml_file.empty()){
        message.append("\u001b[36m\tXML File:\u001b[0m " + config.xml_file);
    }
    if(!config.filename.empty()){
        message.append("\n\u001b[36m\tFilenames:\u001b[0m ");
        for(auto& f : config.filename){
            message.append(f + " ");
        }
    }

    if(!config.generator_files.empty()){
        message.append("\n\u001b[36m\tGenerator Input Files:\u001b[0m ");
        for(auto& f : config.generator_files){
            message.append(f + " ");
        }
    }

    if(!config.generatorInfos.empty()){
        message.append("\n\u001b[36m\tGenerator Input (XML):\u001b[0m " + std::to_string(config.generatorInfos.size()));
    }
    message.append("\n\u001b[36m\tContainer:\u001b[0m ").append(config.linkedCell?"LinkedCellContainer\n":"DirectSumContainer\n");
    message.append("\u001b[36m\tCalculator:\u001b[0m ");
    switch (config.calc_type) {
        case PhysicsCalc::gravitation:
            message.append("Gravitation\n");
            break;
        case PhysicsCalc::lennardJones:
            message.append("Lennard-Jones-Potential").append(config.linkedCell?" (LinkedCellCalculation)\n":"\n");
            break;
        case PhysicsCalc::unknown:
        default:
            message.append("Lennard-Jones-Potential (Default)").append(config.linkedCell?" (LinkedCellCalculation)\n":"\n");
    }
    if(config.useThermostat){
        message.append("\u001b[36m\tThermostat:\u001b[0m ").append("nThermostat: ").append(std::to_string(config.nThermostat)).append(", Tinit: ").append(std::to_string(config.initialTemperature));
        message.append(", Ttarget: ").append(std::to_string(config.targetTemperature));
        if(abs(config.maxDeltaTemperature - DBL_MAX) < 10){
            message.append(", maxDeltaT: infinity\n");
        }else{
            message.append(", maxDeltaT: ").append(std::to_string(config.maxDeltaTemperature) + "\n");
        }
    }
    message.append("\u001b[36m\tWriter:\u001b[0m ");
    switch (config.io_type) {
        case IOWriter::vtk:
            message.append("VTK-Writer\n");
            break;
        case IOWriter::xyz:
            message.append("XYZ-Writer\n");
            break;
        case IOWriter::unknown:
        default:
            message.append("VTK-Writer (Default)\n");
    }
    message.append("\u001b[36m\tWrite Frequency:\u001b[0m ").append(std::to_string(config.writeFrequency));
    if(config.randomGen) message.append("\n\u001b[36m\tRandom Generation\u001b[0m (This needs Python3)\n");
    message.append("\n\u001b[36m\tBrownianMotion:\u001b[0m ").append(std::to_string(config.brownianMotionMean));
    message.append("\n\u001b[36m\tEnd_time:\u001b[0m ").append(std::to_string(config.end_time)).append(config.end_time == 1000 ? " (Default)\n" : "\n");
    message.append("\u001b[36m\tDelta_t:\u001b[0m ").append(std::to_string(config.delta_t)).append(config.delta_t == 0.014 ? " (Default)\n" : "\n");
    std::cout << message << "Calculating..." << std::endl;
}

void MainUtils::initializeLogger() {
    auto logger = spdlog::basic_logger_mt("molsim_logger", "./logs/molsim.log");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::off);
}


