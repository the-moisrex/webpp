#ifndef WEBPP_UNICODE_TO_ASCII_HPP
#define WEBPP_UNICODE_TO_ASCII_HPP

#include "../std/expected.hpp"
#include "../strings/to_case.hpp"
#include "./idna.hpp"
#include "./normalization.hpp"
#include "./punycodes.hpp"
#include "./validity_criteria.hpp"

#include <bit>
#include <cassert>
#include <limits>

namespace webpp::unicode::idna {


    // underlying_type_t<punycode_status> + validity_criteria_status_type
    using to_ascii_status_type = stl::uint32_t;

    namespace details {
        static constexpr std::size_t validity_criteria_shift = 11U;
    } // namespace details

    /**
     * ToASCII status values.
     * Attention: ToASCII function may return a combination of these errors
     */
    enum struct to_ascii_status : to_ascii_status_type {
        valid = 0,

        // Punycode errors:
        punycode_bad_input  = +punycode_status::bad_input,
        punycode_overflow   = +punycode_status::overflow,
        ascii_only_punycode = 0b1U << 3U,
        empty_punycode      = 0b1U << 4U,


        // More errors:
        invalid_code_point = 0b1U << 5U,
        empty_root_label   = 0b1U << 6U,
        empty_domain_label = 0b1U << 7U,
        too_long_label     = 0b1U << 8U, // the subdomain is more than 63
        too_long_domain    = 0b1U << 9U, // the whole domain is more than 253 without the last dot
        unknown            = 0b1U << 10U,

        // Validity Criteria errors:
        validity_nfc_failure   = +validity_criteria_status::nfc_failure << details::validity_criteria_shift,
        validity_hyphen_34     = +validity_criteria_status::hyphen_34 << details::validity_criteria_shift,
        validity_hyphen_around = +validity_criteria_status::hyphen_around << details::validity_criteria_shift,
        validity_ace_found     = +validity_criteria_status::ace_found << details::validity_criteria_shift,
        validity_dot_found     = +validity_criteria_status::dot_found << details::validity_criteria_shift,
        validity_combining_mark_at_start =
          +validity_criteria_status::combining_mark_at_start << details::validity_criteria_shift,
        validity_requires_mapping_failure =
          +validity_criteria_status::requires_mapping_failure << details::validity_criteria_shift,
        validity_joiner_failure = +validity_criteria_status::joiner_failure << details::validity_criteria_shift,
        validity_bidi_failure   = +validity_criteria_status::bidi_failure << details::validity_criteria_shift,
        validity_empty_label    = +validity_criteria_status::empty_label << details::validity_criteria_shift,
        bidi_domain_name = +validity_criteria_status::bidi_domain_name << details::validity_criteria_shift, // not err

        validity_criteria_failure =
          validity_nfc_failure | validity_hyphen_34 | validity_hyphen_around | validity_ace_found | validity_dot_found |
          validity_combining_mark_at_start | validity_requires_mapping_failure | validity_joiner_failure |
          validity_empty_label | validity_bidi_failure,

        // All flags (that are not states themselves)
        all_flags = bidi_domain_name,
    };

    /// Shortcut for `std::to_underlying(status)`
    [[nodiscard]] static constexpr to_ascii_status_type operator+(to_ascii_status const status) noexcept {
        return stl::to_underlying(status);
    }

    [[nodiscard]] static constexpr stl::string_view to_string(to_ascii_status const status) noexcept {
        using enum to_ascii_status;
        switch (status) {
            case valid: return {"Valid ASCII"};
            case punycode_bad_input: return {"Bad input for punycode was given"};
            case punycode_overflow: return {"Punycode overflow"};
            case ascii_only_punycode: return {"The ASCII-Only label was unnecessarily encoded into punycode"};
            case empty_punycode: return {"Empty punycode-encoded label was found"};
            case empty_root_label: return {"Empty root label found (the tailing dot)"};
            case empty_domain_label: return {"Empty domain labels are not valid"};
            case too_long_label: return {"Label was too long"};
            case too_long_domain: return {"The Domain was too long"};
            case unknown: return {"Unknown failure"};
            case invalid_code_point: return {"Invalid code point was found"};

            // Validity Criteria failures:
            case validity_nfc_failure:
            case validity_hyphen_34:
            case validity_hyphen_around:
            case validity_ace_found:
            case validity_dot_found:
            case validity_combining_mark_at_start:
            case validity_requires_mapping_failure:
            case validity_joiner_failure:
            case validity_bidi_failure:
            case validity_empty_label:
            case bidi_domain_name:
                return to_string(static_cast<validity_criteria_status>(+status >> details::validity_criteria_shift));

            case validity_criteria_failure:
                return {"Validity Criteria failure"};

            [[unlikely]] default:
                break;
        }

        // You most likely need `to_ascii_status_iterator` if you're seeing this:
        return {"<unknown-to-ascii-status>"};
    }

