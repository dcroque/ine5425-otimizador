// Copyright [2021] <Matheus Dhanyel Cândido Roque>
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include<stdlib.h>
#include<string>
#include<vector>
#include<ctime>
#include <fstream>

#include "binary_tree.hpp"

namespace logger {
    class Logger {
        private:
            int verb;
            std::string entity;

            void log(std::string tag, std::string message, int priority);
        public:
            Logger(std::string _entity, int _verb);
            ~Logger();

            void set_verb(int _verb);
            void set_entity( std::string _entity);

            int get_verb();
            std::string get_entity();

            void info(std::string message, int priority);
            void warn(std::string message, int priority);
            void error(std::string message);
    };
}

namespace structures {
    class Variable {
        private:
            std::string name;
            double value;
        public:
            Variable(std::string _name, double _value);
            ~Variable();

            std::string get_name();
            double get_value();

            void set_value(double& _value);
    };

    class VarVector {
        public:
            std::vector<Variable> vector;

            VarVector();
            ~VarVector();

            double var_value(std::string name);
    };

    class ExpressionTree {
        private:
            std::string expression;
            int input_size;
            BinaryTree<std::string> calc_tree;

            int first_operator(std::string expr);
            std::vector<std::string> operators = {"+", "-", "*", "/", "^", "%", "l"};
        public:
            ExpressionTree(std::string expression);
            ~ExpressionTree();

    };

    class CLI {
        private:
            std::string config_file;
            logger::Logger logger = logger::Logger("CLI", 2);
        public:
            CLI(int argc, char** argv);
            int verbosity();
            bool loaded();
            std::string cfg_file();
    };

    class Constraint {
        private:
            std::string name;
            ExpressionTree expression;
        public:
            Constraint(std::string _name, std::string expression);
            ~Constraint();
    };

    class Objective {
        private:
            bool max;
            ExpressionTree expression;
        public:
            Objective(bool _max, std::string expression);
            ~Objective();
    };

    class OptimizationConfig {
        private:
            std::vector<std::string> controls;
            std::vector<std::string> responses;
            std::vector<Constraint> constraints;
            std::vector<Objective> objectives;
            std::string model_path;
            std::string output_path;
            std::string simulator_path;
            logger::Logger logger = logger::Logger("OPT_CONFIG_CORE", 2);

            int generation_size = 20;
            int precision = 5;
            int max_persist = 10;
            int persist = 0;
            int gen = 0;
            int min_gen = 10;
        public:
            OptimizationConfig(std::string config_file, int _verb);
    };

    class OptimizationUnit {
        private:
            logger::Logger logger = logger::Logger("OPT_UNIT", 2);
            VarVector controls;
            VarVector responses;
            double fit;
        public:
            OptimizationUnit(OptimizationUnit& parent1, OptimizationUnit& parent2, std::string ID, int verb);
            OptimizationUnit(VarVector _controls, std::string ID, int verb);
            ~OptimizationUnit();
            void config_logger(std::string _entity, int _verb);
    };

    class Solution {
        private:
            OptimizationUnit unit;
        public:
            Solution(OptimizationUnit _unit);
            ~Solution();
    };

    class OptimizationCore {
        private:
            logger::Logger logger = logger::Logger("OPT_CORE", 2);
            std::vector<OptimizationUnit> units;
            OptimizationConfig configs;
            std::vector<Solution> solutions;
        public:
            OptimizationCore();
            ~OptimizationCore();
    };
}

#endif

logger::Logger::Logger(std::string _entity, int _verb) {
    entity = _entity;
    verb = _verb;
}

logger::Logger::~Logger() {}

void logger::Logger::log(std::string tag, std::string message, int priority) {
    time_t now = std::time(0);
    tm *ltm = localtime(&now);
    std::string color;

    if (tag == "ERROR") {
        color = "\033[31m";
    } else if (tag == "INFO") {
        color = "\033[32m";
    } else if (tag == "WARN") {
        color = "\033[33m";
    } else {
        color = "\033[37m";
    }

    if (priority <= verb) {
        std::cout << color << "["<<     ltm->tm_mday <<"/"<< 
                                        ltm->tm_mon <<"/"<< 
                                        ltm->tm_year+1900 <<" "<< 
                                        ltm->tm_hour <<":"<< 
                                        ltm->tm_min <<":"<< 
                                        ltm->tm_sec <<" "<< 
                                        tag <<"]\033[37m "<< 
                                        entity << ": " <<
                                        message << std::endl;
    }
}

void logger::Logger::info(std::string message, int priority) {
    log("INFO",message, priority);
}

void logger::Logger::warn(std::string message, int priority) {
    log("WARN",message, priority);
}

void logger::Logger::error(std::string message) {
    log("ERROR",message, 0);
}

void logger::Logger::set_verb(int _verb) {
    verb = _verb;
}

void logger::Logger::set_entity(std::string _entity) {
    entity = _entity;
}

int logger::Logger::get_verb() {
    return verb;
}

std::string logger::Logger::get_entity() {
    return entity;
}

structures::CLI::CLI(int argc, char** argv) {

    if (argc < 3) {
        logger.error("Too few arguments");
        return;
    } else if (argc > 5) {
        logger.warn("Too many arguments", 0);
    }

    bool config_set = false;
    for (auto i = 1; i < argc; i++) {
        if ((std::string(argv[i]) == "-c" || std::string(argv[i]) == "--config") && i < argc - 1) {
            config_file = argv[i+1];
            if (!config_set) {
                logger.info("Configuration file set", 0);
                config_set = true;
            } else {
                logger.warn("Configuration file overwrited", 0);
            }
            i++;
        } else if ((std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") && i < argc - 1){
            try {
                logger.set_verb(std::stoi(argv[i+1]));
                logger.info("Verbosity level changed", 0);
            } catch (...) {
                logger.warn("Could not read verbosity. Setting default value: 2", 0);
                logger.set_verb(2);
            }
            i++;
        } else {
            logger.warn("Unknown flag ignored", 0);
        }
    }
}

int structures::CLI::verbosity() {
    return logger.get_verb();
}

bool structures::CLI::loaded() {
    if (FILE *file = fopen(config_file.c_str(), "r")) {
        fclose(file);
        logger.info("Configuration file sucessfully found", 2);
        return true;
    } else {
        logger.error("Configuration file not found");
        return false;
    }
}

std::string structures::CLI::cfg_file() {
    return config_file;
}