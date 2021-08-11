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
    auto keep_optimization_running = true;
    auto pause = false;
    auto keep_dont_ask = 2;
    auto force_stop = false;
    auto reset_timer = true;
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::duration::zero();

    while(keep_optimization_running) {
        keep_dont_ask -= 1;

        if (reset_timer) {
            elapsed = std::chrono::steady_clock::duration::zero();
            reset_timer = false;
        }

        auto start_gen = std::chrono::high_resolution_clock::now();
        core.new_gen();

        auto run_status = core.run_gen();

        if (run_status) {
            logger.info("Generation "+ std::to_string(core.get_gen()) + " succesful", 2);
            core.check_gen_results();
        } else {
            logger.error("Couldn't run generation " + std::to_string(core.get_gen()) +". Aborting");
            return 1;
        }

        keep_optimization_running = core.check_optimization_end();

        auto finish_gen = std::chrono::high_resolution_clock::now();
        elapsed += finish_gen - start_gen;

        if (keep_dont_ask == 0 && cli.verbosity() > 1 && keep_optimization_running) {
            logger.info("Keep running for how much more generations? Enter 0 to pause and -1 to end optimization", 2);
            int n = 0;
            try {
                std::cin >> n;

                if (n == 0) {
                    pause = true;
                } else if (n == -1) {
                    force_stop = true;
                } else {
                    keep_dont_ask += n;
                }
            } catch (...) {
                logger.warn("Couldn't understand your input. Pausing optimization", 2);
                pause = true;
            }
        }

        while(((pause && !force_stop) || !keep_optimization_running) && cli.verbosity() > 0) {
            if (!keep_optimization_running) {
                logger.info("Optimization core flagged an stop", 1);
            } else if (pause) {
                logger.info("Pause called by user", 1);
            }

            std::cout << "Elapsed optimization time: " << elapsed.count() << "s" << std::endl;
            core.pause_menu();
            int n = 0;
            std::cin >> n;
            pause = true;

            while(pause) {
                try {
                    switch (n) {
                        case 0:
                            force_stop = true;
                            pause = false;
                            keep_optimization_running = true;
                            break;
                        case 1:
                            core.show_results();
                            break;
                        case 2:
                            pause = false;
                            keep_optimization_running = true;
                            break;
                        case 3:
                            reset_timer = true;
                            break;
                        default:
                            break;
                    }
                } catch (...) {
                    logger.warn("Please choose an valid option", 1);
                    n = 9;
                }

                if (pause) {
                    std::cout << "Elapsed optimization time: " << elapsed.count() << "s" << std::endl;
                    core.pause_menu();
                    std::cin >> n;
                }
            }
        }

        if (force_stop) {
            logger.info("Termination selected by user", 1);
            core.show_results();
            keep_optimization_running = false;
        }
    }

    logger.info("Exiting optimization application", 2);

    if (cli.verbosity() == 0) {
        core.show_results();
    }

    return 0;
}