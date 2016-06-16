#ifndef BEHOLD_TF_H
#define BEHOLD_TF_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <functional>

class OhGodException: public std::runtime_error {
public:
    OhGodException(const std::string &s="silly you") : std::runtime_error(s) { }
};

void myassert(bool cond);

std::string escape(const char *str, size_t s);

std::string escape(const std::string &s);

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
        catch (OhGodException &e) {
            std::stringstream fr; // failure_report

            fr << "\nFailed test " << ++idx << ":\n"
               <<   "==============\n";
            if (ss_status.str().empty()) {
                fr << "StatusLog is empty.\n";
            }
            else {
                fr << "StatusLog contents:\n"
                   << ss_status.str() << "\n\n";
            }

            if (ss_test.str().empty()) {
                fr << "TestLog is empty.\n";
            }
            else {
                fr << "TestLog contents:\n"
                   << escape(ss_test.str());
            }

            fr <<   "\n==============\n";

            failed.push_back(fr.str());
            cout << "F" << flush;
        }
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
