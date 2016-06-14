
#include<logger.h>


struct Loggable {
    int a;
    int b;
    MAKE_LOGGABLE(a, b)
};

struct NotLoggable {
    const char *b;
};

int main() {
    std::vector<int> v = {1, 2, 3, 4};
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

    LogEntry() << "Some std::vector:" << v
               << "A loggable class:" << l
               << "A string:" << raw;

    return 0;
}
