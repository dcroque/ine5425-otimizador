// Copyright [2021] <Matheus Dhanyel Cândido Roque>
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <algorithm>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <random>

#include "binary_tree.hpp"

std::vector<std::string> split(std::string text, std::string delimiter) {
    std::vector<std::string> result;
    std::string temp;
    auto ini = 0;
    auto substr_pos = 0;
    
    while (text.find(delimiter, ini) != std::string::npos) {
        substr_pos = text.find(delimiter, ini);
        temp = text.substr(ini, substr_pos);
        ini += temp.length()+delimiter.length();
        result.push_back(temp);
    }

    result.push_back(text.substr(ini));
    return result;
}

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

namespace data {
    class Variable {
        private:
            std::string name;
            double value;
        public:
            Variable(std::string _name, double _value);
            Variable();
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
            void add_var(Variable _var);
    };

    class ExpressionTree {
        private:
            std::string expression;
            int input_size;
            structures::BinaryTree<std::string> calc_tree;

            int first_operator(std::string expr);
            std::vector<std::string> operators = {"+", "-", "*", "/", "^", "%", "l"};
            std::vector<std::string> comparators = {"!=", ">=", "<=", ">", "<", "="};
        public:
            ExpressionTree(std::string _expression);
            ExpressionTree();
            ~ExpressionTree();

    };

    class Constraint {
        private:
            ExpressionTree expression;
        public:
            Constraint(std::string _expression);
            Constraint();
            ~Constraint();
    };

    class Objective {
        private:
            bool max;
            ExpressionTree expression;
        public:
            Objective(bool _max, std::string _expression);
            Objective();
            ~Objective();
    };
}

namespace structures {

    class CLI {
        private:
            std::string config_filename;
            logger::Logger logger = logger::Logger("CLI", 2);
        public:
            CLI(int argc, char** argv);
            int verbosity();
            bool loaded();
            std::string cfg_file();
    };

    class OptimizationUnit {
        private:
            logger::Logger logger = logger::Logger("OPT_UNIT", 2);
            data::VarVector controls;
            data::VarVector responses;
            double fit;
        public:
            OptimizationUnit(OptimizationUnit& parent1, OptimizationUnit& parent2, std::string ID, int verb);
            OptimizationUnit(data::VarVector _controls, std::string ID, int verb);
            OptimizationUnit();
            ~OptimizationUnit();
            void config_logger(std::string _entity, int _verb);
    };

    class OptimizationConfig {
        private:
            std::vector<std::string> controls;
            std::vector<std::string> responses;
            std::vector<data::Constraint> constraints;
            std::vector<data::Objective> objectives;
            std::string model_path;
            std::string output_path;
            std::string simulator_path;
            logger::Logger logger = logger::Logger("OPT_CONFIG_CORE", 2);
            OptimizationUnit initial_unit;

            int replication_amount = 30;
            int generation_size = 20;
            int precision = 5;
            int max_persist = 10;
            int persist = 0;
            int gen = 0;
            int min_gen = 10;
        public:
            OptimizationConfig(std::string config_filename, int _verb);
            OptimizationConfig();
            ~OptimizationConfig();
    };

    class Solution {
        private:
            OptimizationUnit unit;
        public:
            Solution(OptimizationUnit _unit);
            Solution();
            ~Solution();
    };

    class OptimizationCore {
        private:
            logger::Logger logger = logger::Logger("OPT_CORE", 2);
            OptimizationConfig configs;
            std::vector<Solution> solutions;
        public:
            OptimizationCore(std::string config_filename, int _verb);
            ~OptimizationCore();
    };
}

#endif

// logger::Logger

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
        std::cout << color << "["<<     std::setw(2) << std::setfill('0') << ltm->tm_mday <<"/"<< 
                                        std::setw(2) << std::setfill('0') << ltm->tm_mon <<"/"<< 
                                        ltm->tm_year+1900 <<" "<< 
                                        std::setw(2) << std::setfill('0') << ltm->tm_hour <<":"<< 
                                        std::setw(2) << std::setfill('0') << ltm->tm_min <<":"<< 
                                        std::setw(2) << std::setfill('0') << ltm->tm_sec <<" "<< 
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

// data::Variable

data::Variable::Variable(std::string _name, double _value) {
    name= _name;
    value = _value;
}

data::Variable::Variable() {}

data::Variable::~Variable() {}

// data::VarVector

data::VarVector::VarVector() {}

data::VarVector::~VarVector() {}

void data::VarVector::add_var(Variable _var) {
    vector.push_back(_var);
}

// data::ExpressionTree

data::ExpressionTree::ExpressionTree(std::string _expression) {}

data::ExpressionTree::ExpressionTree() {}

data::ExpressionTree::~ExpressionTree() {}

// data::Constraint

data::Constraint::Constraint(std::string _expression) {
    expression = ExpressionTree(_expression);
}

data::Constraint::Constraint() {}

data::Constraint::~Constraint() {}

// data::Objective

data::Objective::Objective(bool _max, std::string _expression) {
    max = _max;
    expression = ExpressionTree(_expression);
}

data::Objective::Objective() {}

data::Objective::~Objective() {}

// structures::CLI

