// Created by moisrex on 8/11/25.

#ifndef WEBPP_UNICODE_UTF32_ITERATOR_HPP
#define WEBPP_UNICODE_UTF32_ITERATOR_HPP

#include "./checked.hpp"

namespace webpp::unicode::checked {


    /**
     * UTF-32 Bidirectional Iterator
     */
    template <stl::bidirectional_iterator Iter,
              typename EIter               = stl::default_sentinel_t,
              err_policy ErrPolicy = err_policy::return_replacement>
        requires std::sentinel_for<EIter, Iter>
    struct [[nodiscard]] utf32_bidi_iter {
        using difference_type   = stl::iter_difference_t<Iter>;
        using value_type        = char32_t;
        using traits            = stl::iterator_traits<Iter>;
        using pointer           = typename traits::pointer;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using iterator_category = stl::bidirectional_iterator_tag;
        using iterator_concept  = stl::bidirectional_iterator_tag;
        using unit_type         = stl::iter_value_t<Iter>;

      private:
        [[no_unique_address]] istl::begin_iterator<Iter> beg{};
        [[no_unique_address]] stl::remove_cvref_t<Iter>  lpos{}; // lower-bound of the Unicode code point
        [[no_unique_address]] stl::remove_cvref_t<Iter>  upos{}; // upper-bound of the Unicode code point
        [[no_unique_address]] stl::remove_cvref_t<EIter> send{};
        value_type                                       code_point{};

      public:
        explicit constexpr utf32_bidi_iter(Iter inp_pos, EIter inp_end) noexcept
          : beg{istl::begin_sentinel(inp_pos)},
            lpos{stl::move(inp_pos)},
            upos{stl::move(inp_pos)},
            send{stl::move(inp_end)},
            code_point{checked::next_code_point<ErrPolicy>(upos, send)} {}

        constexpr utf32_bidi_iter()                                      = default;
        constexpr utf32_bidi_iter(utf32_bidi_iter const&)                = default;
        constexpr utf32_bidi_iter(utf32_bidi_iter&&) noexcept            = default;
        constexpr utf32_bidi_iter& operator=(utf32_bidi_iter const&)     = default;
        constexpr utf32_bidi_iter& operator=(utf32_bidi_iter&&) noexcept = default;
        constexpr ~utf32_bidi_iter() noexcept                            = default;

        constexpr utf32_bidi_iter& operator++() noexcept {
            lpos       = upos;
            code_point = checked::next_code_point<ErrPolicy>(upos, send);
            return *this;
        }

        constexpr utf32_bidi_iter& operator--() noexcept {
            upos       = lpos;
            code_point = checked::prev_code_point<ErrPolicy>(lpos, beg);
            return *this;
        }

        constexpr const_reference operator*() const noexcept {
            return code_point;
        }

        [[nodiscard]] constexpr utf32_bidi_iter operator--(int) noexcept {
            auto const res = utf32_bidi_iter{*this};
            operator--();
            return res;
        }

        [[nodiscard]] constexpr utf32_bidi_iter operator++(int) noexcept {
            auto const res = utf32_bidi_iter{*this};
            operator++();
            return res;
        }

        [[nodiscard]] constexpr bool operator==(utf32_bidi_iter const& other) const noexcept {
            return lpos == other.lpos;
        }

        [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
            return at_end();
        }

        [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
            requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
        {
            return at_end();
        }

        [[nodiscard]] constexpr bool operator==(istl::begin_sentinel_t) const noexcept {
            return lpos == beg;
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return lpos == send;
        }

        [[nodiscard]] constexpr Iter const& base() const noexcept {
            return lpos;
        }

        /// Return the start of the next Code Point
        [[nodiscard]] constexpr Iter const& upper_base() const noexcept {
            return upos;
        }

        constexpr stl::size_t unsafe_set(char32_t const new_code_point) noexcept {
            code_point      = new_code_point;
            Iter       cpos = lpos;
            auto const len  = unchecked::append(cpos, code_point);
            upos            = stl::next(lpos, static_cast<difference_type>(len));
            return len;
        }

        /// Return the length of the code point
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return static_cast<stl::size_t>(upos - lpos);
        }
    };

    /**
     * UTF-32 Specialization of bidirectional UTF iterator wrapper.
     */
    template <stl::bidirectional_iterator Iter, typename EIter, err_policy ErrPolicy>
        requires(UTF32<stl::iter_value_t<Iter>> && stl::sentinel_for<EIter, Iter>)
    struct [[nodiscard]] utf32_bidi_iter<Iter, EIter, ErrPolicy> {
        using difference_type   = stl::iter_difference_t<Iter>;
        using value_type        = char32_t;
        using traits            = stl::iterator_traits<Iter>;
        using pointer           = typename traits::pointer;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using iterator_category = stl::bidirectional_iterator_tag;
        using iterator_concept  = stl::bidirectional_iterator_tag;
        using unit_type         = stl::iter_value_t<Iter>;

      private:
        // todo: do we need beg in this specialization?
        [[no_unique_address]] istl::begin_iterator<Iter> beg{};
        [[no_unique_address]] stl::remove_cvref_t<Iter>  pos{};
        [[no_unique_address]] stl::remove_cvref_t<EIter> send{};
        value_type                                       code_point{};

      public:
        explicit constexpr utf32_bidi_iter(Iter inp_pos, EIter inp_end) noexcept
          : beg{istl::begin_sentinel(inp_pos)},
            pos{inp_pos},
            send{stl::move(inp_end)},
            code_point{pos != send ? checked::validate_code_point<ErrPolicy>(*pos) : 0} {}