    /**
     * Check if the status code, has the flag you specify.
     */
    [[nodiscard]] static constexpr bool has_flag(to_ascii_status_type const status,
                                                 to_ascii_status const      flag) noexcept {
        return (status & +flag) != 0;
    }

    template <typename... T>
        requires(stl::same_as<T, to_ascii_status> && ...)
    [[nodiscard]] static constexpr bool has_flags(to_ascii_status_type const status, T const... flags) noexcept {
        return (status & (+flags | ...)) != 0;
    }

    [[nodiscard]] static constexpr bool is_valid(to_ascii_status_type const status) noexcept {
        using enum to_ascii_status;
        return (status & ~+all_flags) == +valid;
    }

    /**
     * This is a forward iterator that gives you all the errors inside a to_ascii status.
     *
     * to_ascii_status_iterator is designed to work with ranges.
     *
     * Example Usage with ranges:
     * @code
     *     // Create a string containing all the error messages:
     *     uint32_t status = ...;
     *     std::string errors_string =
     *        to_ascii_status_iterator{status} | transform([](to_ascii_status cur_status) {
     *            return to_string(cur_status);
     *        }) | join_with('\n') | to<std::string>();
     *
     *     // If status is already in type of to_ascii_status, you can just use status directly,
     *     // because std::begin(status) returns to_ascii_status_iterator:
     *     string error_string2 = status | transform(...) | join_with('\n') | to<string>();
     * @endcode
     */
    struct [[nodiscard]] to_ascii_status_iterator {
        using value_type   = to_ascii_status;
        using storage_type = stl::underlying_type_t<value_type>;

        using difference_type   = stl::ptrdiff_t;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using pointer           = value_type*;
        using const_pointer     = value_type const*;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;


        constexpr to_ascii_status_iterator() noexcept = default;

        constexpr explicit to_ascii_status_iterator(to_ascii_status const inp_status) noexcept
          : status{+inp_status},
            current{static_cast<value_type>(stl::bit_floor(status))} {}

        constexpr explicit to_ascii_status_iterator(storage_type const inp_status) noexcept
          : status{inp_status},
            current{static_cast<value_type>(stl::bit_floor(status))} {}

        constexpr to_ascii_status_iterator(to_ascii_status_iterator const&) noexcept            = default;
        constexpr to_ascii_status_iterator(to_ascii_status_iterator&&) noexcept                 = default;
        constexpr to_ascii_status_iterator& operator=(to_ascii_status_iterator const&) noexcept = default;
        constexpr to_ascii_status_iterator& operator=(to_ascii_status_iterator&&) noexcept      = default;
        constexpr ~to_ascii_status_iterator() noexcept                                          = default;

        constexpr to_ascii_status_iterator& operator++() noexcept {
            // remove the first warning
            status  &= static_cast<storage_type>(~stl::bit_floor(status));
            current  = static_cast<value_type>(stl::bit_floor(status));
            return *this;
        }

        [[nodiscard]] constexpr to_ascii_status_iterator operator++(int) noexcept {
            to_ascii_status_iterator const iter{*this};
            ++*this;
            return iter;
        }

        constexpr const_reference operator*() const noexcept {
            // return valid, error, or 0; it doesn't matter because there's only one of them
            return current;
        }

        constexpr const_pointer operator->() const noexcept {
            return &current;
        }

        constexpr pointer operator->() noexcept {
            return &current;
        }

        [[nodiscard]] constexpr bool operator==(to_ascii_status_iterator const rhs) const noexcept {
            return status == rhs.status;
        }

        [[nodiscard]] constexpr auto operator<=>(to_ascii_status_iterator const rhs) const noexcept {
            return status <=> rhs.status;
        }

      private:
        storage_type status  = 0U;
        value_type   current = to_ascii_status::valid;
    };

    [[nodiscard]] static constexpr to_ascii_status_iterator begin(to_ascii_status_iterator status) noexcept {
        return {status};
    }

