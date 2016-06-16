
static const char *LC = "TEST";

#include<stdexcept>

#include<behold.h>
#include<tf.h>

static const bool DO_LOG_TIME = true;
static const bool DONT_LOG_TIME = false;

using TestLog = Logger<decltype(ss_test), ss_test, DONT_LOG_TIME>;
using StatusLog = Logger<decltype(ss_status), ss_status, DO_LOG_TIME>;

void test_const_char() {
    TestLog::devel(__FUNCTION__) << "Test";
    auto ret = remove_header(ss_test.str());
    StatusLog::devel(LC) << escape(ret);

    assert_equal(ret, "Test");
}

void test_format_with_time() {
    time_t t;
    {
        auto log = Logger<decltype(ss_test), ss_test, true>::devel(__FUNCTION__);
        log << "Test";

        t = log.t;
    }

    auto ret = ss_test.str();

    assert_equal(ret, std::string("d ") + std::to_string(t) +  __FUNCTION__ + " | Test\n");
}

void test_format_remove_header() {
    TestLog::devel(__FUNCTION__) << "Test";
    auto ret = ss_test.str();
    assert_equal(ret, std::string("d ") + __FUNCTION__ + " | Test\n");

    ret = remove_header(ret);
    assert_equal(ret, "Test");
}

void test_integer() {
    TestLog::devel(__FUNCTION__) << 1;
    auto ret = remove_header(ss_test.str());

    assert_equal(ret, "1");
}

void test_nosp_1() {
    TestLog::devel(__FUNCTION__) << 1 << LogManip::NO_SPACE << 2;
    auto ret = remove_header(ss_test.str());

    assert_equal(ret, "12");
}

void test_nosp_2() {
    TestLog::devel(__FUNCTION__) << LogManip::NO_SPACE << 1 << 2;
    auto ret = remove_header(ss_test.str());

    assert_equal(ret, "1 2");
}

static std::vector<std::function<void()>> tests = {
    test_nosp_1,
    test_nosp_2,
    test_integer,
    test_const_char,
    test_format_with_time,
    test_format_remove_header,
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
    TestContext() { set_up(); }
    ~TestContext() { tear_down(); }
};

int main(int, char **) {
    return run_tests<TestContext>(tests);
}
