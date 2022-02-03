#include "utils/MainUtils.h"

int main(int argc, char *argv[]) {
    Config config{};

    MainUtils::initializeLogger();

    auto start_setup = std::chrono::steady_clock::now();

    // parse cmd line args and set static values accordingly
    MainUtils::get_arguments(argc, argv, config);

    // ------ setup ------ //
    MainUtils::parseXML(config);

    MainUtils::printConfig(config);

    auto particles = MainUtils::get_container(config);
    MainUtils::initializeParticles(*particles, config);

    auto io = MainUtils::get_io_type(config);
    auto calc = MainUtils::get_calculator(config);

    Thermostat thermostat = MainUtils::get_thermostat(config);
    auto stats = MainUtils::get_statistics_logger(config);

    MainUtils::validateInput(config, *particles);

    calc->setDim(config.DIM);
    calc->setDeltaT(config.delta_t);

    double current_time = config.start_time;
    int iteration = config.start_iteration;
    // ------ end setup ------ //
    if(config.benchmarking){
        std::cout
                << "\033[4mYou have chosen the benchmark mode:\033[0m" << std::endl
                << "\u001b[31m\tElapsed setup time [microseconds]:\u001b[0m "
                << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_setup).count()
                << std::endl
                << "\u001b[31m\tIn total there are:\u001b[0m " << particles->size() << " particles." << std::endl;
    }

    spdlog::info(
            "Start calculating particles with\n\tIO type:\t\t{:<15}\n\tcalculator type:\t{:<15}\n\tend time:\t\t{:<15}\n\ttimestep:\t\t{:<15}",
            io->toString(), calc->toString(), config.end_time, config.delta_t);
    std::cout << "Calculating..." << std::endl;
    // ------ calculation ------ //
    auto start_calc = std::chrono::steady_clock::now();
    // initial setup
    if(config.useThermostat){
        thermostat.setupTemperature(*particles);
    }
    //calc->calcX(*particles);
    //two times setup is needed, so that oldF (which is used when writing) is set to the initial baseForce
    particles->setup();
    particles->setup();
    calc->calcF(*particles);
    if (!config.benchmarking){
        io->write(*particles, config.output_file, iteration);
        if(config.useStatistics){
            stats->writeStatistics(*particles, iteration);
        }
    }
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < config.end_time) {
        spdlog::info("Iteration {}: ", iteration);
        MainUtils::logParticle(*particles);

        calc->calcX(*particles);
        particles->setup();
        calc->calcF(*particles);
        calc->calcV(*particles);

        if(config.useThermostat && iteration % config.nThermostat == 0) { [[unlikely]]
            thermostat.applyTemperature(*particles);
        }

        iteration++;

        if (!config.benchmarking && iteration % config.writeFrequency == 0) { [[unlikely]]
            particles->cleanup();
            // setup after cleanup needed to validate pointers for calcX
            particles->setup();
            // uses abstract write method overwritten by specific IO method
            io->write(*particles, config.output_file, iteration);
        }

        if(config.useStatistics && iteration % config.statsFrequency == 0) { [[unlikely]]
            stats->writeStatistics(*particles, iteration);
        }

        if(config.resetBaseForce && iteration == config.resetBaseForceIteration) { [[unlikely]]
            particles->reset_base_force();
        }

        current_time += config.delta_t;
    }
    if(config.checkpointing){
        CheckpointWriter::writeCheckpoint(config.checkpointOutput, *particles, *calc, iteration);
        std::cout << "Wrote current state to checkpoint file: " << config.checkpointOutput << std::endl;
    }
    // ------ end calculation ------ //
    if(config.benchmarking){
        auto runtimeDuration =
                std::chrono::duration_cast<std::chrono::milliseconds>
                        (std::chrono::steady_clock::now() - start_calc).count();
        double mups = static_cast<double>(particles->size()) * iteration /
                      (static_cast<double>(runtimeDuration) / 1000);
        std::cout
                << "\u001b[31m\tElapsed calculation time [milliseconds]:\u001b[0m "
                << runtimeDuration
                << std::endl;
        if (mups > 1000000) {
            std::cout<<"\u001b[31m\tMMUPS/s:\u001b[0m "<<mups/1000000<<std::endl;
        } else {
            std::cout<<"\u001b[31m\tMUPS/s:\u001b[0m "<<mups<<std::endl;
        }
    } else{
        std::cout << "All files have been written!" << std::endl;
        spdlog::info("All files have been written!");
    }

    return 0;
}