
#include <logger.h>

template <>
LogEntry &LogEntry::operator<<(const std::vector<int> &v) {
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
LogEntry &LogEntry::operator<<(const std::set<int> &s) {
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