structures::CLI::CLI(int argc, char** argv) {

    if (argc < 3) {
        logger.error("Too few arguments");
        return;
    } else if (argc > 5) {
        logger.warn("Too many arguments", 0);
    }

    bool config_set = false;
    for (auto i = 1; i < argc; i++) {
        if ((std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") && i < argc - 1){
            try {
                logger.set_verb(std::stoi(argv[i+1]));
                logger.info("Verbosity level changed", 2);
            } catch (...) {
                logger.warn("Could not read verbosity. Setting default value: 2", 2);
                logger.set_verb(2);
            }
            i++;
        }
    }

    for (auto i = 1; i < argc; i++) {
        if ((std::string(argv[i]) == "-c" || std::string(argv[i]) == "--config") && i < argc - 1) {
            config_filename = argv[i+1];
            if (!config_set) {
                logger.info("Configuration filename set", 2);
                config_set = true;
            } else {
                logger.warn("Configuration filename overwrited", 2);
            }
            i++;
        } else if ((std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") && i < argc - 1){
            i++;
        } else {
            logger.warn("Unknown flag ignored", 2);
        }
    }
}

int structures::CLI::verbosity() {
    return logger.get_verb();
}

bool structures::CLI::loaded() {
    if (FILE *file = fopen(config_filename.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::string structures::CLI::cfg_file() {
    return config_filename;
}

// structures::OptimizationConfig

structures::OptimizationConfig::OptimizationConfig(std::string config_filename, int _verb) {
    logger.set_verb(_verb);
    auto sections = 0;
    data::VarVector initial_data;

    // sections = {ctrl, cons, resp, objc, conf}
    std::vector<bool> section_in = {false, false, false, false, false, false};

    std::fstream config_file;
    config_file.open(config_filename,std::ios::in);

    if (config_file.is_open()){
        std::string line;
        while(getline(config_file, line)){
            if (line.substr(0, 1) == "#") {
                logger.warn("Ignoring comment line in config file", 4);
            } else if (line.substr(0, 1) == "*"){
                if (line.find("*Controls") == 0) {
                    logger.info("Found control variables section", 4);
                    section_in = {false, false, false, false, false, false};
                    section_in[0] = true;
                } else if (line.find("*Constraints") == 0) {
                    logger.info("Found constraints section", 4);
                    section_in = {false, false, false, false, false};
                    section_in[1] = true;
                } else if (line.find("*Responses") == 0) {
                    logger.info("Found response variables section", 4);
                    section_in = {false, false, false, false, false, false};
                    section_in[2] = true;
                } else if (line.find("*Objectives") == 0) {
                    logger.info("Found objectives section", 4);
                    section_in = {false, false, false, false, false, false};
                    section_in[3] = true;
                } else if (line.find("*Initial_conditions") == 0) {
                    logger.info("Found initial conditions section", 4);
                    section_in = {false, false, false, false, false, false};
                    section_in[4] = true;
                } else if (line.find("*Configurations") == 0) {
                    logger.info("Found general configurations section", 4);
                    section_in = {false, false, false, false, false, false};
                    section_in[5] = true;
                }
            } else if (line.length() == 0){
                logger.warn("Ignoring empty line in config file", 4);
            } else {
                if (section_in[0]) {
                    controls.push_back(line);
                    logger.info("Control variable added: "+line, 3);
                } else if (section_in[1]) {
                    constraints.push_back(data::Constraint(line));
                    logger.info("Added new constraint: "+line, 3);
                } else if (section_in[2]) {
                    responses.push_back(line);
                    logger.info("New report response added: "+line, 3);
                } else if (section_in[3]) {
                    if (line.find("MIN") == 0) {
                        objectives.push_back(data::Objective(false, line));
                        logger.info("Added new MIN type objective: "+line, 3);
                    } else if (line.find("MAX") == 0) {
                        objectives.push_back(data::Objective(true, line));
                        logger.info("Added new MAX type objective: "+line, 3);
                    } else {
                        logger.error("Couldn't process one objective, make sure it begins with MIN or MAX: "+line);
                    }
                } else if (section_in[4]) {
                    auto var_info = split(line, "=");
                    if (var_info.size() != 2) {
                        logger.error("Couldn't parse initial condition properly. it should be VAR=VALUE: " + line);
                    } else {
                        try {
                            if (std::find(controls.begin(), controls.end(), var_info[0]) != controls.end()) {
                                auto var = data::Variable(var_info[0], std::stod(var_info[1]));
                                initial_data.add_var(var);
                                logger.info("Initial value of " + var_info[0] + " set to " + var_info[1], 3);
                            } else {
                                logger.error("Couldn't add initial condition for " + var_info[0] + " because this control variable was not declared.");
                            }
                        } catch (...) {
                            logger.error("Couldn't add initial condition for " + var_info[0] + " because " + var_info[1] + " couldn't be parsed into a number.");
                        }
                    }
                } else if (section_in[5]) {
                    logger.info("Parsing general configurations", 4);
                }
            }
        }
        config_file.close();
   }

}

structures::OptimizationConfig::OptimizationConfig() {}

structures::OptimizationConfig::~OptimizationConfig() {}

// structures::OptimizationUnit

structures::OptimizationUnit::OptimizationUnit(OptimizationUnit& parent1, OptimizationUnit& parent2, std::string ID, int verb) {
    logger.set_entity(ID);
    logger.set_verb(verb);
}

structures::OptimizationUnit::OptimizationUnit(data::VarVector _controls, std::string ID, int verb) {
    logger.set_entity(ID);
    logger.set_verb(verb);
    controls = _controls;
}

structures::OptimizationUnit::OptimizationUnit() {}

structures::OptimizationUnit::~OptimizationUnit() {}

// structures::OptimizationCore

structures::OptimizationCore::OptimizationCore(std::string config_filename, int _verb) {
    logger.set_verb(_verb);
    configs = OptimizationConfig(config_filename, _verb);
}

structures::OptimizationCore::~OptimizationCore() {}

// structures::Solution

structures::Solution::Solution(OptimizationUnit _unit) {
    unit = _unit;
}

structures::Solution::Solution() {}

structures::Solution::~Solution() {}