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
        if (ini >= text.length()) {
            return result;
        }
    }

    result.push_back(text.substr(ini));
    return result;
}

std::vector<std::string> split_string(std::string text, char delimiter){
	
    std::vector<std::string> v;
	std::string temp = "";
	for(auto i = 0; i < text.length(); i++){
		
		if(text[i]==delimiter){
			v.push_back(temp);
			temp = "";
		}
		else{
			temp.push_back(text[i]);
		}
		
	}
	v.push_back(temp);
	
    return v;
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

int count_digit(int number) {
   return int(std::log10(number) + 1);
}

std::string n_digit(std::string id, int n) {
    while(id.length() < n) {
        id = "0" + id;
    }

    return id;
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

void data::Variable::set_value(double _value) {
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

// data::Expression

data::Expression::Expression(std::string _expression) {
    expression = _expression;
}

data::Expression::Expression() {}

data::Expression::~Expression() {}

std::string data::Expression::subst(VarVector& vars) {
    auto temp = expression;

    for(auto i = 0; i < vars.vector.size(); i++) {
        auto name = vars.vector[i].get_name();
        auto value = std::to_string(vars.vector[i].get_value());
        auto pos = temp.find(name);
        while (pos != std::string::npos) {
            temp.replace(pos, name.size(), value);
            pos = temp.find(name);
        }
    }
}

double data::Expression::eval(VarVector& vars, int p) {
    auto precision = pow(10, -p);
    while (expression[0] == '(') {
        auto no_border = Expression(expression.substr(1, expression.size()-2));
        if (no_border.good_depth()) {
            expression = expression.substr(1, expression.size()-2);
        } else {
            break;
        }
    }

    int op = -1;
    for(auto i = 0; i < comparators.size(); i++) {
        if (expression.find(comparators[i]) != std::string::npos) {
            op = i;
            break;
        }
    }
    if (op >= 0) {
        auto exprs = split(comparators[op]);
        switch (op) {
            case 0:
                if (fabs(exprs[0].eval(vars, p) - exprs[1].eval(vars, p)) > precision) {return 1;} 
                else {return 0;}
            case 1:
                if (exprs[0].eval(vars, p) - exprs[1].eval(vars, p) >= precision) {return 1;} 
                else {return 0;}
            case 2:
                if (exprs[0].eval(vars, p) - exprs[1].eval(vars, p) <= precision) {return 1;} 
                else {return 0;}
            case 3:
                if (exprs[0].eval(vars, p) - exprs[1].eval(vars, p) > precision) {return 1;} 
                else {return 0;}
            case 4:
                if (exprs[0].eval(vars, p) - exprs[1].eval(vars, p) < precision) {return 1;} 
                else {return 0;}
            case 5:
                if (fabs(exprs[0].eval(vars, p) - exprs[1].eval(vars, p)) < precision) {return 1;} 
                else {return 0;}
        }
    } else {
        for(auto i = 0; i < operators.size(); i++) {
            int ini = 0;
            while (expression.find(operators[i]) != std::string::npos) {
                ini = expression.find(operators[i], ini);
                if (ini == std::string::npos) {
                    break;
                }
                auto exprs = split(ini);
                ini += 1;
                if (exprs[0].good_depth() && exprs[1].good_depth()) {
                    switch (i)
                    {
                    case 0:
                        return exprs[0].eval(vars, p) + exprs[1].eval(vars, p);
                    case 1:
                        return exprs[0].eval(vars, p) - exprs[1].eval(vars, p);
                    case 2:
                        return exprs[0].eval(vars, p) * exprs[1].eval(vars, p);
                    case 3:
                        return exprs[0].eval(vars, p) / exprs[1].eval(vars, p);
                    case 4:
                        return pow(exprs[0].eval(vars, p), exprs[1].eval(vars, p));
                    case 5:
                        return log(exprs[1].eval(vars, p)) /  log(exprs[0].eval(vars, p));
                    }
                }
            }
        }

        if (expression == "PI") {
            return M_PI;
        } else if (expression == "E") {
            return M_E;
        } else if (vars.var_value(expression) != HUGE_VAL) {
            return vars.var_value(expression);
        } else {
            try {
                auto temp = std::stod(expression);
                return temp;
            } catch (...) {
                std::cout << "ERROR IN EXPRESSION: " << expression << std::endl;
                return 0;
            }
        }
    }
    std::cout << "EXPRESSION NOT PARSED: " << expression << std::endl;
    return 0;
}

std::vector<data::Expression> data::Expression::split(std::string op) {
    auto pos = expression.find(op);
    auto expr1 = expression.substr(0, pos);
    auto expr2 = expression.substr(pos+op.size());

    return std::vector<data::Expression>{data::Expression(expr1), data::Expression(expr2)};
}

std::vector<data::Expression> data::Expression::split(int pos) {
    auto expr1 = expression.substr(0, pos);
    auto expr2 = expression.substr(pos+1);

    return std::vector<data::Expression>{data::Expression(expr1), data::Expression(expr2)};
}

bool data::Expression::good_depth() {
    auto counter = 0;
    for(auto i = 0; i < expression.size(); i++) {
        if (expression[i] == '(') {
            counter += 1;
        } else if (expression[i] == ')') {
            counter -= 1;
        }

        if (counter < 0) {
            return false;
        }
    }

    if (counter > 0 ) {
        return false;
    } else {
        return true;
    }
}

std::string data::Expression::get_expr() {
    return expression;
}

void data::Expression::set_expr(std::string expr) {
    expression = expr;
}

// data::Constraint

data::Constraint::Constraint(std::string _expression) {
    expression = Expression(_expression);
}

data::Constraint::Constraint() {}

data::Constraint::~Constraint() {}

bool data::Constraint::eval(VarVector& controls, int p) {
    return expression.eval(controls, p);
}

// data::Objective

data::Objective::Objective(bool _max, std::string _expression) {
    max = _max;
    expression = Expression(_expression);
}

data::Objective::Objective() {}

data::Objective::~Objective() {}

double data::Objective::eval(VarVector& responses, int p) {
    return expression.eval(responses, p);
}

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
                logger.warn("Ignoring comment line in config file", 5);
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
                logger.warn("Ignoring empty line in config file", 5);
            } else {
                if (section_in[0]) {
                    controls.push_back(line.substr(4));
                    controls_types.push_back(line.substr(0,4));
                    logger.info("Control variable of type " + line.substr(0,3) + " added: "+ line.substr(4), 3);
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

    initial_unit = OptimizationUnit(initial_data, "TEST", logger.get_verb());
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

        data::VarVector init_controls;
        for(auto i = 0; i < controls.size(); i++) {
            if (initial_unit.get_controls().var_value(controls[i]) == HUGE_VAL) {
                logger.warn("No initial value set to " + controls[i] + " control variable: set to 0", 3);
                init_controls.add_var(data::Variable(controls[i], 0));
            }
        }

    }
    return true;
}

bool structures::OptimizationConfig::valid_configs() {
    return valid;
}

std::string structures::OptimizationConfig::get_base_run_cmd() {
    auto cmd = "./" + simulator_path + " ";
    return cmd;
}

std::string structures::OptimizationConfig::get_project_path() {
    return "projects/" + project;
}

// structures::OptimizationUnit

structures::OptimizationUnit::OptimizationUnit(OptimizationUnit const& parent1, OptimizationUnit const& parent2, std::string ID, int verb) {
    logger.set_entity("OPT_UNIT_" + ID);
    logger.set_verb(verb);
}

structures::OptimizationUnit::OptimizationUnit(data::VarVector const& _controls, std::string ID, int verb) {
    logger.set_entity("OPT_UNIT_" + ID);
    logger.set_verb(verb);
    controls = _controls;
}

structures::OptimizationUnit::OptimizationUnit() {}

structures::OptimizationUnit::~OptimizationUnit() {}

data::VarVector structures::OptimizationUnit::get_controls() {
    return controls;
}

bool structures::OptimizationUnit::set_var(std::string _name, double _value) {
    auto var_pos = controls.var_exist(_name); 
    if (var_pos >= 0) {
        controls.vector[var_pos].set_value(_value);
        return true;
    } else {
        logger.warn("set_var method couldn't find " + _name + "variable", 4);
        return false;
    }
}

bool structures::OptimizationUnit::run(std::string base_cmd, std::string input, std::string output, std::vector<std::string> responses) {
    logger.info("Running unit simulation", 4);
    std::system(cmd(base_cmd + input + " > " + output));

    auto result = check_file_existance(output);
    
    if (result) {
        logger.info("Simulation done!", 3);
        parse_responses(output, responses);
        // TODO
        return true;
    } else {
        logger.warn("Error runing simulation!", 3);
        return false;
    }
}

std::string structures::OptimizationUnit::get_id() {
    return ID;
}

bool structures::OptimizationUnit::parse_responses(std::string report_filename, std::vector<std::string> response_labels) {
    if (!check_file_existance(report_filename)) {
        logger.error("Output file not found!");
        return false;
    }

    std::fstream report;
    report.open(report_filename,std::ios::in);
    bool conclusion_section = false;
    bool all_responses = false;

    logger.info("Parsing report...", 4);
    if (report.is_open()){
        std::string line;
        while(getline(report, line)){
            if (line.find("Begin of Report for Simulation") != std::string::npos) {
                logger.info("Found report conclusion section", 4);
                conclusion_section = true;
            } else if (conclusion_section) {
                auto response_line = false;
                std::string response_label = "None";

                for(auto i = 0; i < response_labels.size(); i++) {
                    if (line.find(response_labels[i]) != std::string::npos) {
                        response_line = true;
                        response_label = response_labels[i];
                        break;
                    }
                }
                
                if (response_line) {
                    auto spl_line = split_string(line, ' ');
                    // spl_line[28] = average
                    try {
                        auto var = data::Variable(response_label, std::stod(spl_line[28]));
                        responses.add_var(var);
                        logger.info("Saving response for " + response_label + ": " + spl_line[28], 4);
                        if (responses.vector.size() == response_labels.size()) {
                            all_responses = true;
                        }
                    } catch (...) {
                        logger.error("Couldn't parse " + response_label + " value: " + spl_line[28]);
                        return false;
                    }
                }
            }
        }
        report.close();
    }

    if (!all_responses) {
        logger.error("Missing some responses in the reports");
        for (auto i = 0; i < response_labels.size(); i++) {
            if (!responses.var_exist(response_labels[i])) {
                logger.error("Missing: " + response_labels[i]);
            }
        }
        return false;
    }

    return true;
}

// structures::OptimizationCore

structures::OptimizationCore::OptimizationCore(std::string config_filename, int _verb) {
    logger.set_verb(_verb);
    configs = OptimizationConfig(config_filename, _verb);

    base_run_cmd = configs.get_base_run_cmd();
    if (run_test_unit()) {
        logger.info("Test simulation: OK", 3);
    } else {
        logger.error("Error trying to run test model");
        configs.valid = false;
    }
}

structures::OptimizationCore::~OptimizationCore() {}

bool structures::OptimizationCore::valid_configs() {
    return configs.valid_configs();
}

bool structures::OptimizationCore::run_test_unit() {
    logger.info("Running test simulation...", 4);
    return run_unit(configs.initial_unit);
}

void structures::OptimizationCore::setup_unit(OptimizationUnit unit) {
    logger.info("Setuping unit "+ unit.get_id() + "...", 4);
    std::system(cmd("mkdir -p "+ save_folder()));
    //std::system(cmd("cp " + configs.model_path + " " + model_save_path(unit.get_id())));

    std::ofstream file_model;
    file_model.open((model_save_path(unit.get_id()))); 
    std::fstream file_temp;
    file_temp.open(configs.model_path,std::ios::in);
    
    if (file_temp.is_open()) {
        std::string line;
        while(getline(file_temp, line)) {
            logger.info("Cheking line: " + line, 4);
            if(line.find("Resource") != std::string::npos){
                for(auto i = 0; i < configs.controls.size(); i++) {
                    auto ctrl_name = configs.controls[i];
                    auto pos = line.find(ctrl_name);
                    if (pos != std::string::npos) {
                        pos += ctrl_name.size() + 1;
                        auto prev_line = line.substr(0, pos);

                        std::string value;
                        if (configs.controls_types[i].find("INT") != std::string::npos) {
                            int temp = ceil(unit.get_controls().var_value(ctrl_name));
                            value = std::to_string(temp);
                        } else {
                            value = std::to_string(unit.get_controls().var_value(ctrl_name));
                        }

                        auto new_line = prev_line + " capacity=" + value;
                        logger.info("Model new line: " + new_line, 4);
                        file_model << new_line << std::endl;
                    }
                }
            } else {
                file_model << line << std::endl;
            }
        }
    }
    file_temp.close();
    file_model.close();
}

bool structures::OptimizationCore::run_unit(OptimizationUnit& unit) {
    setup_unit(unit);

    std::system(cmd(base_run_cmd + model_save_path(unit.get_id()) + " > " + output_save_path(unit.get_id())));

    auto result = unit.run(base_run_cmd, model_save_path(unit.get_id()), output_save_path(unit.get_id()), configs.responses);

    return result;
}

std::string structures::OptimizationCore::save_folder() {
    return configs.get_project_path() + "/" + "gen_" + n_digit(std::to_string(gen), 3);
}

std::string structures::OptimizationCore::model_name(std::string id) {
    return n_digit(id, count_digit(configs.generation_size)) + "_model.txt";
}

std:: string structures::OptimizationCore::output_name(std::string id) {
    return n_digit(id, count_digit(configs.generation_size)) + "_ouput.txt";
}

std::string structures::OptimizationCore::model_save_path(std::string id) {
    return save_folder() + "/" + model_name(id);
}

std::string structures::OptimizationCore::output_save_path(std::string id) {
    return save_folder() + "/" + output_name(id);
}

int structures::OptimizationCore::get_gen() {return 0;}

void structures::OptimizationCore::new_gen() {}

bool structures::OptimizationCore::run_gen() {return false;}

void structures::OptimizationCore::check_gen_results() {}

bool structures::OptimizationCore::check_optimization_end() {return false;}

void structures::OptimizationCore::show_results() {}

// structures::Solution

structures::Solution::Solution(OptimizationUnit _unit) {
    unit = _unit;
}

structures::Solution::Solution() {}

structures::Solution::~Solution() {}