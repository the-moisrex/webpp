// Created by moisrex on 10/8/20.

#ifndef WEBPP_STRING_TOKENIZER_HPP
#define WEBPP_STRING_TOKENIZER_HPP

#include "../common/meta.hpp"
#include "../std/cassert.hpp"
#include "../std/string_view.hpp"
#include "charset.hpp"

namespace webpp {

    enum struct string_tokenizer_options : stl::uint8_t {
        // Specifies the delimiters should be returned as tokens
        return_delims = 1U << 0U,

        // Specifies that empty tokens should be returned. Treats the beginning and
        // ending of the string as implicit delimiters, though doesn't return them
        // as tokens if "return_delims" is also used.
        return_empty_tokens = 1U << 1U,
    };

    /*
     * basic_string_tokenizer is a simple string tokenizer class.  It works like an
     * iterator that with each step (see the Advance method) updates members that
     * refer to the next token in the input string.  The user may optionally
     * configure the tokenizer to return delimiters.
     *
     * EXAMPLE 1:
     *
     *   char input[] = "this is a test";
     *   c_string_tokenizer t(input, input + strlen(input), " ");
     *   while (t.next()) {
     *     printf("%s\n", t.token().c_str());
     *   }
     *
     * Output:
     *
     *   this
     *   is
     *   a
     *   test
     *
     *
     * EXAMPLE 2:
     *
     *   std::string_view input = "no-cache=\"foo, bar\", private";
     *   while (t.next(charset{',', ' '}, charset{'"'})) {
     *     printf("%s\n", t.token().c_str());
     *   }
     *
     * Output:
     *
     *   no-cache="foo, bar"
     *   private
     *
     *
     * EXAMPLE 3:
     *
     *   bool next_is_option = false, next_is_value = false;
     *   std::string input = "text/html; charset=UTF-8; foo=bar";
     *   string_tokenizer t(input);
     *   while (t.next<string_tokenizer_options::return_delims>("; =")) {
     *     if (t.token_is_delim()) {
     *       switch (*t.token_begin()) {
     *         case ';':
     *           next_is_option = true;
     *           break;
     *         case '=':
     *           next_is_value = true;
     *           break;
     *       }
     *     } else {
     *       const char* label;
     *       if (next_is_option) {
     *         label = "option-name";
     *         next_is_option = false;
     *       } else if (next_is_value) {
     *         label = "option-value";
     *         next_is_value = false;
     *       } else {
     *         label = "mime-type";
     *       }
     *       printf("%s: %s\n", label, t.token().c_str());
     *     }
     *   }
     *
     */
    template <typename StringViewType = stl::string_view,
              typename ConstIterType  = typename StringViewType::const_iterator>
    class string_tokenizer {
        // these are the options that are used internally
        enum hidden_options : stl::uint8_t {
            // Enabled = Expect anything until you find the delimiter
            // Disabled = Expect the delimiters until you find anything else
            allow_chars = 1U << 2U
        };

      public:
        using str_v          = StringViewType;
        using const_iterator = ConstIterType;
        using char_type      = typename str_v::value_type;

        static constexpr bool is_raw_pointer_iterator = stl::is_pointer_v<const_iterator>;

        constexpr explicit string_tokenizer(str_v str) noexcept
          : _token_begin{str.begin()},
            _token_end{str.begin()},
            _end{str.end()} {}

        constexpr string_tokenizer(const_iterator str_begin, const_iterator str_end) noexcept
          : _token_begin{str_begin},
            _token_end{str_begin},
            _end{str_end} {}

        // Call this method to advance the tokenizer to the next delimiter.  This
        // returns false if the tokenizer is complete.  This method must be called
        // before calling any of the token* methods.
        template <stl::uint8_t Options = 0, CharSet DelimsT = decltype(NULL_CHAR<char_type>)>
        [[nodiscard]] constexpr bool next(DelimsT&& delims = NULL_CHAR<char_type>) noexcept {
            if constexpr (Options == 0) {
                return quick_next(stl::forward<DelimsT>(delims));
            } else {
                return full_next<Options>(stl::forward<DelimsT>(delims), NULL_CHAR<char_type>);
            }
        }

