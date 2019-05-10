#include "uri.h"
#include <boost/algorithm/string/split.hpp>

using namespace webpp;
using namespace std;

path::path(string const& str) {
    boost::algorithm::split(slugs, str, [](char const& c) { return c == '/'; });
}
