
#include<map>
#include<list>
#include<vector>
#include<sstream>
#include<iostream>
#include<type_traits>

#include <msgpack.hpp>

struct LogEntry {  
    std::list<std::string> line;
    template <typename T>
    LogEntry &operator <<(const std::vector<T> &v) {
        std::stringstream s;
        s << "Vector<len=" << v.size() << ">";
        line.push_back(s.str());
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
    
    LogEntry &operator<<(const msgpack::v1::type::raw_ref &raw) {
        std::stringstream s;
        s << "Bytes<len=" << raw.size << ">";
        line.push_back(s.str());
        return *this;
    }

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
        for (auto &s: line) {
            std::cout << s << " ";
        }
    }
};

struct Loggable {
    int a;
    void to_logger(LogEntry &l) const {
        std::stringstream ss;
        ss << "Loggable(" << a << ")";
        l << ss.str();
    }
};


int main() {
    std::vector<int> v = {1,2,3,4};
    Loggable l;
    std::string some_string = "some string";

    msgpack::v1::type::raw_ref raw;
    raw.ptr = some_string.c_str();
    raw.size = some_string.size();

    LogEntry() << v << l << raw;
    return 0;
}
