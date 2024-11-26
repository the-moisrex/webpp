// Created by moisrex on 9/2/24.

#ifndef WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
#define WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP

#include "../std/type_traits.hpp"
#include "../std/utility.hpp"
#include "./unicode.hpp"

#include <array>
#include <cassert>

namespace webpp::unicode {

    /**
     * Mark a range of UTF-8 or UTF-16 code units
     * @tparam IterT Iterator Type
     */
    template <typename IterT = char8_t*>
    struct utf_range_marker {
        static_assert(stl::is_default_constructible_v<IterT>, "Iterator is not default constructible");

        using iter_traits     = stl::iterator_traits<IterT>;
        using value_type      = typename iter_traits::value_type;
        using difference_type = typename iter_traits::difference_type;

      private:
        IterT beginp{};
        IterT endp{};

      public:
        constexpr utf_range_marker() noexcept                          = default;
        constexpr utf_range_marker(utf_range_marker const&)            = delete;
        constexpr utf_range_marker(utf_range_marker&&)                 = delete;
        constexpr utf_range_marker& operator=(utf_range_marker const&) = delete;
        constexpr utf_range_marker& operator=(utf_range_marker&&)      = delete;
        constexpr ~utf_range_marker() noexcept                         = default;

        constexpr void mark(IterT inp_beg, IterT inp_end) noexcept {
            beginp = inp_beg;
            endp   = inp_end;
            assert(beginp != endp);
        }

        constexpr void mark(IterT inp_beg) noexcept {
            beginp = inp_beg;
            endp   = stl::next(beginp, required_length_of<value_type, difference_type>(*inp_beg));
            assert(beginp != endp);
        }

        [[nodiscard]] constexpr IterT begin() const noexcept {
            return beginp;
        }

        [[nodiscard]] constexpr IterT end() const noexcept {
            return endp;
        }

        [[nodiscard]] constexpr difference_type size() const noexcept {
            return endp - beginp;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return beginp == endp;
        }

        /// Move the hole
        template <typename IterableT>
        constexpr void move(difference_type diff, IterableT& iters) noexcept {
            // it's most-likely a bug somewhere in the code if diff is zero
            assert(diff != 0);

            auto const length = this->size();
            if (diff < 0) {
                stl::shift_right(beginp + diff, endp, length);
                for (auto& cur : iters) {
                    if (cur >= (beginp + diff) && cur <= endp) {
                        cur += length;
                    }
                }
                beginp += diff;
                endp   += diff;
            } else if (diff > 0) [[likely]] {
                stl::shift_left(beginp, endp + diff, length);
                for (auto& cur : iters) {
                    if (cur >= beginp && cur <= (endp + diff)) {
                        cur -= length;
                    }
                }
                beginp -= diff;
                endp   -= diff;
            }
        }
    };

    /// We don't need UTF-32 ranges, so we disable it
    template <typename IterT>
        requires(UTF32<typename stl::iterator_traits<IterT>::value_type>)
    struct utf_range_marker<IterT> {
        constexpr void mark([[maybe_unused]] auto&&... args) noexcept {
            // do nothing
        }
    };

    template <stl::size_t PinCount = 1, typename IterT = char8_t*, UTF32 CodePointT = char32_t>
    struct utf_reducer;

    template <std::size_t PinCount = 1, typename IterT = char8_t*, UTF32 CodePointT = char32_t>
    struct const_pin_type {
        using reducer_type    = utf_reducer<PinCount, IterT, CodePointT>;
        using iterator        = typename reducer_type::iterator;
        using size_type       = typename reducer_type::size_type;
        using value_type      = typename reducer_type::value_type;
        using unit_type       = typename reducer_type::unit_type;
        using difference_type = typename reducer_type::difference_type;

        static constexpr bool is_nothrow = reducer_type::is_nothrow;

      private:
        template <std::size_t, typename, UTF32>
        friend struct const_pin_type;

        iterator ptr;

        /// guarantee the correctness of the state that we're in
        constexpr void test_state_correctness() noexcept {
            // early blow up in case we did not find the correct ptr position:
            assert(is_code_unit_start(*iter()));
        }

      public:
        constexpr explicit const_pin_type(iterator inp_iter) noexcept : ptr{inp_iter} {}

