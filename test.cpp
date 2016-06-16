
#include<stdexcept>

#include<behold.h>
#include<tf.h>

using TestLog = Logger<decltype(ss_test), ss_test, false>;
using StatusLog = Logger<decltype(ss_status), ss_status, true>;


void test_const_char() {
    TestLog::devel(__FUNCTION__) << "Test";
    auto ret = ss_test.str();

    myassert(ret == "d TCTX | Test\n");
}

void test_integer() {
    TestLog::devel(__FUNCTION__) << 1;
    auto ret = ss_test.str();

    myassert(ret == "d TCTX | Falan\n");
}

static std::vector<std::function<void()>> tests = {
    test_const_char,
    test_integer,
};

void set_up() {

}

void tear_down() {
    ss_test.str(std::string());
    ss_test.clear();

    ss_status.str(std::string());
    ss_status.clear();
}

struct TestContext {
    TestContext() {
        set_up();
    }
    ~TestContext() {
        tear_down();
    }
};

int main(int, char **) {
    return run_tests<TestContext>(tests);
}