        constexpr utf32_bidi_iter()                                      = default;
        constexpr utf32_bidi_iter(utf32_bidi_iter const&)                = default;
        constexpr utf32_bidi_iter(utf32_bidi_iter&&) noexcept            = default;
        constexpr utf32_bidi_iter& operator=(utf32_bidi_iter const&)     = default;
        constexpr utf32_bidi_iter& operator=(utf32_bidi_iter&&) noexcept = default;
        constexpr ~utf32_bidi_iter() noexcept                            = default;

        constexpr utf32_bidi_iter& operator++() noexcept {
            code_point = checked::validate_code_point<ErrPolicy>(*++pos);
            return *this;
        }

        constexpr utf32_bidi_iter& operator--() noexcept {
            code_point = checked::validate_code_point<ErrPolicy>(*--pos);
            return *this;
        }

        constexpr const_reference operator*() const noexcept {
            return code_point;
        }

        [[nodiscard]] constexpr utf32_bidi_iter operator--(int) noexcept {
            auto const res = utf32_bidi_iter{*this};
            operator--();
            return res;
        }

        [[nodiscard]] constexpr utf32_bidi_iter operator++(int) noexcept {
            auto const res = utf32_bidi_iter{*this};
            operator++();
            return res;
        }

        [[nodiscard]] constexpr bool operator==(utf32_bidi_iter const& other) const noexcept {
            return pos == other.pos;
        }

        [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
            return at_end();
        }

        [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
            requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
        {
            return at_end();
        }

        [[nodiscard]] constexpr bool operator==(istl::begin_sentinel_t) const noexcept {
            return pos == beg;
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return pos == send;
        }

        [[nodiscard]] constexpr bool at_start() const noexcept {
            return pos == beg;
        }

        [[nodiscard]] constexpr Iter const& base() const noexcept {
            return pos;
        }

        /// Return the start of the next Code Point
        [[nodiscard]] constexpr Iter upper_base() const noexcept {
            // return stl::next(*this).base();
            Iter upos = pos;
            auto _    = checked::next_code_point(upos, send);
            return upos;
        }

        constexpr stl::size_t unsafe_set(char32_t const new_code_point) noexcept {
            code_point = new_code_point;
            Iter cpos  = pos;
            return unchecked::append(cpos, code_point);
        }

        /// Return the length of the code point
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return code_point_length(pos, send);
        }
    };

    /**
     * UTF-32 Forward Iterator Wrapper.
     */
    template <stl::forward_iterator Iter,
              typename EIter               = stl::default_sentinel_t,
              err_policy ErrPolicy = err_policy::return_replacement>
        requires stl::sentinel_for<EIter, Iter>
    struct [[nodiscard]] utf32_forward_iter {
        using difference_type   = stl::iter_difference_t<Iter>;
        using value_type        = char32_t;
        using traits            = stl::iterator_traits<Iter>;
        using pointer           = typename traits::pointer;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;
        using unit_type         = stl::iter_value_t<Iter>;

      private:
        [[no_unique_address]] stl::remove_cvref_t<Iter>  pos{};
        [[no_unique_address]] stl::remove_cvref_t<EIter> send{};
        value_type                                       code_point{};

      public:
        explicit constexpr utf32_forward_iter(Iter inp_pos, EIter inp_end) noexcept
          : pos{stl::move(inp_pos)},
            send{stl::move(inp_end)},
            code_point{checked::next_code_point_copy<ErrPolicy>(pos, send)} {}

        constexpr utf32_forward_iter()                                         = default;
        constexpr utf32_forward_iter(utf32_forward_iter const&)                = default;
        constexpr utf32_forward_iter(utf32_forward_iter&&) noexcept            = default;
        constexpr utf32_forward_iter& operator=(utf32_forward_iter const&)     = default;
        constexpr utf32_forward_iter& operator=(utf32_forward_iter&&) noexcept = default;
        constexpr ~utf32_forward_iter() noexcept                               = default;

        constexpr utf32_forward_iter& operator++() noexcept {
            if (checked::next_char(pos, send)) {
                code_point = checked::next_code_point_copy<ErrPolicy>(pos, send);
            } else {
                code_point = U'\0';
            }
            return *this;
        }

        [[nodiscard]] constexpr const_reference operator*() const noexcept {
            return code_point;
        }

        [[nodiscard]] constexpr utf32_forward_iter operator++(int) noexcept {
            auto const res = utf32_forward_iter{*this};
            operator++();
            return res;
        }

        [[nodiscard]] constexpr bool operator==(utf32_forward_iter const& other) const noexcept {
            return pos == other.pos;
        }

        [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
            return at_end();
        }

        [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
            requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
        {
            return at_end();
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return pos == send;
        }

        [[nodiscard]] constexpr Iter const& base() const noexcept {
            return pos;
        }

        /// Return the start of the next Code Point
        [[nodiscard]] constexpr Iter upper_base() const noexcept {
            // return stl::next(*this).base();
            Iter upos = pos;
            auto _    = checked::next_code_point(upos, send);
            return upos;
        }

        constexpr stl::size_t unsafe_set(char32_t const new_code_point) noexcept {
            code_point = new_code_point;
            Iter cpos  = pos;
            return unchecked::append(cpos, code_point);
        }

        /// Return the length of the code point
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return code_point_length(pos, send);
        }
    };

} // namespace webpp::unicode::checked

#endif // WEBPP_UNICODE_UTF32_ITERATOR_HPP
