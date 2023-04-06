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
    struct ustring_allocator_wrapper : AllocType {
        using parent_allocator_type   = AllocType;
        using parent_value_type       = typename parent_allocator_type::value_type;
        using parent_allocator_traits = stl::allocator_traits<parent_allocator_type>;

        using wrapper_type = ustring_allocator_wrapper<AllocType>;

        static_assert(unicode::is_storage_unit_v<parent_value_type>,
                      "The specified allocator is not usable for ustring");

        // this right here is the point of the whole wrapper:
        using pointer       = typename parent_value_type::pointer;
        using const_pointer = typename pointer::const_pointer;

        // we need this, because if we don't do this, the "rebind" will use the AllocType and not the
        // wrapped version of AllocType after the "rebind".
        template <typename... T>
        struct rebind {
            using other =
              ustring_allocator_wrapper<typename parent_allocator_traits::template rebind_alloc<T...>>;
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


#if false


#    ifdef __CHAR_BIT__
    static constexpr unsigned char_bits = __CHAR_BIT__;
#    else
    static constexpr unsigned char_bits = 8;
#    endif

    namespace details {

        // Helper for ustring and ustring_view members.
        constexpr stl::size_t sv_check(stl::size_t size, stl::size_t pos, const char* s) {
            if (pos > size)
                throw stl::out_of_range(fmt::format("%s: pos (which is %zu) > size "
                                                    "(which is %zu)",
                                                    s,
                                                    pos,
                                                    size));
            return pos;
        }

        // Helper for ustring members.
        // NB: sv_limit doesn't check for a bad pos value.
        constexpr stl::size_t sv_limit(stl::size_t size, stl::size_t pos, stl::size_t off) noexcept {
            const bool testoff = off < size - pos;
            return testoff ? off : size - pos;
        }



        // Lightweight alternative to numeric_limits<signed integer type>.
        template <typename T, bool = stl::is_signed<T>::value>
        struct int_limits {
            static_assert(stl::is_integral<T>::value, "unsupported specialization");
            using U                     = typename stl::make_unsigned<T>::type;
            static constexpr int digits = sizeof(T) * char_bits - 1;
            static constexpr T   min() noexcept {
                return T(U(1) << digits);
            }
            static constexpr T max() noexcept {
                return T(U(~U(0)) >> 1);
            }
        };

        // Lightweight alternative to numeric_limits<unsigned integer type>.
        template <typename T>
        struct int_limits<T, false> {
            static_assert(stl::is_integral<T>::value, "unsupported specialization");
            static constexpr int digits = sizeof(T) * char_bits;
            static constexpr T   min() noexcept {
                return 0;
            }
            static constexpr T max() noexcept {
                return T(-1);
            }
        };

        template <>
        struct int_limits<bool>; // not defined



        template <typename AllocT>
        constexpr inline void alloc_on_move(AllocT& one, AllocT& two) {
            using traits = stl::allocator_traits<AllocT>;
            using pocma  = typename traits::propagate_on_container_move_assignment;
            if constexpr (pocma::value)
                one = stl::move(two);
        }

        template <typename AllocT>
        constexpr AllocT alloc_on_copy(const AllocT& a) {
            using traits = stl::allocator_traits<AllocT>;
            return traits::select_on_container_copy_construction(a);
        }



        template <typename T>
        inline bool is_null_pointer(T* ptr) noexcept {
            return ptr == 0;
        }

        template <typename T>
        inline bool is_null_pointer(T) noexcept {
            return false;
        }

        inline bool is_null_pointer(stl::nullptr_t) noexcept {
            return true;
        }





        template <typename Iter>
        using iterator_category_t = typename stl::iterator_traits<Iter>::iterator_category;

        template <typename InIter>
        concept RequireInputIter =
          stl::is_convertible_v<iterator_category_t<InIter>, stl::input_iterator_tag>;

        template <typename InIter>
        using RequireInputIterEnableIf =
          stl::enable_if_t<stl::is_convertible_v<iterator_category_t<InIter>, stl::input_iterator_tag>>;


        // Trait to detect Allocator-like types.
        template <typename AllocT, typename = void>
        struct is_allocator : stl::false_type {};

        template <typename AllocT>
        struct is_allocator<
          AllocT,
          stl::void_t<typename AllocT::value_type, decltype(stl::declval<AllocT&>().allocate(stl::size_t{}))>>
          : stl::true_type {};

        template <typename AllocT>
        using RequireAllocator = typename stl::enable_if<is_allocator<AllocT>::value, AllocT>::type;

        template <typename AllocT>
        using RequireNotAllocator = typename stl::enable_if<!is_allocator<AllocT>::value, AllocT>::type;





        template <typename T>
        struct is_char {
            static constexpr bool value = false;
        };

        template <>
        struct is_char<char> {
            static constexpr bool value = true;
        };

        template <>
        struct is_char<wchar_t> {
            static constexpr bool value = true;
        };


#    if __cpp_lib_three_way_comparison
        template <typename ChTraits>
        constexpr auto char_traits_cmp_cat(int cmp) noexcept {
            if constexpr (requires { typename ChTraits::comparison_category; }) {
                using Cat = typename ChTraits::comparison_category;
                static_assert(!stl::is_void_v<stl::common_comparison_category_t<Cat>>);
                return static_cast<Cat>(cmp <=> 0);
            } else {
                return static_cast<stl::weak_ordering>(cmp <=> 0);
            }
        }
#    endif // C++20

    } // namespace details


    template <typename To, stl::size_t N, typename From>
    auto convert_to(From&& from) noexcept {
        // todo: add allocator
        stl::unique_ptr<To[N]> to = stl::make_unique<To[N]>();
        for (stl::size_t i = 0; i < N; i++) {
            to[i] = static_cast<To>(from[i]);
        }
        return to;
    }


    template <stl::size_t Size>
    using char_type_of =
      stl::conditional_t<Size == sizeof(char8_t),
                         char8_t,
                         stl::conditional_t<Size == sizeof(char16_t),
                                            char16_t,
                                            stl::conditional_t<Size == sizeof(char32_t), char32_t, wchar_t>>>;

    template <typename T, typename CharTraits>
    struct ustring_view;

    template <typename CharT          = storage_unit<>,
              typename CharTraitsType = unicode_char_traits<CharT>,
              typename AllocType      = stl::allocator<CharT>>
    // requires(istl::CharTraits<CharTraitsType>&& AllocatorOf<CharT, AllocType>)
    struct ustring {
      private:
        using alloc_traits = stl::allocator_traits<AllocType>;

        template <typename E>
        struct lazy_code_point_extractor {
            using type = typename E::code_point_type;
        };

      public:
        using value_type = CharT;

        // code point type is the value that the user will get when it's run through an iterator.
        // It's CharT if CharT is not a storage unit, if it is, then it's storage_unit::code_point_type
        using code_point_type =
          istl::lazy_conditional_t<is_storage_unit<value_type>,
                                   istl::templated_lazy_type<lazy_code_point_extractor, value_type>,
                                   istl::lazy_type<value_type>>;
        using allocator_type         = typename alloc_traits::template rebind_alloc<value_type>;
        using traits_type            = CharTraitsType;
        using reference              = value_type&;
        using const_reference        = value_type const&;
        using size_type              = typename stl::allocator_traits<allocator_type>::size_type;
        using difference_type        = typename stl::allocator_traits<allocator_type>::difference_type;
        using pointer                = typename stl::allocator_traits<allocator_type>::pointer;
        using const_pointer          = typename stl::allocator_traits<allocator_type>::const_pointer;
        using iterator               = <pointer, value_type>;
        using const_iterator         = unicode_iterator<const_pointer, value_type>;
        using reverse_iterator       = stl::reverse_iterator<iterator>;
        using const_reverse_iterator = stl::reverse_iterator<const_iterator>;

        using string_view_type = ustring_view<value_type, traits_type>;

        // value returned by various member functions when they fail.
        static const size_type npos = static_cast<size_type>(-1);


      private:
        template <typename T>
        static constexpr bool convertible_to_ustring_view =
          stl::is_convertible_v<const T&, string_view_type> &&
          !stl::is_convertible_v<const T*, const ustring> &&
          !stl::is_convertible_v<const T&, const value_type*>;

        struct length_container_the_len {
            size_type length;
        };
        struct length_container_empty {};

        static constexpr bool has_length = sizeof(value_type) > sizeof(char16_t);
        using length_container =
          stl::conditional_t<has_length, length_container_empty, length_container_the_len>;


        // the reason I'm choosing data_end over size is that we're implementing a unicode string and not
        // ascii string; the difference is that "data_start + size != data_end";
        // todo: I need to think if we need a "size" field as well or not!
        pointer                              data_start;
        pointer                              data_end;
        length_container                     string_length;
        [[no_unique_address]] allocator_type alloc;

        static constexpr auto local_capacity = 15 / sizeof(value_type);
        union {
            value_type local_buf[local_capacity + 1];
            size_type  allocated_capacity;
        };

      public:
        template <typename NewStrT, typename... Args>
        constexpr NewStrT to(Args&&... args) const noexcept {
            using new_allocator_type                 = typename NewStrT::allocator_type;
            using new_char_type                      = typename NewStrT::value_type;
            using iterator_value_type                = typename iterator::value_type;
            constexpr bool has_same_allocator        = stl::is_same_v<new_allocator_type, allocator_type>;
            constexpr bool has_compatible_char_types = stl::is_convertible_v<new_char_type, value_type>;
            constexpr stl::size_t full_size          = sizeof(iterator_value_type);
            constexpr stl::size_t part_size          = full_size / sizeof(new_char_type);
            constexpr bool        is_utf8            = sizeof(new_char_type) == sizeof(char8_t);
            constexpr bool        is_utf16           = sizeof(new_char_type) == sizeof(char16_t);
            constexpr bool        requires_breaking  = sizeof(new_char_type) < sizeof(iterator_value_type);

            if constexpr (sizeof...(args) == 0) {
                if constexpr (requires { NewStrT{*this}; }) {
                    return NewStrT{*this}; // has a constructor we can use
                } else if constexpr (requires {
                                         NewStrT{this->data(), this->size(), this->private_get_allocator()};
                                     }) {
                    return NewStrT{this->data(), this->size(), this->private_get_allocator()};
                } else if constexpr (requires { NewStrT{this->data(), this->size()}; }) {
                    return NewStrT{this->data(), this->size()};
                } else if constexpr (has_compatible_char_types && has_same_allocator) {
                    // todo: fix this
                    NewStrT    output(this->private_get_allocator());
                    const auto len = this->size();
                    output.reserve(len);
                    const auto the_end = this->end();
                    for (auto it = this->begin(); it != the_end; ++it) {
                        output += static_cast<new_char_type>(*it);
                    }
                    output += "done";
                    return output;
                } else if constexpr (has_compatible_char_types) {
                    NewStrT    output(stl::forward<Args>(args)...);
                    const auto len = this->size();
                    output.reserve((len - 1) * part_size + 1); // max length required
                    if constexpr (sizeof(new_char_type) >= sizeof(value_type)) {
                        auto       it        = this->data();
                        const auto real_size = static_cast<value_type const*>(this->end().base()) - it;
                        const auto the_end   = it + real_size;
                        for (; it != the_end; ++it) {
                            output.push_back(static_cast<new_char_type>(*it));
                        }
                    } else {
                        const auto the_end = this->end();
                        for (auto it = this->begin(); it != the_end; ++it) {
                            auto ch = *it;
                            if constexpr (requires_breaking && is_utf8) {
                                if (ch <= 0x007F) {
                                    output.push_back(ch);
                                } else if (ch <= 0x07FF) {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x1F) | 0xC0);
                                } else if (ch <= 0xFFFF) {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 12) & 0x0F) | 0xE0);
                                } else {
                                    output.push_back((ch & 0x3F) | 0x80);
                                    output.push_back(((ch >> 6) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 12) & 0x3F) | 0x80);
                                    output.push_back(((ch >> 18) & 0x07) | 0xF0);
                                }
                            } else if constexpr (requires_breaking && is_utf16) {
                                if ((ch <= 0xD7FF) || ((ch >= 0xE000) && (ch <= 0xFFFF))) {
                                    output.push_back(ch);
                                } else {
                                    ch -= 0x010000;
                                    output.push_back((ch & 0x03FF) + 0xDC00);
                                    output.push_back(((ch >> 10) & 0x03FF) + 0xD800);
                                }
                            } else {
                                output.push_back(static_cast<new_char_type>(ch));
                            }
                        }
                    }
                    return output;
                }
            }
        }





        void data(pointer p) noexcept {
            data_start = p;
        }

        void length(size_type length) noexcept {
            if constexpr (has_length) {
                string_length.length = length;
            }
            data_end = data_start + length;
        }

        pointer local_data() {
            return stl::pointer_traits<pointer>::pointer_to(*local_buf);
        }

        [[nodiscard]] const_pointer local_data() const {
            return stl::pointer_traits<const_pointer>::pointer_to(*local_buf);
        }

        void capacity(size_type capacity) {
            allocated_capacity = capacity;
        }

        void set_length(size_type n) {
            length(n);
            traits_type::assign(data()[n], value_type());
        }

        [[nodiscard]] bool is_local() const {
            return data() == local_data();
        }

        // Create & Destroy
        pointer create(size_type&, size_type);

        void dispose() {
            if (!is_local())
                destroy(allocated_capacity);
        }

        void destroy(size_type size) noexcept(false) {
            alloc_traits::deallocate(private_get_allocator(), data(), size + 1);
        }

        // construct_aux is used to implement the 21.3.1 para 15 which
        // requires special behaviour if InIterator is an integral type
        template <typename InIterator>
        void construct_aux(InIterator beg, InIterator end, stl::false_type) {
            typedef typename stl::iterator_traits<InIterator>::iterator_category _Tag;
            construct(beg, end, _Tag());
        }

        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 438. Ambiguity in the "do the right thing" clause
        template <typename Integer>
        void construct_aux(Integer beg, Integer end, stl::true_type) {
            construct_aux_2(static_cast<size_type>(beg), end);
        }

        void construct_aux_2(size_type req, value_type c) {
            construct(req, c);
        }

        template <typename InIterator>
        void construct(InIterator beg, InIterator end) {
            using integral =
              stl::conditional_t<stl::is_integral_v<InIterator>, stl::true_type, stl::false_type>;
            construct_aux(beg, end, integral());
        }

        // For Input Iterators, used in istreambuf_iterators, etc.
        // NB: This is the special case for Input Iterators, used in
        // istreambuf_iterators, etc.
        // Input Iterators have a cost structure very different from
        // pointers, calling for a different coding style.
        template <typename InIterator>
        void construct(InIterator beg, InIterator end, stl::input_iterator_tag) {
            size_type len      = 0;
            size_type capacity = size_type(local_capacity);

            while (beg != end && len < capacity) {
                data()[len++] = *beg;
                ++beg;
            }

            try {
                while (beg != end) {
                    if (len == capacity) {
                        // Allocate more space.
                        capacity        = len + 1;
                        pointer another = create(capacity, len);
                        this->copy(another, data(), len);
                        dispose();
                        data(another);
                        capacity(capacity);
                    }
                    data()[len++] = *beg;
                    ++beg;
                }
            } catch (...) {
                dispose();
                throw;
            }

            set_length(len);
        }

        // For forward_iterators up to random_access_iterators, used for
        // string::iterator, value_type*, etc.
        template <typename FwdIterator>
        void construct(FwdIterator beg, FwdIterator end, stl::forward_iterator_tag) {
            // NB: Not required, but considered best practice.
            if (details::is_null_pointer(beg) && beg != end)
                throw stl::logic_error("ustring::construct null not valid");

            auto dnew = static_cast<size_type>(stl::distance(beg, end));

            if (dnew > size_type(local_capacity)) {
                data(create(dnew, size_type(0)));
                capacity(dnew);
            }

            // Check for out_of_range and length_error exceptions.
            try {
                this->copy_chars(data(), beg, end);
            } catch (...) {
                dispose();
                throw;
            }

            set_length(dnew);
        }

        void construct(size_type n, value_type c) {
            if (n > size_type(local_capacity)) {
                data(create(n, size_type(0)));
                capacity(n);
            }

            if (n)
                this->assign(data(), n, c);

            set_length(n);
        }


        allocator_type& private_get_allocator() {
            return alloc;
        }

        [[nodiscard]] const allocator_type& private_get_allocator() const {
            return alloc;
        }

      private:
        size_type check(size_type pos, const char* s) const {
            if (pos > this->size())
                throw stl::out_of_range(fmt::format("%s: pos (which is %zu) > "
                                                    "this->size() (which is %zu)",
                                                    s,
                                                    pos,
                                                    this->size()));
            return pos;
        }

        void check_length(size_type n1, size_type n2, const char* s) const {
            if (this->max_size() - (this->size() - n1) < n2)
                throw stl::length_error(s);
        }


        // NB: limit doesn't check for a bad pos value.
        [[nodiscard]] size_type limit(size_type pos, size_type off) const noexcept {
            const bool testoff = off < this->size() - pos;
            return testoff ? off : this->size() - pos;
        }

        // True if _Rep and source do not overlap.
        bool disjunct(const value_type* s) const noexcept {
            return (stl::less<const value_type*>()(s, data()) ||
                    stl::less<const value_type*>()(data() + this->size(), s));
        }

        // When n = 1 way faster than the general multichar
        // traits_type::copy/move/assign.
        static void copy(value_type* d, const value_type* s, size_type n) {
            if (n == 1)
                traits_type::assign(*d, *s);
            else
                traits_type::copy(d, s, n);
        }

        static void move(value_type* d, const value_type* s, size_type n) {
            if (n == 1)
                traits_type::assign(*d, *s);
            else
                traits_type::move(d, s, n);
        }

        static void assign(value_type* d, size_type n, value_type c) {
            if (n == 1)
                traits_type::assign(*d, c);
            else
                traits_type::assign(d, n, c);
        }

        // copy_chars is a separate template to permit specialization
        // to optimize for the common case of pointers as iterators.
        template <class IteratorT>
        static void copy_chars(value_type* p, IteratorT k1, IteratorT k2) {
            for (; k1 != k2; ++k1, (void) ++p)
                traits_type::assign(*p, *k1); // These types are off.
        }

        static void copy_chars(value_type* p, iterator k1, iterator k2) noexcept {
            copy_chars(p, k1.base(), k2.base());
        }

        static void copy_chars(value_type* p, const_iterator k1, const_iterator k2) noexcept {
            copy_chars(p, k1.base(), k2.base());
        }

        static void copy_chars(value_type* p, value_type* k1, value_type* k2) noexcept {
            copy(p, k1, k2 - k1);
        }

        static void copy_chars(value_type* p, const value_type* k1, const value_type* k2) noexcept {
            copy(p, k1, k2 - k1);
        }

        static int compare(size_type n1, size_type n2) noexcept {
            const difference_type d = difference_type(n1 - n2);

            if (d > details::int_limits<int>::max())
                return details::int_limits<int>::max();
            else if (d < details::int_limits<int>::min())
                return details::int_limits<int>::min();
            else
                return int(d);
        }

        void private_assign(const ustring& str) {
            if (this != &str) {
                const size_type rsize    = str.length();
                const size_type capacity = capacity();

                if (rsize > capacity) {
                    size_type new_capacity = rsize;
                    pointer   tmp          = create(new_capacity, capacity);
                    dispose();
                    data(tmp);
                    capacity(new_capacity);
                }

                if (rsize)
                    this->copy(data(), str.data(), rsize);

                set_length(rsize);
            }
        }


        void mutate(size_type pos, size_type len1, const value_type* s, size_type len2) {
            const size_type how_much = length() - pos - len1;

            size_type new_capacity = length() + len2 - len1;
            pointer   r            = create(new_capacity, capacity());

            if (pos)
                this->copy(r, data(), pos);
            if (s && len2)
                this->copy(r + pos, s, len2);
            if (how_much)
                this->copy(r + pos + len2, data() + pos + len1, how_much);

            dispose();
            data(r);
            capacity(new_capacity);
        }


        void private_erase(size_type pos, size_type n) {
            const size_type how_much = length() - pos - n;

            if (how_much && n)
                this->move(data() + pos, data() + pos + n, how_much);

            set_length(length() - n);
        }




      public:
        // Construct/copy/destroy:
        // NB: We overload ctors in some cases instead of using default
        // arguments, per 17.4.4.4 para. 2 item 2.

        /**
         *  @brief  Default constructor creates an empty string.
         */
        ustring() noexcept(stl::is_nothrow_default_constructible<allocator_type>::value)
          : data_start(local_data()),
            alloc() {
            set_length(0);
        }

        /**
         *  @brief  Construct an empty string using allocator @a a.
         */
        explicit ustring(const allocator_type& a) noexcept : data_start(local_data()), alloc(a) {
            set_length(0);
        }

        /**
         *  @brief  Construct string with copy of value of @a str.
         *  @param  str  Source string.
         */
        ustring(const ustring& str)
          : data_start(local_data()),
            alloc(alloc_traits::select_on_copy(str.private_get_allocator())) {
            construct(str.data(), str.data() + str.length());
        }

        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 2583. no way to supply an allocator for ustring(str, pos)
        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  a  Allocator to use.
         */
        ustring(const ustring& str, size_type pos, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            const value_type* start = str.data() + str.check(pos, "ustring::ustring");
            construct(start, start + str.limit(pos, npos));
        }

        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  n  Number of characters to copy.
         */
        ustring(const ustring& str, size_type pos, size_type n) : data_start(local_data()), alloc() {
            const value_type* start = str.data() + str.check(pos, "ustring::ustring");
            construct(start, start + str.limit(pos, n));
        }

        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  n  Number of characters to copy.
         *  @param  a  Allocator to use.
         */
        ustring(const ustring& str, size_type pos, size_type n, const allocator_type& a)
          : data_start(local_data()),
            alloc(a) {
            const value_type* start = str.data() + str.check(pos, "string::string");
            construct(start, start + str.limit(pos, n));
        }

        /**
         *  @brief  Construct string initialized by a character %array.
         *  @param  s  Source character %array.
         *  @param  n  Number of characters to copy.
         *  @param  a  Allocator to use (default is default allocator).
         *
         *  NB: @a s must have at least @a n characters, &apos;\\0&apos;
         *  has no special meaning.
         */
        ustring(const value_type* s, size_type n, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            construct(s, s + n);
        }

        /**
         *  @brief  Construct string as copy of a C string.
         *  @param  s  Source C string.
         *  @param  a  Allocator to use (default is default allocator).
         */
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 3076. ustring CTAD ambiguity
        template <typename = details::RequireAllocator<allocator_type>>
        ustring(const value_type* s, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            construct(s, s ? s + traits_type::length(s) : s + npos);
        }

        /**
         *  @brief  Construct string as multiple characters.
         *  @param  n  Number of characters.
         *  @param  c  Character to use.
         *  @param  a  Allocator to use (default is default allocator).
         */
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 3076. ustring CTAD ambiguity
        template <typename = details::RequireAllocator<allocator_type>>
        ustring(size_type n, value_type c, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            construct(n, c);
        }

        /**
         *  @brief  Move construct string.
         *  @param  str  Source string.
         *
         *  The newly-created string contains the exact contents of @a str.
         *  @a str is a valid, but unspecified string.
         **/
        ustring(ustring&& str) noexcept
          : data_start(local_data()),
            alloc(stl::move(str.private_get_allocator())) {
            if (str.is_local()) {
                traits_type::copy(local_buf, str.local_buf, local_capacity + 1);
            } else {
                data(str.data());
                capacity(str.allocated_capacity);
            }

            // Must use length() here not set_length() because
            // ustringbuf relies on writing into unallocated capacity so
            // we mess up the contents if we put a '\0' in the string.
            length(str.length());
            str.data(str.local_data());
            str.set_length(0);
        }

        /**
         *  @brief  Construct string from an initializer %list.
         *  @param  l  stl::initializer_list of characters.
         *  @param  a  Allocator to use (default is default allocator).
         */
        ustring(stl::initializer_list<value_type> l, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            construct(l.begin(), l.end());
        }

        ustring(const ustring& str, const allocator_type& a) : data_start(local_data()), alloc(a) {
            construct(str.begin(), str.end());
        }

        ustring(ustring&& str, const allocator_type& a) noexcept(alloc_traits::always_equal())
          : data_start(local_data()),
            alloc(a) {
            if (str.is_local()) {
                traits_type::copy(local_buf, str.local_buf, local_capacity + 1);
                length(str.length());
                str.set_length(0);
            } else if (alloc_traits::always_equal() || str.private_get_allocator() == a) {
                data(str.data());
                length(str.length());
                capacity(str.allocated_capacity);
                str.data(str.local_buf);
                str.set_length(0);
            } else {
                construct(str.begin(), str.end());
            }
        }


        /**
         *  @brief  Construct string as copy of a range.
         *  @param  beg  Start of range.
         *  @param  end  End of range.
         *  @param  a  Allocator to use (default is default allocator).
         */
        template <typename InputIterator>
        requires(details::RequireInputIter<InputIterator>)
          ustring(InputIterator beg, InputIterator end, const allocator_type& a = allocator_type())
          : data_start(local_data()),
            alloc(a) {
            construct(beg, end);
        }

        /**
         *  @brief  Construct string from a substring of a string_view.
         *  @param  t   Source object convertible to string view.
         *  @param  pos The index of the first character to copy from t.
         *  @param  n   The number of characters to copy from t.
         *  @param  a   Allocator to use.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>)
          ustring(const T& t, size_type pos, size_type n, const allocator_type& a = allocator_type())
          : ustring(to_string_view(t).substr(pos, n), a) {}

        /**
         *  @brief  Construct string from a string_view.
         *  @param  t  Source object convertible to string view.
         *  @param  a  Allocator to use (default is default allocator).
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) explicit ustring(const T&              t,
                                                                  const allocator_type& a = allocator_type())
          : ustring(sv_wrapper(to_string_view(t)), a) {}


        //        template <stl::size_t N>
        //        explicit ustring(const value_type val[N], const allocator_type& a = allocator_type{})
        //          : data_start{val},
        //            alloc{a} {}


        template <stl::size_t N>
        explicit ustring(typename value_type::value_type const val[N],
                         const allocator_type&                 a = allocator_type{})
          : ustring{reinterpret_cast<value_type const*>(val), N, a} {}

        explicit ustring(typename value_type::value_type const* val,
                         const allocator_type&                  a = allocator_type{})
          : ustring{reinterpret_cast<value_type const*>(val), a} {}

        template <typename ValT>
        requires(sizeof(ValT) == sizeof(typename value_type::value_type)) explicit ustring(
          ValT const*           val,
          const allocator_type& a = allocator_type{})
          : ustring{reinterpret_cast<value_type const*>(val), a} {}

        /**
         *  @brief  Destroy the string instance.
         */
        ~ustring() {
            dispose();
        }

        /**
         *  @brief  Assign the value of @a str to this string.
         *  @param  str  Source string.
         */
        ustring& operator=(const ustring& str) {
            return this->assign(str);
        }

        /**
         *  @brief  Copy contents of @a s into this string.
         *  @param  s  Source null-terminated string.
         */
        ustring& operator=(const value_type* s) {
            return this->assign(s);
        }

        /**
         *  @brief  Set value to string of length 1.
         *  @param  c  Source character.
         *
         *  Assigning to a character makes this string length 1 and
         *  (*this)[0] == @a c.
         */
        ustring& operator=(value_type c) {
            this->assign(1, c);
            return *this;
        }

        /**
         *  @brief  Move assign the value of @a str to this string.
         *  @param  str  Source string.
         *
         *  The contents of @a str are moved into this string (without copying).
         *  @a str is a valid, but unspecified string.
         **/
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 2063. Contradictory requirements for string move assignment
        ustring& operator=(ustring&& str) noexcept(alloc_traits::nothrow_move()) {
            if (!is_local() && alloc_traits::propagate_on_move_assign() && !alloc_traits::always_equal() &&
                private_get_allocator() != str.private_get_allocator()) {
                // Destroy existing storage before replacing allocator.
                destroy(allocated_capacity);
                data(local_data());
                set_length(0);
            }
            // Replace allocator if POCMA is true.
            details::alloc_on_move(private_get_allocator(), str.private_get_allocator());

            if (str.is_local()) {
                // We've always got room for a short string, just copy it.
                if (str.size())
                    this->copy(data(), str.data(), str.size());
                set_length(str.size());
            } else if (alloc_traits::propagate_on_move_assign() || alloc_traits::always_equal() ||
                       private_get_allocator() == str.private_get_allocator()) {
                // Just move the allocated pointer, our allocator can free it.
                pointer   data = nullptr;
                size_type capacity;
                if (!is_local()) {
                    if (alloc_traits::always_equal()) {
                        // str can reuse our existing storage.
                        data     = data();
                        capacity = allocated_capacity;
                    } else // str can't use it, so free it.
                        destroy(allocated_capacity);
                }

                data(str.data());
                length(str.length());
                capacity(str.allocated_capacity);
                if (data) {
                    str.data(data);
                    str.capacity(capacity);
                } else
                    str.data(str.local_buf);
            } else // Need to do a deep copy
                assign(str);
            str.clear();
            return *this;
        }

        /**
         *  @brief  Set value to string constructed from initializer %list.
         *  @param  l  stl::initializer_list.
         */
        ustring& operator=(stl::initializer_list<value_type> l) {
            this->assign(l.begin(), l.size());
            return *this;
        }

        /**
         *  @brief  Set value to string constructed from a string_view.
         *  @param  svt  An object convertible to string_view.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& operator=(const T& svt) {
            return this->assign(svt);
        }

        /**
         *  @brief  Convert to a string_view.
         *  @return A string_view.
         */
        operator string_view_type() const noexcept {
            return string_view_type(data(), size());
        }

        // Iterators:
        /**
         *  Returns a read/write iterator that points to the first character in
         *  the %string.
         */
        iterator begin() noexcept {
            return iterator(data());
        }

        /**
         *  Returns a read-only (constant) iterator that points to the first
         *  character in the %string.
         */
        [[nodiscard]] const_iterator begin() const noexcept {
            return const_iterator{data()};
        }

        /**
         *  Returns a read/write iterator that points one past the last
         *  character in the %string.
         */
        iterator end() noexcept {
            return iterator(data() + this->size());
        }

        /**
         *  Returns a read-only (constant) iterator that points one past the
         *  last character in the %string.
         */
        [[nodiscard]] const_iterator end() const noexcept {
            return const_iterator(data() + this->size());
        }

        /**
         *  Returns a read/write reverse iterator that points to the last
         *  character in the %string.  Iteration is done in reverse element
         *  order.
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(this->end());
        }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to the last character in the %string.  Iteration is done in
         *  reverse element order.
         */
        [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(this->end());
        }

        /**
         *  Returns a read/write reverse iterator that points to one before the
         *  first character in the %string.  Iteration is done in reverse
         *  element order.
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(this->begin());
        }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to one before the first character in the %string.  Iteration
         *  is done in reverse element order.
         */
        [[nodiscard]] const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(this->begin());
        }

        /**
         *  Returns a read-only (constant) iterator that points to the first
         *  character in the %string.
         */
        [[nodiscard]] const_iterator cbegin() const noexcept {
            return const_iterator(this->data());
        }

        /**
         *  Returns a read-only (constant) iterator that points one past the
         *  last character in the %string.
         */
        [[nodiscard]] const_iterator cend() const noexcept {
            return const_iterator(this->data() + this->size());
        }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to the last character in the %string.  Iteration is done in
         *  reverse element order.
         */
        [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(this->end());
        }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to one before the first character in the %string.  Iteration
         *  is done in reverse element order.
         */
        [[nodiscard]] const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(this->begin());
        }

      public:
        // Capacity:
        ///  Returns the number of characters in the string, not including any
        ///  null-termination.
        [[nodiscard]] size_type size() const noexcept {
            if constexpr (has_length) {
                return string_length.length;
            } else {
                return data_end - data_start;
            }
        }

        ///  Returns the number of characters in the string, not including any
        ///  null-termination.
        [[nodiscard]] size_type length() const noexcept {
            return size();
        }

        ///  Returns the size() of the largest possible %string.
        [[nodiscard]] size_type max_size() const noexcept {
            return (alloc_traits::max_size(private_get_allocator()) - 1) / 2;
        }

        /**
         *  @brief  Resizes the %string to the specified number of characters.
         *  @param  n  Number of characters the %string should contain.
         *  @param  c  Character to fill any new elements.
         *
         *  This function will %resize the %string to the specified
         *  number of characters.  If the number is smaller than the
         *  %string's current size the %string is truncated, otherwise
         *  the %string is extended and new elements are %set to @a c.
         */
        void resize(size_type n, value_type c) {
            const size_type size = this->size();
            if (size < n)
                this->append(n - size, c);
            else if (n < size)
                this->set_length(n);
        }


        /**
         *  @brief  Resizes the %string to the specified number of characters.
         *  @param  n  Number of characters the %string should contain.
         *
         *  This function will resize the %string to the specified length.  If
         *  the new size is smaller than the %string's current size the %string
         *  is truncated, otherwise the %string is extended and new characters
         *  are default-constructed.  For basic types such as char, this means
         *  setting them to 0.
         */
        void resize(size_type n) {
            this->resize(n, value_type());
        }

        ///  A non-binding request to reduce capacity() to size().
        void shrink_to_fit() noexcept {
#    if cpp_exceptions
            if (capacity() > size()) {
                try {
                    reserve(0);
                } catch (...) {}
            }
#    endif
        }

        /**
         *  Returns the total number of characters that the %string can hold
         *  before needing to allocate more memory.
         */
        [[nodiscard]] size_type capacity() const noexcept {
            return is_local() ? size_type(local_capacity) : allocated_capacity;
        }

        /**
         *  @brief  Attempt to preallocate enough memory for specified number of
         *          characters.
         *  @param  res_arg  Number of characters required.
         *  @throw  stl::length_error  If @a res_arg exceeds @c max_size().
         *
         *  This function attempts to reserve enough memory for the
         *  %string to hold the specified number of characters.  If the
         *  number requested is more than max_size(), length_error is
         *  thrown.
         *
         *  The advantage of this function is that if optimal code is a
         *  necessity and the user can determine the string length that will be
         *  required, the user can reserve the memory in %advance, and thus
         *  prevent a possible reallocation of memory and copying of %string
         *  data.
         */
        void reserve(size_type res = 0) {
            // Make sure we don't shrink below the current size.
            if (res < length())
                res = length();

            const size_type capacity = capacity();
            if (res != capacity) {
                if (res > capacity || res > size_type(local_capacity)) {
                    pointer tmp = create(res, capacity);
                    this->copy(tmp, data(), length() + 1);
                    dispose();
                    data(tmp);
                    capacity(res);
                } else if (!is_local()) {
                    this->copy(local_data(), data(), length() + 1);
                    destroy(capacity);
                    data(local_data());
                }
            }
        }


        /**
         *  Erases the string, making it empty.
         */
        void clear() noexcept {
            set_length(0);
        }

        /**
         *  Returns true if the %string is empty.  Equivalent to
         *  <code>*this == ""</code>.
         */
        [[nodiscard]] bool empty() const noexcept {
            return this->size() == 0;
        }

        // Element access:
        /**
         *  @brief  Subscript access to the data contained in the %string.
         *  @param  pos  The index of the character to access.
         *  @return  Read-only (constant) reference to the character.
         *
         *  This operator allows for easy, array-style, data access.
         *  Note that data access with this operator is unchecked and
         *  out_of_range lookups are not defined. (For checked lookups
         *  see at().)
         */
        const_reference operator[](size_type pos) const noexcept {
            assert(pos <= size());
            return data()[pos];
        }

        /**
         *  @brief  Subscript access to the data contained in the %string.
         *  @param  pos  The index of the character to access.
         *  @return  Read/write reference to the character.
         *
         *  This operator allows for easy, array-style, data access.
         *  Note that data access with this operator is unchecked and
         *  out_of_range lookups are not defined. (For checked lookups
         *  see at().)
         */
        reference operator[](size_type pos) {
            // Allow pos == size() both in C++98 mode, as v3 extension,
            // and in C++11 mode.
            assert(pos <= size());
            // In pedantic mode be strict in C++98 mode.
            _GLIBCXX_DEBUG_PEDASSERT(cplusplus >= 201103L || pos < size());
            return data()[pos];
        }

        /**
         *  @brief  Provides access to the data contained in the %string.
         *  @param n The index of the character to access.
         *  @return  Read-only (const) reference to the character.
         *  @throw  stl::out_of_range  If @a n is an invalid index.
         *
         *  This function provides for safer data access.  The parameter is
         *  first checked that it is in the range of the string.  The function
         *  throws out_of_range if the check fails.
         */
        [[nodiscard]] const_reference at(size_type n) const {
            if (n >= this->size())
                throw stl::out_of_range(fmt::format("ustring::at: n "
                                                    "(which is %zu) >= this->size() "
                                                    "(which is %zu)",
                                                    n,
                                                    this->size()));
            return data()[n];
        }

        /**
         *  @brief  Provides access to the data contained in the %string.
         *  @param n The index of the character to access.
         *  @return  Read/write reference to the character.
         *  @throw  stl::out_of_range  If @a n is an invalid index.
         *
         *  This function provides for safer data access.  The parameter is
         *  first checked that it is in the range of the string.  The function
         *  throws out_of_range if the check fails.
         */
        reference at(size_type n) {
            if (n >= size())
                throw stl::out_of_range(fmt::format("ustring::at: n "
                                                    "(which is %zu) >= this->size() "
                                                    "(which is %zu)",
                                                    n,
                                                    this->size()));
            return data()[n];
        }

        /**
         *  Returns a read/write reference to the data at the first
         *  element of the %string.
         */
        reference front() noexcept {
            assert(!empty());
            return operator[](0);
        }

        /**
         *  Returns a read-only (constant) reference to the data at the first
         *  element of the %string.
         */
        [[nodiscard]] const_reference front() const noexcept {
            assert(!empty());
            return operator[](0);
        }

        /**
         *  Returns a read/write reference to the data at the last
         *  element of the %string.
         */
        reference back() noexcept {
            assert(!empty());
            return operator[](this->size() - 1);
        }

        /**
         *  Returns a read-only (constant) reference to the data at the
         *  last element of the %string.
         */
        [[nodiscard]] const_reference back() const noexcept {
            assert(!empty());
            return operator[](this->size() - 1);
        }

        // Modifiers:
        /**
         *  @brief  Append a string to this string.
         *  @param str  The string to append.
         *  @return  Reference to this string.
         */
        ustring& operator+=(const ustring& str) {
            return this->append(str);
        }

        /**
         *  @brief  Append a C string.
         *  @param s  The C string to append.
         *  @return  Reference to this string.
         */
        ustring& operator+=(const value_type* s) {
            return this->append(s);
        }

        /**
         *  @brief  Append a character.
         *  @param c  The character to append.
         *  @return  Reference to this string.
         */
        ustring& operator+=(value_type c) {
            this->push_back(c);
            return *this;
        }

        /**
         *  @brief  Append an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to be appended.
         *  @return  Reference to this string.
         */
        ustring& operator+=(stl::initializer_list<value_type> l) {
            return this->append(l.begin(), l.size());
        }

        /**
         *  @brief  Append a string_view.
         *  @param svt  An object convertible to string_view to be appended.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& operator+=(const T& svt) {
            return this->append(svt);
        }

        /**
         *  @brief  Append a string to this string.
         *  @param str  The string to append.
         *  @return  Reference to this string.
         */
        ustring& append(const ustring& str) {
            return private_append(str.data(), str.size());
        }

        /**
         *  @brief  Append a substring.
         *  @param str  The string to append.
         *  @param pos  Index of the first character of str to append.
         *  @param n  The number of characters to append.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range if @a pos is not a valid index.
         *
         *  This function appends @a n characters from @a str
         *  starting at @a pos to this string.  If @a n is is larger
         *  than the number of available characters in @a str, the
         *  remainder of @a str is appended.
         */
        ustring& append(const ustring& str, size_type pos, size_type n = npos) {
            return private_append(str.data() + str.check(pos, "ustring::append"), str.limit(pos, n));
        }

        /**
         *  @brief  Append a C substring.
         *  @param s  The C string to append.
         *  @param n  The number of characters to append.
         *  @return  Reference to this string.
         */
        ustring& append(const value_type* s, size_type n) {
            glibcxx_requires_string_len(s, n);
            check_length(size_type(0), n, "ustring::append");
            return private_append(s, n);
        }

        /**
         *  @brief  Append a C string.
         *  @param s  The C string to append.
         *  @return  Reference to this string.
         */
        ustring& append(const value_type* s) {
            glibcxx_requires_string(s);
            const size_type n = traits_type::length(s);
            check_length(size_type(0), n, "ustring::append");
            return private_append(s, n);
        }

        /**
         *  @brief  Append multiple characters.
         *  @param n  The number of characters to append.
         *  @param c  The character to use.
         *  @return  Reference to this string.
         *
         *  Appends n copies of c to this string.
         */
        ustring& append(size_type n, value_type c) {
            return replace_aux(this->size(), size_type(0), n, c);
        }

        /**
         *  @brief  Append an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to append.
         *  @return  Reference to this string.
         */
        ustring& append(stl::initializer_list<value_type> l) {
            return this->append(l.begin(), l.size());
        }

        /**
         *  @brief  Append a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Appends characters in the range [first,last) to this string.
         */
        template <class InputIterator>
        requires(details::RequireInputIter<InputIterator>) ustring& append(InputIterator first,
                                                                           InputIterator last) {
            return this->replace(end(), end(), first, last);
        }

        /**
         *  @brief  Append a string_view.
         *  @param svt  An object convertible to string_view to be appended.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& append(const T& svt) {
            string_view_type sv = svt;
            return this->append(sv.data(), sv.size());
        }

        /**
         *  @brief  Append a range of characters from a string_view.
         *  @param svt  An object convertible to string_view to be appended from.
         *  @param pos The position in the string_view to append from.
         *  @param n   The number of characters to append from the string_view.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& append(const T&  svt,
                                                                 size_type pos,
                                                                 size_type n = npos) {
            string_view_type sv = svt;
            return append(sv.data() + details::sv_check(sv.size(), pos, "ustring::append"),
                          details::sv_limit(sv.size(), pos, n));
        }

        /**
         *  @brief  Append a single character.
         *  @param c  Character to append.
         */
        void push_back(value_type c) {
            const size_type size = this->size();
            if (size + 1 > this->capacity())
                this->mutate(size, size_type(0), 0, size_type(1));
            traits_type::assign(this->data()[size], c);
            this->set_length(size + 1);
        }

        /**
         *  @brief  Set value to contents of another string.
         *  @param  str  Source string to use.
         *  @return  Reference to this string.
         */
        ustring& assign(const ustring& str) {
            if (alloc_traits::propagate_on_copy_assign()) {
                if (!alloc_traits::always_equal() && !is_local() &&
                    private_get_allocator() != str.private_get_allocator()) {
                    // Propagating allocator cannot free existing storage so must
                    // deallocate it before replacing current allocator.
                    if (str.size() <= local_capacity) {
                        destroy(allocated_capacity);
                        data(local_data());
                        set_length(0);
                    } else {
                        const auto len       = str.size();
                        auto       the_alloc = str.private_get_allocator();
                        // If this allocation throws there are no effects:
                        auto ptr = alloc_traits::allocate(the_alloc, len + 1);
                        destroy(allocated_capacity);
                        data(ptr);
                        capacity(len);
                        set_length(len);
                    }
                }
                details::alloc_on_copy(private_get_allocator(), str.private_get_allocator());
            }
            this->private_assign(str);
            return *this;
        }

        /**
         *  @brief  Set value to contents of another string.
         *  @param  str  Source string to use.
         *  @return  Reference to this string.
         *
         *  This function sets this string to the exact contents of @a str.
         *  @a str is a valid, but unspecified string.
         */
        ustring& assign(ustring&& str) noexcept(alloc_traits::nothrow_move()) {
            // _GLIBCXX_RESOLVE_LIB_DEFECTS
            // 2063. Contradictory requirements for string move assignment
            return *this = stl::move(str);
        }

        /**
         *  @brief  Set value to a substring of a string.
         *  @param str  The string to use.
         *  @param pos  Index of the first character of str.
         *  @param n  Number of characters to use.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range if @a pos is not a valid index.
         *
         *  This function sets this string to the substring of @a str
         *  consisting of @a n characters at @a pos.  If @a n is
         *  is larger than the number of available characters in @a
         *  str, the remainder of @a str is used.
         */
        ustring& assign(const ustring& str, size_type pos, size_type n = npos) {
            return private_replace(size_type(0),
                                   this->size(),
                                   str.data() + str.check(pos, "ustring::assign"),
                                   str.limit(pos, n));
        }

        /**
         *  @brief  Set value to a C substring.
         *  @param s  The C string to use.
         *  @param n  Number of characters to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to the first @a n
         *  characters of @a s.  If @a n is is larger than the number of
         *  available characters in @a s, the remainder of @a s is used.
         */
        ustring& assign(const value_type* s, size_type n) {
            glibcxx_requires_string_len(s, n);
            return private_replace(size_type(0), this->size(), s, n);
        }

        /**
         *  @brief  Set value to contents of a C string.
         *  @param s  The C string to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to the value of @a s.
         *  The data is copied, so there is no dependence on @a s once the
         *  function returns.
         */
        ustring& assign(const value_type* s) {
            glibcxx_requires_string(s);
            return private_replace(size_type(0), this->size(), s, traits_type::length(s));
        }

        /**
         *  @brief  Set value to multiple characters.
         *  @param n  Length of the resulting string.
         *  @param c  The character to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to @a n copies of
         *  character @a c.
         */
        ustring& assign(size_type n, value_type c) {
            return replace_aux(size_type(0), this->size(), n, c);
        }

        /**
         *  @brief  Set value to a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Sets value of string to characters in the range [first,last).
         */
        template <class InputIterator>
        requires(details::RequireInputIter<InputIterator>) ustring& assign(InputIterator first,
                                                                           InputIterator last) {
            return this->replace(begin(), end(), first, last);
        }

        /**
         *  @brief  Set value to an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to assign.
         *  @return  Reference to this string.
         */
        ustring& assign(stl::initializer_list<value_type> l) {
            return this->assign(l.begin(), l.size());
        }

        /**
         *  @brief  Set value from a string_view.
         *  @param svt  The source object convertible to string_view.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& assign(const T& svt) {
            string_view_type sv = svt;
            return this->assign(sv.data(), sv.size());
        }

        /**
         *  @brief  Set value from a range of characters in a string_view.
         *  @param svt  The source object convertible to string_view.
         *  @param pos  The position in the string_view to assign from.
         *  @param n  The number of characters to assign.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& assign(const T&  svt,
                                                                 size_type pos,
                                                                 size_type n = npos) {
            string_view_type sv = svt;
            return private_replace(size_type(0),
                                   this->size(),
                                   sv.data() + details::sv_check(sv.size(), pos, "ustring::assign"),
                                   details::sv_limit(sv.size(), pos, n));
        }

        /**
         *  @brief  Insert multiple characters.
         *  @param p  Const_iterator referencing location in string to
         *              insert at.
         *  @param n  Number of characters to insert
         *  @param c  The character to insert.
         *  @return  Iterator referencing the first inserted char.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Inserts @a n copies of character @a c starting at the
         *  position referenced by iterator @a p.  If adding
         *  characters causes the length to exceed max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        iterator insert(const_iterator p, size_type n, value_type c) {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            this->replace(p, p, n, c);
            return iterator(this->data() + pos);
        }

        /**
         *  @brief  Insert a range of characters.
         *  @param p  Const_iterator referencing location in string to
         *              insert at.
         *  @param beg  Start of range.
         *  @param end  End of range.
         *  @return  Iterator referencing the first inserted char.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Inserts characters in range [beg,end).  If adding characters
         *  causes the length to exceed max_size(), length_error is
         *  thrown.  The value of the string doesn't change if an error
         *  is thrown.
         */
        template <class InputIterator>
        requires(details::RequireInputIter<InputIterator>) iterator
          insert(const_iterator p, InputIterator beg, InputIterator end) {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            this->replace(p, p, beg, end);
            return iterator(this->data() + pos);
        }
        /**
         *  @brief  Insert an stl::initializer_list of characters.
         *  @param p  Iterator referencing location in string to insert at.
         *  @param l  The stl::initializer_list of characters to insert.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         */
        iterator insert(const_iterator p, stl::initializer_list<value_type> l) {
            return this->insert(p, l.begin(), l.end());
        }

        // See PR libstdc++/83328
        void insert(iterator p, stl::initializer_list<value_type> l) {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            this->insert(p - begin(), l.begin(), l.size());
        }

        /**
         *  @brief  Insert value of a string.
         *  @param pos1 Position in string to insert at.
         *  @param str  The string to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Inserts value of @a str starting at @a pos1.  If adding
         *  characters causes the length to exceed max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        ustring& insert(size_type pos1, const ustring& str) {
            return this->replace(pos1, size_type(0), str.data(), str.size());
        }

        /**
         *  @brief  Insert a substring.
         *  @param pos1  Position in string to insert at.
         *  @param str   The string to insert.
         *  @param pos2  Start of characters in str to insert.
         *  @param n  Number of characters to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *  @throw  stl::out_of_range  If @a pos1 > size() or
         *  @a pos2 > @a str.size().
         *
         *  Starting at @a pos1, insert @a n character of @a str
         *  beginning with @a pos2.  If adding characters causes the
         *  length to exceed max_size(), length_error is thrown.  If @a
         *  pos1 is beyond the end of this string or @a pos2 is
         *  beyond the end of @a str, out_of_range is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        ustring& insert(size_type pos1, const ustring& str, size_type pos2, size_type n = npos) {
            return this->replace(pos1,
                                 size_type(0),
                                 str.data() + str.check(pos2, "ustring::insert"),
                                 str.limit(pos2, n));
        }

        /**
         *  @brief  Insert a C substring.
         *  @param pos  Position in string to insert at.
         *  @param s  The C string to insert.
         *  @param n  The number of characters to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *  @throw  stl::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Inserts the first @a n characters of @a s starting at @a
         *  pos.  If adding characters causes the length to exceed
         *  max_size(), length_error is thrown.  If @a pos is beyond
         *  end(), out_of_range is thrown.  The value of the string
         *  doesn't change if an error is thrown.
         */
        ustring& insert(size_type pos, const value_type* s, size_type n) {
            return this->replace(pos, size_type(0), s, n);
        }

        /**
         *  @brief  Insert a C string.
         *  @param pos  Position in string to insert at.
         *  @param s  The C string to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *  @throw  stl::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Inserts the first @a n characters of @a s starting at @a pos.  If
         *  adding characters causes the length to exceed max_size(),
         *  length_error is thrown.  If @a pos is beyond end(), out_of_range is
         *  thrown.  The value of the string doesn't change if an error is
         *  thrown.
         */
        ustring& insert(size_type pos, const value_type* s) {
            glibcxx_requires_string(s);
            return this->replace(pos, size_type(0), s, traits_type::length(s));
        }

        /**
         *  @brief  Insert multiple characters.
         *  @param pos  Index in string to insert at.
         *  @param n  Number of characters to insert
         *  @param c  The character to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *  @throw  stl::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Inserts @a n copies of character @a c starting at index
         *  @a pos.  If adding characters causes the length to exceed
         *  max_size(), length_error is thrown.  If @a pos > length(),
         *  out_of_range is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        ustring& insert(size_type pos, size_type n, value_type c) {
            return replace_aux(check(pos, "ustring::insert"), size_type(0), n, c);
        }

        /**
         *  @brief  Insert one character.
         *  @param p  Iterator referencing position in string to insert at.
         *  @param c  The character to insert.
         *  @return  Iterator referencing newly inserted char.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Inserts character @a c at position referenced by @a p.
         *  If adding character causes the length to exceed max_size(),
         *  length_error is thrown.  If @a p is beyond end of string,
         *  out_of_range is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        iterator insert(const_iterator p, value_type c) {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            replace_aux(pos, size_type(0), size_type(1), c);
            return iterator(data() + pos);
        }

        /**
         *  @brief  Insert a string_view.
         *  @param pos  Position in string to insert at.
         *  @param svt  The object convertible to string_view to insert.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& insert(size_type pos, const T& svt) {
            string_view_type sv = svt;
            return this->insert(pos, sv.data(), sv.size());
        }

        /**
         *  @brief  Insert a string_view.
         *  @param pos1  Position in string to insert at.
         *  @param svt   The object convertible to string_view to insert from.
         *  @param pos2  Start of characters in str to insert.
         *  @param n    The number of characters to insert.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& insert(size_type pos1,
                                                                 const T&  svt,
                                                                 size_type pos2,
                                                                 size_type n = npos) {
            string_view_type sv = svt;
            return this->replace(pos1,
                                 size_type(0),
                                 sv.data() + details::sv_check(sv.size(), pos2, "ustring::insert"),
                                 details::sv_limit(sv.size(), pos2, n));
        }

        /**
         *  @brief  Remove characters.
         *  @param pos  Index of first character to remove (default 0).
         *  @param n  Number of characters to remove (default remainder).
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Removes @a n characters from this string starting at @a
         *  pos.  The length of the string is reduced by @a n.  If
         *  there are < @a n characters to remove, the remainder of
         *  the string is truncated.  If @a p is beyond end of string,
         *  out_of_range is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        ustring& erase(size_type pos = 0, size_type n = npos) {
            check(pos, "ustring::erase");
            if (n == npos)
                this->set_length(pos);
            else if (n != 0)
                this->private_erase(pos, limit(pos, n));
            return *this;
        }

        /**
         *  @brief  Remove one character.
         *  @param position  Iterator referencing the character to remove.
         *  @return  iterator referencing same location after removal.
         *
         *  Removes the character at @a position from this string. The value
         *  of the string doesn't change if an error is thrown.
         */
        iterator erase(const_iterator position) {
            _GLIBCXX_DEBUG_PEDASSERT(position >= begin() && position < end());
            const size_type pos = position - begin();
            this->private_erase(pos, size_type(1));
            return iterator(data() + pos);
        }

        /**
         *  @brief  Remove a range of characters.
         *  @param first  Iterator referencing the first character to remove.
         *  @param last  Iterator referencing the end of the range.
         *  @return  Iterator referencing location of first after removal.
         *
         *  Removes the characters in the range [first,last) from this string.
         *  The value of the string doesn't change if an error is thrown.
         */
        iterator erase(const_iterator first, const_iterator last) {
            _GLIBCXX_DEBUG_PEDASSERT(first >= begin() && first <= last && last <= end());
            const size_type pos = first - begin();
            if (last == end())
                this->set_length(pos);
            else
                this->private_erase(pos, last - first);
            return iterator(this->data() + pos);
        }

        /**
         *  @brief  Remove the last character.
         *
         *  The string must be non-empty.
         */
        void pop_back() noexcept {
            assert(!empty());
            private_erase(size() - 1, 1);
        }

        /**
         *  @brief  Replace characters with value from another string.
         *  @param pos  Index of first character to replace.
         *  @param n  Number of characters to be replaced.
         *  @param str  String to insert.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos is beyond the end of this
         *  string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos,pos+n) from
         *  this string.  In place, the value of @a str is inserted.
         *  If @a pos is beyond end of string, out_of_range is thrown.
         *  If the length of the result exceeds max_size(), length_error
         *  is thrown.  The value of the string doesn't change if an
         *  error is thrown.
         */
        ustring& replace(size_type pos, size_type n, const ustring& str) {
            return this->replace(pos, n, str.data(), str.size());
        }

        /**
         *  @brief  Replace characters with value from another string.
         *  @param pos1  Index of first character to replace.
         *  @param n1  Number of characters to be replaced.
         *  @param str  String to insert.
         *  @param pos2  Index of first character of str to use.
         *  @param n2  Number of characters from str to use.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos1 > size() or @a pos2 >
         *  str.size().
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos1,pos1 + n) from this
         *  string.  In place, the value of @a str is inserted.  If @a pos is
         *  beyond end of string, out_of_range is thrown.  If the length of the
         *  result exceeds max_size(), length_error is thrown.  The value of the
         *  string doesn't change if an error is thrown.
         */
        ustring&
        replace(size_type pos1, size_type n1, const ustring& str, size_type pos2, size_type n2 = npos) {
            return this->replace(pos1,
                                 n1,
                                 str.data() + str.check(pos2, "ustring::replace"),
                                 str.limit(pos2, n2));
        }

        /**
         *  @brief  Replace characters with value of a C substring.
         *  @param pos  Index of first character to replace.
         *  @param n1  Number of characters to be replaced.
         *  @param s  C string to insert.
         *  @param n2  Number of characters from @a s to use.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos1 > size().
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos,pos + n1)
         *  from this string.  In place, the first @a n2 characters of
         *  @a s are inserted, or all of @a s if @a n2 is too large.  If
         *  @a pos is beyond end of string, out_of_range is thrown.  If
         *  the length of result exceeds max_size(), length_error is
         *  thrown.  The value of the string doesn't change if an error
         *  is thrown.
         */
        ustring& replace(size_type pos, size_type n1, const value_type* s, size_type n2) {
            glibcxx_requires_string_len(s, n2);
            return private_replace(check(pos, "ustring::replace"), limit(pos, n1), s, n2);
        }

        /**
         *  @brief  Replace characters with value of a C string.
         *  @param pos  Index of first character to replace.
         *  @param n1  Number of characters to be replaced.
         *  @param s  C string to insert.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos > size().
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos,pos + n1)
         *  from this string.  In place, the characters of @a s are
         *  inserted.  If @a pos is beyond end of string, out_of_range
         *  is thrown.  If the length of result exceeds max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        ustring& replace(size_type pos, size_type n1, const value_type* s) {
            glibcxx_requires_string(s);
            return this->replace(pos, n1, s, traits_type::length(s));
        }

        /**
         *  @brief  Replace characters with multiple characters.
         *  @param pos  Index of first character to replace.
         *  @param n1  Number of characters to be replaced.
         *  @param n2  Number of characters to insert.
         *  @param c  Character to insert.
         *  @return  Reference to this string.
         *  @throw  stl::out_of_range  If @a pos > size().
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos,pos + n1) from this
         *  string.  In place, @a n2 copies of @a c are inserted.
         *  If @a pos is beyond end of string, out_of_range is thrown.
         *  If the length of result exceeds max_size(), length_error is
         *  thrown.  The value of the string doesn't change if an error
         *  is thrown.
         */
        ustring& replace(size_type pos, size_type n1, size_type n2, value_type c) {
            return replace_aux(check(pos, "ustring::replace"), limit(pos, n1), n2, c);
        }

        /**
         *  @brief  Replace range of characters with string.
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param str  String value to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  the value of @a str is inserted.  If the length of result
         *  exceeds max_size(), length_error is thrown.  The value of
         *  the string doesn't change if an error is thrown.
         */
        ustring& replace(const_iterator i1, const_iterator i2, const ustring& str) {
            return this->replace(i1, i2, str.data(), str.size());
        }

        /**
         *  @brief  Replace range of characters with C substring.
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param s  C string value to insert.
         *  @param n  Number of characters from s to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  the first @a n characters of @a s are inserted.  If the
         *  length of result exceeds max_size(), length_error is thrown.
         *  The value of the string doesn't change if an error is
         *  thrown.
         */
        ustring& replace(const_iterator i1, const_iterator i2, const value_type* s, size_type n) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            return this->replace(i1 - begin(), i2 - i1, s, n);
        }

        /**
         *  @brief  Replace range of characters with C string.
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param s  C string value to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  the characters of @a s are inserted.  If the length of
         *  result exceeds max_size(), length_error is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        ustring& replace(const_iterator i1, const_iterator i2, const value_type* s) {
            glibcxx_requires_string(s);
            return this->replace(i1, i2, s, traits_type::length(s));
        }

        /**
         *  @brief  Replace range of characters with multiple characters
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param n  Number of characters to insert.
         *  @param c  Character to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  @a n copies of @a c are inserted.  If the length of
         *  result exceeds max_size(), length_error is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        ustring& replace(const_iterator i1, const_iterator i2, size_type n, value_type c) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            return replace_aux(i1 - begin(), i2 - i1, n, c);
        }

        /**
         *  @brief  Replace range of characters with range.
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param k1  Iterator referencing start of range to insert.
         *  @param k2  Iterator referencing end of range to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  characters in the range [k1,k2) are inserted.  If the
         *  length of result exceeds max_size(), length_error is thrown.
         *  The value of the string doesn't change if an error is
         *  thrown.
         */
        template <class InputIterator>
        requires(details::RequireInputIter<InputIterator>) ustring& replace(const_iterator i1,
                                                                            const_iterator i2,
                                                                            InputIterator  k1,
                                                                            InputIterator  k2) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace_dispatch(i1, i2, k1, k2, stl::false_type());
        }

        // Specializations for the common case of pointer and iterator:
        // useful to avoid the overhead of temporary buffering in replace.
        ustring& replace(const_iterator i1, const_iterator i2, value_type* k1, value_type* k2) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1, k1, k2 - k1);
        }

        ustring& replace(const_iterator i1, const_iterator i2, const value_type* k1, const value_type* k2) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1, k1, k2 - k1);
        }

        ustring& replace(const_iterator i1, const_iterator i2, iterator k1, iterator k2) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1, k1.base(), k2 - k1);
        }

        ustring& replace(const_iterator i1, const_iterator i2, const_iterator k1, const_iterator k2) {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2 && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1, k1.base(), k2 - k1);
        }

        /**
         *  @brief  Replace range of characters with stl::initializer_list.
         *  @param i1  Iterator referencing start of range to replace.
         *  @param i2  Iterator referencing end of range to replace.
         *  @param l  The stl::initializer_list of characters to insert.
         *  @return  Reference to this string.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [i1,i2).  In place,
         *  characters in the range [k1,k2) are inserted.  If the
         *  length of result exceeds max_size(), length_error is thrown.
         *  The value of the string doesn't change if an error is
         *  thrown.
         */
        ustring& replace(const_iterator i1, const_iterator i2, stl::initializer_list<value_type> l) {
            return this->replace(i1, i2, l.begin(), l.size());
        }

        /**
         *  @brief  Replace range of characters with string_view.
         *  @param pos  The position to replace at.
         *  @param n    The number of characters to replace.
         *  @param svt  The object convertible to string_view to insert.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& replace(size_type pos, size_type n, const T& svt) {
            string_view_type sv = svt;
            return this->replace(pos, n, sv.data(), sv.size());
        }

        /**
         *  @brief  Replace range of characters with string_view.
         *  @param pos1  The position to replace at.
         *  @param n1    The number of characters to replace.
         *  @param svt   The object convertible to string_view to insert from.
         *  @param pos2  The position in the string_view to insert from.
         *  @param n2    The number of characters to insert.
         *  @return  Reference to this string.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& replace(size_type pos1,
                                                                  size_type n1,
                                                                  const T&  svt,
                                                                  size_type pos2,
                                                                  size_type n2 = npos) {
            string_view_type sv = svt;
            return this->replace(pos1,
                                 n1,
                                 sv.data() + details::sv_check(sv.size(), pos2, "ustring::replace"),
                                 details::sv_limit(sv.size(), pos2, n2));
        }

        /**
         *  @brief  Replace range of characters with string_view.
         *  @param i1    An iterator referencing the start position
            to replace at.
         *  @param i2    An iterator referencing the end position
            for the replace.
         *  @param svt   The object convertible to string_view to insert from.
         *  @return  Reference to this string.
        */
        template <typename T>
        requires(convertible_to_ustring_view<T>) ustring& replace(const_iterator i1,
                                                                  const_iterator i2,
                                                                  const T&       svt) {
            string_view_type sv = svt;
            return this->replace(i1 - begin(), i2 - i1, sv);
        }

      private:
        template <class Integer>
        ustring&
        replace_dispatch(const_iterator i1, const_iterator i2, Integer n, Integer val, stl::true_type) {
            return replace_aux(i1 - begin(), i2 - i1, n, val);
        }

        template <class InputIterator>
        ustring& replace_dispatch(const_iterator i1,
                                  const_iterator i2,
                                  InputIterator  k1,
                                  InputIterator  k2,
                                  stl::false_type) {
            // _GLIBCXX_RESOLVE_LIB_DEFECTS
            // 2788. unintentionally require a default constructible allocator
            const ustring   s(k1, k2, this->private_get_allocator());
            const size_type n1 = i2 - i1;
            return private_replace(i1 - begin(), n1, s.data(), s.size());
        }

        ustring& replace_aux(size_type pos1, size_type n1, size_type n2, value_type c) {
            check_length(n1, n2, "ustring::replace_aux");

            const size_type old_size = this->size();
            const size_type new_size = old_size + n2 - n1;

            if (new_size <= this->capacity()) {
                pointer p = this->data() + pos1;

                const size_type how_much = old_size - pos1 - n1;
                if (how_much && n1 != n2)
                    this->move(p + n2, p + n1, how_much);
            } else
                this->mutate(pos1, n1, 0, n2);

            if (n2)
                this->assign(this->data() + pos1, n2, c);

            this->set_length(new_size);
            return *this;
        }


        ustring& private_replace(size_type pos, size_type len1, const value_type* s, const size_type len2) {
            check_length(len1, len2, "ustring::replace");

            const size_type old_size = this->size();
            const size_type new_size = old_size + len2 - len1;

            if (new_size <= this->capacity()) {
                pointer p = this->data() + pos;

                const size_type how_much = old_size - pos - len1;
                if (disjunct(s)) {
                    if (how_much && len1 != len2)
                        this->move(p + len2, p + len1, how_much);
                    if (len2)
                        this->copy(p, s, len2);
                } else {
                    // Work in-place.
                    if (len2 && len2 <= len1)
                        this->move(p, s, len2);
                    if (how_much && len1 != len2)
                        this->move(p + len2, p + len1, how_much);
                    if (len2 > len1) {
                        if (s + len2 <= p + len1)
                            this->move(p, s, len2);
                        else if (s >= p + len1)
                            this->copy(p, s + len2 - len1, len2);
                        else {
                            const size_type nleft = (p + len1) - s;
                            this->move(p, s, nleft);
                            this->copy(p + nleft, p + len2, len2 - nleft);
                        }
                    }
                }
            } else
                this->mutate(pos, len1, s, len2);

            this->set_length(new_size);
            return *this;
        }

        ustring& private_append(const value_type* s, size_type n) {
            const size_type len = n + this->size();

            if (len <= this->capacity()) {
                if (n)
                    this->copy(this->data() + this->size(), s, n);
            } else
                this->mutate(this->size(), size_type(0), s, n);

            this->set_length(len);
            return *this;
        }


      public:
        /**
         *  @brief  Copy substring into C string.
         *  @param s  C string to copy value into.
         *  @param n  Number of characters to copy.
         *  @param pos  Index of first character to copy.
         *  @return  Number of characters actually copied
         *  @throw  stl::out_of_range  If pos > size().
         *
         *  Copies up to @a n characters starting at @a pos into the
         *  C string @a s.  If @a pos is %greater than size(),
         *  out_of_range is thrown.
         */
        size_type copy(value_type* s, size_type n, size_type pos = 0) const;

        /**
         *  @brief  Swap contents with another string.
         *  @param s  String to swap with.
         *
         *  Exchanges the contents of this string with that of @a s in constant
         *  time.
         */
        void swap(ustring& s) noexcept;

        // String operations:
        /**
         *  @brief  Return const pointer to null-terminated contents.
         *
         *  This is a handle to internal data.  Do not modify or dire things may
         *  happen.
         */
        [[nodiscard]] const value_type* c_str() const noexcept {
            return data();
        }

        /**
         *  @brief  Return const pointer to contents.
         *
         *  This is a pointer to internal data.  It is undefined to modify
         *  the contents through the returned pointer. To get a pointer that
         *  allows modifying the contents use @c &str[0] instead,
         *  (or in C++17 the non-const @c str.data() overload).
         */
        [[nodiscard]] const value_type* data() const noexcept {
            return data_start;
        }

        /**
         *  @brief  Return non-const pointer to contents.
         *
         *  This is a pointer to the character sequence held by the string.
         *  Modifying the characters in the sequence is allowed.
         */
        value_type* data() noexcept {
            return data_start;
        }

        /**
         *  @brief  Return copy of allocator used to construct this string.
         */
        [[nodiscard]] allocator_type get_allocator() const noexcept {
            return private_get_allocator();
        }

        /**
         *  @brief  Find position of a C substring.
         *  @param s  C string to locate.
         *  @param pos  Index of character to search from.
         *  @param n  Number of characters from @a s to search for.
         *  @return  Index of start of first occurrence.
         *
         *  Starting from @a pos, searches forward for the first @a
         *  n characters in @a s within this string.  If found,
         *  returns the index where it begins.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type find(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            const size_type size = this->size();

            if (n == 0)
                return pos <= size ? pos : npos;
            if (pos >= size)
                return npos;

            const value_type        elem0 = s[0];
            const value_type* const data  = data();
            const value_type*       first = data + pos;
            const value_type* const last  = data + size;
            size_type               len   = size - pos;

            while (len >= n) {
                // Find the first occurrence of elem0:
                first = traits_type::find(first, len - n + 1, elem0);
                if (!first)
                    return npos;
                // Compare the full strings from the first occurrence of elem0.
                // We already know that first[0] == s[0] but compare them again
                // anyway because s is probably aligned, which helps memcmp.
                if (traits_type::compare(first, s, n) == 0)
                    return first - data;
                len = last - ++first;
            }
            return npos;
        }

        /**
         *  @brief  Find position of a string.
         *  @param str  String to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of start of first occurrence.
         *
         *  Starting from @a pos, searches forward for value of @a str within
         *  this string.  If found, returns the index where it begins.  If not
         *  found, returns npos.
         */
        [[nodiscard]] size_type find(const ustring& str, size_type pos = 0) const noexcept {
            return this->find(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find position of a string_view.
         *  @param svt  The object convertible to string_view to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of start of first occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type find(const T& svt, size_type pos = 0)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->find(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find position of a C string.
         *  @param s  C string to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of start of first occurrence.
         *
         *  Starting from @a pos, searches forward for the value of @a
         *  s within this string.  If found, returns the index where
         *  it begins.  If not found, returns npos.
         */
        size_type find(const value_type* s, size_type pos = 0) const noexcept {
            glibcxx_requires_string(s);
            return this->find(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find position of a character.
         *  @param c  Character to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for @a c within
         *  this string.  If found, returns the index where it was
         *  found.  If not found, returns npos.
         */
        [[nodiscard]] size_type find(value_type c, size_type pos = 0) const noexcept {
            size_type       ret  = npos;
            const size_type size = this->size();
            if (pos < size) {
                const value_type* data = data();
                const size_type   n    = size - pos;
                const value_type* p    = traits_type::find(data + pos, n, c);
                if (p)
                    ret = p - data;
            }
            return ret;
        }


        /**
         *  @brief  Find last position of a string.
         *  @param str  String to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of start of last occurrence.
         *
         *  Starting from @a pos, searches backward for value of @a
         *  str within this string.  If found, returns the index where
         *  it begins.  If not found, returns npos.
         */
        [[nodiscard]] size_type rfind(const ustring& str, size_type pos = npos) const noexcept {
            return this->rfind(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find last position of a string_view.
         *  @param svt  The object convertible to string_view to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of start of last occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type rfind(const T& svt, size_type pos = npos)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->rfind(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find last position of a C substring.
         *  @param s  C string to locate.
         *  @param pos  Index of character to search back from.
         *  @param n  Number of characters from s to search for.
         *  @return  Index of start of last occurrence.
         *
         *  Starting from @a pos, searches backward for the first @a
         *  n characters in @a s within this string.  If found,
         *  returns the index where it begins.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type rfind(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            const size_type size = this->size();
            if (n <= size) {
                pos               = stl::min(size_type(size - n), pos);
                const CharT* data = data();
                do {
                    if (traits_type::compare(data + pos, s, n) == 0)
                        return pos;
                } while (pos-- > 0);
            }
            return npos;
        }


        /**
         *  @brief  Find last position of a C string.
         *  @param s  C string to locate.
         *  @param pos  Index of character to start search at (default end).
         *  @return  Index of start of  last occurrence.
         *
         *  Starting from @a pos, searches backward for the value of
         *  @a s within this string.  If found, returns the index
         *  where it begins.  If not found, returns npos.
         */
        size_type rfind(const value_type* s, size_type pos = npos) const {
            glibcxx_requires_string(s);
            return this->rfind(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find last position of a character.
         *  @param c  Character to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for @a c within
         *  this string.  If found, returns the index where it was
         *  found.  If not found, returns npos.
         */
        [[nodiscard]] size_type rfind(value_type c, size_type pos = npos) const noexcept {
            size_type size = this->size();
            if (size) {
                if (--size > pos)
                    size = pos;
                for (++size; size-- > 0;)
                    if (traits_type::eq(data()[size], c))
                        return size;
            }
            return npos;
        }


        /**
         *  @brief  Find position of a character of string.
         *  @param str  String containing characters to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for one of the
         *  characters of @a str within this string.  If found,
         *  returns the index where it was found.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type find_first_of(const ustring& str, size_type pos = 0) const noexcept {
            return this->find_first_of(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find position of a character of a string_view.
         *  @param svt  An object convertible to string_view containing
         *                characters to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type find_first_of(const T& svt, size_type pos = 0)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->find_first_of(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find position of a character of C substring.
         *  @param s  String containing characters to locate.
         *  @param pos  Index of character to search from.
         *  @param n  Number of characters from s to search for.
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for one of the
         *  first @a n characters of @a s within this string.  If
         *  found, returns the index where it was found.  If not found,
         *  returns npos.
         */
        [[nodiscard]] size_type
        find_first_of(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            for (; n && pos < this->size(); ++pos) {
                const value_type* p = traits_type::find(s, n, data()[pos]);
                if (p)
                    return pos;
            }
            return npos;
        }


        /**
         *  @brief  Find position of a character of C string.
         *  @param s  String containing characters to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for one of the
         *  characters of @a s within this string.  If found, returns
         *  the index where it was found.  If not found, returns npos.
         */
        size_type find_first_of(const value_type* s, size_type pos = 0) const noexcept {
            glibcxx_requires_string(s);
            return this->find_first_of(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find position of a character.
         *  @param c  Character to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for the character
         *  @a c within this string.  If found, returns the index
         *  where it was found.  If not found, returns npos.
         *
         *  Note: equivalent to find(c, pos).
         */
        [[nodiscard]] size_type find_first_of(value_type c, size_type pos = 0) const noexcept {
            return this->find(c, pos);
        }

        /**
         *  @brief  Find last position of a character of string.
         *  @param str  String containing characters to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for one of the
         *  characters of @a str within this string.  If found,
         *  returns the index where it was found.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type find_last_of(const ustring& str, size_type pos = npos) const noexcept {
            return this->find_last_of(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find last position of a character of string.
         *  @param svt  An object convertible to string_view containing
         *                characters to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type find_last_of(const T& svt, size_type pos = npos)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->find_last_of(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find last position of a character of C substring.
         *  @param s  C string containing characters to locate.
         *  @param pos  Index of character to search back from.
         *  @param n  Number of characters from s to search for.
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for one of the
         *  first @a n characters of @a s within this string.  If
         *  found, returns the index where it was found.  If not found,
         *  returns npos.
         */
        size_type find_last_of(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            size_type size = this->size();
            if (size && n) {
                if (--size > pos)
                    size = pos;
                do {
                    if (traits_type::find(s, n, data()[size]))
                        return size;
                } while (size-- != 0);
            }
            return npos;
        }


        /**
         *  @brief  Find last position of a character of C string.
         *  @param s  C string containing characters to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for one of the
         *  characters of @a s within this string.  If found, returns
         *  the index where it was found.  If not found, returns npos.
         */
        size_type find_last_of(const value_type* s, size_type pos = npos) const noexcept {
            glibcxx_requires_string(s);
            return this->find_last_of(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find last position of a character.
         *  @param c  Character to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for @a c within
         *  this string.  If found, returns the index where it was
         *  found.  If not found, returns npos.
         *
         *  Note: equivalent to rfind(c, pos).
         */
        [[nodiscard]] size_type find_last_of(value_type c, size_type pos = npos) const noexcept {
            return this->rfind(c, pos);
        }

        /**
         *  @brief  Find position of a character not in string.
         *  @param str  String containing characters to avoid.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for a character not contained
         *  in @a str within this string.  If found, returns the index where it
         *  was found.  If not found, returns npos.
         */
        [[nodiscard]] size_type find_first_not_of(const ustring& str, size_type pos = 0) const noexcept {
            return this->find_first_not_of(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find position of a character not in a string_view.
         *  @param svt  A object convertible to string_view containing
         *                characters to avoid.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type find_first_not_of(const T& svt, size_type pos = 0)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->find_first_not_of(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find position of a character not in C substring.
         *  @param s  C string containing characters to avoid.
         *  @param pos  Index of character to search from.
         *  @param n  Number of characters from s to consider.
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for a character not
         *  contained in the first @a n characters of @a s within
         *  this string.  If found, returns the index where it was
         *  found.  If not found, returns npos.
         */
        [[nodiscard]] size_type
        find_first_not_of(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            for (; pos < this->size(); ++pos)
                if (!traits_type::find(s, n, data()[pos]))
                    return pos;
            return npos;
        }


        /**
         *  @brief  Find position of a character not in C string.
         *  @param s  C string containing characters to avoid.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for a character not
         *  contained in @a s within this string.  If found, returns
         *  the index where it was found.  If not found, returns npos.
         */
        [[nodiscard]] size_type find_first_not_of(const value_type* s, size_type pos = 0) const noexcept {
            glibcxx_requires_string(s);
            return this->find_first_not_of(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find position of a different character.
         *  @param c  Character to avoid.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         *
         *  Starting from @a pos, searches forward for a character
         *  other than @a c within this string.  If found, returns the
         *  index where it was found.  If not found, returns npos.
         */
        [[nodiscard]] size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept {
            for (; pos < this->size(); ++pos)
                if (!traits_type::eq(data()[pos], c))
                    return pos;
            return npos;
        }


        /**
         *  @brief  Find last position of a character not in string.
         *  @param str  String containing characters to avoid.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for a character
         *  not contained in @a str within this string.  If found,
         *  returns the index where it was found.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type find_last_not_of(const ustring& str, size_type pos = npos) const noexcept {
            return this->find_last_not_of(str.data(), pos, str.size());
        }

        /**
         *  @brief  Find last position of a character not in a string_view.
         *  @param svt  An object convertible to string_view containing
         *                characters to avoid.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) size_type
          find_last_not_of(const T& svt, size_type pos = npos)
        const noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return this->find_last_not_of(sv.data(), pos, sv.size());
        }

        /**
         *  @brief  Find last position of a character not in C substring.
         *  @param s  C string containing characters to avoid.
         *  @param pos  Index of character to search back from.
         *  @param n  Number of characters from s to consider.
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for a character not
         *  contained in the first @a n characters of @a s within this string.
         *  If found, returns the index where it was found.  If not found,
         *  returns npos.
         */
        [[nodiscard]] size_type
        find_last_not_of(const value_type* s, size_type pos, size_type n) const noexcept {
            glibcxx_requires_string_len(s, n);
            size_type size = this->size();
            if (size) {
                if (--size > pos)
                    size = pos;
                do {
                    if (!traits_type::find(s, n, data()[size]))
                        return size;
                } while (size--);
            }
            return npos;
        }

        /**
         *  @brief  Find last position of a character not in C string.
         *  @param s  C string containing characters to avoid.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for a character
         *  not contained in @a s within this string.  If found,
         *  returns the index where it was found.  If not found, returns
         *  npos.
         */
        [[nodiscard]] size_type find_last_not_of(const value_type* s, size_type pos = npos) const noexcept {
            glibcxx_requires_string(s);
            return this->find_last_not_of(s, pos, traits_type::length(s));
        }

        /**
         *  @brief  Find last position of a different character.
         *  @param c  Character to avoid.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         *
         *  Starting from @a pos, searches backward for a character other than
         *  @a c within this string.  If found, returns the index where it was
         *  found.  If not found, returns npos.
         */
        [[nodiscard]] size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept {
            size_type size = this->size();
            if (size) {
                if (--size > pos)
                    size = pos;
                do {
                    if (!traits_type::eq(data()[size], c))
                        return size;
                } while (size--);
            }
            return npos;
        }


        /**
         *  @brief  Get a substring.
         *  @param pos  Index of first character (default 0).
         *  @param n  Number of characters in substring (default remainder).
         *  @return  The new string.
         *  @throw  stl::out_of_range  If pos > size().
         *
         *  Construct and return a new string using the @a n
         *  characters starting at @a pos.  If the string is too
         *  short, use the remainder of the characters.  If @a pos is
         *  beyond the end of the string, out_of_range is thrown.
         */
        [[nodiscard]] ustring substr(size_type pos = 0, size_type n = npos) const {
            return ustring(*this, check(pos, "ustring::substr"), n);
        }

        /**
         *  @brief  Compare to a string.
         *  @param str  String to compare against.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Returns an integer < 0 if this string is ordered before @a
         *  str, 0 if their values are equivalent, or > 0 if this
         *  string is ordered after @a str.  Determines the effective
         *  length rlen of the strings to compare as the smallest of
         *  size() and str.size().  The function then compares the two
         *  strings by calling traits::compare(data(), str.data(),rlen).
         *  If the result of the comparison is nonzero returns it,
         *  otherwise the shorter one is ordered first.
         */
        [[nodiscard]] int compare(const ustring& str) const {
            const size_type size  = this->size();
            const size_type osize = str.size();
            const size_type len   = stl::min(size, osize);

            int r = traits_type::compare(data(), str.data(), len);
            if (!r)
                r = compare(size, osize);
            return r;
        }

        /**
         *  @brief  Compare to a string_view.
         *  @param svt An object convertible to string_view to compare against.
         *  @return  Integer < 0, 0, or > 0.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) int compare(const T& svt) const
          noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv    = svt;
            const size_type  size  = this->size();
            const size_type  osize = sv.size();
            const size_type  len   = stl::min(size, osize);

            int r = traits_type::compare(data(), sv.data(), len);
            if (!r)
                r = compare(size, osize);
            return r;
        }

        /**
         *  @brief  Compare to a string_view.
         *  @param pos  A position in the string to start comparing from.
         *  @param n  The number of characters to compare.
         *  @param svt  An object convertible to string_view to compare
         *                against.
         *  @return  Integer < 0, 0, or > 0.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) int compare(size_type pos, size_type n, const T& svt) const
          noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return string_view_type(*this).substr(pos, n).compare(sv);
        }

        /**
         *  @brief  Compare to a string_view.
         *  @param pos1  A position in the string to start comparing from.
         *  @param n1  The number of characters to compare.
         *  @param svt  An object convertible to string_view to compare
         *                against.
         *  @param pos2  A position in the string_view to start comparing from.
         *  @param n2  The number of characters to compare.
         *  @return  Integer < 0, 0, or > 0.
         */
        template <typename T>
        requires(convertible_to_ustring_view<T>) int compare(size_type pos1,
                                                             size_type n1,
                                                             const T&  svt,
                                                             size_type pos2,
                                                             size_type n2 = npos) const
          noexcept(stl::is_same<T, string_view_type>::value) {
            string_view_type sv = svt;
            return string_view_type(*this).substr(pos1, n1).compare(sv.substr(pos2, n2));
        }

        /**
         *  @brief  Compare substring to a string.
         *  @param pos  Index of first character of substring.
         *  @param n  Number of characters in substring.
         *  @param str  String to compare against.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Form the substring of this string from the @a n characters
         *  starting at @a pos.  Returns an integer < 0 if the
         *  substring is ordered before @a str, 0 if their values are
         *  equivalent, or > 0 if the substring is ordered after @a
         *  str.  Determines the effective length rlen of the strings
         *  to compare as the smallest of the length of the substring
         *  and @a str.size().  The function then compares the two
         *  strings by calling
         *  traits::compare(substring.data(),str.data(),rlen).  If the
         *  result of the comparison is nonzero returns it, otherwise
         *  the shorter one is ordered first.
         */
        [[nodiscard]] int compare(size_type pos, size_type n, const ustring& str) const {
            check(pos, "ustring::compare");
            n                     = limit(pos, n);
            const size_type osize = str.size();
            const size_type len   = stl::min(n, osize);
            int             r     = traits_type::compare(data() + pos, str.data(), len);
            if (!r)
                r = compare(n, osize);
            return r;
        }


        /**
         *  @brief  Compare substring to a substring.
         *  @param pos1  Index of first character of substring.
         *  @param n1  Number of characters in substring.
         *  @param str  String to compare against.
         *  @param pos2  Index of first character of substring of str.
         *  @param n2  Number of characters in substring of str.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Form the substring of this string from the @a n1
         *  characters starting at @a pos1.  Form the substring of @a
         *  str from the @a n2 characters starting at @a pos2.
         *  Returns an integer < 0 if this substring is ordered before
         *  the substring of @a str, 0 if their values are equivalent,
         *  or > 0 if this substring is ordered after the substring of
         *  @a str.  Determines the effective length rlen of the
         *  strings to compare as the smallest of the lengths of the
         *  substrings.  The function then compares the two strings by
         *  calling
         *  traits::compare(substring.data(),str.substr(pos2,n2).data(),rlen).
         *  If the result of the comparison is nonzero returns it,
         *  otherwise the shorter one is ordered first.
         */
        [[nodiscard]] int
        compare(size_type pos1, size_type n1, const ustring& str, size_type pos2, size_type n2 = npos) const {
            check(pos1, "ustring::compare");
            str.check(pos2, "ustring::compare");
            n1                  = limit(pos1, n1);
            n2                  = str.limit(pos2, n2);
            const size_type len = stl::min(n1, n2);
            int             r   = traits_type::compare(data() + pos1, str.data() + pos2, len);
            if (!r)
                r = compare(n1, n2);
            return r;
        }


        /**
         *  @brief  Compare to a C string.
         *  @param s  C string to compare against.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Returns an integer < 0 if this string is ordered before @a s, 0 if
         *  their values are equivalent, or > 0 if this string is ordered after
         *  @a s.  Determines the effective length rlen of the strings to
         *  compare as the smallest of size() and the length of a string
         *  constructed from @a s.  The function then compares the two strings
         *  by calling traits::compare(data(),s,rlen).  If the result of the
         *  comparison is nonzero returns it, otherwise the shorter one is
         *  ordered first.
         */
        [[nodiscard]] int compare(const value_type* s) const noexcept {
#    ifdef __glibcxx_requires_string
            __glibcxx_requires_string(s);
#    endif
            const size_type size  = this->size();
            const size_type osize = traits_type::length(s);
            const size_type len   = stl::min(size, osize);
            int             r     = traits_type::compare(data(), s, len);
            if (!r)
                r = compare(size, osize);
            return r;
        }


        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 5 String::compare specification questionable
        /**
         *  @brief  Compare substring to a C string.
         *  @param pos  Index of first character of substring.
         *  @param n1  Number of characters in substring.
         *  @param s  C string to compare against.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Form the substring of this string from the @a n1
         *  characters starting at @a pos.  Returns an integer < 0 if
         *  the substring is ordered before @a s, 0 if their values
         *  are equivalent, or > 0 if the substring is ordered after @a
         *  s.  Determines the effective length rlen of the strings to
         *  compare as the smallest of the length of the substring and
         *  the length of a string constructed from @a s.  The
         *  function then compares the two string by calling
         *  traits::compare(substring.data(),s,rlen).  If the result of
         *  the comparison is nonzero returns it, otherwise the shorter
         *  one is ordered first.
         */
        [[nodiscard]] int compare(size_type pos, size_type n1, const value_type* s) const {
            glibcxx_requires_string(s);
            check(pos, "ustring::compare");
            n1                    = limit(pos, n1);
            const size_type osize = traits_type::length(s);
            const size_type len   = stl::min(n1, osize);
            int             r     = traits_type::compare(data() + pos, s, len);
            if (!r)
                r = compare(n1, osize);
            return r;
        }


        /**
         *  @brief  Compare substring against a character %array.
         *  @param pos  Index of first character of substring.
         *  @param n1  Number of characters in substring.
         *  @param s  character %array to compare against.
         *  @param n2  Number of characters of s.
         *  @return  Integer < 0, 0, or > 0.
         *
         *  Form the substring of this string from the @a n1
         *  characters starting at @a pos.  Form a string from the
         *  first @a n2 characters of @a s.  Returns an integer < 0
         *  if this substring is ordered before the string from @a s,
         *  0 if their values are equivalent, or > 0 if this substring
         *  is ordered after the string from @a s.  Determines the
         *  effective length rlen of the strings to compare as the
         *  smallest of the length of the substring and @a n2.  The
         *  function then compares the two strings by calling
         *  traits::compare(substring.data(),s,rlen).  If the result of
         *  the comparison is nonzero returns it, otherwise the shorter
         *  one is ordered first.
         *
         *  NB: s must have at least n2 characters, &apos;\\0&apos; has
         *  no special meaning.
         */
        int compare(size_type pos, size_type n1, const value_type* s, size_type n2) const {
            glibcxx_requires_string_len(s, n2);
            check(pos, "ustring::compare");
            n1                  = limit(pos, n1);
            const size_type len = stl::min(n1, n2);
            int             r   = traits_type::compare(data() + pos, s, len);
            if (!r)
                r = compare(n1, n2);
            return r;
        }


        [[nodiscard]] bool starts_with(ustring_view<value_type, traits_type> x) const noexcept {
            return string_view_type(this->data(), this->size()).starts_with(x);
        }

        [[nodiscard]] bool starts_with(value_type x) const noexcept {
            return string_view_type(this->data(), this->size()).starts_with(x);
        }

        bool starts_with(const value_type* x) const noexcept {
            return string_view_type(this->data(), this->size()).starts_with(x);
        }

        [[nodiscard]] bool ends_with(ustring_view<value_type, traits_type> x) const noexcept {
            return string_view_type(this->data(), this->size()).ends_with(x);
        }

        [[nodiscard]] bool ends_with(value_type x) const noexcept {
            return string_view_type(this->data(), this->size()).ends_with(x);
        }

        bool ends_with(const value_type* x) const noexcept {
            return string_view_type(this->data(), this->size()).ends_with(x);
        }

        // todo
        // Allow ustringbuf::xfer_bufptrs to call length:
        template <typename, typename, typename>
        friend class ustringbuf;
    };

    template <typename InputIterator,
              typename CharT  = typename stl::iterator_traits<InputIterator>::value_type,
              typename AllocT = stl::allocator<CharT>,
              typename        = details::RequireInputIterEnableIf<InputIterator>,
              typename        = details::RequireAllocator<AllocT>>
    ustring(InputIterator, InputIterator, AllocT = AllocT())
      -> ustring<CharT, stl::char_traits<CharT>, AllocT>;

    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 3075. ustring needs deduction guides from ustring_view
    template <typename CharT,
              typename ChTraitsT,
              typename AllocT = stl::allocator<CharT>,
              typename        = details::RequireAllocator<AllocT>>
    ustring(ustring_view<CharT, ChTraitsT>, const AllocT& = AllocT()) -> ustring<CharT, ChTraitsT, AllocT>;

    template <typename CharT,
              typename ChTraitsT,
              typename AllocT = stl::allocator<CharT>,
              typename        = details::RequireAllocator<AllocT>>
    ustring(ustring_view<CharT, ChTraitsT>,
            typename ustring<CharT, ChTraitsT, AllocT>::size_type,
            typename ustring<CharT, ChTraitsT, AllocT>::size_type,
            const AllocT& = AllocT()) -> ustring<CharT, ChTraitsT, AllocT>;




    // operator+
    /**
     *  @brief  Concatenate two strings.
     *  @param lhs  First string.
     *  @param rhs  Last string.
     *  @return  New string with value of @a lhs followed by @a rhs.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    ustring<CharT, ChTraitsT, AllocT> operator+(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                                                const ustring<CharT, ChTraitsT, AllocT>& rhs) {
        ustring<CharT, ChTraitsT, AllocT> str(lhs);
        str.append(rhs);
        return str;
    }

    /**
     *  @brief  Concatenate C string and string.
     *  @param lhs  First string.
     *  @param rhs  Last string.
     *  @return  New string with value of @a lhs followed by @a rhs.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    ustring<CharT, ChTraitsT, AllocT> operator+(const CharT*                             lhs,
                                                const ustring<CharT, ChTraitsT, AllocT>& rhs);

    /**
     *  @brief  Concatenate character and string.
     *  @param lhs  First string.
     *  @param rhs  Last string.
     *  @return  New string with @a lhs followed by @a rhs.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    ustring<CharT, ChTraitsT, AllocT> operator+(CharT lhs, const ustring<CharT, ChTraitsT, AllocT>& rhs);

    /**
     *  @brief  Concatenate string and C string.
     *  @param lhs  First string.
     *  @param rhs  Last string.
     *  @return  New string with @a lhs followed by @a rhs.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                                                       const CharT*                             rhs) {
        ustring<CharT, ChTraitsT, AllocT> str(lhs);
        str.append(rhs);
        return str;
    }

    /**
     *  @brief  Concatenate string and character.
     *  @param lhs  First string.
     *  @param rhs  Last string.
     *  @return  New string with @a lhs followed by @a rhs.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                                                       CharT                                    rhs) {
        typedef ustring<CharT, ChTraitsT, AllocT> string_type;
        typedef typename string_type::size_type   size_type;
        string_type                               str(lhs);
        str.append(size_type(1), rhs);
        return str;
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(ustring<CharT, ChTraitsT, AllocT>&&      lhs,
                                                       const ustring<CharT, ChTraitsT, AllocT>& rhs) {
        return stl::move(lhs.append(rhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                                                       ustring<CharT, ChTraitsT, AllocT>&&      rhs) {
        return stl::move(rhs.insert(0, lhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(ustring<CharT, ChTraitsT, AllocT>&& lhs,
                                                       ustring<CharT, ChTraitsT, AllocT>&& rhs) {
        using alloc_traits = stl::allocator_traits<AllocT>;
        bool use_rhs       = false;
        if constexpr (typename alloc_traits::is_always_equal{}) {
            use_rhs = true;
        } else {
            if (lhs.private_get_allocator() == rhs.private_get_allocator())
                use_rhs = true;
        }
        if (use_rhs) {
            const auto size = lhs.size() + rhs.size();
            if (size > lhs.capacity() && size <= rhs.capacity())
                return stl::move(rhs.insert(0, lhs));
        }
        return stl::move(lhs.append(rhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(const CharT*                        lhs,
                                                       ustring<CharT, ChTraitsT, AllocT>&& rhs) {
        return stl::move(rhs.insert(0, lhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(CharT lhs, ustring<CharT, ChTraitsT, AllocT>&& rhs) {
        return stl::move(rhs.insert(0, 1, lhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(ustring<CharT, ChTraitsT, AllocT>&& lhs,
                                                       const CharT*                        rhs) {
        return stl::move(lhs.append(rhs));
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline ustring<CharT, ChTraitsT, AllocT> operator+(ustring<CharT, ChTraitsT, AllocT>&& lhs, CharT rhs) {
        return stl::move(lhs.append(1, rhs));
    }

    // operator ==
    /**
     *  @brief  Test equivalence of two strings.
     *  @param lhs  First string.
     *  @param rhs  Second string.
     *  @return  True if @a lhs.compare(@a rhs) == 0.  False otherwise.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline bool operator==(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                           const ustring<CharT, ChTraitsT, AllocT>& rhs) noexcept {
        return lhs.compare(rhs) == 0;
    }

    template <typename CharT>
    requires(details::is_char<CharT>::value) inline bool operator==(const ustring<CharT>& lhs,
                                                                    const ustring<CharT>& rhs) noexcept {
        return (lhs.size() == rhs.size() &&
                !stl::char_traits<CharT>::compare(lhs.data(), rhs.data(), lhs.size()));
    }

    /**
     *  @brief  Test equivalence of string and C string.
     *  @param lhs  String.
     *  @param rhs  C string.
     *  @return  True if @a lhs.compare(@a rhs) == 0.  False otherwise.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline bool operator==(const ustring<CharT, ChTraitsT, AllocT>& lhs, const CharT* rhs) {
        return lhs.compare(rhs) == 0;
    }


    template <typename CharT, typename ChTraitsT, typename AllocT, typename CharT2>
    requires(sizeof(CharT) == sizeof(CharT2)) inline bool
    operator==(const ustring<CharT, ChTraitsT, AllocT>& lhs, const CharT2* rhs) {
        return lhs.compare(reinterpret_cast<const CharT*>(rhs)) == 0;
    }


    /**
     *  @brief  Three-way comparison of a string and a C string.
     *  @param lhs  A string.
     *  @param rhs  A null-terminated string.
     *  @return  A value indicating whether `lhs` is less than, equal to,
     *	       greater than, or incomparable with `rhs`.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline auto operator<=>(const ustring<CharT, ChTraitsT, AllocT>& lhs,
                            const ustring<CharT, ChTraitsT, AllocT>& rhs) noexcept
      -> decltype(details::char_traits_cmp_cat<ChTraitsT>(0)) {
        return details::char_traits_cmp_cat<ChTraitsT>(lhs.compare(rhs));
    }

    /**
     *  @brief  Three-way comparison of a string and a C string.
     *  @param lhs  A string.
     *  @param rhs  A null-terminated string.
     *  @return  A value indicating whether `lhs` is less than, equal to,
     *	       greater than, or incomparable with `rhs`.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline auto operator<=>(const ustring<CharT, ChTraitsT, AllocT>& lhs, const CharT* rhs) noexcept
      -> decltype(details::char_traits_cmp_cat<ChTraitsT>(0)) {
        return details::char_traits_cmp_cat<ChTraitsT>(lhs.compare(rhs));
    }

    // template <typename CharT, typename ChTraitsT, typename AllocT, typename CharT2>
    // requires(sizeof(CharT2) == sizeof(CharT)) inline auto
    // operator<=>(const ustring<CharT, ChTraitsT, AllocT>& lhs, const CharT2* rhs) noexcept
    //   -> decltype(details::char_traits_cmp_cat<ChTraitsT>(0)) {
    //     return details::char_traits_cmp_cat<ChTraitsT>(lhs.compare(reinterpret_cast<const CharT*>(rhs)));
    // }


    /**
     *  @brief  Swap contents of two strings.
     *  @param lhs  First string.
     *  @param rhs  Second string.
     *
     *  Exchanges the contents of @a lhs and @a rhs in constant time.
     */
    template <typename CharT, typename ChTraitsT, typename AllocT>
    inline void swap(ustring<CharT, ChTraitsT, AllocT>& lhs,
                     ustring<CharT, ChTraitsT, AllocT>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }




    template <typename CharT, typename ChTraitsT, typename AllocT>
    const typename ustring<CharT, ChTraitsT, AllocT>::size_type ustring<CharT, ChTraitsT, AllocT>::npos;

    template <typename CharT, typename ChTraitsT, typename AllocT>
    void ustring<CharT, ChTraitsT, AllocT>::swap(ustring& s) noexcept {
        if (this == &s)
            return;

        alloc_traits::on_swap(private_get_allocator(), s.private_get_allocator());

        if (is_local())
            if (s.is_local()) {
                if (length() && s.length()) {
                    CharT tmp_data[local_capacity + 1];
                    traits_type::copy(tmp_data, s.local_buf, local_capacity + 1);
                    traits_type::copy(s.local_buf, local_buf, local_capacity + 1);
                    traits_type::copy(local_buf, tmp_data, local_capacity + 1);
                } else if (s.length()) {
                    traits_type::copy(local_buf, s.local_buf, local_capacity + 1);
                    length(s.length());
                    s.set_length(0);
                    return;
                } else if (length()) {
                    traits_type::copy(s.local_buf, local_buf, local_capacity + 1);
                    s.length(length());
                    set_length(0);
                    return;
                }
            } else {
                const size_type tmp_capacity = s.allocated_capacity;
                traits_type::copy(s.local_buf, local_buf, local_capacity + 1);
                data(s.data());
                s.data(s.local_buf);
                capacity(tmp_capacity);
            }
        else {
            const size_type tmp_capacity = allocated_capacity;
            if (s.is_local()) {
                traits_type::copy(local_buf, s.local_buf, local_capacity + 1);
                s.data(data());
                data(local_buf);
            } else {
                pointer tmp_ptr = data();
                data(s.data());
                s.data(tmp_ptr);
                capacity(s.allocated_capacity);
            }
            s.capacity(tmp_capacity);
        }

        const size_type tmp_length = length();
        length(s.length());
        s.length(tmp_length);
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    typename ustring<CharT, ChTraitsT, AllocT>::pointer
    ustring<CharT, ChTraitsT, AllocT>::create(size_type& capacity, size_type old_capacity) {
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 83.  String::npos vs. string::max_size()
        if (capacity > max_size())
            throw stl::length_error("ustring::create");

        // The below implements an exponential growth policy, necessary to
        // meet amortized linear time requirements of the library: see
        // http://gcc.gnu.org/ml/libstdc++/2001-07/msg00085.html.
        if (capacity > old_capacity && capacity < 2 * old_capacity) {
            capacity = 2 * old_capacity;
            // Never allocate a string bigger than max_size.
            if (capacity > max_size())
                capacity = max_size();
        }

        // NB: Need an array of char_type[capacity], plus a terminating
        // null char_type() element.
        return alloc_traits::allocate(private_get_allocator(), capacity + 1);
    }





    template <typename CharT, typename ChTraitsT, typename AllocT>
    typename ustring<CharT, ChTraitsT, AllocT>::size_type
    ustring<CharT, ChTraitsT, AllocT>::copy(CharT* s, size_type n, size_type pos) const {
        check(pos, "ustring::copy");
        n = limit(pos, n);
        __glibcxx_requires_string_len(s, n);
        if (n)
            copy(s, data() + pos, n);
        // 21.3.5.7 par 3: do not append null.  (good.)
        return n;
    }


    template <typename CharT, typename ChTraitsT, typename AllocT>
    ustring<CharT, ChTraitsT, AllocT> operator+(const CharT*                             lhs,
                                                const ustring<CharT, ChTraitsT, AllocT>& rhs) {
        glibcxx_requires_string(lhs);
        using string_type     = ustring<CharT, ChTraitsT, AllocT>;
        using size_type       = typename string_type::size_type;
        using char_alloc_type = typename stl::allocator_traits<AllocT>::template rebind<CharT>::other;
        using alloc_traits    = stl::allocator_traits<char_alloc_type>;
        const size_type len   = ChTraitsT::length(lhs);
        string_type     str(alloc_traits::select_on_copy(rhs.private_get_allocator()));
        str.reserve(len + rhs.size());
        str.append(lhs, len);
        str.append(rhs);
        return str;
    }

    template <typename CharT, typename ChTraitsT, typename AllocT>
    ustring<CharT, ChTraitsT, AllocT> operator+(CharT lhs, const ustring<CharT, ChTraitsT, AllocT>& rhs) {
        using string_type     = ustring<CharT, ChTraitsT, AllocT>;
        using size_type       = typename string_type::size_type;
        using char_alloc_type = typename stl::allocator_traits<AllocT>::template rebind<CharT>::other;
        using alloc_traits    = stl::allocator_traits<char_alloc_type>;
        string_type     str(alloc_traits::select_on_copy(rhs.private_get_allocator()));
        const size_type len = rhs.size();
        str.reserve(len + 1);
        str.append(size_type(1), lhs);
        str.append(rhs);
        return str;
    }


    //
    //    /**
    //     *  @brief  Read stream into a string.
    //     *  @param is  Input stream.
    //     *  @param str  Buffer to store into.
    //     *  @return  Reference to the input stream.
    //     *
    //     *  Stores characters from @a is into @a str until whitespace is
    //     *  found, the end of the stream is encountered, or str.max_size()
    //     *  is reached.  If is.width() is non-zero, that is the limit on the
    //     *  number of characters stored into @a str.  Any previous
    //     *  contents of @a str are erased.
    //     */
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    stl::basic_istream<CharT, ChTraitsT>& operator>>(stl::basic_istream<CharT, ChTraitsT>& is,
    //                                                     ustring<CharT, ChTraitsT, AllocT>&    str);
    //
    //    template <>
    //    stl::basic_istream<char>& operator>>(stl::basic_istream<char>& is, ustring<char>& str);
    //
    //    /**
    //     *  @brief  Write string to a stream.
    //     *  @param os  Output stream.
    //     *  @param str  String to write out.
    //     *  @return  Reference to the output stream.
    //     *
    //     *  Output characters of @a str into os following the same rules as for
    //     *  writing a C string.
    //     */
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    inline stl::basic_ostream<CharT, ChTraitsT>& operator<<(stl::basic_ostream<CharT, ChTraitsT>&    os,
    //                                                            const ustring<CharT, ChTraitsT, AllocT>&
    //                                                            str) {
    //        // _GLIBCXX_RESOLVE_LIB_DEFECTS
    //        // 586. string inserter not a formatted function
    //        return ostream_insert(os, str.data(), str.size());
    //    }
    //
    //    /**
    //     *  @brief  Read a line from stream into a string.
    //     *  @param is  Input stream.
    //     *  @param str  Buffer to store into.
    //     *  @param delim  Character marking end of line.
    //     *  @return  Reference to the input stream.
    //     *
    //     *  Stores characters from @a is into @a str until @a delim is
    //     *  found, the end of the stream is encountered, or str.max_size()
    //     *  is reached.  Any previous contents of @a str are erased.  If
    //     *  @a delim is encountered, it is extracted but not stored into
    //     *  @a str.
    //     */
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    stl::basic_istream<CharT, ChTraitsT>&
    //    getline(stl::basic_istream<CharT, ChTraitsT>& is, ustring<CharT, ChTraitsT, AllocT>& str, CharT
    //    delim);
    //
    //    /**
    //     *  @brief  Read a line from stream into a string.
    //     *  @param is  Input stream.
    //     *  @param str  Buffer to store into.
    //     *  @return  Reference to the input stream.
    //     *
    //     *  Stores characters from is into @a str until &apos;\n&apos; is
    //     *  found, the end of the stream is encountered, or str.max_size()
    //     *  is reached.  Any previous contents of @a str are erased.  If
    //     *  end of line is encountered, it is extracted but not stored into
    //     *  @a str.
    //     */
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    inline stl::basic_istream<CharT, ChTraitsT>& getline(stl::basic_istream<CharT, ChTraitsT>& is,
    //                                                         ustring<CharT, ChTraitsT, AllocT>&    str) {
    //        return stl::getline(is, str, is.widen('\n'));
    //    }
    //
    //    /// Read a line from an rvalue stream into a string.
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    inline stl::basic_istream<CharT, ChTraitsT>&
    //    getline(stl::basic_istream<CharT, ChTraitsT>&& is, ustring<CharT, ChTraitsT, AllocT>& str, CharT
    //    delim) {
    //        return stl::getline(is, str, delim);
    //    }
    //
    //    /// Read a line from an rvalue stream into a string.
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    inline stl::basic_istream<CharT, ChTraitsT>& getline(stl::basic_istream<CharT, ChTraitsT>&& is,
    //                                                         ustring<CharT, ChTraitsT, AllocT>&     str) {
    //        return stl::getline(is, str);
    //    }
    //
    //    template <>
    //    stl::basic_istream<char>& getline(stl::basic_istream<char>& in, ustring<char>& str, char delim);
    //
    //    template <>
    //    stl::basic_istream<wchar_t>&
    //    getline(stl::basic_istream<wchar_t>& in, ustring<wchar_t>& str, wchar_t delim);
    //    // 21.3.7.9 ustring::getline and operators
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    stl::basic_istream<CharT, ChTraitsT>& operator>>(stl::basic_istream<CharT, ChTraitsT>& in,
    //                                                     ustring<CharT, ChTraitsT, AllocT>&    str) {
    //        typedef stl::basic_istream<CharT, ChTraitsT> istream_type;
    //        typedef ustring<CharT, ChTraitsT, AllocT>    string_type;
    //        typedef typename istream_type::ios_base      ios_base;
    //        typedef typename istream_type::int_type      int_type;
    //        typedef typename string_type::size_type      size_type;
    //        typedef stl::ctype<CharT>                    ctype_type;
    //        typedef typename ctype_type::ctype_base      ctype_base;
    //
    //        size_type                     extracted = 0;
    //        typename ios_base::iostate    err       = ios_base::goodbit;
    //        typename istream_type::sentry cerb(in, false);
    //        if (cerb) {
    //            try {
    //                // Avoid reallocation for common case.
    //                str.erase();
    //                CharT                 buf[128];
    //                size_type             len = 0;
    //                const stl::streamsize w   = in.width();
    //                const size_type       n   = w > 0 ? static_cast<size_type>(w) : str.max_size();
    //                const ctype_type&     ct  = use_facet<ctype_type>(in.getloc());
    //                const int_type        eof = ChTraitsT::eof();
    //                int_type              c   = in.rdbuf()->sgetc();
    //
    //                while (extracted < n && !ChTraitsT::eq_int_type(c, eof) &&
    //                       !ct.is(ctype_base::space, ChTraitsT::to_char_type(c))) {
    //                    if (len == sizeof(buf) / sizeof(CharT)) {
    //                        str.append(buf, sizeof(buf) / sizeof(CharT));
    //                        len = 0;
    //                    }
    //                    buf[len++] = ChTraitsT::to_char_type(c);
    //                    ++extracted;
    //                    c = in.rdbuf()->snextc();
    //                }
    //                str.append(buf, len);
    //
    //                if (ChTraitsT::eq_int_type(c, eof))
    //                    err |= ios_base::eofbit;
    //                in.width(0);
    //            } catch (cxxabiv1::forced_unwind&) {
    //                in.setstate(ios_base::badbit);
    //                throw;
    //            } catch (...) {
    //                // _GLIBCXX_RESOLVE_LIB_DEFECTS
    //                // 91. Description of operator>> and getline() for string<>
    //                // might cause endless loop
    //                in.setstate(ios_base::badbit);
    //            }
    //        }
    //        // 211.  operator>>(istream&, string&) doesn't set failbit
    //        if (!extracted)
    //            err |= ios_base::failbit;
    //        if (err)
    //            in.setstate(err);
    //        return in;
    //    }
    //
    //    template <typename CharT, typename ChTraitsT, typename AllocT>
    //    stl::basic_istream<CharT, ChTraitsT>&
    //    getline(stl::basic_istream<CharT, ChTraitsT>& in, ustring<CharT, ChTraitsT, AllocT>& str, CharT
    //    delim) {
    //        typedef stl::basic_istream<CharT, ChTraitsT> istream_type;
    //        typedef ustring<CharT, ChTraitsT, AllocT>    string_type;
    //        typedef typename istream_type::ios_base      ios_base;
    //        typedef typename istream_type::int_type      int_type;
    //        typedef typename string_type::size_type      size_type;
    //
    //        size_type                     extracted = 0;
    //        const size_type               n         = str.max_size();
    //        typename ios_base::iostate    err       = ios_base::goodbit;
    //        typename istream_type::sentry cerb(in, true);
    //        if (cerb) {
    //            try {
    //                str.erase();
    //                const int_type idelim = ChTraitsT::to_int_type(delim);
    //                const int_type eof    = ChTraitsT::eof();
    //                int_type       c      = in.rdbuf()->sgetc();
    //
    //                while (extracted < n && !ChTraitsT::eq_int_type(c, eof) &&
    //                       !ChTraitsT::eq_int_type(c, idelim)) {
    //                    str += ChTraitsT::to_char_type(c);
    //                    ++extracted;
    //                    c = in.rdbuf()->snextc();
    //                }
    //
    //                if (ChTraitsT::eq_int_type(c, eof))
    //                    err |= ios_base::eofbit;
    //                else if (ChTraitsT::eq_int_type(c, idelim)) {
    //                    ++extracted;
    //                    in.rdbuf()->sbumpc();
    //                } else
    //                    err |= ios_base::failbit;
    //            } catch (cxxabiv1::forced_unwind&) {
    //                in.setstate(ios_base::badbit);
    //                throw;
    //            } catch (...) {
    //                // _GLIBCXX_RESOLVE_LIB_DEFECTS
    //                // 91. Description of operator>> and getline() for string<>
    //                // might cause endless loop
    //                in.setstate(ios_base::badbit);
    //            }
    //        }
    //        if (!extracted)
    //            err |= ios_base::failbit;
    //        if (err)
    //            in.setstate(err);
    //        return in;
    //    }
    //
    //    // Inhibit implicit instantiations for required instantiations,
    //    // which are defined via explicit instantiations elsewhere.
    //#if _GLIBCXX_EXTERN_TEMPLATE
    //    // The explicit instantiation definitions in src/c++11/string-inst.cc and
    //    // src/c++17/string-inst.cc only instantiate the members required for C++17
    //    // and earlier standards (so not C++20's starts_with and ends_with).
    //    // Suppress the explicit instantiation declarations for C++20, so C++20
    //    // code will implicitly instantiate stl::string and stl::wstring as needed.
    //#    if cplusplus <= 201703L && _GLIBCXX_EXTERN_TEMPLATE > 0
    //    extern template class ustring<char>;
    //#    elif !_GLIBCXX_USE_CXX11_ABI
    //    // Still need to prevent implicit instantiation of the COW empty rep,
    //    // to ensure the definition in libstdc++.so is unique (PR 86138).
    //    extern template ustring<char>::size_type ustring<char>::_Rep::empty_rep_storage[];
    //#    endif
    //
    //    extern template stl::basic_istream<char>& operator>>(stl::basic_istream<char>&, string&);
    //    extern template stl::basic_ostream<char>& operator<<(stl::basic_ostream<char>&, const string&);
    //    extern template stl::basic_istream<char>& getline(stl::basic_istream<char>&, string&, char);
    //    extern template stl::basic_istream<char>& getline(stl::basic_istream<char>&, string&);
    //
    //#    if cplusplus <= 201703L && _GLIBCXX_EXTERN_TEMPLATE > 0
    //    extern template class ustring<wchar_t>;
    //#    elif !_GLIBCXX_USE_CXX11_ABI
    //    extern template ustring<wchar_t>::size_type ustring<wchar_t>::_Rep::empty_rep_storage[];
    //#    endif
    //
    //    extern template stl::basic_istream<wchar_t>& operator>>(stl::basic_istream<wchar_t>&, wstring&);
    //    extern template stl::basic_ostream<wchar_t>& operator<<(stl::basic_ostream<wchar_t>&, const
    //    wstring&); extern template stl::basic_istream<wchar_t>& getline(stl::basic_istream<wchar_t>&,
    //    wstring&, wchar_t); extern template stl::basic_istream<wchar_t>&
    //    getline(stl::basic_istream<wchar_t>&, wstring&);
    //#endif // _GLIBCXX_EXTERN_TEMPLATE
    //

#endif

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
