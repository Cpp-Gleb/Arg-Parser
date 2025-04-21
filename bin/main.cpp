#include <functional>
#include "lib/ArgParser.h"

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program");
    parser.AddArgument<int>("N")->MultiValue(1).Positional().StoreValues(values);
    parser.AddFlag("sum", "add args")->StoreValue(opt.sum);
    parser.AddFlag("mult", "multiply args")->StoreValue(opt.mult);
    parser.AddHelp('h', "help", "Program accumulate arguments");

    if (!parser.Parse(argc, argv)) {
        std::cout << "Wrong argument" << '\n';
        std::cout << parser.HelpDescription() << '\n';
        return 1;
    }

    if (parser.Help()) {
        std::cout << parser.HelpDescription() << '\n';
        return 0;
    }

    if (opt.sum) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 0) << '\n';
    } else if (opt.mult) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << '\n';
    } else {
        std::cout << "No one options had chosen" << '\n';
        std::cout << parser.HelpDescription();
        return 1;
    }

    return 0;

}
