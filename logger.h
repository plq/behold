
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
    void to_logger(LogEntry &l) const { \
        l << typeid(*this).name() \
          << LogManip::NO_SPACE << "(" << LogManip::NO_SPACE; \
        LogHelper(l).to_logger(__VA_ARGS__); \
        l << LogManip::NO_SPACE << ")" ; \
    } \


enum class LogManip {NO_SPACE};

struct LogEntry {  
    std::list<std::string> line;
    static const int SET_MAX_SIZE = 5;
    static const int VECTOR_MAX_SIZE = 5;
    std::vector<bool> no_space_indexes;

    LogEntry() {
        no_space_indexes.reserve(64);
    }

    template <typename T>
    LogEntry &operator <<(const std::vector<T> &v) {
        std::stringstream s;
        s << "std::vector<len=" << v.size() << ">";
        line.push_back(s.str());
        return *this;
    }

    template <typename T>
    LogEntry &operator <<(const std::set<T> &s) {
        std::stringstream sstr;
        s << "std::set<len=" << s.size() << ">";
        line.push_back(sstr.str());
        return *this;
    }

    LogEntry &operator<<(LogManip lm) {
        switch (lm) {
        case LogManip::NO_SPACE:
            auto s = line.size();
            no_space_indexes.resize(s+1);
            no_space_indexes[s] = true;
            break;
        }

        return *this;
    }

    LogEntry &operator<<(const std::string &s) {
        line.push_back(s);
        return *this;
    }

    LogEntry &operator<<(const char *s) {
        line.push_back(s);
        return *this;
    }

#ifdef HAVE_MSGPACK
    LogEntry &operator<<(const msgpack::v1::type::raw_ref &raw) {
        std::stringstream s;
        s << "msgpack::raw<len=" << raw.size << ">";
        line.push_back(s.str());
        return *this;
    }
#endif

    template <typename K, typename V>
    LogEntry &operator <<(const std::map<K, V> &m) {
        std::stringstream s;
        s << "Map<len=" << m.size() << ">";
        line.push_back(s.str());
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, LogEntry &>::type
    operator<<(T t) {
        std::stringstream s;
        s << t;
        line.push_back(s.str());
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
        line.push_back(s.str());
        return *this;
    }

    ~LogEntry() {
        std::cout << time(NULL) << ": ";
        auto b = no_space_indexes.cbegin();
        auto e = no_space_indexes.cend();
        auto i = b + 1;
        for (auto &s: line) {
            std::cout << s;

            auto has_nosp = (i < e && (*i));
            if (! has_nosp) {
                std::cout << " ";
            }

            ++i;
        }
        std::cout << std::endl;
    }
};

// we need this because of trailing comma issue with __VA_ARGS__
struct LogHelper {
    LogEntry &logger;
    explicit LogHelper(LogEntry &l): logger(l) { }

    inline void to_logger() { }

    template<typename First>
    void to_logger(First && first) {
        logger << std::forward<First>(first);
    }

    template<typename First, typename ...Rest>
    void to_logger(First && first, Rest && ...rest) {
        std::stringstream s;
        s << std::forward<First>(first) << ", ";
        logger << s.str();
        to_logger(std::forward<Rest>(rest)...);
    }
};
