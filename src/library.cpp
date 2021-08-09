#include "library.hpp"

// helper functions

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

bool check_file_existance(std::string filename) {
    if (FILE *file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

const char* cmd(std::string cmd) {
    return cmd.c_str();
}


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

data::Variable::Variable() {
    name = "NO_NAME";
    value = HUGE_VAL;
}

data::Variable::~Variable() {}

std::string data::Variable::get_name() {
    return name;
}

double data::Variable::get_value() {
    return value;
}

void data::Variable::set_value(double& _value) {
    value = _value;
}

// data::VarVector

data::VarVector::VarVector() {}

data::VarVector::~VarVector() {}

double data::VarVector::var_value(std::string name) {
    auto var_pos = var_exist(name);
    if (var_pos >= 0) {
        return vector[var_pos].get_value();
    } else {
        return HUGE_VAL;
    }
}

void data::VarVector::add_var(Variable _var) {
    vector.push_back(_var);
}

int data::VarVector::var_exist(std::string name) {
    for(auto i = 0; i < vector.size();i++) {
        if (vector[i].get_name() == name) {
            return i;
        }
    }
    return -1;
}

bool data::VarVector::set_var(std::string _name, double _value) {
    auto var_pos = var_exist(_name);
    if (var_pos >= -1) {
        vector[var_pos].set_value(_value);
        return true;
    } else {
        return false;
    }
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

structures::OptimizationConfig::OptimizationConfig(std::string _config_filename, int _verb) {
    logger.set_verb(_verb);
    auto sections = 0;
    data::VarVector initial_data;

    // sections = {ctrl, cons, resp, objc, conf}
    std::vector<bool> section_in = {false, false, false, false, false, false};

    std::fstream config_file;
    config_file.open(_config_filename,std::ios::in);

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
                    auto expr = line.substr(4);
                    if (line.find("MIN") == 0) {
                        objectives.push_back(data::Objective(false, expr));
                        logger.info("Added new MIN type objective: " + expr, 3);
                    } else if (line.find("MAX") == 0) {
                        objectives.push_back(data::Objective(true, expr));
                        logger.info("Added new MAX type objective: " + expr, 3);
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
                    auto config_info = split(line, "=");
                    if (config_info.size() != 2) {
                        logger.error("Couldn't parse a configuration properly. it should be VAR=VALUE: " + line);
                    } else {
                        auto success_config = false;
                        try {
                            if (config_info[0] == "MODEL_PATH") {
                                model_path = config_info[1];
                                success_config = true;
                            } else if (config_info[0] == "PROJECT_NAME") {
                                project = config_info[1];
                                success_config = true;
                            } else if (config_info[0] == "SIMULATOR_PATH") {
                                simulator_path = config_info[1];
                                success_config = true;
                            } else if (config_info[0] == "REPLICATIONS") {
                                replication_amount = std::stoi(config_info[1]);
                                success_config = true;
                            } else if (config_info[0] == "PRECISION") {
                                precision = std::stoi(config_info[1]);
                                success_config = true;
                            } else if (config_info[0] == "POPULATION") {
                                generation_size = std::stoi(config_info[1]);
                                success_config = true;
                            } else if (config_info[0] == "SOLUTIONS") {
                                solutions = std::stoi(config_info[1]);
                                success_config = true;
                            } else if (config_info[0] == "MIN_GEN") {
                                min_gen = std::stoi(config_info[1]);
                                success_config = true;
                            } else if (config_info[0] == "PERSISTENCE") {
                                max_persist = std::stoi(config_info[1]);
                                success_config = true;
                            } else {
                                logger.error("Couldn't add initial condition for " + config_info[0] + " because this control variable was not declared.");
                            }

                            if (success_config) {
                                logger.info("Set " + config_info[0] + " to " + config_info[1], 3);
                            }
                        } catch (...) {
                            logger.error("Couldn't set configuration for " + config_info[0] + " because " + config_info[1] + " couldn't be parsed into a number.");
                        }
                    }
                }
            }
        }
        config_file.close();
    }

    config_filename = _config_filename;
    logger.info(config_filename + " fully parsed", 3);
    valid = check_integrity();
}

structures::OptimizationConfig::OptimizationConfig() {}

structures::OptimizationConfig::~OptimizationConfig() {}

bool structures::OptimizationConfig::check_integrity() {
    logger.info("Checking for integrity of inputs", 4);

    std::system(cmd("mkdir -p projects"));
    std::system(cmd("mkdir -p simulator"));
    std::system(cmd("mkdir -p projects/" + project));

    if (!check_file_existance(model_path)) {
        logger.error("Model path invalid");
        return false;
    } else if (!check_file_existance(simulator_path)) {
        logger.error("Simulator path invalid");
        return false;
    } else {
        logger.info("Copying simulator binary", 4);
        std::system(cmd("cp "+ simulator_path + " simulator/GenESys"));
        simulator_path = "simulator/GenESys";

        logger.info("Copying input model file", 4);
        std::system(cmd("cp "+ model_path + " projects/" + project + "/base_model.txt"));
        model_path = "projects/" + project + "/base_model.txt";

        logger.info("Copying configurations file", 4);
        std::system(cmd("cp " + config_filename + " projects/" + project + "/config.txt"));

        for(auto i = 0; i < controls.size(); i++) {
            if (initial_unit.get_controls().var_value(controls[i]) == HUGE_VAL) {
                
            }
        }

    }
    return true;
}

bool structures::OptimizationConfig::valid_configs() {
    return valid;
}

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

data::VarVector structures::OptimizationUnit::get_controls() {
    return controls;
}

// structures::OptimizationCore

structures::OptimizationCore::OptimizationCore(std::string config_filename, int _verb) {
    logger.set_verb(_verb);
    configs = OptimizationConfig(config_filename, _verb);
}

structures::OptimizationCore::~OptimizationCore() {}

bool structures::OptimizationCore::valid_configs() {
    return configs.valid_configs();
}

// structures::Solution

structures::Solution::Solution(OptimizationUnit _unit) {
    unit = _unit;
}

structures::Solution::Solution() {}

structures::Solution::~Solution() {}