// Copyright [2021] <Matheus Dhanyel Cândido Roque>
#ifndef LIBRARY_H
#define LIBRARY_H

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
            int var_exist(std::string name);
            bool set_var(std::string _name, double _value);
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
            data::VarVector get_controls();
            data::VarVector get_responses();

            void set_fit(double& _fit);

            double get_fit();
    };

    class OptimizationConfig {
        private:
            std::vector<std::string> controls;
            std::vector<std::string> responses;
            std::vector<data::Constraint> constraints;
            std::vector<data::Objective> objectives;
            std::string model_path;
            std::string project;
            std::string simulator_path;
            std::string config_filename;
            logger::Logger logger = logger::Logger("OPT_CONFIG_CORE", 2);
            OptimizationUnit initial_unit;

            int replication_amount = 30;
            int generation_size = 20;
            int precision = 5;
            int solutions = 10;
            int max_persist = 10;
            int min_gen = 10;
            bool valid;

            bool check_integrity();
        public:
            OptimizationConfig(std::string config_filename, int _verb);
            OptimizationConfig();
            ~OptimizationConfig();
            bool valid_configs();
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

            int persist = 0;
            int gen = 0;
        public:
            OptimizationCore(std::string config_filename, int _verb);
            ~OptimizationCore();
            bool valid_configs();
    };
}

#endif