        template <stl::uint8_t Options = 0, CharSet DelimsT, CharSet QuotesT>
        [[nodiscard]] constexpr bool next(DelimsT&& delims, QuotesT&& quotes) noexcept {
            return full_next<Options>(stl::forward<DelimsT>(delims), stl::forward<QuotesT>(quotes));
        }

        /**
         * Expect the allowed characters and put them in the specified output if found.
         */
        template <CharSet AllowedCharsT>
        constexpr bool expect(AllowedCharsT const& allowed_chars) noexcept {
            bool found   = false;
            _token_begin = _token_end;
            if (_token_end != _end && allowed_chars.contains(*_token_end)) {
                found = true;
                ++_token_end;
            }
            while (_token_end != _end && allowed_chars.contains(*_token_end)) {
                ++_token_end;
            }
            return found;
        }

        /**
         * Expect the allowed characters and put them in the specified output if found.
         */
        template <CharSet AllowedCharsT, typename StrT>
        constexpr bool expect(AllowedCharsT&& allowed_chars, StrT& out) noexcept(noexcept(token(out))) {
            bool const res = expect(stl::forward<AllowedCharsT>(allowed_chars));
            if (res) {
                token(out);
            }
            return res;
        }

        template <CharSet AllowedCharsT, CharSet QuotesT, typename StrT>
        constexpr bool expect(AllowedCharsT&& allowed_chars, QuotesT&& quotes, StrT& out)
          noexcept(noexcept(token(out))) {
            webpp_static_constexpr stl::uint8_t Options = hidden_options::allow_chars;
            return next<Options>(stl::forward<AllowedCharsT>(allowed_chars),
                                 stl::forward<QuotesT>(quotes),
                                 out);
        }

        template <CharSet AllowedCharsT, CharSet QuotesT, typename StrT, typename ErrorType>
        constexpr bool
        expect(AllowedCharsT&& allowed_chars, QuotesT&& quotes, StrT& out, ErrorType& err, ErrorType err_val)
          noexcept(noexcept(token(out))) {
            webpp_static_constexpr stl::uint8_t Options = hidden_options::allow_chars;
            return next<Options>(
              stl::forward<AllowedCharsT>(allowed_chars),
              stl::forward<QuotesT>(quotes),
              out,
              err,
              err_val);
        }

        template <CharSet AllowedCharsT, typename StrT, typename ErrorType>
        constexpr bool expect(AllowedCharsT&& allowed_chars, StrT& out, ErrorType& err, ErrorType err_val)
          noexcept(noexcept(token(out))) {
            bool const res = expect(stl::forward<AllowedCharsT>(allowed_chars), out);
            if (!res) {
                err = err_val;
            }
            return res;
        }

        template <CharSet AllowedCharsT, typename ErrorType>
        constexpr bool expect(AllowedCharsT&& allowed_chars, ErrorType& err, ErrorType err_val) noexcept {
            bool const res = expect(stl::forward<AllowedCharsT>(allowed_chars));
            if (!res) {
                err = err_val;
            }
            return res;
        }

        // Expect anything but the specified charset
        constexpr void skip_but(CharSet auto&& chars) noexcept {
            while (_token_end != _end && !chars.contains(*_token_end)) {
                ++_token_end;
            }
            _token_begin = _token_end;
        }

        constexpr void skip(CharSet auto&& chars) noexcept {
            while (_token_end != _end && chars.contains(*_token_end)) {
                ++_token_end;
            }
            _token_begin = _token_end;
        }

        /**
         * Skip charset 1, then skip charset 2
         * This method is different from calling .skip with a combined chars, meaning these 2 examples
         * are not the same thing even though they might act the same way some times:
         * @code
         *   tok.skip(charset{' ', ','});
         *   tok.skip(charset{','}, charset{' '});
         * @endcode
         */
        template <CharSet ExpectedCharsT1, CharSet... ExpectedCharsTs>
        constexpr void skip(ExpectedCharsT1&& chars1, ExpectedCharsTs&&... chars) noexcept {
            skip(stl::forward<ExpectedCharsT1>(chars1));
            (skip(stl::forward<ExpectedCharsTs>(chars)), ...);
        }

