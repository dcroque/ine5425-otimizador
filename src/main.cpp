// Copyright [2021] <Matheus Dhanyel Cândido Roque>

#include<iostream>

#include "structures.hpp"

int main(int argc, char** argv) {
// 1. Read command

    // 1.1. Parse command line

    auto cli = structures::CLI(argc, argv);
    auto logger = logger::Logger("MAIN", cli.verbosity());

    // 1.2. Check input file existance

    if (!cli.loaded()) {
        logger.error("Exiting application with an error");
        return 1;
    } else {
        logger.info("Configuration file succesfully loaded", 2);
    }

// 2. Setup stuff

    // 2.1. Parse input files

    // 2.2. Initialize OptimizationCore

    // 2.3. Run model simulation to check integrity

    // 2.4. Check simulation output file existance

    // 2.5 Initialize optimization logger

// 3. Run simulation

    // 3.1. Create model input variations

    // 3.2. Split simulator calls in threads

    // 3.3. Make calls to the simulator

    // 3.4. Wait for threads to join

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