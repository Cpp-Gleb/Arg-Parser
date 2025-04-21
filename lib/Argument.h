#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "BaseArgument.h"

namespace ArgumentParser {

    template <typename T>
    class Argument final : public BaseArgument {
    public:
        explicit Argument(const std::string& name);
        Argument(char short_name, const std::string& long_name);

        Argument& Default(const T& value);
        Argument& StoreValue(T& variable);
        Argument& StoreValues(std::vector<T>& variable);
        Argument& MultiValue(size_t min_count = 0);
        Argument& Positional();
        Argument& Description(const std::string& desc);
        Argument& Required();

        bool ParseValue(const std::string& value_str) override;
        void SetDefault() override;
        [[nodiscard]] std::string HelpInfo() const override;

        T GetValue() const;
        T GetValue(size_t index) const;

    private:
        T value_;
        std::vector<T> values_;
        T default_value_;
        bool has_default_ = false;
        T* external_variable_ = nullptr;
        std::vector<T>* external_values_ = nullptr;
    };

    template <>
    class Argument<bool> : public BaseArgument {
    public:
        explicit Argument(const std::string& name, const std::string& description = "");
        Argument(char short_name, const std::string& long_name, const std::string& description = "");

        Argument& Default(bool value);
        Argument& StoreValue(bool& variable);
        Argument& Description(const std::string& desc);
        Argument& Required();

        bool ParseValue(const std::string& value) override;
        void SetDefault() override;
        [[nodiscard]] std::string HelpInfo() const override;

        [[nodiscard]] bool GetValue() const;

    private:
        bool value_ = false;
        bool default_value_ = false;
        bool has_default_ = false;
        bool* external_variable_ = nullptr;
    };

    template <typename T>
    Argument<T>::Argument(const std::string& name) {
        name_ = name;
        has_value_ = false;
        values_count_ = 0;
    }

    template <typename T>
    Argument<T>::Argument(char short_name, const std::string& long_name) {
        short_name_ = short_name;
        name_ = long_name;
        has_value_ = false;
        values_count_ = 0;
    }

    template <typename T>
    Argument<T>& Argument<T>::Default(const T& value) {
        default_value_ = value;
        has_default_ = true;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::StoreValue(T& variable) {
        external_variable_ = &variable;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::StoreValues(std::vector<T>& variable) {
        external_values_ = &variable;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::MultiValue(size_t min_count) {
        is_multi_value_ = true;
        min_count_ = min_count;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::Positional() {
        is_positional_ = true;
        return *this;
    }

    template<typename T>
    Argument<T> &Argument<T>::Description(const std::string &desc) {
        description_ = desc;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::Required() {
        is_required_ = true;
        return *this;
    }

    template <typename T>
    bool Argument<T>::ParseValue(const std::string& value_str) {
        std::istringstream iss(value_str);
        T value;
        iss >> value;

        if (iss.fail()) {
            return false;
        }

        has_value_ = true;

        if (is_multi_value_) {
            values_.push_back(value);
            values_count_ = values_.size();
            if (external_values_) {
                external_values_->push_back(value);
            }
        } else {
            value_ = value;
            values_count_ = 1;
            if (external_variable_) {
                *external_variable_ = value;
            }
        }

        return true;
    }

    template <typename T>
    void Argument<T>::SetDefault() {
        if (has_default_) {
            value_ = default_value_;
            if (external_variable_) {
                *external_variable_ = default_value_;
            }
        }
    }

    template <typename T>
    std::string Argument<T>::HelpInfo() const {
        std::ostringstream oss;
        if (short_name_ != '\0') {
            oss << "-" << short_name_ << ", ";
        }
        oss << "--" << name_;
        if (!std::is_same<T, bool>::value) {
            oss << "=<" << typeid(T).name() << ">";
        }
        if (!description_.empty()) {
            oss << ", " << description_;
        }
        if (has_default_) {
            oss << " [default = " << default_value_ << "]";
        }
        if (is_multi_value_) {
            oss << " [repeated";
            if (min_count_ > 0) {
                oss << ", min args = " << min_count_;
            }
            oss << "]";
        }
        return oss.str();
    }

    template <typename T>
    T Argument<T>::GetValue() const {
        return value_;
    }

    template <typename T>
    T Argument<T>::GetValue(size_t index) const {
        if (index < values_.size()) {
            return values_[index];
        }
        return T();
    }

    inline Argument<bool>::Argument(const std::string& name, const std::string& description) {
        name_ = name;
        description_ = description;
        has_value_ = false;
        values_count_ = 0;
    }

    inline Argument<bool>::Argument(char short_name, const std::string& long_name, const std::string& description) {
        short_name_ = short_name;
        name_ = long_name;
        description_ = description;
        has_value_ = false;
        values_count_ = 0;
    }

    inline Argument<bool>& Argument<bool>::Default(bool value) {
        default_value_ = value;
        has_default_ = true;
        return *this;
    }

    inline Argument<bool>& Argument<bool>::StoreValue(bool& variable) {
        external_variable_ = &variable;
        return *this;
    }

    inline Argument<bool> &Argument<bool>::Description(const std::string &desc) {
        description_ = desc;
        return *this;
    }

    inline Argument<bool>& Argument<bool>::Required() {
        is_required_ = true;
        return *this;
    }

    inline bool Argument<bool>::ParseValue(const std::string& value) {
        value_ = true;
        has_value_ = true;
        values_count_ = 1;
        if (external_variable_) {
            *external_variable_ = true;
        }
        return true;
    }

    inline void Argument<bool>::SetDefault() {
        if (has_default_) {
            value_ = default_value_;
            if (external_variable_) {
                *external_variable_ = default_value_;
            }
        }
    }

    inline std::string Argument<bool>::HelpInfo() const {
        std::ostringstream oss;
        if (short_name_ != '\0') {
            oss << "-" << short_name_ << ", ";
        }
        oss << "--" << name_;
        if (!description_.empty()) {
            oss << ", " << description_;
        }
        if (has_default_) {
            oss << " [default = " << (default_value_ ? "true" : "false") << "]";
        }
        return oss.str();
    }

    inline bool Argument<bool>::GetValue() const {
        return value_;
    }
}