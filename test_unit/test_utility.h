#include <core/config_file.h>
#include <core/single_instance.h>
#include <core/string_utility.h>
#include <string>

TEST(Utility, SingleInstance)
{
	core::SingleInstance singleton;

    bool b = singleton();

    EXPECT_TRUE(b==true);
}

TEST(Utility, ConfigFile)
{
	core::ConfigFile x;
    x.loadFromFile("./test_config.txt");

    string val;
    val = x.getStringValue("THREAD_STACK_SIZE");

    EXPECT_STREQ("0", val.c_str()); // string equal

    auto symbols = x.getArray("SYMBOL");

    auto first = symbols[0];
    auto second = symbols[1];

    EXPECT_STREQ("MSFT", first.c_str()); // string equal
    EXPECT_STREQ("AAPL", second.c_str()); // string equal
}

TEST(Utility, ReplaceInString)
{
    std::string orig = "xxxyyxx";
	core::replaceInString(orig, "yy", "jjj");
    EXPECT_STREQ("xxxjjjxx", orig.c_str()); // string equal
}