    [[nodiscard]] static constexpr to_ascii_status_iterator end(
      [[maybe_unused]] to_ascii_status_iterator iter) noexcept {
        return {};
    }

    [[nodiscard]] static constexpr to_ascii_status_iterator begin(to_ascii_status const status) noexcept {
        return to_ascii_status_iterator{status};
    }

    [[nodiscard]] static constexpr to_ascii_status_iterator end([[maybe_unused]] to_ascii_status status) noexcept {
        return {};
    }

    /**
     * This class helps you get information about your string before you allocate enough storage for toASCII
     * algorithm. And also to help you check some simple things in one go.
     */
    struct [[nodiscard]] to_ascii_info {
        using flag_type  = validity_flag_type;
        using flag_types = validity_flags;

        // array<flag_types, 256>
        static constexpr auto interesting_characters = categorize<256U>(
          cat{.set = NON_ASCII_CODE_UNITS, .value = flag_types::non_ascii},
          cat{.set = ".", .value = flag_types::dot},
          cat{.set = "xX", .value = flag_types::x},
          cat{.set = "nN", .value = flag_types::n},
          cat{.set = "-", .value = flag_types::dash},
          cat{.set = UPPER_ALPHA<char8_t>, .value = flag_types::ascii_upper});

        /**
         * @returns maximum required storage length for conversion; zero if no need for conversion.
         */
        template <UTF OutCharT, stl::random_access_iterator Iter>
        [[nodiscard]] static constexpr stl::size_t operator()(Iter spos, Iter send) noexcept {
            using details::idna_default_max_len_factor;
            using inp_char_type = stl::iter_value_t<Iter>;

            auto const cur_len = adjust_utf_output_size<inp_char_type, OutCharT>(static_cast<stl::size_t>(send - spos));
            stl::size_t max_size  = (cur_len * static_cast<stl::size_t>(idna_default_max_len_factor + 3));
            max_size             += 4; // At least one UTF-8 code point

            // We're not going to apply this since the toASCII function itself may encounter undefined
            // behaviors when we don't reserve enough storage for it, and we don't want to make that algorithm
            // slower.
            return max_size;
        }
    };