        constexpr const_pin_type(const_pin_type&& other) noexcept                 = default;
        constexpr const_pin_type& operator=(const_pin_type&& other) noexcept      = default;
        constexpr ~const_pin_type() noexcept                                      = default;
        constexpr const_pin_type(const_pin_type const& other) noexcept            = default;
        constexpr const_pin_type& operator=(const_pin_type const& other) noexcept = default;

        [[nodiscard]] constexpr iterator const& iter() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr iterator& iter() noexcept {
            return ptr;
        }

        constexpr const_pin_type& operator++() noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                ++iter();
            } else {
                unchecked::next_char(iter());
            }
            // test_state_correctness();
            return *this;
        }

        [[nodiscard]] constexpr value_type operator*() noexcept(is_nothrow) {
            assert(is_code_unit_start(*iter()));
            if constexpr (UTF32<unit_type>) {
                return *iter();
            } else {
                return next_code_point_copy(iter());
            }
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator==(NIterT other) noexcept(is_nothrow) {
            return iter() == other;
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator!=(NIterT other) noexcept(is_nothrow) {
            return iter() != other;
        }
    };

    /**
     * Pin is a forward-iterator-like type that is designed to be used inside utf_reducer.
     *
     * Pin Acts (capabilities of pins):
     *   - Get: get the code point
     *   - Set (or Replace): replace a code point inplace if possible
     *   - Change Pin position to another pin location
     *   - Forward: go to the next code point
     *
     *
     *   enum struct pin_states {
     *       filled,  // [X|'|']      All code units are filled
     *       extra,   // [X|'|'] [']  There are extra code units left that can't be fit
     *       partial, // [X|'| ]      We've only used parts of the previous code units' space available
     *       // deleted  // [ | | ]      All code units are empty now
     *   };
     */
    template <std::size_t PinIndex = 0,
              std::size_t PinCount = 1,
              typename IterT       = char8_t*,
              UTF32 CodePointT     = char32_t>
    struct pin_type {
        static_assert(PinIndex < PinCount, "Either Pin count is invalid or Pin index.");

        using reducer_type    = utf_reducer<PinCount, IterT, CodePointT>;
        using iterator        = typename reducer_type::iterator;
        using size_type       = typename reducer_type::size_type;
        using value_type      = typename reducer_type::value_type;
        using unit_type       = typename reducer_type::unit_type;
        using difference_type = typename reducer_type::difference_type;

        static constexpr bool is_nothrow = reducer_type::is_nothrow;
        static constexpr auto npos       = reducer_type::npos;

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        template <typename T>
        using if_not_utf32 = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, T>;

        template <typename T>
        using if_utf32 = stl::conditional_t<!UTF32<unit_type>, istl::nothing_type, T>;

        using const_pin_t = const_pin_type<PinCount, IterT, CodePointT>;

        [[no_unique_address]] if_not_utf32<reducer_type*> reducer;
        [[no_unique_address]] if_utf32<iterator>          ptr;

        /// guarantee the correctness of the state that we're in
        constexpr void test_state_correctness() noexcept {
            if constexpr (!UTF32<unit_type>) {
                // guarantee that each pin's position will be less than or equal to the next one:
                assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) + 1>() >= iter());

                // guarantee that each pin's position will be more than or equal to the previous one:
                assert(reducer->template pin_iter<static_cast<difference_type>(PinIndex) - 1>() <= iter());
            }

            // early blow up in case we did not find the correct ptr position:
            assert(is_code_unit_start(*iter()));
        }

        /// The number of code units required to set this new code point with the specified length
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units_of_len(stl::int_fast8_t const cp_len)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                stl::int_fast8_t count    = 0;
                auto             iter_cpy = istl::deref(iter());
                while (count <= cp_len) {
                    assert(iter_cpy <= reducer->endptr);
                    auto const cur_len  = required_length_of<unit_type, stl::int_fast8_t>(*iter_cpy);
                    iter_cpy           += cur_len;
                    count               += cur_len;
                }
                webpp_assume(count <= 6);
                return count;
            }
        }

        /// The number of code units required to set this new code point
        [[nodiscard]] constexpr stl::int_fast8_t required_code_units(value_type const inp_cp)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                return 1; // always
            } else {
                return required_code_units_of_len(utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_cp));
            }
        }

      public:
        constexpr explicit pin_type(reducer_type* inp_reducer) noexcept
            requires(!UTF32<unit_type>)
          : reducer(inp_reducer) {}

        constexpr explicit pin_type(reducer_type* inp_reducer) noexcept
            requires(UTF32<unit_type>)
          : ptr(inp_reducer->begin()) {}

        constexpr pin_type(pin_type&& other) noexcept            = default;
        constexpr pin_type& operator=(pin_type&& other) noexcept = default;
        constexpr ~pin_type() noexcept                           = default;

        /// most likely a not so much great algorithm is being written, so, let's shut copying down:
        constexpr pin_type(pin_type const& other)            = delete;
        constexpr pin_type& operator=(pin_type const& other) = delete;

        /// Pin Act: Change position to another pin location
        template <stl::size_t OPinIndex>
        constexpr pin_type& operator=(
          pin_type<OPinIndex, PinCount, IterT, CodePointT> const& other) noexcept {
            static_assert(OPinIndex < PinCount,
                          "You cannot go back; these are forward-only set of constructs.");
            static_assert(OPinIndex != PinIndex, "Self-assignment is probably a mistake.");

            assert(other.iter() >= iter());
            if constexpr (UTF32<unit_type>) {
                ptr = other.ptr;
            } else {
                // this is not a copy assignment operator, it's a pin act.
                assert(reducer == other.reducer);
                reducer->iters[PinIndex] = reducer->iters[OPinIndex];
                test_state_correctness();
            }
            return *this;
        }

        /// Pin Act: Forward
        constexpr pin_type& operator++() noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                ++iter();
            } else {
                unchecked::next_char(iter());
            }
            test_state_correctness();
            return *this;
        }

        [[nodiscard]] constexpr iterator& iter() noexcept {
            if constexpr (UTF32<unit_type>) {
                return ptr;
            } else {
                return reducer->iters[PinIndex];
            }
        }

        [[nodiscard]] constexpr iterator const& iter() const noexcept {
            if constexpr (UTF32<unit_type>) {
                return ptr;
            } else {
                return reducer->iters[PinIndex];
            }
        }

        [[nodiscard]] constexpr value_type operator*() const noexcept {
            if constexpr (UTF32<unit_type>) {
                return *iter();
            } else {
                return next_code_point_copy(iter());
            }
        }

        [[nodiscard]] constexpr iterator operator->() const noexcept {
            return iter();
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator==(NIterT other) const noexcept {
            return iter() == other;
        }

        template <typename NIterT = iterator>
        [[nodiscard]] constexpr bool operator!=(NIterT other) const noexcept {
            return iter() != other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(iterator other) const noexcept {
            // this might have unintentional consequences if other is in the middle of a UTF-8 code point
            // and the ptr is in the beginning of that code point. we're assuming pointers can't be
            // pointed to the middle of the Unicode code points.
            return iter() <=> other;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(pin_type const& other) const noexcept {
            return iter() <=> other.iter();
        }

        constexpr pin_type& operator=(const_pin_t const& other) noexcept(is_nothrow) {
            operator=(other.iter());
            return *this;
        }

        constexpr pin_type& operator=(iterator other) noexcept(is_nothrow) {
            assert(other >= iter());
            if constexpr (UTF32<unit_type>) {
                ptr = other;
            } else {
                iter() = other;
            }
            test_state_correctness();
            return *this;
        }

        constexpr pin_type& operator=(value_type other) noexcept(is_nothrow) {
            set(other);
            return *this;
        }

        template <stl::size_t PinIndex2, typename Iter2>
        constexpr pin_type& operator=(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other)
          noexcept(is_nothrow) {
            set(other);
            return *this;
        }

      private:
        constexpr void move_iterators(iterator old_iter, difference_type const diff) noexcept {
            if (diff != 0) {
                stl::advance(reducer->newend, diff);
                *reducer->newend = static_cast<unit_type>('\0');
            }
            for (auto index = PinIndex + 1; index < PinCount; ++index) {
                auto& cur = reducer->iters[index];
                if (cur > old_iter) {
                    stl::advance(cur, diff);
                }
            }
        }

        constexpr void shift_range(iterator begin, iterator end, difference_type diff) noexcept(is_nothrow) {
            assert(diff >= 1);
            stl::shift_left(begin, end, diff);
            move_iterators(begin, -diff);
        }

        constexpr void set_diff(value_type inp_code_point, stl::int_fast8_t const diff) noexcept(is_nothrow) {
            assert(diff >= 0);
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() != reducer->endptr);

                auto iter_cpy = istl::deref(iter());
                unchecked::append(iter_cpy, inp_code_point);

                // state: filled
                if (diff == 0) {
                    return;
                }

                // state: partial or deleted
                shift_range(iter_cpy, reducer->newend, static_cast<difference_type>(diff));

                test_state_correctness();
            }
        }

      public:
        /// Pin Act: Set
        constexpr void set(iterator other) noexcept(is_nothrow) {
            if constexpr (stl::is_pointer_v<iterator>) {
                assert(other != nullptr);
            } else {
                assert(other != reducer->endptr);
            }
            if constexpr (UTF32<unit_type>) {
                *iter() = *other;
            } else {
                auto const cur_len = required_length_of<unit_type, stl::int_fast8_t>(*iter());
                auto const new_len = required_length_of<unit_type, stl::int_fast8_t>(*other);

                assert(cur_len >= new_len);
                set_diff(*other, cur_len - new_len);
            }
        }

        /// Pin Act: Set
        constexpr void set(value_type inp_code_point) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() != reducer->endptr);

                auto const cur_len = required_length_of<unit_type, stl::int_fast8_t>(*iter());
                auto const new_len = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);

                assert(cur_len >= new_len);
                set_diff(inp_code_point, cur_len - new_len);
            }
        }

        /// Pin Act: Set
        constexpr void set(value_type inp_code_point, [[maybe_unused]] utf_range_marker<iterator>& hole)
          noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() != reducer->endptr);
                assert(reducer->beg <= hole.begin());
                assert(reducer->endptr >= hole.end());

                auto       cur_len = required_length_of<unit_type, stl::int_fast8_t>(*iter());
                auto const new_len = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);

                // Move the hole to the current place in order to make cur_len bigger than the new_len
                if (new_len > cur_len) {
                    assert(!hole.empty());
                    assert(hole.begin() != hole.end());

                    auto const new_loc = iter() + cur_len;
                    auto const old_loc = hole.begin();
                    auto const diff    = new_loc - old_loc;
                    assert(iter() < hole.begin());
                    hole.move(diff, reducer->iters);
                    assert(hole.end() < reducer->endptr);
                    assert(hole.end() < reducer->newend);
                    assert(hole.begin() < reducer->endptr);
                    assert(hole.begin() < reducer->newend);
                    assert(hole.begin() == stl::next(iter(), cur_len));
                    cur_len += hole.size();

                    // storing the length of the hole, inside the hole itself.
                    // todo: optimize this:
                    auto cur_diff = hole.size();
                    for (auto& unit : hole) {
                        unit = unicode::utf_leading_code_units<unit_type>[cur_diff--];
                    }
                }
                assert(cur_len >= new_len);
                set_diff(inp_code_point, cur_len - new_len);
            }
        }

        /// Pin Act: Replace
        constexpr void set(value_type inp_code_point, stl::int_fast8_t const code_unit_len)
          noexcept(is_nothrow) {
            assert(code_unit_len >= 0);
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                auto const cp_len = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);
                difference_type const diff = code_unit_len - cp_len;
                set_diff(inp_code_point, diff);
            }
        }

        /// Pin Act: Set
        template <stl::size_t PinIndex2, typename Iter2>
        constexpr void set(pin_type<PinIndex2, PinCount, Iter2, CodePointT> const& other_ptr)
          noexcept(is_nothrow) {
            set(other_ptr.iter());
        }

        /// Pin Act: Set Spillover
        constexpr void spillover_set(value_type inp_code_point) noexcept(is_nothrow) {
            if constexpr (UTF32<unit_type>) {
                *iter() = inp_code_point;
            } else {
                assert(iter() != reducer->endptr);

                auto const cp_len  = utf_length_from_utf32<unit_type, stl::int_fast8_t>(inp_code_point);
                auto const rep_len = required_code_units_of_len(cp_len);

                assert(rep_len >= cp_len);
                set_diff(inp_code_point, rep_len - cp_len);
            }
        }
    };

    /**
     * Reduce a UTF-8/16/32-encoded inplace.
     *
     * Usage cases:
     *   - If you have a range and want to replace something in it, and you're sure you're not going to change
     *     the length of the string.
     *   - It gives you multiple pins (forward-only-iterators of some sort) to modify inplace
     *
     * Rules:
     *   1. Pointers/Iterators should not be pointing to the middle of a Unicode code unit.
     *   2. Pins are ordered, meaning pin-1 cannot point to location after the pin-2's location.
     *   3. Pins can point to the same place though
     *   4. This class is not thread-safe, though you probably don't need that anyway.
     *
     * @tparam IterT Iterator type
     * @tparam CodePointT The Code Point Type (Must be UTF-32)
     */
    template <stl::size_t PinCount, typename IterT, UTF32 CodePointT>
    struct utf_reducer {
        using value_type      = CodePointT;
        using iterator        = IterT;
        using unit_type       = typename stl::iterator_traits<IterT>::value_type;
        using difference_type = typename stl::iterator_traits<IterT>::difference_type;
        using size_type       = stl::size_t;

        static constexpr value_type  npos      = stl::numeric_limits<value_type>::max();
        static constexpr stl::size_t pin_count = PinCount;
        static constexpr bool        is_nothrow =
          stl::is_nothrow_copy_assignable_v<unit_type> && requires(iterator iter, unit_type unit) {
              {
                  *iter = unit
              } noexcept;
          };

        static_assert(pin_count != 0, "This class is useless where you don't need the pins.");
        static_assert(stl::random_access_iterator<iterator>,
                      "Iterator must at least be a random access iterator");

      private:
        template <std::size_t, std::size_t, typename, UTF32>
        friend struct pin_type;

        template <typename T>
        using if_not_utf32 = stl::conditional_t<UTF32<unit_type>, istl::nothing_type, T>;

        iterator beg;
        iterator endptr;
        iterator newend;

        [[no_unique_address]] if_not_utf32<stl::array<iterator, PinCount>> iters;

        template <stl::size_t Index>
        using pin_type_of = pin_type<Index, PinCount, IterT, CodePointT>;

        using const_pin_t = const_pin_type<PinCount, IterT, CodePointT>;

      public:
        // the reason why we take length instead of end position is because the end might be a const, but
        // we don't want the end to be a const internally.
        explicit constexpr utf_reducer(iterator inp_pos, size_type const inp_length)
          : beg{inp_pos},
            endptr{stl::next(inp_pos, static_cast<difference_type>(inp_length))},
            newend{endptr} {
            if constexpr (std::is_pointer_v<iterator>) {
                assert(inp_pos != nullptr);
                assert(endptr != nullptr);
            }
            assert(inp_pos != endptr);
            assert(is_code_unit_start(*inp_pos));
            if constexpr (!UTF32<unit_type>) {
                iters.fill(beg);
            }
        }

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit constexpr utf_reducer(iterator inp_pos, iterator inp_endp)
          : beg{inp_pos},
            endptr{inp_endp},
            newend{endptr} {
            if constexpr (std::is_pointer_v<iterator>) {
                assert(inp_pos != nullptr);
                assert(endptr != nullptr);
            }
            assert(is_code_unit_start(*inp_pos));
            assert(inp_pos <= inp_endp);
            if constexpr (!UTF32<unit_type>) {
                iters.fill(beg);
            }
        }

        constexpr utf_reducer(utf_reducer const&)                = default;
        constexpr utf_reducer(utf_reducer&&) noexcept            = default;
        constexpr utf_reducer& operator=(utf_reducer const&)     = default;
        constexpr utf_reducer& operator=(utf_reducer&&) noexcept = default;
        constexpr ~utf_reducer() noexcept                        = default;

        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr auto pin() noexcept {
            static_assert(Index < PinCount, "Index must be in range.");
            return pin_type_of<Index>{this};
        }

        [[nodiscard]] constexpr const_pin_t const_pin() noexcept {
            return const_pin_t{beg};
        }

        template <difference_type Index = 0>
        [[nodiscard]] constexpr iterator& pin_iter() noexcept
            requires(!UTF32<unit_type>)
        {
            // Create a compile time error and not allow bad code:
            static_assert(Index <= static_cast<difference_type>(PinCount) && Index >= -1,
                          "Index must be in range, or the last element.");
            if constexpr (Index == -1) {
                return beg;
            } else if constexpr (Index == static_cast<difference_type>(PinCount)) {
                return endptr;
            } else {
                return iters[Index];
            }
        }

        [[nodiscard]] constexpr iterator& pin_iter(difference_type index = 0) noexcept
            requires(!UTF32<unit_type>)
        {
            assert(index <= static_cast<difference_type>(PinCount) && index >= -1);
            if (index == -1) {
                return beg;
            }
            if (index == static_cast<difference_type>(PinCount)) {
                return endptr;
            }
            return iters[static_cast<size_type>(index)];
        }

        [[nodiscard]] constexpr iterator end_pin_iter(size_type index = 0) noexcept
            requires(!UTF32<unit_type>)
        {
            assert(index < PinCount);
            auto ptr = iters[index];
            return ptr + required_length_of<unit_type, difference_type>(*ptr);
        }

        /// Get all pins in a tuple construct
        /// Usage:
        ///   auto [pin1, pin2, pin3] = reducer.pins();
        [[nodiscard]] constexpr auto pins() noexcept {
            return ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                return stl::make_tuple(pin<I>()...);
            })(stl::make_index_sequence<PinCount>{});
        }

        /// Get a new set of const pins.
        /// "Const Pins" are pins that cannot modify anything and are less restricted in moving around.
        /// Usage:
        ///   auto [pin1, pin2, pin3] = reducer.template new_const_pins<3>();
        template <size_type Count = PinCount>
        [[nodiscard]] constexpr auto new_const_pins() noexcept {
            return ([&]<stl::size_t... I>(stl::index_sequence<I...>) constexpr noexcept {
                return stl::make_tuple((static_cast<void>(I), const_pin())...);
            })(stl::make_index_sequence<Count>{});
        }

        [[nodiscard]] constexpr iterator begin() const noexcept {
            return beg;
        }

        [[nodiscard]] constexpr iterator end() const noexcept {
            return newend;
        }

        constexpr void set_end(iterator inp_end) noexcept {
            newend = inp_end;
        }

        template <stl::size_t Index>
        constexpr void set_end(pin_type_of<Index> const& inp_end) noexcept {
            newend = inp_end.iter();
        }

        constexpr void set_end(const_pin_t const& inp_end) noexcept {
            newend = inp_end.iter();
            // *newend = static_cast<unit_type>('\0');
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return static_cast<size_type>(newend - beg);
        }
    };

    template <std::size_t PinIndex1,
              std::size_t PinIndex2,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT>
    [[nodiscard]] static constexpr auto operator-(pin_type<PinIndex1, PinCount, IterT, CodePointT> const& lhs,
                                                  pin_type<PinIndex2, PinCount, IterT, CodePointT> const& rhs)
      noexcept(noexcept(lhs.iter() - rhs.iter())) {
        using difference_type = typename pin_type<PinIndex1, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs.iter() - rhs.iter());
    }

    template <std::size_t PinIndex,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT,
              typename RIterT>
    [[nodiscard]] static constexpr auto operator-(pin_type<PinIndex, PinCount, IterT, CodePointT> const& lhs,
                                                  RIterT rhs_iter) noexcept(noexcept(lhs.iter() - rhs_iter)) {
        using difference_type = typename pin_type<PinIndex, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs.iter() - rhs_iter);
    }

    template <typename LIterT,
              std::size_t PinIndex,
              std::size_t PinCount,
              typename IterT,
              typename CodePointT>
    [[nodiscard]] static constexpr auto operator-(
      LIterT                                                 lhs_iter,
      pin_type<PinIndex, PinCount, IterT, CodePointT> const& rhs) noexcept(noexcept(lhs_iter - rhs.iter())) {
        using difference_type = typename pin_type<PinIndex, PinCount, IterT, CodePointT>::difference_type;
        return static_cast<difference_type>(lhs_iter - rhs.iter());
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CODE_POINT_ITERATOR_HPP
