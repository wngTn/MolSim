#include "MainUtils.h"

void MainUtils::get_arguments(int argc, char **argv) {
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
                xml_file = optarg;
                break;
            case 'i':
                filename.emplace_back(optarg);
                break;
            case 'g':
                generator_files.emplace_back(optarg);
                break;
            case 'e':
                end_time = std::stod(optarg);
                break;
            case 'd':
                delta_t = std::stod(optarg);
                break;
            case 'w':
                if (std::string("vtk") == optarg || std::string("v") == optarg) {
                    io_type = IOWriter::vtk;
                } else if (std::string("xyz") == optarg || std::string("x") == optarg) {
                    io_type = IOWriter::xyz;
                }
                break;
            case 'c':
                if (std::string("lj") == optarg || std::string("lennardjones") == optarg) {
                    calc_type = PhysicsCalc::lennardJones;
                } else if (std::string("g") == optarg || std::string("grav") == optarg
                           || std::string("gravitation") == optarg) {
                    calc_type = PhysicsCalc::gravitation;
                }
                break;
            case 'b':
                brownianMotion = true;
                brownianMotionMean = std::stod(optarg);
                break;
            case 'r':
                randomGen = true;
                break;
            case 'm':
                if(std::string("benchmark") == optarg){
                    spdlog::set_level(spdlog::level::off);
                    benchmarking = true;
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

std::unique_ptr<IOWriter> MainUtils::get_io_type() {
    switch (io_type) {
        case IOWriter::xyz:
            return std::make_unique<outputWriter::XYZWriter>();
        case IOWriter::vtk:
        case IOWriter::unknown:
        default:
            return std::make_unique<outputWriter::VTKWriter>();
    }
}

std::unique_ptr<PhysicsCalc> MainUtils::get_calculator() {
    switch (calc_type) {
        case PhysicsCalc::gravitation:
            return std::make_unique<calculator::Gravitation>();
        case PhysicsCalc::lennardJones:
        case PhysicsCalc::unknown:
        default:
            if(linkedCell){
                return std::make_unique<calculator::LinkedCell>(sigma, eps, rCut);
            }
            return std::make_unique<calculator::LennardJones>(sigma, eps);
    }
}

std::unique_ptr<ParticleContainer> MainUtils::get_container() {
    if(linkedCell){
        return std::make_unique<LinkedCellContainer>(linkedCellSize[0], linkedCellSize[1], linkedCellSize[2], rCut, boundaryConditions);
    }
    return std::make_unique<DirectSumParticleContainer>();
}

void MainUtils::initializeParticles(ParticleContainer &particles) {
    // read normal input file
    for(auto& file : filename){
        FileReader::readFile(particles, file);
    }

    // if -r flag is set, generate random input using python script
    if (randomGen) {
        // maybe change particle amount
        if (std::system("python ../input/generate_input.py -n 24 -o input.txt")) {
            spdlog::critical("Error while generating random input.");
            exit(-1);
        }
        FileReader::readFile(particles, "../input/files/input.txt");
    }
    // apply brownian motion for everything except the particles generated by the ParticleGenerator
    if (brownianMotion) {
        initializeBrownianMotion(particles, brownianMotionMean);
    }
    // generate Particles from generator input JSONs
    for(auto& genFile : generator_files){
        ParticleGenerator::generateParticles(particles, genFile);
    }
    // generate Particles directly specified in XML
    ParticleGenerator::generateParticles(particles, generatorInfos);
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

void MainUtils::parseXML() {
    spdlog::info("Starting XML parsing!");
    if(xml_file.empty()) return;

    XMLReader::XMLInfo info = XMLReader::readFile(xml_file);

    if(!filename.empty()){
        info.inputFiles.push_back(filename.front());
    }
    filename = info.inputFiles;
    io_type = info.outputWriterType;
    output_file = info.outputfile;
    end_time = info.t_end;
    delta_t = info.delta_t;
    writeFrequency = info.writeFrequency;
    randomGen = info.random;

    if(!generator_files.empty()){
        info.generatorInputFiles.push_back(generator_files.front());
    }
    generator_files = info.generatorInputFiles;
    generatorInfos = info.generatorInfos;

    calc_type = info.calculatorType;
    if(calc_type == PhysicsCalc::lennardJones){
        eps = info.epsilon;
        sigma = info.sigma;
        brownianMotionMean = info.brownianMotionMean;
        if(info.linkedcell){
            linkedCell = info.linkedcell;
            rCut = info.rCut;
            linkedCellSize = info.linkedCellSize;
            boundaryConditions = info.boundaryConditions;
        }

       grav = info.gravityFactor;

    }
    spdlog::info("Finished XML parsing!");
}

void MainUtils::printConfig() {
    std::string message = "Your configurations are:\n";
    if(!xml_file.empty()){
        message.append("\u001b[36m\tXML File:\u001b[0m " + xml_file);
    }
    if(!filename.empty()){
        message.append("\n\u001b[36m\tFilenames:\u001b[0m ");
        for(auto& f : filename){
            message.append(f + " ");
        }
    }

    if(!generator_files.empty()){
        message.append("\n\u001b[36m\tGenerator Input Files:\u001b[0m ");
        for(auto& f : generator_files){
            message.append(f + " ");
        }
    }

    if(!generatorInfos.empty()){
        message.append("\n\u001b[36m\tGenerator Input (XML):\u001b[0m " + std::to_string(generatorInfos.size()));
    }
    message.append("\n\u001b[36m\tContainer:\u001b[0m ").append(linkedCell?"LinkedCellContainer\n":"DirectSumContainer\n");
    message.append("\u001b[36m\tCalculator:\u001b[0m ");
    switch (calc_type) {
        case PhysicsCalc::gravitation:
            message.append("Gravitation\n");
            break;
        case PhysicsCalc::lennardJones:
            message.append("Lennard-Jones-Potential").append(linkedCell?" (LinkedCellCalculation)\n":"\n");
            break;
        case PhysicsCalc::unknown:
        default:
            message.append("Lennard-Jones-Potential (Default)").append(linkedCell?" (LinkedCellCalculation)\n":"\n");
    }
    message.append("\u001b[36m\tWriter:\u001b[0m ");
    switch (io_type) {
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
    message.append("\u001b[36m\tWrite Frequency:\u001b[0m ").append(std::to_string(writeFrequency));
    if(randomGen) message.append("\n\u001b[36m\tRandom Generation\u001b[0m (This needs Python3)\n");
    message.append("\n\u001b[36m\tBrownianMotion:\u001b[0m ").append(std::to_string(brownianMotionMean));
    message.append("\n\u001b[36m\tEnd_time:\u001b[0m ").append(std::to_string(end_time)).append(end_time == 1000 ? " (Default)\n" : "\n");
    message.append("\u001b[36m\tDelta_t:\u001b[0m ").append(std::to_string(delta_t)).append(delta_t == 0.014 ? " (Default)\n" : "\n");
    std::cout << message << "Calculating..." << std::endl;
}

void MainUtils::initializeLogger() {
    auto logger = spdlog::basic_logger_mt("molsim_logger", "./logs/molsim.log");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::off);
}

int MainUtils::main(int argc, char *argv[]) {
    initializeLogger();

    auto start_setup = std::chrono::steady_clock::now();

    // parse cmd line args and set static values accordingly
    get_arguments(argc, argv);

    // ------ setup ------ //
    parseXML();

    printConfig();

    auto particles = MainUtils::get_container();
    initializeParticles(*particles);

    auto io = MainUtils::get_io_type();
    auto calc = MainUtils::get_calculator();

    nThermostat = 100;

    calc->setDim(DIM);
    calc->setDeltaT(delta_t);
    calc->setG(grav);

    double current_time = start_time;
    int iteration = 0;
    // ------ end setup ------ //
    if(benchmarking){
        std::cout
                << "\u001b[31mYou have chosen the benchmark mode!\u001b[0m" << std::endl
                << "\u001b[31mElapsed setup time [microseconds]:\u001b[0m "
                << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_setup).count()
                << std::endl
                << "\u001b[31mIn total there are:\u001b[0m " << particles->size() << " particles." << std::endl;
    }

    spdlog::info(
            "Start calculating particles with\n\tIO type:\t\t{:<15}\n\tcalculator type:\t{:<15}\n\tend time:\t\t{:<15}\n\ttimestep:\t\t{:<15}",
            io->toString(), calc->toString(), end_time, delta_t);

    // ------ calculation ------ //
    auto start_calc = std::chrono::steady_clock::now();
    // initial setup
    // TODO setup temperature
    calc->calcX(*particles);
    particles->setup();
    calc->calcF(*particles);
    if (!benchmarking){
        io->write(*particles, "output", iteration);
    }
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
        spdlog::info("Iteration {}: ", iteration);
        MainUtils::logParticle(*particles);


        calc->calcX(*particles);
        particles->setup();
        calc->calcF(*particles);
        calc->calcV(*particles);

        MainUtils::logParticle(*particles);

        iteration++;

        // todo apply thermostat stuff using Thermostat::applyTemperature()

        if (!benchmarking && iteration % writeFrequency == 0) {
            particles->cleanup();
            // setup after cleanup needed to validate pointers for calcX
            particles->setup();
            // uses abstract write method overwritten by specific IO method
            io->write(*particles, output_file, iteration);
        }

        current_time += delta_t;
    }
    // ------ end calculation ------ //
    if(benchmarking){
        auto runtimeDuration =
                std::chrono::duration_cast<std::chrono::milliseconds>
                        (std::chrono::steady_clock::now() - start_calc).count();
        double mups = static_cast<double>(particles->size()) * (end_time/delta_t) /
                      (static_cast<double>(runtimeDuration) / 1000);
        std::cout
                << "\u001b[31mElapsed calculation time [milliseconds]:\u001b[0m "
                << runtimeDuration
                << std::endl;
        if (mups > 1000000) {
            std::cout<<"\u001b[31mMMUPS/s:\u001b[0m "<<mups/1000000<<std::endl;
        } else {
            std::cout<<"\u001b[31mMUPS/s:\u001b[0m "<<mups<<std::endl;
        }
    } else{
        std::cout << "All files have been written!" << std::endl;
        spdlog::info("All files have been written!");
    }

    return 0;
}


