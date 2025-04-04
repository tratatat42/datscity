#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace argparse {

class ArgumentParser {
private:
    std::map<std::string, std::string> args_;
    std::vector<std::string> positional_;

public:
    void parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --option=value or --option value
            if (arg.substr(0, 2) == "--") {
                std::string name = arg.substr(2);
                std::string value;
                
                // Check for --option=value format
                size_t pos = name.find('=');
                if (pos != std::string::npos) {
                    value = name.substr(pos + 1);
                    name = name.substr(0, pos);
                } 
                // Check for --option value format
                else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                }
                // Boolean flag
                else {
                    value = "true";
                }
                
                args_[name] = value;
            }
            // Handle -o value 
            else if (arg[0] == '-' && arg.size() > 1) {
                std::string name = arg.substr(1);
                std::string value;
                
                // Check for -o=value format
                size_t pos = name.find('=');
                if (pos != std::string::npos) {
                    value = name.substr(pos + 1);
                    name = name.substr(0, pos);
                }
                // Check for -o value format
                else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                }
                // Boolean flag
                else {
                    value = "true";
                }
                
                args_[name] = value;
            }
            // Positional argument
            else {
                positional_.push_back(arg);
            }
        }
    }
    
    bool has(const std::string& name) const {
        return args_.find(name) != args_.end();
    }
    
    std::string get(const std::string& name, const std::string& default_value = "") const {
        auto it = args_.find(name);
        return it != args_.end() ? it->second : default_value;
    }
    
    int get_int(const std::string& name, int default_value = 0) const {
        auto it = args_.find(name);
        if (it != args_.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }
    
    bool get_bool(const std::string& name, bool default_value = false) const {
        auto it = args_.find(name);
        if (it != args_.end()) {
            std::string val = it->second;
            if (val == "true" || val == "1" || val == "yes") {
                return true;
            } else if (val == "false" || val == "0" || val == "no") {
                return false;
            }
        }
        return default_value;
    }
    
    const std::vector<std::string>& positional() const {
        return positional_;
    }
};

} // namespace argparse

#endif // ARGPARSE_HPP
