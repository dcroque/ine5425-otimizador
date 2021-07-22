// Copyright [2021] <Matheus Dhanyel Cândido Roque>
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include<stdlib.h>
#include<string>
#include<vector>
#include<functional>

namespace structures {
    class Operation {
        private:
            double* opr_a;
            double* opr_b;
            std::function<double(double, double)> op;
    };

    class Control {
        private:
            std::string name;
            double value;
    };

    class Constraint {
        private:
            std::string name;
            std::vector<Operation> left_side;
            std::vector<Operation> right_side;
            std::vector<double> temporary_values;
            Operation comparation;
    };

    class Response {
        private:
            std::string name;
            double value;
    };

    class Objective {
        private:
            bool max;
            std::string variable;
            bool require;
    };

    class Solution {
        private:
            std::vector<Response> responses;
            bool all_require;
            bool all_objectives;
    };
}

#endif