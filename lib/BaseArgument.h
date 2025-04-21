#pragma once

#include <string>
class BaseArgument {
public:
    virtual ~BaseArgument() = default;

    virtual bool ParseValue(const std::string& value) = 0;
    virtual void SetDefault() = 0;
    [[nodiscard]] virtual std::string HelpInfo() const = 0;

    [[nodiscard]] const std::string& GetName() const { return name_; }
    [[nodiscard]] char GetShortName() const { return short_name_; }
    [[nodiscard]] const std::string& GetDescription() const { return description_; }
    [[nodiscard]] bool IsPositional() const { return is_positional_; }
    [[nodiscard]] bool IsRequired() const { return is_required_; }
    [[nodiscard]] bool IsMultiValue() const { return is_multi_value_; }
    [[nodiscard]] size_t GetMinCount() const { return min_count_; }
    [[nodiscard]] bool HasValue() const { return has_value_; }
    [[nodiscard]] size_t GetValuesCount() const { return values_count_; }

protected:
    std::string name_;
    char short_name_ = '\0';
    std::string description_;
    bool is_positional_ = false;
    bool is_required_ = false;
    bool is_multi_value_ = false;
    size_t min_count_ = 0;

    bool has_value_ = false;
    size_t values_count_ = 0;
};