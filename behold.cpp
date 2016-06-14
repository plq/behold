
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

#include <behold.h>

Behold &Behold::operator<<(LogManip lm) {
    switch (lm) {
    case LogManip::NO_SPACE:
        auto s = line.size();
        no_space_indexes.resize(s+1);
        no_space_indexes[s] = true;
        break;
    }

    return *this;
}

Behold &Behold::operator<<(const std::string &s) {
    line.push_back(s);
    return *this;
}

Behold &Behold::operator<<(const char *s) {
    line.push_back(s);
    return *this;
}

#ifdef HAVE_MSGPACK
Behold &Behold::operator<<(const msgpack::v1::type::raw_ref &raw) {
    std::stringstream s;
    s << "msgpack::raw<len=" << raw.size << ">";
    line.push_back(s.str());
    return *this;
}
#endif

template <>
Behold &Behold::operator<<(const std::vector<int> &v) {
    auto size = v.size();
    std::stringstream s;
    if (size < VECTOR_MAX_SIZE) {
        s << "[";

        int i = 0;
        for (const auto &t: v) {
            if (i++ > 0) {
                s << ", ";
            }
            s << t;
        }
        s << "]";
    }
    else {
        s << "std::vector<len=" << v.size() << ">";
    }
    line.push_back(s.str());
    return *this;
}

template <>
Behold &Behold::operator<<(const std::set<int> &s) {
    auto size = s.size();
    std::stringstream sstr;
    if (size < SET_MAX_SIZE) {
        sstr << "{";

        int i = 0;
        for (const auto &t: s) {
            if (i++ > 0) {
                sstr << ", ";
            }
            sstr << t;
        }
        sstr << "}";
    }
    else {
        sstr << "std::set<len=" << s.size() << ">";
    }
    line.push_back(sstr.str());
    return *this;
}