    /**
     * Convert the piece of label into ASCII.
     * This algorithm is only part of ToASCII algorithm, that works on each label, not the whole domain.
     */
    template <idna_options Options, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr to_ascii_status_type
    label_to_ascii(Iter lbeg, Iter& lend, Iter const& oend, to_ascii_info::flag_type flag) noexcept {
        using enum to_ascii_status;
        using enum to_ascii_info::flag_types;
        using enum err_policy;

        assert(lbeg <= lend);
        assert(lend < oend);

        auto       status           = +valid;
        Iter const lcbeg            = lbeg;
        Iter const lcend            = lend;
        auto const src_label_length = stl::distance(lbeg, lend);
        bool const had_unicode      = has_flag(flag, non_ascii); // flags will change later

        if (had_unicode) {
            // 1.2. Normalize inplace
            Iter const cur_lend = lend;
            lend                = lbeg;
            normalize<norm_form::NFC, return_recoverable>(lbeg, cur_lend, lend);
            assert(lend <= oend); // we ran out of space.
        }

        // 1.4. Convert/Validate. For each label in the domain_name string:
        if (has_flag(flag, ace) && src_label_length >= 4 && lbeg[0] == 'x' && lbeg[1] == 'n' && lbeg[2] == '-' &&
            lbeg[3] == '-')
        {
            // Found xn--.
            // 1.4.1. If the label contains any non-ASCII code point (i.e., a Code Point greater
            // than U+007F), record that there was an error, and continue with the next label.
            if (had_unicode) [[unlikely]] {
                status |= Options.CheckDecodeAndValidateLabels ? +invalid_code_point : +valid;
            } else {
                // Decode Punycode
                // 1.4.2. Attempt to convert the rest of the label to Unicode according to Punycode [RFC3492].
                // If that conversion fails and if not IgnoreInvalidPunycode, record that there was an error,
                // and continue with the next label. Otherwise, replace the original label in the string by the
                // results of the conversion.

                // Give enough room for re-conversion
                // No need to take xn-- into account, it's already in 'src length'.
                // todo: optimize this to use UTF-32 storage since it's completely temporary
                auto const max_punycode_len = src_label_length * (4 - 1);
                auto       plend            = stl::next(lend, max_punycode_len);
                auto const plbeg            = plend;
                auto const pun_status       = punycode_decode(stl::next(lcbeg, 4), lcend, plend);
                auto const new_label_len    = stl::distance(plbeg, plend);
                assert(plend <= oend);

                if (pun_status != punycode_status::success) [[unlikely]] {
                    return Options.CheckInvalidPunycode && Options.CheckDecodeAndValidateLabels ? +pun_status : +valid;
                }

                // Replace the original label with the result of the conversion:
                lbeg = plbeg;
                lend = plend;

                // Re-calculate the flag based on the new decoded label
                flag = or_all(to_ascii_info::interesting_characters, lbeg, lend);

                // 1.4.3. If the label is empty, or if the label contains only ASCII code points,
                // record that there was an error.
                bool const     all_ascii = !has_flag(flag, non_ascii);
                constexpr auto max_label = 63U;
                status |= Options.CheckDecodeAndValidateLabels && new_label_len == 0 ? +empty_punycode : +valid;
                status |= Options.CheckDecodeAndValidateLabels && all_ascii ? +ascii_only_punycode : +valid;
                status |= Options.VerifyDnsLength && new_label_len > max_label ? +too_long_label : +valid;

                // the label might be empty, so we have to manually add it back
                // flag |= +non_ascii;

                // All ascii labels, like `xn--ascii` or `xn--` will need to turn back to ascii without `xn--`
                if (all_ascii) [[unlikely]] {
                    lend = stl::copy(lbeg, lend, lcbeg);
                    lbeg = lcbeg;
                }
            }
        }

        // 1.4.4. Verify that the label meets the validity criteria in Section 4.1, Validity Criteria.
        // If any of the validity criteria are not satisfied, record that there was an error.
        //
        // Here we convert the status returned from validity criteria function to our own:
        status |= static_cast<to_ascii_status_type>(
          label_validity_status<Options>(lbeg, lend, flag) << details::validity_criteria_shift);

        // Early bailout
        if (!is_valid(status & ~+validity_bidi_failure)) [[unlikely]] {
            return status;
        }


        // 3. Encode Punycode
        // Converts each label with non-ASCII characters into Punycode [RFC3492], and prefixes by “xn--”.
        // This may record an error.
        if (has_flag(flag, non_ascii)) {
            bool const rotate_required = lcbeg == lbeg;
            Iter       outend          = rotate_required ? lcend : lcbeg;
            istl::iter_append(outend, 'x', 'n', '-', '-');

            if (auto const p_status = punycode_encode(lbeg, lend, outend); p_status == punycode_status::success) {
                if (rotate_required) {
                    lend = stl::copy(lcend, outend, lcbeg);
                } else {
                    lend = outend;
                }

                assert(outend <= oend); // We ran out of space
                assert(lend <= oend);   // We ran out of space
            } else [[unlikely]] {
                status |= Options.CheckInvalidPunycode ? +p_status : +valid;
            }
        }

        return status;
    }

