// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_HPP
#define WEBPP_USTRING_HPP

#include "../memory/allocator_concepts.hpp"
#include "../std/format.hpp"
#include "../std/memory_resource.hpp"
#include "unicode_char_traits.hpp"
#include "ustring_iterator.hpp"

/**
 * Resources to use for optimization purposes:
 * CppCon 2018: Bob Steagall “Fast Conversion From UTF-8 with C++, DFAs, and SSE Intrinsics”
 * https://youtu.be/5FQ87-Ecb-A
 * https://github.com/BobSteagall/utf_utils
 * https://github.com/tzlaine/text
 *
 */
namespace webpp {

    template <Allocator AllocType = stl::allocator<unicode::storage_unit<>>>
    struct ustring_allocator_wrapper : AllocType {};

    template <Allocator AllocType>
        requires(unicode::is_storage_unit_v<typename AllocType::value_type>)
    struct ustring_allocator_wrapper<AllocType> : AllocType {
        using parent_allocator_type   = AllocType;
        using parent_value_type       = typename parent_allocator_type::value_type;
        using parent_allocator_traits = stl::allocator_traits<parent_allocator_type>;

        using wrapper_type = ustring_allocator_wrapper<AllocType>;

        // this right here is the point of the whole wrapper:
        using pointer       = typename parent_value_type::pointer;
        using const_pointer = typename pointer::const_pointer;

        // we need this, because if we don't do this, the "rebind" will use the AllocType and not the
        // wrapped version of AllocType after the "rebind".
        template <typename T>
        struct rebind {
            using other =
              ustring_allocator_wrapper<typename parent_allocator_traits::template rebind_alloc<T>>;
        };

        using AllocType::AllocType;
    };


    template <unicode::StorageUnit CharT     = unicode::storage_unit<>,
              Allocator            AllocType = stl::allocator<CharT>>
    struct ustring
      : stl::basic_string<CharT, unicode_char_traits<CharT>, ustring_allocator_wrapper<AllocType>> {

        using basic_string_type =
          stl::basic_string<CharT, unicode_char_traits<CharT>, ustring_allocator_wrapper<AllocType>>;


        using value_type       = typename basic_string_type::value_type;
        using allocator_type   = typename basic_string_type::allocator_type;
        using unit_type        = typename value_type::char_type;
        using pointer          = typename basic_string_type::pointer;
        using const_pointer    = typename basic_string_type::const_pointer;
        using char_traits_type = typename basic_string_type::traits_type;


        static_assert(unicode::is_storage_unit_v<value_type>,
                      "The specified value type is not a ustring storage unit");

        template <typename T>
        static constexpr bool same_size_unit = sizeof(T) == sizeof(unit_type);

        // ctor
        using stl::basic_string<CharT, unicode_char_traits<CharT>, ustring_allocator_wrapper<AllocType>>::
          basic_string;


        template <typename NewCharT>
            requires(same_size_unit<NewCharT>) // both are the same size
        constexpr explicit ustring(NewCharT const* val, const allocator_type& a = allocator_type{})
          : basic_string_type{reinterpret_cast<value_type const*>(val), a} {}

        /*
        template <typename NewCharT>
        requires(same_size_unit<NewCharT> && !stl::same_as<NewCharT, value_type>) // both are the same size
          constexpr auto
          operator==(NewCharT const* val) noexcept {
            return operator==
              <value_type, char_traits_type, allocator_type>(static_cast<basic_string_type const&>(*this),
                                                             reinterpret_cast<value_type const*>(val));
        }
        */

        template <typename NewCharT>
            requires(same_size_unit<NewCharT> &&
                     !stl::same_as<NewCharT, value_type>) // both are the same size
        constexpr auto operator<=>(NewCharT const* val) noexcept {
            return *this <=> reinterpret_cast<value_type const*>(val);
        }


        constexpr basic_string_type& basic_string() noexcept {
            return static_cast<basic_string_type&>(*this);
        }

        constexpr basic_string_type const& basic_string() const noexcept {
            return static_cast<basic_string_type const&>(*this);
        }
    };


    template <typename NewCharT, typename CharT, typename AllocT>
    constexpr auto operator==(const ustring<CharT, AllocT>& lhs, NewCharT const* val) noexcept {
        using ustring_type      = ustring<CharT, AllocT>;
        using basic_string_type = typename ustring_type::basic_string_type;
        using value_type        = typename basic_string_type::value_type;
        using allocator_type    = typename basic_string_type::allocator_type;
        using char_traits_type  = typename basic_string_type::traits_type;
        return stl::operator==
          <value_type, char_traits_type, allocator_type>(lhs.basic_string(),
                                                         reinterpret_cast<value_type const*>(val));
    }



    template <Allocator AllocT = stl::allocator<unicode::utf8_storage_unit>>
    using utf8 = ustring<unicode::utf8_storage_unit, AllocT>;

    template <Allocator AllocT = stl::allocator<unicode::utf16_storage_unit>>
    using utf16 = ustring<unicode::utf16_storage_unit, AllocT>;

    template <Allocator AllocT = stl::allocator<unicode::utf32_storage_unit>>
    using utf32 = ustring<unicode::utf32_storage_unit, AllocT>;

#if false and __cpp_lib_memory_resource
    namespace pmr {
        using namespace std::pmr;

        template <typename T = stl::byte>
        struct polymorphic_allocator;

        using utf8 =
          ustring<unicode::utf8_storage_unit, polymorphic_allocator<>>;
        using utf16 =
          ustring<unicode::utf16_storage_unit, polymorphic_allocator<>>;
        using utf32 =
          ustring<unicode::utf32_storage_unit, polymorphic_allocator<>>;
    } // namespace pmr
#endif

} // namespace webpp

#endif // WEBPP_USTRING_HPP
