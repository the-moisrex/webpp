// Created by moisrex on 9/27/20.

#ifndef WEBPP_ISTRING_HPP
#define WEBPP_ISTRING_HPP

#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"
#include "../traits/std_traits.hpp"
#include "to_case.hpp"

namespace webpp {

    /**
     * istring = improved string
     *
     * This class is going to be designed in such a way that improves the standard capabilities and
     * shortcomings. I'll try to use the most optimized versions of algorithms available like SIMD and
     * other techniques.
     *
     * Since this class will be and should be used throughout the whole library, I should put a lot of time
     * optimizing this class.
     *
     * @tparam StringType
     */
    template <typename StringType,
              Traits TraitsType = typename std_traits_from<stl::remove_cvref_t<StringType>>::type>
    struct istring : public stl::remove_cvref_t<StringType> {
        using traits_type      = TraitsType;
        using string_type      = stl::remove_cvref_t<StringType>;
        using char_type        = istl::char_type_of<string_type>;
        using char_traits_type = istl::char_traits_type_of<string_type>;
        using istring_type     = istring<StringType, TraitsType>;

        static constexpr bool has_allocator = requires {
            typename string_type::allocator_type;
        };

      private:
        // this will get you the allocator inside the StrT which should be the above "string_type"
        // this is aimed to find the allocator_type inside the string but make sure not to raise any errors,
        // when there's no allocator_type
        template <typename StrT>
        struct allocator_extractor {
            using type = typename StrT::allocator_type;
        };

      public:
        using allocator_type =
          istl::lazy_conditional_t<has_allocator,
                                   istl::templated_lazy_type<allocator_extractor, string_type>,
                                   istl::lazy_type<typename traits_type::template allocator<char_type>>>;
        using alternate_std_string_type      = stl::basic_string<char_type, char_traits_type, allocator_type>;
        using alternate_std_string_view_type = stl::basic_string_view<char_type, char_traits_type>;

      public:
        // todo
        struct range {
            char_type* start;
            char_type* end;

            constexpr range(char_type* _start, char_type* _end) noexcept : start(_start), end(_end) {}
            constexpr range(char_type* _start, stl::size_t _size) noexcept
              : start(_start),
                end(_start + _size) {}
        };

        template <typename ...Args>
        constexpr istring(Args&& ...args) noexcept(noexcept(string_type(stl::forward<Args>(args)...))) :
            string_type{stl::forward<Args>(args)...} {}

        auto get_allocator() const noexcept {
            if constexpr (has_allocator) {
                return this->get_allocator();
            } else {
                return allocator_type{};
            }
        }

        [[nodiscard]] constexpr alternate_std_string_view_type std_string_view() const noexcept {
            return istl::to_string_view(*this);
        }


        [[nodiscard]] constexpr alternate_std_string_type std_string() const noexcept {
            return istl::to_string(*this, get_allocator());
        }

        explicit operator alternate_std_string_view_type() const noexcept {
            return std_string_view();
        }

        explicit operator alternate_std_string_type() const noexcept {
            return std_string();
        }

        void for_each(auto&& func, auto&& simd_func) noexcept(
          noexcept(func(this->data()))
#ifdef WEBPP_EVE
            && noexcept(simd_func(stl::declval<eve::wide<char_type>>()))
#endif
        ) {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;

#ifdef WEBPP_EVE
            using simd_type          = eve::wide<char_type>;
            constexpr auto simd_size = simd_type::size();

            if (_size > simd_size) {
                const auto* almost_end = it_end - (_size % simd_size);
                for (; it != almost_end; it += simd_size) {
                    stl::invoke(simd_func, simd_type{it});
                }
                // do the rest
                it -= simd_size;
            }
#endif
            for (; it != it_end; ++it) {
                stl::invoke(func, it);
            }
        }

        void for_each(auto&& func) noexcept(noexcept(func(this->data()))) {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;
            for (; it != it_end; ++it) {
                func(it);
            }
        }