    /**
     * The ToASCII operation takes a sequence of Unicode code points that
     * make up one label and transforms it into a sequence of code points in
     * the ASCII range (0..7F).  If ToASCII succeeds, the original sequence
     * and the resulting sequence are equivalent labels.
     *  - From RFC 3490
     *
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     *  Steps From: https://www.unicode.org/reports/tr46/#Processing
     *    Used by:  https://url.spec.whatwg.org/#idna
     */
    template <idna_options                Options = idna_options{},
              stl::random_access_iterator Iter,
              stl::random_access_iterator OIter>
    [[nodiscard]] static constexpr to_ascii_status_type
      to_ascii(Iter ipos, Iter const& iend, OIter& out, stl::size_t out_len) noexcept {
        using enum to_ascii_status;
        using enum to_ascii_info::flag_types;
        using enum err_policy;
        using unicode::norm_form;
        using flag_type     = to_ascii_info::flag_type;
        using diff_type     = stl::iter_difference_t<Iter>;
        using out_char_type = stl::iter_value_t<OIter>;

        constexpr auto max_label  = static_cast<stl::uint16_t>(63U);
        constexpr auto max_domain = static_cast<stl::uint16_t>(253U);

        // We can't rely on finding dots and using them as label lengths since this is before IDNA Mapping
        // takes place and here, the dots may be in Unicode. But, if the dots are in Unicode, then we
        // consider the whole string as one big label.
        // flag_type const flags = or_all(to_ascii_info::interesting_characters, ipos, iend);

        // Normalization is guaranteed to not require more space than 3 times the input.
        // If VerifyDnsLength is needed, IDNA Mapping will require no more than 254 max size
        // Otherwise, the max size is essentially unlimited or limited by integer overflows.

        auto const    src_length   = stl::distance(ipos, iend);
        auto          status       = +valid;
        OIter const   out_beg      = out;
        auto const    out_end      = stl::next(out, static_cast<diff_type>(out_len));
        flag_type     label_flags  = 0U;
        OIter         label_start  = out_beg;
        stl::uint16_t accum_length = 0;

        // If output is in between the input, it's a disaster waiting to happen.
        if constexpr (stl::same_as<Iter, OIter>) {
            assert(!(out >= ipos && out < iend));
        }
        assert(src_length < stl::numeric_limits<stl::uint32_t>::max());
        assert(out_len < stl::numeric_limits<stl::uint32_t>::max());

        // 1. Processing
        // https://www.unicode.org/reports/tr46/#Processing
        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        while (ipos != iend) {
            auto const unit = *ipos;

            // we're using char32_t so by accident we won't accept big code points as valid,
            // and also we don't want to have multiple versions of this in the executable and create bloatware.
            constexpr auto lower_ascii = ALL_ASCII<char32_t>.except(UPPER_ALPHA<char32_t>).except(charset{U'.'});
            if (lower_ascii.contains(static_cast<char32_t>(unit))) [[likely]] {
                label_flags |= or_one(to_ascii_info::interesting_characters, unit);
                // this cast is safe since they're all guaranteed to be ASCII values and can be hold in a char8_t
                *out++       = static_cast<out_char_type>(*ipos++);
                continue;
            }

            // Handle Unicode code points:
            if (unit != '.') [[unlikely]] {
                auto code_point = checked::next_code_point<return_replacement>(ipos, iend);

                // 1.1 Map (and/or copy to output)
                OIter c_out = out;
                idna::map(code_point, out);

                // First mapped/decomposed/composed code point.
                // Don't use unchecked version since we might have removed/ignored a code point.
                code_point = checked::next_code_point(c_out, out);

                if (code_point != U'.') {
                    label_flags |= or_one(to_ascii_info::interesting_characters, code_point);
                    continue;
                }

                // A Unicode label separator was found, and was mapped to "dot".
                --out;
            } else {
                ++ipos;
            }

            // don't worry about length being longer than uint16_t, it'll require it to be more than the max
            // size for that to happen.
            auto const label_length  = stl::distance(label_start, out);
            accum_length            |= static_cast<stl::uint16_t>(stl::min<diff_type>(label_length, max_label + 1));
            auto lstatus             = label_to_ascii<Options>(label_start, out, out_end, label_flags);
            status                  |= lstatus;
            lstatus                 &= ~+validity_bidi_failure;
            if (!is_valid(lstatus)) [[unlikely]] {
                if (lstatus != +invalid_code_point) {
                    // early bailout
                    return status;
                }
            }

            // 6. Join the labels using U+002E FULL STOP as a separator and return the result
            label_flags = 0U; // reset the flags because we're going into the next label
            *out++      = U'.';
            label_start = out;
        }

        // Convert the last label as well:
        status |= label_to_ascii<Options>(label_start, out, out_end, label_flags);

        // Validity Criteria are only need to be checked if the domain is a "Bidi Domain Names";
        // So, if the domain (the whole domain and not just a label) is not a bidi domain name, then we
        // need to remove the unnecessary error.
        // We're doing this so we don't have to do 2 passes to figure this out.
        //
        // Check if bidi_failure exists, but bidi_domain_name does not:
        if constexpr (Options.CheckBidi) {
            if ((status & (+bidi_domain_name | +validity_bidi_failure)) == +validity_bidi_failure) [[unlikely]] {
                status &= ~+validity_bidi_failure;
            }
        }

        // 4. VerifyDnsLength
        if constexpr (Options.VerifyDnsLength) {
            // No need to bailout early
            // The punycode-encoded labels have these restrictions as well
            auto const last_label_len       = stl::distance(label_start, out);
            auto const domain_len           = stl::distance(out_beg, out);
            bool const has_empty_root_label = (domain_len != 0 && *stl::prev(out) == '.');

            accum_length |= static_cast<stl::uint16_t>(stl::min<diff_type>(last_label_len, max_label + 1));
            status       |= accum_length == 0 ? +empty_domain_label : +valid;
            status       |= accum_length > max_label ? +too_long_label : +valid;
            // When VerifyDnsLength is true, the empty root label is disallowed.
            status       |= has_empty_root_label ? +empty_root_label : +valid;
            status       |= domain_len > max_domain && (domain_len != max_domain + 1 || !has_empty_root_label)
                              ? +too_long_domain
                              : +valid;
        }


        // Implementations are advised to apply additional tests to these labels, such as those described in
        // Unicode Technical Report #36, Unicode Security Considerations [UTR36] and Unicode Technical
        // Standard #39, Unicode Security Mechanisms [UTS39], and take appropriate actions. For example, a
        // label with mixed scripts or confusables may be called out in the UI. Note that the use of Punycode
        // to signal problems may be counter-productive, as described in [UTR36].

        // 5. If an error was recorded in steps 1-4, then the operation has failed and a failure value is
        // returned. No DNS lookup should be done.
        if (!is_valid(status)) [[unlikely]] {
            out = out_beg;
        }
        *out = '\0';
        return status;
    }