        constexpr void skip_spaces() noexcept {
            skip(charset<char_type, 1>(' '));
        }

        constexpr void reset(const_iterator string_begin, const_iterator string_end) noexcept {
            _token_begin = string_begin;
            _token_end   = string_begin;
            _end         = string_end;
            _is_delim    = true;
        }

        // Start iterating through tokens from the beginning of the string.
        constexpr void reset(const_iterator string_begin) noexcept {
            _token_end   = string_begin;
            _token_begin = string_begin;
            _token_end   = string_begin;
            _is_delim    = true;
        }

        /**
         * Call next and get the token
         *   - if you found it, put it in the `out` variable,
         *   - otherwise, put `err_value` to `err`
         *
         * @tparam StrT String|StringView
         * @tparam ErrorType Enum
         * @param delims Delimiters
         * @param out output string
         * @param err
         * @param err_value
         */
        template <stl::uint8_t Options = 0, CharSet DelimsT, typename StrT, typename ErrorType>
        constexpr bool next(DelimsT&& delims, StrT& out, ErrorType& err, ErrorType err_value)
          noexcept(noexcept(token(out))) {
            if (next<Options>(stl::forward<DelimsT>(delims))) {
                token(out);
                return true;
            }
            err = err_value;
            return false;
        }

        // same as other "next", except that it gets a "quotes" as well
        template <stl::uint8_t Options = 0,
                  CharSet      DelimsT,
                  CharSet      QuotedCharsT,
                  typename StrT,
                  typename ErrorType>
        constexpr bool
        next(DelimsT&& delims, QuotedCharsT&& quotes, StrT& out, ErrorType& err, ErrorType err_value)
          noexcept(noexcept(token(out))) {
            if (next<Options>(stl::forward<DelimsT>(delims), stl::forward<QuotedCharsT>(quotes))) {
                token(out);
                return true;
            }
            err = err_value;
            return false;
        }

        /**
         * Call next and get the token
         *
         * @tparam StrT String|StringView
         * @param delims Delimiters
         * @param out output string
         * @return true if we found the token
         */
        template <stl::uint8_t Options = 0, CharSet DelimsT, typename StrT>
        constexpr bool next(DelimsT&& delims, StrT& out) noexcept(noexcept(token(out))) {
            if (next<Options>(stl::forward<DelimsT>(delims))) {
                token(out);
                return true;
            }
            return false;
        }

        // same as other "next", except that it accepts a `quotes` as well
        template <stl::uint8_t Options = 0, CharSet DelimsT, CharSet QuotedCharsT, typename StrT>
        constexpr bool next(DelimsT&& delims, QuotedCharsT&& quotes, StrT& out)
          noexcept(noexcept(token(out))) {
            if (next<Options>(stl::forward<DelimsT>(delims), stl::forward<QuotedCharsT>(quotes))) {
                token(out);
                return true;
            }
            return false;
        }

        // Returns true if token is a delimiter.  When the tokenizer is constructed
        // with the return_delims option, this method can be used to check if the
        // returned token is actually a delimiter. Returns true before the first
        // time next() has been called, and after next() returns false.
        [[nodiscard]] constexpr bool token_is_delim() const noexcept {
            return _is_delim;
        }

        // If next() returned true, then these methods may be used to read the
        // value of the token.
        [[nodiscard]] constexpr const_iterator token_begin() const noexcept {
            return _token_begin;
        }

        [[nodiscard]] constexpr const_iterator token_end() const noexcept {
            return _token_end;
        }

        [[nodiscard]] constexpr str_v token() const noexcept {
            if constexpr (is_raw_pointer_iterator) {
                return str_v{_token_begin, static_cast<stl::size_t>(_token_end - _token_begin)};
            } else {
                return str_v{_token_begin, _token_end};
            }
        }

        // Append to a "String"
        template <typename StrT = str_v>
            requires requires(str_v str, StrT out) { out.operator+=(str); }
        constexpr void token(StrT& out) const noexcept(noexcept(out.operator+=(token()))) {
            out += token();
        }

        // output to a "StringView"
        template <typename StrT = str_v>
            requires(!requires(str_v str, StrT out) { out.operator+=(str); })
        constexpr void token(StrT& out) const noexcept {
            out = token();
        }

