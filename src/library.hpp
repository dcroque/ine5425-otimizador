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
#include <sstream>
#include <iterator>
#include <pthread.h>
#include <random>

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

            void set_value(double _value);
    };

    class VarVector {
        public:
            std::vector<Variable> vector = {};

            VarVector();
            ~VarVector();

            double var_value(std::string name);
            void add_var(Variable _var);
            int var_exist(std::string name);
            bool set_var(std::string _name, double _value);
    };

    class Expression {
        private:
            std::string expression;
            std::vector<std::string> operators = {"+", "-", "*", "/", "^", "l"};
            std::vector<std::string> comparators = {"!=", ">=", "<=", ">", "<", "="};

            std::vector<Expression> split(std::string op);
            std::vector<Expression> split(int pos);
            std::string subst(VarVector& vars);
        public:
            Expression(std::string _expression);
            Expression();
            ~Expression();

            double eval(VarVector& vars, int p);
            bool good_depth();
            std::string get_expr();
            void set_expr(std::string expr);
    };

    class Constraint {
        private:
            Expression expression;
        public:
            Constraint(std::string _expression);
            Constraint();
            ~Constraint();

            bool eval(VarVector& controls, int p);
    };

    class Objective {
        private:
            bool max;
            Expression expression;
        public:
            Objective(bool _max, std::string _expression);
            Objective();
            ~Objective();

            double eval(VarVector& responses, int p);
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
            std::string ID; 
            logger::Logger logger = logger::Logger("OPT_UNIT", 2);
            data::VarVector controls;
            data::VarVector responses;
            double fit;

            bool parse_responses(std::string report_filename, std::vector<std::string> response_labels);
            std::string build_expr(std::string expr);
        public:
            OptimizationUnit(OptimizationUnit const& parent1, OptimizationUnit const& parent2, std::string ID, int verb);
            OptimizationUnit(data::VarVector const& _controls, std::string ID, int verb);
            OptimizationUnit();
            ~OptimizationUnit();
            void config_logger(std::string _entity, int _verb);

            data::VarVector get_controls();
            data::VarVector get_responses();
            double get_fit();
            std::string get_id();

            bool set_var(std::string _name, double _value);
            bool run(std::string base_cmd, std::string input, std::string output, std::vector<std::string> responses);
            void set_fit(data::Objective objc);
            void set_fit(std::vector<data::Objective> objc, double factor);
    };

    class OptimizationConfig {
        private:
            logger::Logger logger = logger::Logger("OPT_CONFIG_CORE", 2);

            bool check_integrity();
        public:
            OptimizationConfig(std::string config_filename, int _verb);
            OptimizationConfig();
            ~OptimizationConfig();

            std::vector<std::string> controls;
            std::vector<std::string> controls_types;
            std::vector<std::string> responses;
            std::vector<data::Constraint> constraints;
            std::vector<data::Objective> objectives;
            OptimizationUnit initial_unit;

            std::string model_path;
            std::string project;
            std::string simulator_path;
            std::string config_filename;

            int replication_amount = 30;
            int generation_size = 20;
            int precision = 5;
            int solutions = 10;
            int max_persist = 10;
            int min_gen = 10;
            bool valid;

            bool valid_configs();
            std::string get_base_run_cmd();
            std::string get_project_path();
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
            std::string base_run_cmd;
            std::vector<OptimizationUnit> generation;

            int persist = 0;
            int gen = 0;

            std::string save_folder();
            std::string model_name(std::string id);
            std::string output_name(std::string id);
            std::string model_save_path(std::string id);
            std::string output_save_path(std::string id);

            bool run_test_unit();
            void setup_unit(OptimizationUnit unit);
        public:
            OptimizationCore(std::string config_filename, int _verb);
            ~OptimizationCore();
            bool valid_configs();
            bool run_unit(OptimizationUnit& unit);
            int get_gen();
            void new_gen();
            bool run_gen();
            void check_gen_results();
            bool check_optimization_end();
            void show_results();
    };
}

#endif