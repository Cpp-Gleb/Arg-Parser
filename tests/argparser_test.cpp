#include <sstream>
#include <fstream>

#include "gtest/gtest.h"
#include "lib/ArgParser.h"

using namespace ArgumentParser;

/*
    Функция принимает в качество аргумента строку, разделяет ее по "пробелу"
    и возвращает вектор полученных слов
*/
std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}


TEST(ArgParserTestSuite, EmptyTest) {
    ArgParser parser("My Empty Parser");

    ASSERT_TRUE(parser.Parse(SplitString("")));
}


TEST(ArgParserTestSuite, StringTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<std::string>("param1");

    ASSERT_TRUE(parser.Parse(SplitString(" --param1=value1")));
    ASSERT_EQ(parser.GetValue<std::string>("param1"), "value1");
}


TEST(ArgParserTestSuite, ShortNameTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<std::string>('p', "param1");

    ASSERT_TRUE(parser.Parse(SplitString(" -p=value1")));
    ASSERT_EQ(parser.GetValue<std::string>("param1"), "value1");
}


TEST(ArgParserTestSuite, DefaultTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<std::string>("param1")->Default("value1");

    ASSERT_TRUE(parser.Parse(SplitString("")));
    ASSERT_EQ(parser.GetValue<std::string>("param1"), "value1");
}

//Мой тест по аналогии с тестом выше проверяю можно ли добавить не строку а число по умолчанию
TEST(ArgParserTestSuite, DefaultValueTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<int>("param")->Default(10);

    ASSERT_TRUE(parser.Parse(SplitString("")));
    ASSERT_EQ(parser.GetValue<int>("param"), 10);
}


TEST(ArgParserTestSuite, NoDefaultTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<std::string>("param1")->Required();

    ASSERT_FALSE(parser.Parse(SplitString("")));
}


TEST(ArgParserTestSuite, StoreValueTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddArgument<std::string>("param1")->StoreValue(value);

    ASSERT_TRUE(parser.Parse(SplitString(" --param1=value1")));
    ASSERT_EQ(value, "value1");
}


TEST(ArgParserTestSuite, MultiStringTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddArgument<std::string>("param1")->StoreValue(value);
    parser.AddArgument<std::string>('a', "param2");

    ASSERT_TRUE(parser.Parse(SplitString(" --param1=value1 --param2=value2")));
    ASSERT_EQ(parser.GetValue<std::string>("param2"), "value2");
}


TEST(ArgParserTestSuite, IntTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<int>("param1");

    ASSERT_TRUE(parser.Parse(SplitString(" --param1=100500")));
    ASSERT_EQ(parser.GetValue<int>("param1"), 100500);
}

//Мой тест на невалидный тип аргумента
TEST(ArgParserTestSuite, InvalidTypeTest) {
    ArgParser parser("My Parser");
    parser.AddArgument<int>("number");

    ASSERT_FALSE(parser.Parse(SplitString("--number=abc")));
}


