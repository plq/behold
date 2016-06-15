
/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Burak Arslan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include<behold.h>

static const char *LC = "MAIN";

struct Loggable {
    int a;
    int b;
    MAKE_LOGGABLE(a, b)
};

struct NotLoggable {
    const char *b;
};

// another way of using the logger;
using Witness = Behold(std::cerr);

int main(int, char **) {
    std::set<int> si = {4, 3, 2, 1};
    std::vector<int> vi = {1, 2, 3, 4};
    std::vector<std::string> vs = {"a", "b", "c"};
    Loggable l;
    //NotLoggable nl;

#ifdef HAVE_MSGPACK
    std::string some_string = "some string";
    msgpack::v1::type::raw_ref raw;
    raw.ptr = some_string.c_str();
    raw.size = some_string.size();

#else
    std::string raw = "no msgpack";

#endif

    Behold(std::cout)::info(LC) << "A std::set<int>:" << si;
    Behold(std::cout)::info(LC) << "A std::vector<int>:" << vi;
    Behold(std::cout)::debug(LC) << "A std::vector<std::string>:" << vs;
    Behold(std::cout)::error(LC) << "A loggable class:" << l;
    Behold(std::cout)::critical(LC) << "A string:" << raw;
    Behold(std::cout)::devel(LC) << "Some numbers without spaces:"
        << 5 << LogManip::NO_SPACE
        << 6 << LogManip::NO_SPACE
        << 7 << LogManip::NO_SPACE
        << 8 << LogManip::NO_SPACE;

    Witness::info(LC) << "This goes to stderr";

    return 0;
}
