
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
#include<mutex>
#include<vector>
#include<sstream>
#include<iostream>
#include<typeinfo>
#include<type_traits>

#ifdef HAVE_MSGPACK
#include <msgpack.hpp>
#endif


#define MAKE_LOGGABLE(...) \
    void to_logger(LogEntry &l) const { \
        l << typeid(*this).name() \
          << LogManip::NO_SPACE << "(" << LogManip::NO_SPACE; \
        LogHelper(l).to_logger(__VA_ARGS__); \
        l << LogManip::NO_SPACE << ")" ; \
    } \


enum class LogManip {NO_SPACE};


enum LogLevel {
    LOG_DEVEL = 0,
    LOG_DEBUG = 10,
    LOG_INFO = 20,
    LOG_WARNING = 30,
    LOG_ERROR = 40,
    LOG_CRITICAL = 50,
    LOG_FATAL = 60,
} ;


struct LogEntry {
    explicit LogEntry(LogLevel l=LOG_DEVEL, const char *lc="");

    LogEntry &operator<<(LogManip lm);
    LogEntry &operator<<(const std::string &s);
    LogEntry &operator<<(const char *s);

    #ifdef HAVE_MSGPACK
    LogEntry &operator<<(const msgpack::v1::type::raw_ref &raw);
    #endif

    template <typename T>
    LogEntry &operator <<(const std::vector<T> &v) {
        std::stringstream s;
        s << "std::vector<len=" << v.size() << ">";
        m_line.push_back(s.str());
        return *this;
    }

    template <typename T>
    LogEntry &operator <<(const std::set<T> &s) {
        std::stringstream sstr;
        sstr << "std::set<len=" << s.size() << ">";
        m_line.push_back(sstr.str());
        return *this;
    }

    template <typename K, typename V>
    LogEntry &operator <<(const std::map<K, V> &m) {
        std::stringstream s;
        s << "Map<len=" << m.size() << ">";
        m_line.push_back(s.str());
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, LogEntry &>::type
    operator<<(T t) {
        std::stringstream s;
        s << t;
        m_line.push_back(s.str());
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_class<T>::value, LogEntry &>::type
    operator<<(const T &t) {
        t.to_logger(*this);
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, LogEntry &>::type
    operator<<(const T &t) {
        std::stringstream s;
        s << t;
        m_line.push_back(s.str());
        return *this;
    }

    ~LogEntry();

private:
    static const int SET_MAX_SIZE = 5;
    static const int VECTOR_MAX_SIZE = 5;

    static std::mutex s_mutex;

    std::vector<bool> m_no_space_indexes;

    LogLevel m_level;

    std::list<std::string> m_line;

};

// we need this because of trailing comma issue with __VA_ARGS__
struct LogHelper {
    LogEntry &behold;
    explicit LogHelper(LogEntry &l): behold(l) { }

    inline void to_logger() { }

    template<typename First>
    void to_logger(First && first) {
        behold << std::forward<First>(first);
    }

    template<typename First, typename ...Rest>
    void to_logger(First && first, Rest && ...rest) {
        behold << std::forward<First>(first) << LogManip::NO_SPACE << ",";
        to_logger(std::forward<Rest>(rest)...);
    }
};

//template <typename T>
struct Behold {
    static LogEntry devel(const char *lc);
    static LogEntry debug(const char *lc);
    static LogEntry info(const char *lc);
    static LogEntry warning(const char *lc);
    static LogEntry error(const char *lc);
    static LogEntry critical(const char *lc);
    static LogEntry fatal(const char *lc);
};
