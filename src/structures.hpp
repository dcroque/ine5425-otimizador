// Copyright [2021] <Matheus Dhanyel Cândido Roque>
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include<stdlib.h>
#include<string>
#include<vector>

#include "binary_tree.hpp"

namespace structures {
    class OptimizationConfig {
        private:
            std::vector<std::string> controls;
            std::vector<std::string> responses;
            std::vector<Constraint> constraints;
            std::vector<Objective> objectives;
            std::string model_path;
            std::string output_path;
            std::string simulator_path;

            int generation_size = 20;
            int precision = 5;
            int max_persist = 10;
            int persist = 0;
            int gen = 0;
            int min_gen = 10;
        public:
            OptimizationConfig(std::string config_file);
    };

    class OptimizationUnit {
        private:
            VarVector controls;
            VarVector responses;
            double fit;
        public:
            OptimizationUnit(OptimizationUnit& parent1, OptimizationUnit& parent2);
            OptimizationUnit(VarVector& _controls);
            ~OptimizationUnit();
    };

    class OptimizationCore {
        private:
            std::vector<OptimizationUnit> units;
            OptimizationConfig configs;
            std::vector<Solution> solutions;
        public:
            OptimizationCore();
            ~OptimizationCore();
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

    class Solution {
        private:
            OptimizationUnit unit;
        public:
            Solution(OptimizationUnit _unit);
            ~Solution();
    };

    class ExpressionTree {
        private:
            std::string expression;
            BinaryTree<std::string> calc_tree;
        public:
            ExpressionTree(std::string expression);
            ~ExpressionTree();

    };

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
}

#endif