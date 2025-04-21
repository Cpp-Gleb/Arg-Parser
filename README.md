# Arg Parser

Утилита для анализа аргументов командной строки

## Тесты
Для покрытия функционала были применены тесты с фреймворком google test
Например, тест

```cpp
TEST(ArgParserTestSuite, StringTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}
```

Проверяет, что

- Вызов `parser.Parse(SplitString("app --param1=value1"))` вернет `true`
- Вызов `parser.GetStringValue("param1")` вернет `"value1"`

### Примеры запуска

Пример программы с использованием парсера находится в [bin](bin/main.cpp). Программа умеет складывать или умножать переданные ей аргументы.

*ArgParser --sum 1 2 3 4 5*

*ArgParser --mult 1 2 3 4 5*
