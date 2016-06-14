
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

#include<map>
#include<set>
#include<list>
#include<vector>
#include<sstream>
#include<iostream>
#include<typeinfo>
#include<type_traits>

#ifdef HAVE_MSGPACK
#include <msgpack.hpp>
#endif


#define MAKE_LOGGABLE(...) \
    void to_behold(Behold &l) const { \
        l << typeid(*this).name() \
          << LogManip::NO_SPACE << "(" << LogManip::NO_SPACE; \
        LogHelper(l).to_behold(__VA_ARGS__); \
        l << LogManip::NO_SPACE << ")" ; \
    } \


enum class LogManip {NO_SPACE};

struct Behold {  
    std::list<std::string> line;
    static const int SET_MAX_SIZE = 5;
    static const int VECTOR_MAX_SIZE = 5;
    std::vector<bool> no_space_indexes;

    Behold() {
        no_space_indexes.reserve(64);
    }

    Behold &operator<<(LogManip lm);
    Behold &operator<<(const std::string &s);
    Behold &operator<<(const char *s);

    #ifdef HAVE_MSGPACK
    Behold &operator<<(const msgpack::v1::type::raw_ref &raw);
    #endif

    template <typename T>
    Behold &operator <<(const std::vector<T> &v) {
        std::stringstream s;
        s << "std::vector<len=" << v.size() << ">";
        line.push_back(s.str());
        return *this;
    }

    template <typename T>
    Behold &operator <<(const std::set<T> &s) {
        std::stringstream sstr;
        s << "std::set<len=" << s.size() << ">";
        line.push_back(sstr.str());
        return *this;
    }

    template <typename K, typename V>
    Behold &operator <<(const std::map<K, V> &m) {
        std::stringstream s;
        s << "Map<len=" << m.size() << ">";
        line.push_back(s.str());
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, Behold &>::type
    operator<<(T t) {
        std::stringstream s;
        s << t;
        line.push_back(s.str());
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_class<T>::value, Behold &>::type
    operator<<(const T &t) {
        t.to_behold(*this);
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, Behold &>::type
    operator<<(const T &t) {
        std::stringstream s;
        s << t;
        line.push_back(s.str());
        return *this;
    }

    ~Behold();

};

// we need this because of trailing comma issue with __VA_ARGS__
struct LogHelper {
    Behold &behold;
    explicit LogHelper(Behold &l): behold(l) { }

    inline void to_behold() { }

    template<typename First>
    void to_behold(First && first) {
        behold << std::forward<First>(first);
    }

    template<typename First, typename ...Rest>
    void to_behold(First && first, Rest && ...rest) {
        behold << std::forward<First>(first) << LogManip::NO_SPACE << ",";
        to_behold(std::forward<Rest>(rest)...);
    }
};
