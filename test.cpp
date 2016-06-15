
static const char *LC = "TEST";

#include<cassert>
#include<behold.h>

extern std::stringstream ss;

std::stringstream ss;

using TestLog = Logger<decltype(ss), ss, false>;

// Define user defined literal "_quoted" operator.
std::string quote(const std::string &text) {
    return "\"" + text + "\"";
}

void test_spaces() {
    TestLog::devel("TCTX") << "Falan";
    auto ret = ss.str();

    Behold(std::cerr)::info(LC) << "Log line:" << quote(ret);

    assert(ret == "d TCTX | Falan \n");
}

static std::vector<std::function<void()>> tests = {
    test_spaces,
};

void set_up() {

}

void tear_down() {
    ss.clear();
}

int main(int, char **) {

    for (auto &f: tests) {
        set_up();
        f();
        tear_down();
    }
    return 0;
}
