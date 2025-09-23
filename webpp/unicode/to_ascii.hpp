#ifndef WEBPP_UNICODE_TO_ASCII_HPP
#define WEBPP_UNICODE_TO_ASCII_HPP

#include "../std/expected.hpp"
#include "../strings/to_case.hpp"
#include "./bidi.hpp"
#include "./general_category.hpp"
#include "./idna.hpp"
#include "./joiners.hpp"
#include "./normalization.hpp"
#include "./punycodes.hpp"
#include "./validity_criteria.hpp"

#include <bit>
#include <cassert>
#include <limits>

namespace webpp::unicode::idna {


    // underlying_type_t<punycode_status> + validity_criteria_status_type
    using to_ascii_status_type = stl::uint32_t;

    /**
     * ToASCII status values.
     * Attention: ToASCII function may return a combination of these errors
     */
    enum struct to_ascii_status : to_ascii_status_type {
        valid = 0,

        // Punycode errors:
        invalid_code_point             = +punycode_status::bad_input,
        punycode_overflow              = +punycode_status::overflow,
        ascii_only_punycode            = 0b1U << 3U,
        empty_punycode                 = 0b1U << 4U,
        non_normalized_punycode        = 0b1U << 5U,
        punycode_requires_idna_mapping = 0b1U << 6U,


        // More errors:
        empty_domain_label = 0b1U << 7U,
        too_long_label     = 0b1U << 8U, // the subdomain is more than 63
        too_long_domain    = 0b1U << 9U, // the whole domain is more than 253 without the last dot
        unknown            = 0b1U << 10U,

        // Validity Criteria errors:
        validity_nfc_failure             = +validity_criteria_status::nfc_failure << 11U,
        validity_hyphen_34               = +validity_criteria_status::hyphen_34 << 11U,
        validity_hyphen_around           = +validity_criteria_status::hyphen_around << 11U,
        validity_ace_found               = +validity_criteria_status::ace_found << 11U,
        validity_dot_found               = +validity_criteria_status::dot_found << 11U,
        validity_combining_mark_at_start = +validity_criteria_status::combining_mark_at_start << 11U,
        validity_status_values_failure   = +validity_criteria_status::status_values_failure << 11U,
        validity_joiner_failure          = +validity_criteria_status::joiner_failure << 11U,
        validity_bidi_failure            = +validity_criteria_status::bidi_failure << 11U,
        bidi_domain_name                 = +validity_criteria_status::bidi_domain_name << 11U, // flag, not an error

        validity_criteria_failure =
          validity_nfc_failure | validity_hyphen_34 | validity_hyphen_around | validity_ace_found | validity_dot_found |
          validity_combining_mark_at_start | validity_status_values_failure | validity_joiner_failure |
          validity_bidi_failure,

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
            case invalid_code_point: return {"Bad input for punycode was given"};
            case punycode_overflow: return {"Punycode overflow"};
            case ascii_only_punycode: return {"The ASCII-Only label was unnecessarily encoded into punycode"};
            case empty_punycode: return {"Empty punycode-encoded label was found"};
            case non_normalized_punycode: return {"The punycode-encoded label was not in NFC form"};
            case punycode_requires_idna_mapping: return {"The punycode-encoded label requires IDNA mapping"};
            case empty_domain_label: return {"Empty domain labels are not valid"};
            case too_long_label: return {"Label was too long"};
            case too_long_domain: return {"The Domain was too long"};
            case unknown: return {"Unknown failure"};

            // Validity Criteria failures:
            case validity_nfc_failure:
            case validity_hyphen_34:
            case validity_hyphen_around:
            case validity_ace_found:
            case validity_dot_found:
            case validity_combining_mark_at_start:
            case validity_status_values_failure:
            case validity_joiner_failure:
            case validity_bidi_failure:
            case bidi_domain_name: return to_string(static_cast<validity_criteria_status>(+status >> 11U));

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
    struct to_ascii_status_iterator {
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
     * algorithm.
     */
    struct to_ascii_info {
        using flag_type = stl::uint_fast8_t;
        enum struct flag_types : flag_type {
            // ASCII and Non-ASCII:
            non_ascii   = 0b1000U,
            ascii       = 0b1'0000U,
            ascii_upper = 0b10'0000U | ascii,
            dot         = 0b100'0000U | ascii,

            // xn-- (Called ACE Prefix):
            x    = 0b1U | ascii,
            n    = 0b10U | ascii,
            dash = 0b100U | ascii,
            ace  = x | n | dash, // ACE prefix

            // Misc:
            clean         = static_cast<flag_type>(~dot | ascii),
            length_police = (dot | non_ascii) & ~ascii,
            ascii_mask    = non_ascii | ascii | ascii_upper,
            all           = 0b1111'1111U, // all possibilities
        };