    template <idna_options                Options = idna_options{},
              stl::random_access_iterator Iter,
              istl::String                StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(Iter spos, Iter const send, StrT& out) {
        using output_char_type      = istl::char_type_of_t<StrT>;
        to_ascii_status_type status = 0;
        to_ascii_info        info;
        auto const           max_size = info.operator()<output_char_type>(spos, send);
        istl::resize_and_overwrite(
          out,
          max_size,
          [&](output_char_type* buf, stl::size_t const max_len) constexpr noexcept {
              auto const beg = buf;
              status         = to_ascii<Options>(spos, send, buf, max_len);
              auto const len = static_cast<stl::size_t>(buf - beg);
              assert(len <= max_len); // let's not rely on -D_GLIBCXX_ASSERTS or -D_GLIBCXX_DEBUG
              return len;
          });
        return status;
    }

    template <idna_options Options = idna_options{}, istl::StringViewifiable StrVT, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(StrVT&& src, StrT& out) {
        auto const src_v = istl::string_viewify(stl::forward<StrVT>(src));
        return to_ascii<Options>(src_v.begin(), src_v.end(), out);
    }

    [[nodiscard]] static constexpr bool operator==(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs == static_cast<to_ascii_status_type>(rhs);
    }

    [[nodiscard]] static constexpr bool operator!=(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs != static_cast<to_ascii_status_type>(rhs);
    }

#ifdef __cpp_lib_expected
    template <istl::String            OutStrT = stl::u8string,
              idna_options            Options = idna_options{},
              istl::StringViewifiable StrT,
              typename... Args>
    [[nodiscard]] static constexpr stl::expected<OutStrT, to_ascii_status_type> to_ascii(StrT&& src, Args&&... args) {
        auto const str = istl::string_viewify(stl::forward<StrT>(src));
        OutStrT    out{stl::forward<Args>(args)...};
        auto const status = to_ascii<Options>(str.begin(), str.end(), out);
        if (is_valid(status)) [[likely]] {
            return out;
        }
        return stl::unexpected{status};
    }

    template <istl::String OutStrT>
    [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const& lhs,
                                                   to_ascii_status const                               rhs) noexcept {
        to_ascii_status_type const status = lhs.has_value() ? +to_ascii_status::valid : lhs.error();
        return status == static_cast<to_ascii_status_type>(rhs);
    }

    template <istl::String OutStrT, istl::StringViewifiable StrV>
    [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const& lhs,
                                                   StrV&&                                              rhs) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrV>(rhs));
        if (lhs.has_value()) {
            return lhs.value() == str;
        }
        return false;
    }
#endif

    [[nodiscard]] static constexpr to_ascii_status_type operator&(
      to_ascii_status_type const lhs,
      to_ascii_status const      rhs) noexcept {
        return lhs & static_cast<to_ascii_status_type>(rhs);
    }

    [[nodiscard]] static constexpr to_ascii_status_type operator&(
      to_ascii_status const      lhs,
      to_ascii_status_type const rhs) noexcept {
        return static_cast<to_ascii_status_type>(lhs) & rhs;
    }



} // namespace webpp::unicode::idna

#endif // WEBPP_UNICODE_TO_ASCII_HPP
