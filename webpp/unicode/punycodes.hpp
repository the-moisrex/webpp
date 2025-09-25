#ifndef WEBPP_URL_PUNY_CODES_HPP
#define WEBPP_URL_PUNY_CODES_HPP

#include "../std/functional.hpp"
#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./unicode.hpp"

#include <cstdint>

namespace webpp::unicode::idna {

    // NOLINTBEGIN(*-magic-numbers)

    using punycode_uint = stl::uint32_t;

    enum struct punycode_status : stl::uint16_t { // NOLINT(*-enum-size)
        // these values are a match for to to_ascii_status as well, so if you ever change these values, you
        // may need to change them as well.

        success   = 0,
        bad_input = 0b1U << 1U, // Input is invalid.
        overflow  = 0b1U << 2U  // Input needs wider integers to process.
    };

    static constexpr stl::string_view to_string(punycode_status const status) noexcept {
        switch (status) {
            case punycode_status::success: return {"Success"};
            case punycode_status::bad_input: return {"Input is invalid"};
            case punycode_status::overflow:
                return {"Input needs wider integers to process"};
            [[unlikely]] default:
                return {"Unknown punycode error"};
        }
    }

    /// Shortcut for `std::to_underlying(status)`
    [[nodiscard]] static constexpr stl::underlying_type_t<punycode_status> operator+(
      punycode_status const status) noexcept {
        return stl::to_underlying(status);
    }

    /**
     * Default values for punycode parameters
     * From: https://www.rfc-editor.org/rfc/rfc3492.html#section-5
     */
    struct alignas(32) punycode_options {
        punycode_uint base         = 36U;
        punycode_uint tmin         = 1U;
        punycode_uint tmax         = 26U;
        punycode_uint skew         = 38U;
        punycode_uint damp         = 700U;
        punycode_uint initial_bias = 72U;
        punycode_uint initial_n    = 0x80U;
        punycode_uint delimiter    = 0x2DU;
    };

    /**
     * This function returns the numeric value of a basic code
     * point (for use in representing integers) in the range 0 to
     * base-1, or base if cp is, does not represent a value.
     */
    template <punycode_options Options = punycode_options{}>
    static constexpr punycode_uint decode_digit(punycode_uint const code_point) noexcept {
        // NOLINTBEGIN(*-avoid-nested-conditional-operator)
        return code_point - 48 < 10   ? code_point - 22
               : code_point - 65 < 26 ? code_point - 65
               : code_point - 97 < 26 ? code_point - 97
                                      : Options.base;
        // NOLINTEND(*-avoid-nested-conditional-operator)
    }

    /**
     * This function returns the basic code point whose value
     * (when used for representing integers) is d, which needs to be in
     * the range 0 to base-1.  The lowercase form is used unless flag is
     * nonzero, in which case the uppercase form is used.  The behavior
     * is undefined if flag is nonzero and digit d has no uppercase form.
     */
    template <typename CharT = char>
    static constexpr CharT encode_digit(punycode_uint const code_point) noexcept {
        //  0...25 map to ASCII a...z or A...Z
        // 26...35 map to ASCII 0...9
        return code_point < 26 ? static_cast<CharT>(code_point + 97) : static_cast<CharT>(code_point + 22);
    }

    /**
     * Bias adaptation function
     * https://www.rfc-editor.org/rfc/rfc3492.html#section-6.1
     */
    template <punycode_options Options = punycode_options{}>
    static constexpr punycode_uint
    adapt(punycode_uint delta, punycode_uint const num_points, bool const first_time) noexcept {
        delta                = first_time ? delta / Options.damp : delta / 2;
        delta               += delta / num_points;
        punycode_uint k_val  = 0;
        while (delta > (Options.base - Options.tmin) * Options.tmax / 2) {
            delta /= Options.base - Options.tmin;
            k_val += Options.base;
        }
        return k_val + ((Options.base - Options.tmin + 1) * delta / (delta + Options.skew));
    }

