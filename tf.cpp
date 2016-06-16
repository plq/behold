
#include<tf.h>

std::stringstream ss_test;
std::stringstream ss_status;

// FIXME: eww
std::string escape(const char *str, size_t s) {
    std::string retval="\"";

    for (size_t i=0; i < s; ++i) {
        auto c = (uint8_t)(*(str + i));

        switch (c) {
        case 0x0a:
            retval += "\\n";
            break;

        case 0x0c:
            retval += "\\r";
            break;

        case '"':
            retval += "\\\"";
            break;

        default:
            retval += c;
        }
    }

    retval += "\"";

    return retval;
}

std::string escape(const std::string &s) {
    return escape(s.c_str(), s.size());
}

template<>
EqualityError<std::string, std::string>::EqualityError(const std::string &x, const std::string &y): std::runtime_error("") {
    Logger<decltype(ss), ss, false>::error("TFWR") << escape(x) << "!=" << escape(y);
    s = ss.str();
}

std::string remove_header(const std::string &s) {
    auto ret = s.substr(s.find_first_of("|") + 2);
    return ret.substr(0, ret.size() - 1);
}