        constexpr void skip_token() noexcept {
            _token_begin = _token_end;
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return _token_end == _end;
        }

      private:
        // Implementation of next() for when we have no quote characters. We have
        // two separate implementations because advance_one() is a hot spot in large
        // text files with large tokens.
        constexpr bool quick_next(CharSet auto&& delims) noexcept {
            _is_delim = false;
            for (_token_begin = _token_end;; ++_token_begin) {
                if (_token_begin == _end) {
                    _is_delim = true;
                    return false;
                }
                if (!delims.contains(*_token_begin)) {
                    break;
                }
            }
            _token_end = _token_begin;
            ++_token_end;
            while (_token_end != _end && !delims.contains(*_token_end)) {
                ++_token_end;
            }
            return true;
        }

        // Implementation of next() for when we have to take quotes into account.
        template <stl::uint8_t Options>
        constexpr bool full_next(CharSet auto&& delims, CharSet auto&& quotes) noexcept {
            advance_state state;

            for (;;) {
                if (_is_delim) {
                    // Last token was a delimiter. Note: This is also the case at the start.
                    //
                    //    ... D T T T T D ...
                    //        ^ ^
                    //        | |
                    //        | |_token_end| : The next character to look at or |end_|.
                    //        |
                    //        |_token_begin| : Points to delimiter or |_token_end|.
                    //
                    // The next token is always a non-delimiting token. It could be empty, however.
                    _is_delim    = false;
                    _token_begin = _token_end;

                    // Slurp all non-delimiter characters into the token.
                    while (_token_end != _end && advance_one<Options>(delims, quotes, &state, *_token_end)) {
                        ++_token_end;
                    }

                    // If it's non-empty, or empty tokens were requested, return the token.
                    // NOLINTBEGIN(bugprone-branch-clone)
                    if constexpr (
                      Options & static_cast<stl::uint8_t>(string_tokenizer_options::return_empty_tokens))
                    {
                        return true;
                    } else if (_token_begin != _token_end) {
                        return true;
                    }
                    // NOLINTEND(bugprone-branch-clone)
                }

                // Last token was a regular token.
                //
                //    ... T T T D T T ...
                //        ^     ^
                //        |     |
                //        |     _token_end : The next character to look at. Always one
                //        |                  char beyond the token boundary.
                //        |
                //        _token_begin : Points to beginning of token. Note: token could
                //                       be empty, in which case
                //                       _token_begin == _token_end.
                //
                // The next token is always a delimiter. It could be |end_| however, but
                // |end_| is also an implicit delimiter.
                _is_delim    = true;
                _token_begin = _token_end;

                if (_token_end == _end) {
                    return false;
                }

                // Look at the delimiter.
                ++_token_end;
                if constexpr (Options & static_cast<stl::uint8_t>(string_tokenizer_options::return_delims)) {
                    return true;
                }
            }
        }

        struct advance_state {
            char_type quote_char = '\0';
            bool      in_quote   = false;
            bool      in_escape  = false;
        };

        // Returns true if a delimiter was not hit.
        // Returns true if a allowed_chars (delims) was hit (if the hidden_options::allow_chars is set)
        template <stl::uint8_t Options = 0>
        static constexpr bool advance_one(
          CharSet auto&& delims,
          CharSet auto&& quotes,
          advance_state* state,
          char_type      inp_char) noexcept {
            webpp_static_constexpr bool hit =
              !(Options & static_cast<stl::uint8_t>(hidden_options::allow_chars));
            if (state->in_quote) {
                if (state->in_escape) {
                    state->in_escape = false;
                } else if (inp_char == '\\') {
                    state->in_escape = true;
                } else if (inp_char == state->quote_char) {
                    state->in_quote = false;
                }
            } else {
                if (delims.contains(inp_char)) {
                    return !hit;
                }
                state->in_quote = quotes.contains(state->quote_char = inp_char);
            }
            return hit;
        }

        const_iterator _token_begin;
        const_iterator _token_end;
        const_iterator _end;
        bool           _is_delim = true;
    };

} // namespace webpp

#endif // WEBPP_STRING_TOKENIZER_HPP