        [[nodiscard]] constexpr auto if_all(auto&& func, auto&& simd_func) const noexcept {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;

#ifdef WEBPP_EVE
            using simd_type          = eve::wide<char_type>;
            constexpr auto simd_size = simd_type::size();

            if (_size > simd_size) {
                const auto* almost_end = it_end - (_size % simd_size);
                for (; it != almost_end; it += simd_size) {
                    if (!stl::invoke(simd_func, simd_type{it})) {
                        return false;
                    }
                }
                // do the rest
                it -= simd_size;
            }
#endif
            for (; it != it_end; ++it) {
                if (!stl::invoke(func, it)) {
                    return false;
                }
            }
            return true;
        }


        [[nodiscard]] constexpr auto if_all(auto&& func) const noexcept {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;
            for (; it != it_end; ++it) {
                if (!stl::invoke(func, it)) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] constexpr auto if_any(auto&& func, auto&& simd_func) const noexcept {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;

#ifdef WEBPP_EVE
            using simd_type          = eve::wide<char_type>;
            constexpr auto simd_size = simd_type::size();

            if (_size > simd_size) {
                const auto* almost_end = it_end - (_size % simd_size);
                for (; it != almost_end; it += simd_size) {
                    if (stl::invoke(simd_func, simd_type{it})) {
                        return true;
                    }
                }
                // do the rest
                it -= simd_size;
            }
#endif
            for (; it != it_end; ++it) {
                if (stl::invoke(func, it)) {
                    return true;
                }
            }
            return false;
        }


        [[nodiscard]] constexpr auto if_any(auto&& func) const noexcept {
            auto*       it     = this->data();
            const auto  _size  = this->size();
            const auto* it_end = it + _size;
            for (; it != it_end; ++it) {
                if (stl::invoke(func, it)) {
                    return true;
                }
            }
            return false;
        }




        [[nodiscard]] constexpr bool is_ascii_lower() const noexcept {
            return if_all(
              [](auto* it) constexpr noexcept { return *it >= 'a' && *it <= 'z'; },
              [](auto&& values) noexcept {
#ifdef WEBPP_EVE
                  using simd_utype = eve::wide<stl::make_unsigned_t<char_type>>;
                  static const simd_utype small_a{'a'};
                  const auto              u_values = eve::bit_cast(values, eve::as_<simd_utype>());
                  const auto              res      = eve::is_less(eve::sub(u_values, small_a), 25);
                  return eve::all(res);
#endif
              });
        }

        [[nodiscard]] constexpr bool is_ascii_upper() const noexcept {
            // todo: add a benchmark for this
            return if_all(
              [](auto* it) constexpr noexcept { return *it >= 'A' && *it <= 'Z'; },
              [](auto&& values) noexcept {
#ifdef WEBPP_EVE
                  using simd_utype = eve::wide<stl::make_unsigned_t<char_type>>;
                  static const simd_utype big_a{'A'};
                  const auto              u_values = eve::bit_cast(values, eve::as_<simd_utype>());
                  const auto              res      = eve::is_less(eve::sub(u_values, big_a), 25);
                  return eve::all(res);
#endif
              });
        }

        //        [[nodiscard]] constexpr bool is_lower(range) const noexcept;
        //        [[nodiscard]] constexpr bool is_upper(range) const noexcept;

        auto& ascii_to_upper() noexcept {
            webpp::ascii::to_upper(*this);
            return *this;
        }

        auto& ascii_to_lower() noexcept {
            webpp::ascii::to_lower(*this);
            return *this;
        }

        [[nodiscard]] constexpr istring_type ascii_to_upper_copy() const noexcept {
            return istring_type{*this}.ascii_to_upper();
        }

        [[nodiscard]] constexpr istring_type ascii_to_lower_copy() const noexcept {
            return istring_type{*this}.ascii_to_lower();
        }
    };


    using std_istring      = istring<stl::string>;
    using std_istring_view = istring<stl::string_view>;

} // namespace webpp

#endif // WEBPP_ISTRING_HPP
