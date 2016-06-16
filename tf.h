#ifndef BEHOLD_TF_H
#define BEHOLD_TF_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <functional>

#include <behold.h>

std::string escape(const char *str, size_t s);

std::string escape(const std::string &s);

std::string remove_header(const std::string &s);


template<typename X, typename Y>
class NotEqualError: public std::runtime_error {
public:
    static std::stringstream ss;
    std::string s;
    NotEqualError(const X &x, const Y &y) : std::runtime_error("") {
        Logger<decltype(ss), ss, false>::error("TFWR") << x << "!=" << y;
        s = ss.str();
    }
    virtual const char *what() const throw() override {
        return s.c_str();
    }
};

template<typename X, typename Y>
std::stringstream NotEqualError<X, Y>::ss;


template <typename X, typename Y>
void assert_equal(const X &x, const Y &y) {
    if (x != y) {
        throw NotEqualError<X, Y>(x, y);
    }
}

extern std::stringstream ss_test;
extern std::stringstream ss_status;

template <typename T>
int run_tests(std::vector<std::function<void()>> &tests) {
    using std::cout;
    using std::endl;
    using std::flush;

    std::vector<std::string> failed;
    int idx = 0;

    cout << "Testing: " << flush;

    for (auto &f: tests) {
        T ctx;

        try{
            f();
            cout << "." << flush;
        }
        catch (std::runtime_error &e) {
            std::stringstream fr; // failure_report

            fr << "\nFailed test " << idx << ":\n"
               <<   "==============\n";
            if (ss_status.str().empty()) {
                fr << "StatusLog is empty.\n";
            }
            else {
                fr << "StatusLog contents:\n"
                   << ss_status.str() << "\n\n";
            }

            fr << "\n";
            if (ss_test.str().empty()) {
                fr << "TestLog is empty.\n";
            }
            else {
                fr << "TestLog contents:\n"
                   << escape(ss_test.str());
            }

            fr << "\n\nException text:\n" << e.what();

            fr <<   "\n==============\n";

            failed.push_back(fr.str());
            cout << "F" << flush;
        }

        ++idx;
    }

    cout << "\n";

    for (auto &f: failed) {
        cout << f << "\n";
    }

    cout << "\n";

    if (failed.empty()) {
        cout << tests.size() << " tests OK" << endl;
    }
    else {
        cout << failed.size() << " tests failed out of " << tests.size() << endl;
    }

    return failed.size();
}

#endif