    /**
     * Converts a UTF-8/16/32 input into punycode.
     *
     * We don't need to use unchecked::append(...) to append the code in the implementation,
     * since anything that we append must be in the ASCII range.
     *
     * https://www.rfc-editor.org/info/rfc3492
     * https://www.rfc-editor.org/info/rfc5891
     */
    template <punycode_options            Options = punycode_options{},
              stl::random_access_iterator IterT   = char32_t const *,
              istl::Appendable            OIterT  = std::u8string::iterator>
    [[nodiscard]] static constexpr punycode_status punycode_encode(IterT const &spos, IterT const &send, OIterT &out)
      noexcept(istl::NothrowAppendable<OIterT>) {
        using enum punycode_status;
        using enum checked::error_handling;
        using istl::iter_append;
        using char_type = stl::iter_value_t<IterT>;
        using size_type = istl::size_type_of_t<OIterT>;

        assert(send >= spos);

        // out can be an iterator
        auto const src_length = static_cast<size_type>(send - spos);
        if constexpr (istl::String<OIterT>) {
            out.reserve(src_length + out.size());
        }

        if constexpr (stl::same_as<IterT, OIterT>) {
            // Inplace encoding is not allowed.
            assert(!(out >= spos && out < send));
            // assert(out < (spos - src_length * 3) || out >= send);
            assert(out < spos - src_length || out >= send);
        }

        punycode_uint n_val       = Options.initial_n;
        punycode_uint delta       = 0;
        punycode_uint bias        = Options.initial_bias;
        size_type     handled_len = 0; // it's the number of code points that have been handled
        size_type     utf32_size  = 0;

        // ASCII characters are put in order they appear:
        for (auto pos = spos; pos != send;) {
            if (is_ascii(*pos)) {
                iter_append(out, *pos);
                ++handled_len;
                ++pos;
            } else {
                auto const code_point = checked::next_code_point<return_replacement_char>(pos, send);
                if (code_point == 0) {
                    break;
                }
                ++utf32_size;
                if (!is_code_point_valid(code_point)) [[unlikely]] {
                    return bad_input;
                }
            }
        }
        utf32_size += handled_len;
        assert(utf32_size <= src_length);

        auto const basics_len = handled_len; // it's the number of basic code points
        if (basics_len > 0) {
            iter_append(out, Options.delimiter);
        }
        while (handled_len < utf32_size) {
            // Find the next larger non-ascii code point:
            punycode_uint max_m = max_legal_utf32;
            for (auto pos = spos;;) {
                auto const code_point = checked::next_code_point<return_replacement_char>(pos, send);
                if (code_point == 0) {
                    break;
                }
                if (code_point >= n_val && code_point < max_m) {
                    max_m = code_point;
                }
            }

            auto const diff = max_m - n_val;

            // Increase delta enough to advance the decoder's <n,i> state to <m,0>, but guard against overflow
            // the standard uses max-integer, but we use max-utf32
            if (diff > (max_utf32 - delta) / (handled_len + 1)) [[unlikely]] {
                return overflow;
            }
            delta += static_cast<punycode_uint>(diff * (handled_len + 1));
            n_val  = max_m;

            for (auto pos = spos;;) {
                auto const code_point = checked::next_code_point<return_replacement_char>(pos, send);
                if (code_point == 0) {
                    break;
                }

                if (code_point < n_val) {
                    if (delta == max_utf32) [[unlikely]] {
                        return overflow;
                    }
                    ++delta;
                }

                if (code_point == n_val) {
                    punycode_uint q_val = delta;
                    for (punycode_uint k_val = Options.base;; k_val += Options.base) {
                        punycode_uint const t_val =
                          k_val <= bias ? Options.tmin
                          : k_val >= bias + Options.tmax // NOLINT(*-avoid-nested-conditional-operator)
                            ? Options.tmax
                            : k_val - bias;

                        if (q_val < t_val) {
                            break;
                        }
                        auto const ascii_char =
                          encode_digit<char_type>(t_val + ((q_val - t_val) % (Options.base - t_val)));
                        iter_append(out, ascii_char);
                        q_val = (q_val - t_val) / (Options.base - t_val);
                    }
                    iter_append(out, encode_digit<char_type>(q_val));
                    bias  = adapt(delta, static_cast<punycode_uint>(handled_len + 1), handled_len == basics_len);
                    delta = 0;
                    ++handled_len;
                }
            }
            ++delta;
            ++n_val;
        }
        return success;
    }

