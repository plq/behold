
#include<tf.h>

std::stringstream ss_test;
std::stringstream ss_status;

void myassert(bool cond) {
    if (! cond) {
        throw OhGodException();
    }
}

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
