// Copyright [2021] <Matheus Dhanyel Cândido Roque>

#include "library.hpp"

int main(int argc, char** argv) {
    auto cli = structures::CLI(argc, argv);
    auto logger = logger::Logger("MAIN", cli.verbosity());

    if (!cli.loaded()) {
        logger.error("Configuration file not found. Exiting application with an error");
        return 1;
    } else {
        logger.info("Configuration file succesfully loaded", 2);
    }

    auto core = structures::OptimizationCore(cli.cfg_file(), cli.verbosity());
    auto keep_optimization_running = false;
    auto pause = false;
    auto keep_dont_ask = 0;

// 3. Run simulation

    // 3.1. Create model input variations

    // 3.2. Split simulator calls in threads

    // 3.3. Make calls to the simulator

    // 3.4. Wait for threads to join

    while(keep_optimization_running) {
        core.new_gen();

        auto run_status = core.run_gen();

        if (run_status) {
            logger.info("Generation "+ std::to_string(core.get_gen()) + " succesful", 3);
        } else {
            logger.error("Couldn't run this generation " + std::to_string(core.get_gen()) +". Aborting");
            return 1;
        }

        keep_optimization_running = core.check_optimization_end();

        if (keep_dont_ask == 0 && cli.verbosity() > 1 && keep_optimization_running) {
            logger.info("Keep running for how much more generations? Enter 0 to pause and -1 to end optimization", 2);
            int n = 0;
            try {
                std::cin >> n;
            } catch (...) {
                logger.warn("Couldn't understand your input. Pausing optimization", 2);
                pause = true;
            }
        }

        while((pause || !keep_optimization_running) && cli.verbosity() > 0) {
            core.show_results();
        }
    }
// 4. Read results

    // 4.1. Check existance for all reports

    // 4.2. Split parser calls in threads

    // 4.3. Parse reports and fill response vectors

    // 4.4. Wait for threads to join

// 5. Decide on optimization or end process (then goto 6 or 7)

    // 5.1. Check constraints of each OptimizationUnit

    // 5.2. Calculate fit for each OptimizationUnit

    // 5.3. Compare fit with solutions vector

    // 5.4. Check for pause instruction - GOTO 7.1

    // 5.5. Check for stop conditions - GOTO 7.1

    // 5.6. Continue optimization if there's no stop reason - GOTO 6.1 

// 6. Optimize

    // 6.1. Get two best fit from generation

    // 6.2. Mix control values

    // 6.3. Mutate some values randomly

    // 6.4. Run next generation simulations - GOTO 3.1

// 7. Post optimization interations

    // 7.1. Show stop reason and optimization results

    // 7.2. Check for optimization resume - GOTO 6.1

    // 7.3. Write results

    // 7.4. Close application

    logger.info("Exiting optimization application", 2);
    return 0;
}