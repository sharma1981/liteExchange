#include <core/config_file.h>
#include <core/single_instance.h>
#include <core/string_utility.h>
#include <core/timer.h>
#include <core/concurrency/thread.h>
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

class timedWorker
{
    public:
        void* run(void* arg)
        {
            int* p_val = static_cast<int *>(arg);
            ++(*p_val);
            return nullptr;
        }
};

TEST(Utility, Timer)
{
    timedWorker w;
    core::Timer timer;
    int i = 0;

    long intervalMicroseconds = 100000;
    long sleepMicroseconds = 1000000;
    int leastExpectedNumber = (sleepMicroseconds / intervalMicroseconds) - 1;

    // We start a timer with 100 milliseconds interval that increments i in each invoke
    timer.start(new core::Task(&timedWorker::run, &w, &i), 100000, 10);
    core::Thread::sleep(1000000);
    timer.stop();

    // We sleep for 1000 milliseconds so we expected i to be incremented at least 9 times
    EXPECT_GE(i, leastExpectedNumber);
}