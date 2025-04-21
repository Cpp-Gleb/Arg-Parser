#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "BaseArgument.h"
#include "Argument.h"

namespace ArgumentParser {

    class ArgParser {
    public:
        explicit ArgParser(const std::string& program_name);

        template <typename T>
        std::shared_ptr<Argument<T>> AddArgument(const std::string& name);

        template <typename T>
        std::shared_ptr<Argument<T>> AddArgument(char short_name, const std::string& long_name);

        template <typename T>
        std::shared_ptr<Argument<T>> AddArgument(const std::string& name, const std::string& description);

        template <typename T>
        std::shared_ptr<Argument<T>> AddArgument(char short_name, const std::string& long_name, const std::string& description);


        std::shared_ptr<Argument<bool>> AddFlag(const std::string& name, const std::string& description);
        std::shared_ptr<Argument<bool>> AddFlag(char short_name, const std::string& long_name, const std::string& description);
        std::shared_ptr<Argument<bool>> AddFlag(char short_name, const std::string& long_name);
        std::shared_ptr<Argument<bool>> AddFlag(const std::string& name);

        void AddHelp(char short_name, const std::string& long_name, const std::string& description);

        bool Parse(int argc, char** argv);
        bool Parse(const std::vector<std::string>& args);
        bool Help() const;
        bool GetFlag(const std::string& name) const;

        std::string HelpDescription() const;

        template <typename T>
        T GetValue(const std::string& name) const;

        template <typename T>
        T GetValue(const std::string& name, size_t index) const;

    private:
        std::string program_name_;
        bool help_flag_ = false;
        std::vector<std::shared_ptr<BaseArgument>> arguments_;
        std::unordered_map<std::string, std::shared_ptr<BaseArgument>> arguments_map_;
        std::unordered_map<char, std::shared_ptr<BaseArgument>> short_arguments_map_;

        void RegisterArgument(const std::shared_ptr<BaseArgument>& arg);
    };

    inline std::shared_ptr<Argument<bool>> ArgParser::AddFlag(const std::string& name) {
        return AddFlag('\0', name, "");
    }

    inline std::shared_ptr<Argument<bool>> ArgParser::AddFlag(const std::string& name, const std::string& description) {
        return AddFlag('\0', name, description);
    }

    inline std::shared_ptr<Argument<bool>> ArgParser::AddFlag(char short_name, const std::string& long_name) {
        return AddFlag(short_name, long_name, "");
    }

    inline std::shared_ptr<Argument<bool>> ArgParser::AddFlag(char short_name, const std::string& long_name, const std::string& description) {
        auto arg = std::make_shared<Argument<bool>>(short_name, long_name, description);
        RegisterArgument(arg);
        return arg;
    }

    template <typename T>
    std::shared_ptr<Argument<T>> ArgParser::AddArgument(const std::string& name) {
        auto arg = std::make_shared<Argument<T>>(name);
        RegisterArgument(arg);
        return arg;
    }

    template <typename T>
    std::shared_ptr<Argument<T>> ArgParser::AddArgument(char short_name, const std::string& long_name) {
        auto arg = std::make_shared<Argument<T>>(short_name, long_name);
        RegisterArgument(arg);
        return arg;
    }

    template <typename T>
    std::shared_ptr<Argument<T>> ArgParser::AddArgument(const std::string& name, const std::string& description) {
        auto arg = std::make_shared<Argument<T>>(name);
        arg->Description(description);
        RegisterArgument(arg);
        return arg;
    }

    template <typename T>
    std::shared_ptr<Argument<T>> ArgParser::AddArgument(char short_name, const std::string& long_name, const std::string& description) {
        auto arg = std::make_shared<Argument<T>>(short_name, long_name);
        arg->Description(description);
        RegisterArgument(arg);
        return arg;
    }

    template <typename T>
    T ArgParser::GetValue(const std::string& name) const {
        auto it = arguments_map_.find(name);
        if (it != arguments_map_.end()) {
            auto arg = std::dynamic_pointer_cast<Argument<T>>(it->second);
            if (arg) {
                return arg->GetValue();
            }
        }
        return T();
    }

    template <typename T>
    T ArgParser::GetValue(const std::string& name, size_t index) const {
        auto it = arguments_map_.find(name);
        if (it != arguments_map_.end()) {
            auto arg = std::dynamic_pointer_cast<Argument<T>>(it->second);
            if (arg) {
                return arg->GetValue(index);
            }
        }
        return T();
    }

}