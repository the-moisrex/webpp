#include "../../webpp/uri/uri_string.hpp"
#include "../benchmark.hpp"
#include "./liburlparser/include/urlparser.h"

#include <array>
#include <string_view>

std::string_view getURI() noexcept {
    static constexpr std::array<std::string_view, 51> uri_list = {
      {"mailto:someone@example.com",
       "http://foo.com/blah_blah",
       "http://foo.com/blah_blah/",
       "http://foo.com/blah_blah_(wikipedia)",
       "http://foo.com/blah_blah_(wikipedia)_(again)",
       "http://www.example.com/wpstyle/?p=364",
       "https://www.example.com/foo/?bar=baz&inga=42&quux",
       "http://✪df.ws/123",
       "http://userid:password@example.com:8080",
       "http://userid:password@example.com:8080/",
       "http://userid@example.com",
       "http://userid@example.com/",
       "http://userid@example.com:8080",
       "http://userid@example.com:8080/",
       "http://userid:password@example.com",
       "http://userid:password@example.com/",
       "http://142.42.1.1/",
       "http://142.42.1.1:8080/",
       "http://➡.ws/䨹",
       "http://⌘.ws",
       "http://⌘.ws/",
       "http://foo.com/blah_(wikipedia)#cite-1",
       "http://foo.com/blah_(wikipedia)_blah#cite-1",
       "http://foo.com/unicode_(✪)_in_parens",
       "http://foo.com/(something)?after=parens",
       "http://☺.ha-ha.com/",
       "http://code.google.com/events/#&product=browser",
       "http://j.mp",
       "ftp://foo.bar/baz",
       "http://foo.bar/?q=Test%20URL-encoded%20stuff",
       "http://مثال.إختبار",
       "http://例子.测试",
       "http://उदाहरण.परीक्षा",
       "http://-.~_!$&()*+,;=:%40:80%2f::::::@example.com",
       "http://1337.net",
       "http://a.b-c.de",
       "http://223.255.255.254",
       "http://0.0.0.0",
       "http://10.1.1.0",
       "http://10.1.1.255",
       "http://224.1.1.1",
       "http://1.1.1.1.1",
       "http://123.123.123",
       "http://3628126748",
       "http://10.1.1.1",
       "http://10.1.1.254",
       "ftps://foo.bar/",
       "http://-error-.invalid/",
       "http://a.b--c.de/",
       "http://-a.b.co",
       "http://a.b-.co"}};
    static size_t index = 0;
    auto          uri   = uri_list[index];
    index               = (index + 1) % uri_list.size(); // Wrap around after reaching the end.
    return uri;
}



static void WebppURIString_Host(benchmark::State& state) {
    for (auto _ : state) {
        webpp::uri::uri_view u{getURI()};
        auto                 host = u.host_raw();
        benchmark::DoNotOptimize(u);
        benchmark::DoNotOptimize(host);
    }
}
BENCHMARK(WebppURIString_Host);

static void URIParser_Host(benchmark::State& state) {

    for (auto _ : state) {
        TLD::Host host     = TLD::Host::fromUrl("https://ee.aut.ac.ir/about");
        auto      host_str = host.str();
        benchmark::DoNotOptimize(host);
        benchmark::DoNotOptimize(host_str);
    }
}
BENCHMARK(URIParser_Host);

static void WebppURIString_All(benchmark::State& state) {
    for (auto _ : state) {
        webpp::uri::uri_view u{getURI()};
        auto                 host_str     = u.host();
        auto                 proto_str    = u.scheme();
        auto                 path_str     = u.path_raw();
        auto                 userinfo_str = u.user_info_raw();
        auto                 frag_str     = u.fragment();
        benchmark::DoNotOptimize(u);
        benchmark::DoNotOptimize(host_str);
        benchmark::DoNotOptimize(proto_str);
        benchmark::DoNotOptimize(path_str);
        benchmark::DoNotOptimize(userinfo_str);
        benchmark::DoNotOptimize(frag_str);
    }
}
BENCHMARK(WebppURIString_All);

static void URIParser_All(benchmark::State& state) {
    for (auto _ : state) {
        TLD::Url u            = TLD::Url(std::string{getURI()});
        auto     host_str     = u.host();
        auto     proto_str    = u.protocol();
        auto     path_str     = u.abspath();
        auto     userinfo_str = u.userinfo();
        auto     frag_str     = u.fragment();
        benchmark::DoNotOptimize(u);
        benchmark::DoNotOptimize(host_str);
        benchmark::DoNotOptimize(proto_str);
        benchmark::DoNotOptimize(path_str);
        benchmark::DoNotOptimize(userinfo_str);
        benchmark::DoNotOptimize(frag_str);
    }
}
BENCHMARK(URIParser_All);
