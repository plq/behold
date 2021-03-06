#ifndef BEHOLD_BEHOLD_H
#define BEHOLD_BEHOLD_H

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
    template <typename S, S &out> \
    void to_logger(LogEntry<S, out> &l) const { \
        l << typeid(*this).name() \
          << LogManip::NO_SPACE << "(" << LogManip::NO_SPACE; \
        LogHelper<S, out>(l).to_logger(__VA_ARGS__); \
        l << LogManip::NO_SPACE << ")" ; \
    } \


enum class LogManip {
    NO_SPACE,
};


enum LogLevel {
    LOG_DEVEL = 0,
    LOG_DEBUG = 10,
    LOG_INFO = 20,
    LOG_WARNING = 30,
    LOG_ERROR = 40,
    LOG_CRITICAL = 50,
    LOG_FATAL = 60,
} ;

template<typename S, S &out>
struct LogEntry {
    static const int SET_MAX_SIZE = 5;
    static const int VECTOR_MAX_SIZE = 5;

    time_t t;
    explicit LogEntry(LogLevel l=LOG_DEVEL, const char *lc="", bool log_time=true);

    LogEntry &operator<<(LogManip lm);
    LogEntry &operator<<(const std::string &);
    LogEntry &operator<<(const char *);
    LogEntry &operator<<(const std::set<int> &v);
    LogEntry &operator<<(const std::vector<int> &);
    LogEntry &operator<<(const std::vector<std::string> &);

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
        s << "std::map<len=" << m.size() << ">";
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
    static std::mutex s_mutex;

    std::vector<bool> m_no_space_indexes;

    LogLevel m_level;

    std::list<std::string> m_line;
    std::list<std::string> m_prelude;
};

// we need this because of trailing comma issue with __VA_ARGS__
template <typename S, S &out>
struct LogHelper {
    LogEntry<S, out> &behold;
    explicit LogHelper(LogEntry<S, out> &l): behold(l) { }

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



template <typename S, S &out>
std::mutex LogEntry<S, out>::s_mutex;


template <typename S, S &out>
LogEntry<S, out>::LogEntry(LogLevel l, const char *lc, bool log_time): t(time(NULL)), m_level(l) {
    if (m_level <= LOG_DEVEL) {
        m_prelude.push_back("d");
    }
    else if (m_level <= LOG_DEBUG) {
        m_prelude.push_back("D");
    }
    else if (m_level <= LOG_INFO) {
        m_prelude.push_back("I");
    }
    else if (m_level <= LOG_WARNING) {
        m_prelude.push_back("W");
    }
    else if (m_level <= LOG_ERROR) {
        m_prelude.push_back("E");
    }
    else if (m_level <= LOG_CRITICAL) {
        m_prelude.push_back("C");
    }
    else if (m_level <= LOG_FATAL) {
        m_prelude.push_back("F");
    }
    else {
        m_prelude.push_back("?");
    }

    if (log_time) {
        std::stringstream s;
        s << t;
        m_prelude.push_back(s.str());
    }

    m_prelude.push_back(lc);
    m_prelude.push_back("|");
}

template <typename S, S &out>
LogEntry<S, out>::~LogEntry() {
    if (m_line.empty()) {
        return;
    }

    auto b = m_no_space_indexes.cbegin();
    auto e = m_no_space_indexes.cend();
    auto i = b;

    std::lock_guard<std::mutex> guard(s_mutex);
    for (auto &s: m_prelude) {
        out << s << " ";
    }

    for (auto &s: m_line) {
        auto has_nosp = (i == b || (i < e && (*i)));
        if (! has_nosp) {
            out << " ";
        }

        out << s;

        ++i;
    }

    out << std::endl;
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(LogManip lm) {
    switch (lm) {
    case LogManip::NO_SPACE: {
        if (! m_line.empty()) {
            auto s = m_line.size();
            m_no_space_indexes.resize(s + 1, false);
            m_no_space_indexes[s] = true;
        }
        break;
    }

    }

    return *this;
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const std::string &s) {
    m_line.push_back(s);
    return *this;
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const char *s) {
    m_line.push_back(s);
    return *this;
}

#ifdef HAVE_MSGPACK

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const msgpack::v1::type::raw_ref &raw) {
    std::stringstream s;
    s << "msgpack::raw<len=" << raw.size << ">";
    m_line.push_back(s.str());
    return *this;
}

#endif


template <typename S, S &out, bool f>
struct Logger {
    static const auto VECTOR_MAX_SIZE = LogEntry<S, out>::VECTOR_MAX_SIZE;
    static const auto SET_MAX_SIZE = LogEntry<S, out>::SET_MAX_SIZE;

    static LogEntry<S, out> devel(const char *lc);
    static LogEntry<S, out> debug(const char *lc);
    static LogEntry<S, out> info(const char *lc);
    static LogEntry<S, out> warn(const char *lc);
    static LogEntry<S, out> warning(const char *lc);
    static LogEntry<S, out> err(const char *lc);
    static LogEntry<S, out> error(const char *lc);
    static LogEntry<S, out> crit(const char *lc);
    static LogEntry<S, out> critical(const char *lc);
    static LogEntry<S, out> fatal(const char *lc);
};


template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::devel(const char *lc) {
    return LogEntry<S, out>(LOG_DEVEL, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::debug(const char *lc) {
    return LogEntry<S, out>(LOG_DEBUG, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::info(const char *lc) {
    return LogEntry<S, out>(LOG_INFO, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::warn(const char *lc) {
    return LogEntry<S, out>(LOG_WARNING, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::warning(const char *lc) {
    return LogEntry<S, out>(LOG_WARNING, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::err(const char *lc) {
    return LogEntry<S, out>(LOG_ERROR, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::error(const char *lc) {
    return LogEntry<S, out>(LOG_ERROR, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::crit(const char *lc) {
    return LogEntry<S, out>(LOG_CRITICAL, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::critical(const char *lc) {
    return LogEntry<S, out>(LOG_CRITICAL, lc, f);
}

template <typename S, S &out, bool f>
LogEntry<S, out> Logger<S, out, f>::fatal(const char *lc) {
    return LogEntry<S, out>(LOG_FATAL, lc, f);
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const std::vector<int> &v) {
    auto size = v.size();
    std::stringstream s;
    if (size > VECTOR_MAX_SIZE) {
        s << "std::vector<int>(len=" << v.size() << ")";
    }
    else {
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
    m_line.push_back(s.str());
    return *this;
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const std::vector<std::string> &v) {
    auto size = v.size();
    std::stringstream s;
    if (size > VECTOR_MAX_SIZE) {
        s << "std::vector<std::string>(len=" << v.size() << ")";
    }
    else {
        s << "[";

        int i = 0;
        for (const auto &t: v) {
            if (i++ > 0) {
                s << ", ";
            }
            s << "\"" << t << "\"";
        }
        s << "]";
    }
    m_line.push_back(s.str());
    return *this;
}

template <typename S, S &out>
LogEntry<S, out> &LogEntry<S, out>::operator<<(const std::set<int> &s) {
    auto size = s.size();
    std::stringstream sstr;
    if (size > SET_MAX_SIZE) {
        sstr << "std::set<len=" << s.size() << ">";
    }
    else {
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
    m_line.push_back(sstr.str());
    return *this;
}

#define Behold(X) Logger<decltype(X), X, true>

#endif