        // array<flag_types, 256>
        static constexpr auto interesting_characters = categorize<256U>(
          cat{.set = ".", .value = flag_types::dot},
          cat{.set = "xX", .value = flag_types::x},
          cat{.set = "nN", .value = flag_types::n},
          cat{.set = "-", .value = flag_types::dash},
          cat{.set = NON_ASCII_CODE_UNITS, .value = flag_types::non_ascii},
          cat{.set = ALL_ASCII<char8_t>, .value = flag_types::ascii},
          cat{.set = UPPER_ALPHA<char8_t>, .value = flag_types::ascii_upper});


        stl::size_t max_size = 0; // not adjusted to the output size if the input and output's character types
                                  // are different.

        [[nodiscard]] static constexpr stl::uint8_t best_factor_of(char32_t const code_point) noexcept {
            constexpr stl::uint32_t split = 24U;
            constexpr stl::uint32_t mask  = (0b1U << split) - 1U;
            auto const              inf   = details::idna_max_len_factors[code_point % details::idna_rem];
            if ((inf & mask) == code_point) [[unlikely]] {
                return static_cast<stl::uint8_t>(inf >> split);
            }
            return details::idna_default_max_len_factor;
        }

        /**
         * @returns maximum required storage length for conversion; zero if no need for conversion.
         */
        template <UTF OutCharT, stl::random_access_iterator Iter>
        [[nodiscard]] constexpr flag_type operator()(Iter spos, Iter send) noexcept {
            using enum flag_types;
            using enum checked::error_handling;
            using details::idna_default_max_len_factor;
            using stl::to_underlying;
            using inp_char_type = stl::iter_value_t<Iter>;

            auto const cur_len = adjust_utf_output_size<inp_char_type, OutCharT>(static_cast<stl::size_t>(send - spos));
            flag_type  flags   = 0U;
            stl::size_t biggest_label = 0U;
            auto        lbeg          = spos;

            max_size  = cur_len;
            max_size *= static_cast<stl::size_t>(idna_default_max_len_factor);

            // We can't rely on finding dots and using them as label lengths since this is before IDNA Mapping
            // takes place and here, the dots may be in Unicode. But, if the dots are in Unicode, then we
            // consider the whole string as one big label.
            while (spos != send) {
                flag_type const flag =
                  or_all_if(interesting_characters, spos, send, [](flag_type const cur_flag) constexpr noexcept {
                      return (cur_flag & to_underlying(length_police)) != 0;
                  });

                flags |= flag;

                if ((flag & to_underlying(dot)) == to_underlying(dot)) {
                    biggest_label = stl::max<stl::size_t>(biggest_label, static_cast<stl::size_t>(spos - lbeg));
                    lbeg          = spos;
                } else if ((flag & to_underlying(non_ascii)) != 0) {
                    // or_all_if will go past that bad code point, so we need prev(spos)
                    --spos;
                    auto const code_point = checked::next_code_point<return_negated>(spos, send);

                    // Update the max size
                    auto map_count  = adjust_utf_output_size<char32_t, OutCharT>(best_factor_of(code_point));
                    map_count      *= 4; // For punycode: each code point at max may turn into N ascii chars
                    max_size       += map_count;
                    max_size       -= idna_default_max_len_factor; // remove the default max len factor
                    max_size       += 4;                           // each label can have an ACE Prefix (xn--)
                }
            }

            biggest_label = stl::max<stl::size_t>(biggest_label, static_cast<stl::size_t>(spos - lbeg));

            // We only care about the biggest label because punycode conversions happen on each label, and the
            // biggest label would become the maximum required length for processing.
            max_size += biggest_label * 3U; // max punycode

            // We're not going to apply this since the toASCII function itself may encounter undefined
            // behaviors when we don't reserve enough storage for it, and we don't want to make that algorithm
            // slower.
            //
            // if constexpr (Options.VerifyDnsLength) {
            //     // The length of the domain name, excluding the root label and its dot, is from 1 to 253.
            //     max_size = stl::max<stl::size_t>(max_size, 254U); // NOLINT(*-magic-numbers)
            // }

            return flags;
        }
    };

