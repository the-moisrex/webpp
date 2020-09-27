// Created by moisrex on 9/27/20.

#ifndef WEBPP_ISTRING_HPP
#define WEBPP_ISTRING_HPP

#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"
#include "../traits/std_traits.hpp"

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
    template <typename StringType, Traits TraitsType = typename std_traits_from_string<stl::remove_cvref_t<StringType>>::type>
    struct istring : public stl::remove_cvref_t<StringType> {
        using traits_type = TraitsType;
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type = istl::char_type_of<string_type>;
        using char_traits_type = istl::char_traits_type_of<string_type>;

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

        using allocator_type = istl::lazy_conditional_t<has_allocator,
                                                        istl::lazy_type<typename traits_type::template allocator<char_type>>,
                                                        istl::lazy_sub_type<allocator_extractor, string_type>
                                                        >;
        using alternate_std_string_type = stl::basic_string<char_type, char_traits_type, allocator_type>;
        using alternate_std_string_view_type = stl::basic_string_view<char_type, char_traits_type>;

      public:
        using string_type::string_type; // ctors of the daddy

        [[nodiscard]] constexpr alternate_std_string_view_type std_string_view() const noexcept {
            return istl::to_string_view(*this);
        }


        [[nodiscard]] constexpr alternate_std_string_type std_string() const noexcept {
            return istl::to_string(*this);
        }

        operator alternate_std_string_view_type() const noexcept {
            return std_string_view();
        }

        operator alternate_std_string_type() const noexcept {
            return std_string();
        }


    };


}

#endif // WEBPP_ISTRING_HPP
