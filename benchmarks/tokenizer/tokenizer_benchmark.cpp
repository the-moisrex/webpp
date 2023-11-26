#include "../../webpp/strings/to_case.hpp"
#include "../../webpp/strings/trim.hpp"
#include "../benchmark.hpp"
#include "string_tokenizer_v1.hpp"
#include "string_tokenizer_v2.hpp"

using namespace std;


static constexpr auto VALID_COOKIE_NAME = webpp::charset(
  webpp::ALPHA_DIGIT<>,
  webpp::charset<char, 16>{'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~'});


static constexpr auto VALID_COOKIE_VALUE = webpp::charset(
  webpp::ALPHA_DIGIT<char>,
  webpp::charset<char, 28>{'!', '#', '$', '%', '&', '\'', '(', ')', '*', '+', '-', '.', '/', ':',
                           '<', '=', '>', '?', '@', '[',  ']', '^', '_', '`', '{', '|', '}', '~'});

static constexpr webpp::charset<char, 2> OWS{0x20, 0x09}; // SP, HTAB

namespace v1 {
    using namespace webpp;

    template <typename ErrorType>
    constexpr void parse_SE_pair(
      istl::StringView auto& str,
      auto&                  name,
      auto&                  value,
      ErrorType&             err,
      ErrorType              err_value) noexcept {
        using namespace webpp::benchmark::v1;
        string_tokenizer tok{str};
        tok.skip(OWS);
        tok.expect(VALID_COOKIE_NAME, name, err, err_value);
        tok.skip(OWS);
        if (err != err_value && tok.expect(charset{'='})) {
            tok.skip(OWS);
            tok.expect(VALID_COOKIE_VALUE, charset{'\"'}, value, err, err_value);
        }
    }

    constexpr void parse_SE_name(istl::StringView auto& str, auto& _name, bool& _valid) noexcept {
        using name_t           = stl::remove_cvref_t<decltype(_name)>;
        using string_view_type = stl::remove_cvref_t<decltype(str)>;
        ascii::ltrim(str);
        if (auto equal_pos = str.find_first_not_of(VALID_COOKIE_NAME.data());
            equal_pos != string_view_type::npos)
        {
            // setting the name we found it
            _name = name_t{str.substr(0, equal_pos)};

            // prepare the string for the next value
            str.remove_prefix(equal_pos);
            _valid = true;
        } else {
            // there's no value in the string
            _valid = false;
        }
    }

    constexpr void
    parse_SE_value(istl::StringView auto& str, auto& _name, auto& _value, bool& _valid) noexcept {
        using string_view_type = stl::remove_cvref_t<decltype(str)>;

        parse_SE_name(str, _name, _valid);
        if (!_valid) {
            return; // do not continue if there's no name
        }
        ascii::ltrim(str);
        if (ascii::starts_with(str, '=')) {
            str.remove_prefix(1);
        }
        ascii::ltrim(str);
        if (ascii::starts_with(str, '"')) {
            if (auto d_quote_end = str.find_first_not_of(VALID_COOKIE_VALUE.data(), 1);
                d_quote_end != string_view_type::npos)
            {
                if (str[d_quote_end] == '"') {
                    _value = str.substr(1, d_quote_end - 1);
                    str.remove_prefix(d_quote_end + 1);
                } else {
                    // You can't use non-double-quote chars when you used
                    // one already. You can't even use backslash to escape,
                    // so there's no worry here
                    _valid = false;
                    return;
                }
            } else {
                // It won't be a valid string if there's a double quote
                // without another one finishing it off.
                _valid = false;
                return;
            }
        } else {
            // there's no double quote in the value
            if (auto semicolon_pos = str.find_first_not_of(VALID_COOKIE_VALUE.data());
                semicolon_pos != string_view_type::npos)
            {
                _value = str.substr(0, semicolon_pos);
                str.remove_prefix(semicolon_pos);
            } else {
                _value = str;
                str.remove_prefix(str.size());
            }
        }

        // Attention: here we are not checking the rest of the string.
        // There might be invalid characters after this. We have to
        // check the whole string for validation. But if it's determined
        // that it's invalid so far, it really is invalid.
    }


} // namespace v1

namespace v2 {
    using namespace webpp;

    template <typename ErrorType>
    constexpr void parse_SE_pair(
      istl::StringView auto& str,
      auto&                  name,
      auto&                  value,
      ErrorType&             err,
      ErrorType              err_value) noexcept {
        using namespace webpp::benchmark::v2;
        string_tokenizer tok{str};
        tok.skip(OWS);
        tok.expect(VALID_COOKIE_NAME, name, err, err_value);
        tok.skip(OWS);
        if (err != err_value && tok.expect(charset{'='})) {
            tok.skip(OWS);
            tok.expect(VALID_COOKIE_VALUE, charset{'\"'}, value, err, err_value);
        }
    }
} // namespace v2

static void StrViewParser(benchmark::State& state) {
    for (auto _ : state) {
        string_view str = "_HFID=hfid:86715526";
        string_view name, value;
        bool        is_valid = true;
        v1::parse_SE_value(str, name, value, is_valid);
        benchmark::DoNotOptimize(str);
        benchmark::DoNotOptimize(name);
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(is_valid);
    }
}

BENCHMARK(StrViewParser);

static void TokenizerParserV1(benchmark::State& state) {
    for (auto _ : state) {
        string_view str = "_HFID=hfid:86715526";
        string_view name, value;
        bool        is_valid = true;
        v1::parse_SE_pair(str, name, value, is_valid, false);
        benchmark::DoNotOptimize(str);
        benchmark::DoNotOptimize(name);
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(is_valid);
    }
}

BENCHMARK(TokenizerParserV1);

static void TokenizerParserV2(benchmark::State& state) {
    for (auto _ : state) {
        string_view str = "_HFID=hfid:86715526";
        string_view name, value;
        bool        is_valid = true;
        v2::parse_SE_pair(str, name, value, is_valid, false);
        benchmark::DoNotOptimize(str);
        benchmark::DoNotOptimize(name);
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(is_valid);
    }
}

BENCHMARK(TokenizerParserV2);
