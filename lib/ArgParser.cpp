#include "ArgParser.h"
#include <iostream>

namespace ArgumentParser {
    ArgParser::ArgParser(const std::string& program_name)
    : program_name_(program_name) {}

    bool ArgParser::Parse(int argc, char** argv) {
        std::vector<std::string> args(argv + 1, argv + argc);
        return Parse(args);
    }

    bool ArgParser::Parse(const std::vector<std::string>& args) {
        size_t i = 0;
        while (i < args.size()) {
            std::string arg = args[i];
            if (arg.substr(0, 2) == "--") {
                std::string name_value = arg.substr(2);
                size_t eq_pos = name_value.find('=');
                std::string name = name_value.substr(0, eq_pos);
                std::string value = eq_pos != std::string::npos ? name_value.substr(eq_pos + 1) : "";

                auto it = arguments_map_.find(name);
                if (it != arguments_map_.end()) {
                    auto argument = it->second;
                    if (std::dynamic_pointer_cast<Argument<bool>>(argument)) {
                        argument->ParseValue("");
                    } else {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[++i];
                            } else {
                                std::cerr << "Missing value for argument --" << name << std::endl;
                                return false;
                            }
                        }
                        if (!argument->ParseValue(value)) {
                            std::cerr << "Invalid value for argument --" << name << std::endl;
                            return false;
                        }
                    }
                } else {
                    std::cerr << "Unknown argument --" << name << std::endl;
                    return false;
                }
            }   else if (arg[0] == '-') {
                size_t arg_length = arg.length();
                size_t j = 1;
                while (j < arg_length) {
                    char short_name = arg[j];
                    auto it = short_arguments_map_.find(short_name);
                    if (it != short_arguments_map_.end()) {
                        auto argument = it->second;
                        if (std::dynamic_pointer_cast<Argument<bool>>(argument)) {
                            argument->ParseValue("");
                            ++j;
                        } else {
                            std::string value;
                            if (j + 1 < arg_length && arg[j + 1] == '=') {
                                value = arg.substr(j + 2);
                                j = arg_length;
                            } else if (j + 1 < arg_length) {
                                value = arg.substr(j + 1);
                                j = arg_length;
                            } else if (i + 1 < args.size()) {
                                value = args[++i];
                                ++j;
                            } else {
                                std::cerr << "Missing value for argument -" << short_name << std::endl;
                                return false;
                            }
                            if (!argument->ParseValue(value)) {
                                std::cerr << "Invalid value for argument -" << short_name << std::endl;
                                return false;
                            }
                            break;
                        }
                    } else {
                        std::cerr << "Unknown argument -" << short_name << std::endl;
                        return false;
                    }
                }
            } else {
                bool positional_handled = false;
                for (auto& argument : arguments_) {
                    if (argument->IsPositional()) {
                        if (!argument->ParseValue(arg)) {
                            std::cerr << "Invalid positional argument " << arg << std::endl;
                            return false;
                        }
                        positional_handled = true;
                        break;
                    }
                }
                if (!positional_handled) {
                    std::cerr << "Unexpected positional argument: " << arg << std::endl;
                    return false;
                }
            }
            ++i;
        }

        if (help_flag_) {
            return true;
        }

        for (auto& argument : arguments_) {
            if (!argument->IsMultiValue() && !argument->HasValue()) {
                if (argument->IsRequired()) {
                    std::cerr << "Missing required argument --" << argument->GetName() << std::endl;
                    return false;
                }
                argument->SetDefault();
            }
            if (argument->IsMultiValue() && argument->GetMinCount() > 0) {
                if (argument->GetValuesCount() < argument->GetMinCount()) {
                    std::cerr << "Argument --" << argument->GetName() << " requires at least " << argument->GetMinCount() << " values" << std::endl;
                    return false;
                }
            }
        }

        return true;
    }

    bool ArgParser::Help() const {
        return help_flag_;
    }

    bool ArgParser::GetFlag(const std::string& name) const {
        auto it = arguments_map_.find(name);
        if (it != arguments_map_.end()) {
            auto arg = std::dynamic_pointer_cast<Argument<bool>>(it->second);
            if (arg) {
                return arg->GetValue();
            }
        }
        return false;
    }

    std::string ArgParser::HelpDescription() const {
        std::ostringstream oss;
        oss << program_name_ << "\n";
        for (const auto& arg : arguments_) {
            oss << arg->HelpInfo() << "\n";
        }
        return oss.str();
    }

    void ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
        auto help_arg = std::make_shared<Argument<bool>>(short_name, long_name, description);
        RegisterArgument(help_arg);
        help_arg->StoreValue(help_flag_);
    }

    void ArgParser::RegisterArgument(const std::shared_ptr<BaseArgument>& arg) {
        arguments_.push_back(arg);
        arguments_map_[arg->GetName()] = arg;
        if (arg->GetShortName() != '\0') {
            short_arguments_map_[arg->GetShortName()] = arg;
        }
    }

}