TEST(ArgParserTestSuite, MultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    parser.AddArgument<int>('p', "param1")->MultiValue().StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString(" --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(parser.GetValue<int>("param1", 0), 1);
    ASSERT_EQ(int_values[1], 2);
    ASSERT_EQ(int_values[2], 3);
}


TEST(ArgParserTestSuite, MinCountMultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 10;
    parser.AddArgument<int>('p', "param1")->MultiValue(MinArgsCount).StoreValues(int_values);

    ASSERT_FALSE(parser.Parse(SplitString(" --param1=1 --param1=2 --param1=3")));
}

//Мой тест на проверку суммирования других типов данных
TEST(ArgParserTestSuite, SumDoubleTest) {
    ArgParser parser("My Parser");
    std::vector<double> double_values;
    bool sum_flag = false;

    parser.AddFlag("sum", "Enable summing")->StoreValue(sum_flag);
    parser.AddArgument<double>("N")->MultiValue(1).Positional().StoreValues(double_values);

    ASSERT_TRUE(parser.Parse(SplitString("--sum 1.1 2.5 3.3 4")));
    ASSERT_TRUE(sum_flag);

    ASSERT_EQ(double_values.size(), 4);
    ASSERT_DOUBLE_EQ(double_values[0], 1.1);
    ASSERT_DOUBLE_EQ(double_values[1], 2.5);
    ASSERT_DOUBLE_EQ(double_values[2], 3.3);
    ASSERT_DOUBLE_EQ(double_values[3], 4.0);
}


TEST(ArgParserTestSuite, FlagTest) {
    ArgParser parser("My Parser");
    parser.AddFlag('f', "flag1");

    ASSERT_TRUE(parser.Parse(SplitString(" --flag1")));
    ASSERT_TRUE(parser.GetFlag("flag1"));
}


TEST(ArgParserTestSuite, FlagsTest) {
    ArgParser parser("My Parser");
    bool flag3 ;
    parser.AddFlag('a', "flag1");
    parser.AddFlag('b', "flag2")->Default(true);
    parser.AddFlag('c', "flag3")->StoreValue(flag3);

    ASSERT_TRUE(parser.Parse(SplitString(" -ac")));
    ASSERT_TRUE(parser.GetFlag("flag1"));
    ASSERT_TRUE(parser.GetFlag("flag2"));
    ASSERT_TRUE(flag3);
}


TEST(ArgParserTestSuite, PositionalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.AddArgument<int>("Param1")->MultiValue(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString(" 1 2 3 4 5")));
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, PositionalAndNormalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.AddFlag('f', "flag", "Flag");
    parser.AddArgument<int>('n', "number", "Some Number");
    parser.AddArgument<int>("Param1")->MultiValue(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString(" -n 0 1 2 3 4 5 -f")));
    ASSERT_TRUE(parser.GetFlag("flag"));
    ASSERT_EQ(parser.GetValue<int>("number"), 0);
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, RepeatedParsingTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");
    parser.AddArgument<std::string>('i', "input", "File path for input file");
    parser.AddArgument<std::string>('o', "output", "File path for output directory");
    parser.AddFlag('s', "flag1", "Read first number");
    parser.AddFlag('p', "flag2", "Read second number");
    parser.AddArgument<int>("number", "Some Number");

    ASSERT_TRUE(parser.Parse(SplitString(" --number 2 -s -i test -o=test")));

    if (parser.GetFlag("flag1")) {
      parser.AddArgument<int>("first", "First Number");
    } else if (parser.GetFlag("flag2")) {
      parser.AddArgument<int>("second", "Second Number");
    }

    ASSERT_TRUE(parser.Parse(SplitString(" --number 2 -s -i test -o=test --first=52")));
    ASSERT_EQ(parser.GetValue<int>("first"), 52);
}


TEST(ArgParserTestSuite, HelpTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");

    ASSERT_TRUE(parser.Parse(SplitString(" --help")));
    ASSERT_TRUE(parser.Help());
}


TEST(ArgParserTestSuite, HelpStringTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");
    parser.AddArgument<std::string>('i', "input", "File path for input file")->MultiValue(1);
    parser.AddFlag('s', "flag1", "Use some logic")->Default(true);
    parser.AddFlag('p', "flag2", "Use some logic");
    parser.AddArgument<int>("numer", "Some Number");


    ASSERT_TRUE(parser.Parse(SplitString(" --help")));
    // Проверка закоментирована намеренно. Ождиается, что результат вызова функции будет приблизительно такой же,
    // но не с точностью до символа

    // ASSERT_EQ(
    //     parser.HelpDescription(),
    //     "My Parser\n"
    //     "Some Description about program\n"
    //     "\n"
    //     "-i,  --input=<string>,  File path for input file [repeated, min args = 1]\n"
    //     "-s,  --flag1,  Use some logic [default = true]\n"
    //     "-p,  --flag2,  Use some logic\n"
    //     "     --number=<int>,  Some Number\n"
    //     "\n"
    //     "-h, --help Display this help and exit\n"
    // );
}