    /**
     * Punycode Decode.
     * The output length cannot exceed the input length.
     */
    template <punycode_options            Options = punycode_options{},
              stl::random_access_iterator IterT   = char32_t const *,
              istl::Appendable            OIterT  = std::u8string::iterator>
    [[nodiscard]] static constexpr punycode_status punycode_decode(IterT spos, IterT const &send, OIterT &out)
      noexcept(istl::NothrowAppendable<OIterT>) {
        using enum punycode_status;
        using enum checked::error_handling;
        using istl::iter_append;

        auto const src_length = send - spos;
        if constexpr (istl::String<OIterT>) {
            out.reserve(src_length + out.size());
        }

        punycode_uint out_len{0};
        punycode_uint n_val = Options.initial_n;
        punycode_uint i_val = 0;
        punycode_uint bias  = Options.initial_bias;

        assert(send >= spos);
        if (spos == send) [[unlikely]] {
            return success;
        }

        // Consume all code points before the last delimiter (if there is one)
        // and copy them to output, fail on any non-basic code point
        auto last_delim = stl::prev(send);
        for (; last_delim != spos && *last_delim != Options.delimiter; --last_delim) {
            // finding the last '-' character
        }
        auto pos = spos;
        if (last_delim != spos) {
            for (;; ++pos) {
                if (pos == last_delim) {
                    if (last_delim != send) {
                        ++pos; // skip the delimiter
                    }
                    break;
                }
                if (is_ascii(*pos)) {
                    ++out_len;
                    iter_append(out, *pos);
                } else [[unlikely]] {
                    return bad_input;
                }
            }
        }

        if constexpr (!istl::String<OIterT>) {
            stl::advance(out, -static_cast<stl::int32_t>(out_len));
        }

        // Main decoding loop: Start just after the last delimiter if any
        // basic code points were copied; start at the beginning otherwise.
        while (pos != send) {
            punycode_uint const oldi  = i_val;
            punycode_uint       w_val = 1;
            punycode_uint       k_val = Options.base;

            // Decode a generalized variable-length integer into 'delta', which gets added to "i".
            // The overflow checking is easier if we increase i as we go, then subtract off its starting
            // value at the end to obtain 'delta'.
            for (;; k_val += Options.base) {
                if (pos >= send) [[unlikely]] {
                    return bad_input;
                }
                // no need for Unicode Code Point handling, a valid punycode is all ascii.
                punycode_uint const digit = decode_digit<Options>(static_cast<punycode_uint>(*pos++));
                if (digit >= Options.base) [[unlikely]] {
                    return bad_input;
                }
                if (digit > (max_utf32 - i_val) / w_val) [[unlikely]] {
                    return overflow;
                }
                i_val += digit * w_val;
                punycode_uint const t_val =
                  k_val <= bias ? Options.tmin
                  : k_val >= bias + Options.tmax // NOLINT(*-avoid-nested-conditional-operator)
                    ? Options.tmax
                    : k_val - bias;
                if (digit < t_val) {
                    break;
                }
                if (w_val > max_utf32 / (Options.base - t_val)) [[unlikely]] {
                    return overflow;
                }
                w_val *= Options.base - t_val;
            }
            bias = adapt(i_val - oldi, out_len + 1, oldi == 0);

            // "i" was supposed to wrap around from out+1 to 0,
            // incrementing n each time, so we'll fix that now:
            if (i_val / (out_len + 1) > max_utf32 - n_val) [[unlikely]] {
                return overflow;
            }
            n_val += i_val / (out_len + 1);
            i_val %= out_len + 1;
            if (n_val < 0x80) [[unlikely]] { // fail if it's ascii
                return bad_input;
            }
            if constexpr (istl::String<OIterT>) {
                checked::insert(out, i_val, n_val);
            } else {
                // todo: try to optimize this
                // todo: is using src_length okay? source and output types may not be the same!
                checked::insert_at(out, stl::next(out, src_length), i_val, n_val);
            }
            ++out_len;
            ++i_val;
        }
        if constexpr (!istl::String<OIterT>) {
            // stl::advance(out, -static_cast<int32_t>(ascii_len));
            unchecked::next_char(out, out_len);
        }
        return success;
    }

    /// Convert to punycode
    template <istl::CharType CharT = char32_t, istl::Appendable Iter = std::u8string::iterator>
    [[nodiscard]] static constexpr punycode_status to_punycode(stl::basic_string_view<CharT> src, Iter &out)
      noexcept(istl::NothrowAppendable<Iter>) {
        return punycode_encode<punycode_options{}>(src.begin(), src.end(), out);
    }

    /// Convert to unicode (from punycode)
    template <istl::CharType CharT = char32_t, istl::Appendable Iter = std::u8string::iterator>
    [[nodiscard]] static constexpr punycode_status to_unicode(stl::basic_string_view<CharT> src, Iter &out)
      noexcept(istl::NothrowAppendable<Iter>) {
        return punycode_decode<punycode_options{}>(src.begin(), src.end(), out);
    }

    /// Same as punycode_to, but it returns the resulting string; this function ignores the status of the
    /// conversions, so you may not use this function for serious work.
    template <istl::String OutStrT = std::string, istl::StringViewifiable StrVT = stl::string_view, typename... Args>
    [[nodiscard]] static constexpr OutStrT punycode_of(StrVT &&src, Args &&...args) {
        auto const src_view = istl::string_viewify(stl::forward<StrVT>(src));
        using char_type     = istl::char_type_of_t<decltype(src_view)>;
        OutStrT out{stl::forward<Args>(args)...};
        static_cast<void>(to_punycode<char_type, OutStrT>(src_view, out));
        return out;
    }

    /// Same as to_unicode, but it returns the resulting string; this function ignores the status of the
    /// conversions, so you may not use this function for serious work.
    template <istl::String OutStrT = std::string, istl::StringViewifiable StrVT = stl::string_view, typename... Args>
    [[nodiscard]] static constexpr OutStrT unicode_of(StrVT &&src, Args &&...args) {
        auto const src_view = istl::string_viewify(stl::forward<StrVT>(src));
        using char_type     = istl::char_type_of_t<decltype(src_view)>;
        OutStrT out{stl::forward<Args>(args)...};
        static_cast<void>(to_unicode<char_type, OutStrT>(src_view, out));
        return out;
    }

    // NOLINTEND(*-magic-numbers)
} // namespace webpp::unicode::idna


#endif // WEBPP_URL_PUNY_CODES_HPP