    /// Shortcut for `std::to_underlying(status)`
    [[nodiscard]] static constexpr to_ascii_info::flag_type operator+(to_ascii_info::flag_types const status) noexcept {
        return stl::to_underlying(status);
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
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(
      Iter                           ipos,
      Iter const                     iend,
      OIter&                         out,
      stl::size_t                    out_len,
      to_ascii_info::flag_type const flags = +to_ascii_info::flag_types::all) noexcept {
        using enum to_ascii_status;
        using enum to_ascii_info::flag_types;
        using enum checked::error_handling;
        using istl::iter_append;
        using unicode::norm_form;
        using flag_type = to_ascii_info::flag_type;
        using diff_type = stl::iter_difference_t<OIter>;


        // Normalization is guaranteed to not require more space than 3 times the input.
        // If VerifyDnsLength is needed, IDNA Mapping will require no more than 254 max size
        // Otherwise, the max size is essentially unlimited or limited by integer overflows.

        auto const  src_length          = iend - ipos;
        auto        status              = +valid;
        OIter const out_beg             = out;
        bool const  all_ascii           = (flags & +non_ascii) == 0;
        bool const  might_have_punycode = (flags & +ace) != 0;
        bool const  all_lower_ascii     = (flags & +ascii_mask) == +ascii;
        OIter       spos                = out;
        auto        send                = stl::next(spos, src_length); // init
        auto const  oend                = stl::next(out, static_cast<diff_type>(out_len));

        // If output is in between the input, it's a disaster waiting to happen.
        if constexpr (stl::same_as<Iter, OIter>) {
            assert(!(out >= ipos && out < iend));
        }
        assert(src_length < stl::numeric_limits<stl::uint32_t>::max());
        assert(out_len < stl::numeric_limits<stl::uint32_t>::max());

        // 1. Processing
        // https://www.unicode.org/reports/tr46/#Processing
        if (all_lower_ascii) {
            stl::copy(ipos, iend, out);
            stl::advance(out, src_length);
            if (!might_have_punycode) {
                return status;
            }
        } else if (all_ascii) {
            // 1.1 ASCII Map (and/or copy to output)
            ascii::lower_to(ipos, iend, out);
            stl::advance(out, src_length);
            if (!might_have_punycode) {
                return status;
            }
        } else {
            // 1.1 Map (and/or copy to output)
            if (!idna::map(ipos, iend, out)) [[unlikely]] {
                // Disallowed code point was found
                status |= +invalid_code_point;
            }

            // 1.2. Normalize inplace
            {
                send = spos;
                normalize<norm_form::NFC>(out_beg, out, send); // inplace normalization
            }
            assert(out <= oend);                               // we ran out of space.
        }

        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        stl::uint16_t accum_length = 0;
        while (spos != send) {
            assert(spos <= send);
            auto const lcbeg = spos;
            OIter      lbeg  = spos; // start of label

            // find the label:
            flag_type flag = or_all_if(
              to_ascii_info::interesting_characters,
              spos,
              send,
              [](flag_type const cur_flags) constexpr noexcept -> bool {
                  return cur_flags >= +dot; // we found a dot
              });

            bool const contains_dot     = (flag & +dot) == +dot;
            auto const lcend            = contains_dot ? stl::prev(spos) : spos;
            OIter      lend             = lcend;
            auto const src_label_length = lend - lbeg;

            // 1.4. Convert/Validate. For each label in the domain_name string:
            switch (flag & +clean) {
                [[unlikely]] case 0:
                    // If the label is empty, or ..., record that there was an error.
                    status |= +empty_domain_label;
                    break;
                case +ace:
                    if (src_label_length >= 4 && lbeg[0] == 'x' && lbeg[1] == 'n' && lbeg[2] == '-' && lbeg[3] == '-') {
                        // Found xn--.
                        // 1.4.1. If the label contains any non-ASCII code point (i.e., a Code Point greater
                        // than U+007F), record that there was an error, and continue with the next label.
                        if ((flag & +non_ascii) != 0) [[unlikely]] {
                            status |= +invalid_code_point;
                            continue;
                        }

                        // Decode Punycode
                        // 1.4.2. Attempt to convert the rest of the label to Unicode according to Punycode
                        // [RFC3492]. If that conversion fails and if not IgnoreInvalidPunycode, record that
                        // there was an error, and continue with the next label. Otherwise, replace the
                        // original label in the string by the results of the conversion.

                        // Give enough room for re-conversion
                        // No need to take xn-- into account, it's already in 'src length'.
                        // todo: optimize this to use UTF-32 storage since it's completely temporary
                        auto const max_punycode_len = src_label_length * (4 - 1);
                        lend                        = stl::next(send, max_punycode_len);
                        lbeg                        = lend;
                        auto const pun_status       = punycode_decode(stl::next(lcbeg, 4), lcend, lend);
                        auto const new_label_len    = lend - send;
                        assert(lend <= oend);
                        if constexpr (!Options.IgnoreInvalidPunycode) {
                            if (pun_status != punycode_status::success) [[unlikely]] {
                                // restore the original label:
                                status       |= +pun_status;
                                accum_length |= static_cast<stl::uint16_t>(lend - lbeg);
                                continue;
                            }
                        }
                        flag |= +non_ascii; // make sure to re-convert it back to punycode

                        // 1.4.3. If the label is empty, or if the label contains only ASCII code points,
                        // record that there was an error.
                        if (new_label_len == 0) [[unlikely]] {
                            status |= +empty_punycode;
                        }

                        char32_t      accum   = 0;
                        stl::uint16_t map_pos = 0;
                        OIter         pos     = lbeg;


                        while (pos != lend) {
                            auto const code_point  = checked::next_code_point<return_negated>(pos, lend);
                            map_pos               |= status_of(code_point);
                            accum                  |= code_point;
                        }

                        if (is_ascii(accum)) [[unlikely]] {
                            status |= +ascii_only_punycode;
                        }

                        if (map_pos != details::valid) [[unlikely]] {
                            status |= +punycode_requires_idna_mapping;
                        }

                        // todo: optimize this into the above loop
                        if (!is_normalized<norm_form::NFC>(lbeg, lend)) [[unlikely]] {
                            status |= +non_normalized_punycode;
                        }
                    }
                    [[fallthrough]];
                [[likely]] default:
                    // 1.4.4. Verify that the label meets the validity criteria in Section 4.1, Validity
                    // Criteria. If any of the validity criteria are not satisfied, record that there was
                    // an error.
                    //
                    // Here we convert the status returned from validity criteria function to our own:
                    status |= label_validity_status<Options>(lbeg, lend) << 11U;
                    break;
            }

            // don't worry about length being longer than uint16_t, it'll require it to be more than the max
            // size for that to happen.
            accum_length |= static_cast<stl::uint16_t>(lend - lbeg);

            // 3. Encode Punycode
            // Converts each label with non-ASCII characters into Punycode [RFC3492], and prefixes by “xn--”.
            // This may record an error.
            if ((flag & +non_ascii) != 0) {
                out                = send;
                auto const tmp_beg = out;
                iter_append(out, 'x', 'n', '-', '-');
                [[maybe_unused]] auto const p_status         = punycode_encode(lbeg, lend, out);
                auto const                  out_label_length = stl::distance(tmp_beg, out);

                // We ran out of space
                assert(out <= oend);

                // Move the new generated label to its rightful place:
                lbeg = lcbeg;
                stl::rotate(lcend, tmp_beg, out);
                stl::shift_left(lbeg, out, src_label_length);
                stl::advance(out, -src_label_length);
                stl::advance(spos, out_label_length - src_label_length);
                send = out;

                if constexpr (!Options.IgnoreInvalidPunycode) {
                    if (p_status != punycode_status::success) [[unlikely]] {
                        status |= +p_status;
                    }
                }
            }


            // 6. Join the labels using U+002E FULL STOP as a separator and return the result
            // if (contains_dot) {
            //     *lcend = '.';
            // }
        }

        // Validity Criteria are only need to be checked if the domain is a "Bidi Domain Names"
        // So, if the domain (the whole domain and not just a label) is not a bidi domain name, then we
        // need to remove the unnecessary error.
        // We're doing this so we don't have to do 2 passes to figure this out.
        //
        // Check if bidi_failure exists, but bidi_domain_name does not:
        if ((status & (+bidi_domain_name | +validity_bidi_failure)) == +validity_bidi_failure) {
            status &= static_cast<to_ascii_status_type>(~+validity_bidi_failure);
        }

        // 4. VerifyDnsLength
        if constexpr (Options.VerifyDnsLength) {
            // No need to bailout early
            constexpr auto max_label   = 63U;
            constexpr auto max_domain  = 253U;
            auto const     cur_out_len = out - out_beg;
            if (accum_length > max_label) [[unlikely]] {
                status |= +too_long_label;
            }
            if (cur_out_len > max_domain && (cur_out_len != max_domain + 1 || *stl::prev(out) != '.')) [[unlikely]] {
                status |= +too_long_domain;
            }
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
        auto const           flags = info.operator()<output_char_type>(spos, send);
        istl::resize_and_overwrite(
          out,
          info.max_size,
          [&, flags](output_char_type* buf, stl::size_t const max_len) constexpr noexcept {
              auto const beg = buf;
              status         = to_ascii<Options>(spos, send, buf, max_len, flags);
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

    [[nodiscard]] static constexpr bool operator==(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs == static_cast<to_ascii_status_type>(rhs);
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

    [[nodiscard]] static constexpr bool operator!=(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs != static_cast<to_ascii_status_type>(rhs);
    }

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
