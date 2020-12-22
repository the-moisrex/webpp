// Created by moisrex on 12/21/20.

#ifndef WEBPP_USTRING_HPP
#define WEBPP_USTRING_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "unicode_char_traits.hpp"
#include "ustring_iterator.hpp"

#include <memory_resource>
#include <limits>

// testing area: http://localhost:10240/z/z39ErG

namespace webpp {


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

    template <typename CharT                    = glyph<>,
              istl::CharTraits   CharTraitsType = unicode_char_traits<CharT>,
              AllocatorOf<CharT> AllocType      = stl::allocator<CharT>>
    struct ustring {
      private:
        using alloc_traits = stl::allocator_traits<AllocType>;

      public:
        using value_type             = CharT;
        using allocator_type         = typename alloc_traits::template rebind_alloc<value_type>;
        using traits_type            = CharTraitsType;
        using reference              = value_type&;
        using const_reference        = value_type const&;
        using size_type              = typename stl::allocator_traits<allocator_type>::size_type;
        using difference_type        = typename stl::allocator_traits<allocator_type>::difference_type;
        using pointer                = typename stl::allocator_traits<allocator_type>::pointer;
        using const_pointer          = typename stl::allocator_traits<allocator_type>::const_pointer;
        using iterator               = unicode_iterator<value_type>;
        using const_iterator         = const iterator;
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
          !stl::is_convertible_v<const T&, const value_type*>
        ;




        // the reason I'm choosing data_end over size is that we're implementing a unicode string and not
        // ascii string; the difference is that "data_start + size != data_end";
        // todo: I need to think if we need a "size" field as well or not!
        pointer                              data_start;
        pointer                              data_end;
        [[no_unique_address]] allocator_type alloc;

        static constexpr auto local_capacity = 15 / sizeof(value_type);
        union {
            value_type local_buf[local_capacity + 1];
            size_type  allocated_capacity;
        };

        void set_length(size_type) noexcept {}

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
                                         NewStrT{this->data(), this->size(), this->get_allocator()};
                                     }) {
                    return NewStrT{this->data(), this->size(), this->get_allocator()};
                } else if constexpr (requires { NewStrT{this->data(), this->size()}; }) {
                    return NewStrT{this->data(), this->size()};
                } else if constexpr (has_compatible_char_types && has_same_allocator) {
                    // todo: fix this
                    NewStrT    output(this->get_allocator());
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







        void
        _M_data(pointer p)
        { data_start = p; }

        void
        _M_length(size_type length)
        { _M_string_length = length; }

        pointer
        _M_data() const
        { return data_start; }

        pointer
        _M_local_data()
        {
#if cplusplus >= 201103L
            return stl::pointer_traits<pointer>::pointer_to(*local_buf);
#else
            return pointer(local_buf);
#endif
        }

        const_pointer
        _M_local_data() const
        {
#if cplusplus >= 201103L
            return stl::pointer_traits<const_pointer>::pointer_to(*local_buf);
#else
            return const_pointer(local_buf);
#endif
        }

        void
        _M_capacity(size_type capacity)
        { allocated_capacity = capacity; }

        void
        _M_set_length(size_type n)
        {
            _M_length(n);
            traits_type::assign(_M_data()[n], value_type());
        }

        bool
        _M_is_local() const
        { return _M_data() == _M_local_data(); }

        // Create & Destroy
        pointer
        _M_create(size_type&, size_type);

        void
        _M_dispose()
        {
            if (!_M_is_local())
                _M_destroy(allocated_capacity);
        }

        void
        _M_destroy(size_type size) throw()
        { _Alloc_traits::deallocate(_M_get_allocator(), _M_data(), size + 1); }

        // _M_construct_aux is used to implement the 21.3.1 para 15 which
        // requires special behaviour if _InIterator is an integral type
        template<typename _InIterator>
        void
        _M_construct_aux(_InIterator beg, _InIterator end,
                         stl::false_type)
        {
            typedef typename stl::iterator_traits<_InIterator>::iterator_category _Tag;
            _M_construct(beg, end, _Tag());
        }

        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 438. Ambiguity in the "do the right thing" clause
        template<typename _Integer>
        void
        _M_construct_aux(_Integer beg, _Integer end, stl::true_type)
        { _M_construct_aux_2(static_cast<size_type>(beg), end); }

        void
        _M_construct_aux_2(size_type req, value_type c)
        { _M_construct(req, c); }

        template<typename _InIterator>
        void
        _M_construct(_InIterator beg, _InIterator end)
        {
            typedef typename stl::is_integer<_InIterator>::type _Integral;
            _M_construct_aux(beg, end, _Integral());
        }

        // For Input Iterators, used in istreambuf_iterators, etc.
        template<typename _InIterator>
        void
        _M_construct(_InIterator beg, _InIterator end,
                     stl::input_iterator_tag);

        // For forward_iterators up to random_access_iterators, used for
        // string::iterator, value_type*, etc.
        template<typename _FwdIterator>
        void
        _M_construct(_FwdIterator beg, _FwdIterator end,
                     stl::forward_iterator_tag);

        void
        _M_construct(size_type req, value_type c);

        allocator_type&
        _M_get_allocator()
        { return _M_dataplus; }

        const allocator_type&
        _M_get_allocator() const
        { return _M_dataplus; }

      private:

#ifdef _GLIBCXX_DISAMBIGUATE_REPLACE_INST
        // The explicit instantiations in misc-inst.cc require this due to
      // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64063
      template<typename _Tp, bool _Requires =
	       !are_same<_Tp, value_type*>::value
	       && !are_same<_Tp, const value_type*>::value
	       && !are_same<_Tp, iterator>::value
	       && !are_same<_Tp, const_iterator>::value>
	struct enable_if_not_native_iterator
	{ typedef ustring& type; };
      template<typename _Tp>
	struct enable_if_not_native_iterator<_Tp, false> { };
#endif

        size_type
        _M_check(size_type pos, const char* s) const
        {
            if (pos > this->size())
                throw_out_of_range_fmt(N("%s: pos (which is %zu) > "
                                             "this->size() (which is %zu)"),
                                         s, pos, this->size());
            return pos;
        }

        void
        _M_check_length(size_type n1, size_type n2, const char* s) const
        {
            if (this->max_size() - (this->size() - n1) < n2)
                throw_length_error(N(s));
        }


        // NB: _M_limit doesn't check for a bad pos value.
        size_type
        _M_limit(size_type pos, size_type off) const _GLIBCXX_NOEXCEPT
        {
            const bool testoff =  off < this->size() - pos;
            return testoff ? off : this->size() - pos;
        }

        // True if _Rep and source do not overlap.
        bool
        _M_disjunct(const value_type* s) const _GLIBCXX_NOEXCEPT
        {
            return (less<const value_type*>()(s, _M_data())
                    || less<const value_type*>()(_M_data() + this->size(), s));
        }

        // When n = 1 way faster than the general multichar
        // traits_type::copy/move/assign.
        static void
        _S_copy(value_type* d, const value_type* s, size_type n)
        {
            if (n == 1)
                traits_type::assign(*d, *s);
            else
                traits_type::copy(d, s, n);
        }

        static void
        _S_move(value_type* d, const value_type* s, size_type n)
        {
            if (n == 1)
                traits_type::assign(*d, *s);
            else
                traits_type::move(d, s, n);
        }

        static void
        _S_assign(value_type* d, size_type n, value_type c)
        {
            if (n == 1)
                traits_type::assign(*d, c);
            else
                traits_type::assign(d, n, c);
        }

        // _S_copy_chars is a separate template to permit specialization
        // to optimize for the common case of pointers as iterators.
        template<class _Iterator>
        static void
        _S_copy_chars(value_type* p, _Iterator k1, _Iterator k2)
        {
            for (; k1 != k2; ++k1, (void)++p)
                traits_type::assign(*p, *k1); // These types are off.
        }

        static void
        _S_copy_chars(value_type* p, iterator k1, iterator k2) _GLIBCXX_NOEXCEPT
        { _S_copy_chars(p, k1.base(), k2.base()); }

        static void
        _S_copy_chars(value_type* p, const_iterator k1, const_iterator k2)
        _GLIBCXX_NOEXCEPT
        { _S_copy_chars(p, k1.base(), k2.base()); }

        static void
        _S_copy_chars(value_type* p, value_type* k1, value_type* k2) _GLIBCXX_NOEXCEPT
        { _S_copy(p, k1, k2 - k1); }

        static void
        _S_copy_chars(value_type* p, const value_type* k1, const value_type* k2)
        _GLIBCXX_NOEXCEPT
        { _S_copy(p, k1, k2 - k1); }

        static int
        _S_compare(size_type n1, size_type n2) _GLIBCXX_NOEXCEPT
        {
            const difference_type d = difference_type(n1 - n2);

            if (d > stl::numeric_traits<int>::max)
                return stl::numeric_traits<int>::max;
            else if (d < stl::numeric_traits<int>::min)
                return stl::numeric_traits<int>::min;
            else
                return int(d);
        }

        void
        _M_assign(const ustring&);

        void
        _M_mutate(size_type pos, size_type len1, const value_type* s,
                  size_type len2);

        void
        _M_erase(size_type pos, size_type n);










        // Construct/copy/destroy:
        // NB: We overload ctors in some cases instead of using default
        // arguments, per 17.4.4.4 para. 2 item 2.

        /**
         *  @brief  Default constructor creates an empty string.
         */
        ustring()
        _GLIBCXX_NOEXCEPT_IF(is_nothrow_default_constructible<allocator_type>::value)
          : _M_dataplus(_M_local_data())
        { _M_set_length(0); }

        /**
         *  @brief  Construct an empty string using allocator @a a.
         */
        explicit
        ustring(const allocator_type& a) noexcept
          : _M_dataplus(_M_local_data(), a)
        { _M_set_length(0); }

        /**
         *  @brief  Construct string with copy of value of @a str.
         *  @param  str  Source string.
         */
        ustring(const ustring& str)
          : _M_dataplus(_M_local_data(),
                        _Alloc_traits::_S_select_on_copy(str._M_get_allocator()))
        { _M_construct(str._M_data(), str._M_data() + str.length()); }

        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 2583. no way to supply an allocator for ustring(str, pos)
        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  a  Allocator to use.
         */
        ustring(const ustring& str, size_type pos,
                     const allocator_type& a = allocator_type())
          : _M_dataplus(_M_local_data(), a)
        {
            const value_type* start = str._M_data()
                                    + str._M_check(pos, "ustring::ustring");
            _M_construct(start, start + str._M_limit(pos, npos));
        }

        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  n  Number of characters to copy.
         */
        ustring(const ustring& str, size_type pos,
                     size_type n)
          : _M_dataplus(_M_local_data())
        {
            const value_type* start = str._M_data()
                                    + str._M_check(pos, "ustring::ustring");
            _M_construct(start, start + str._M_limit(pos, n));
        }

        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  str  Source string.
         *  @param  pos  Index of first character to copy from.
         *  @param  n  Number of characters to copy.
         *  @param  a  Allocator to use.
         */
        ustring(const ustring& str, size_type pos,
                     size_type n, const allocator_type& a)
          : _M_dataplus(_M_local_data(), a)
        {
            const value_type* start
              = str._M_data() + str._M_check(pos, "string::string");
            _M_construct(start, start + str._M_limit(pos, n));
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
        ustring(const value_type* s, size_type n,
                     const allocator_type& a = allocator_type())
          : _M_dataplus(_M_local_data(), a)
        { _M_construct(s, s + n); }

        /**
         *  @brief  Construct string as copy of a C string.
         *  @param  s  Source C string.
         *  @param  a  Allocator to use (default is default allocator).
         */
#if cpp_deduction_guides && ! defined _GLIBCXX_DEFINING_STRING_INSTANTIATIONS
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 3076. ustring CTAD ambiguity
        template<typename = _RequireAllocator<allocator_type>>
#endif
        ustring(const value_type* s, const allocator_type& a = allocator_type())
          : _M_dataplus(_M_local_data(), a)
        { _M_construct(s, s ? s + traits_type::length(s) : s+npos); }

        /**
         *  @brief  Construct string as multiple characters.
         *  @param  n  Number of characters.
         *  @param  c  Character to use.
         *  @param  a  Allocator to use (default is default allocator).
         */
#if cpp_deduction_guides && ! defined _GLIBCXX_DEFINING_STRING_INSTANTIATIONS
        // _GLIBCXX_RESOLVE_LIB_DEFECTS
        // 3076. ustring CTAD ambiguity
        template<typename = _RequireAllocator<allocator_type>>
#endif
        ustring(size_type n, value_type c, const allocator_type& a = allocator_type())
        : _M_dataplus(_M_local_data(), a)
        { _M_construct(n, c); }

        /**
         *  @brief  Move construct string.
         *  @param  str  Source string.
         *
         *  The newly-created string contains the exact contents of @a str.
         *  @a str is a valid, but unspecified string.
         **/
        ustring(ustring&& str) noexcept
        : _M_dataplus(_M_local_data(), stl::move(str._M_get_allocator()))
        {
            if (str._M_is_local())
            {
                traits_type::copy(local_buf, str.local_buf,
                                  local_capacity + 1);
            }
            else
            {
                _M_data(str._M_data());
                _M_capacity(str.allocated_capacity);
            }

            // Must use _M_length() here not _M_set_length() because
            // ustringbuf relies on writing into unallocated capacity so
            // we mess up the contents if we put a '\0' in the string.
            _M_length(str.length());
            str._M_data(str._M_local_data());
            str._M_set_length(0);
        }

        /**
         *  @brief  Construct string from an initializer %list.
         *  @param  l  stl::initializer_list of characters.
         *  @param  a  Allocator to use (default is default allocator).
         */
        ustring(stl::initializer_list<value_type> l, const allocator_type& a = allocator_type())
        : _M_dataplus(_M_local_data(), a)
        { _M_construct(l.begin(), l.end()); }

        ustring(const ustring& str, const allocator_type& a)
          : _M_dataplus(_M_local_data(), a)
        { _M_construct(str.begin(), str.end()); }

        ustring(ustring&& str, const allocator_type& a)
        noexcept(_Alloc_traits::_S_always_equal())
        : _M_dataplus(_M_local_data(), a)
        {
            if (str._M_is_local())
            {
                traits_type::copy(local_buf, str.local_buf,
                                  local_capacity + 1);
                _M_length(str.length());
                str._M_set_length(0);
            }
            else if (_Alloc_traits::_S_always_equal()
                     || str.get_allocator() == a)
            {
                _M_data(str._M_data());
                _M_length(str.length());
                _M_capacity(str.allocated_capacity);
                str._M_data(str.local_buf);
                str._M_set_length(0);
            }
            else
                _M_construct(str.begin(), str.end());
        }


        /**
         *  @brief  Construct string as copy of a range.
         *  @param  beg  Start of range.
         *  @param  end  End of range.
         *  @param  a  Allocator to use (default is default allocator).
         */
        template<typename _InputIterator,
          typename = stl::_RequireInputIter<_InputIterator>>
        ustring(_InputIterator beg, _InputIterator end,
        const allocator_type& a = allocator_type())
        : _M_dataplus(_M_local_data(), a)
        { _M_construct(beg, end); }

        /**
         *  @brief  Construct string from a substring of a string_view.
         *  @param  t   Source object convertible to string view.
         *  @param  pos The index of the first character to copy from t.
         *  @param  n   The number of characters to copy from t.
         *  @param  a   Allocator to use.
         */
        template<typename _Tp, typename = requires (convertible_to_ustring_view<_Tp>) void>
        ustring(const _Tp& t, size_type pos, size_type n,
                     const allocator_type& a = allocator_type())
          : ustring(_S_to_string_view(t).substr(pos, n), a) { }

        /**
         *  @brief  Construct string from a string_view.
         *  @param  t  Source object convertible to string view.
         *  @param  a  Allocator to use (default is default allocator).
         */
        template<typename _Tp, typename = requires (convertible_to_ustring_view<_Tp>) void>
        explicit
        ustring(const _Tp& t, const allocator_type& a = allocator_type())
          : ustring(sv_wrapper(_S_to_string_view(t)), a) { }

        /**
         *  @brief  Destroy the string instance.
         */
        ~ustring()
        { _M_dispose(); }

        /**
         *  @brief  Assign the value of @a str to this string.
         *  @param  str  Source string.
         */
        ustring&
        operator=(const ustring& str)
        {
            return this->assign(str);
        }

        /**
         *  @brief  Copy contents of @a s into this string.
         *  @param  s  Source null-terminated string.
         */
        ustring&
        operator=(const value_type* s)
        { return this->assign(s); }

        /**
         *  @brief  Set value to string of length 1.
         *  @param  c  Source character.
         *
         *  Assigning to a character makes this string length 1 and
         *  (*this)[0] == @a c.
         */
        ustring&
        operator=(value_type c)
        {
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
        ustring&
        operator=(ustring&& str)
        noexcept(_Alloc_traits::_S_nothrow_move())
        {
            if (!_M_is_local() && _Alloc_traits::_S_propagate_on_move_assign()
                && !_Alloc_traits::_S_always_equal()
                && _M_get_allocator() != str._M_get_allocator())
            {
                // Destroy existing storage before replacing allocator.
                _M_destroy(allocated_capacity);
                _M_data(_M_local_data());
                _M_set_length(0);
            }
            // Replace allocator if POCMA is true.
            stl::alloc_on_move(_M_get_allocator(), str._M_get_allocator());

            if (str._M_is_local())
            {
                // We've always got room for a short string, just copy it.
                if (str.size())
                    this->_S_copy(_M_data(), str._M_data(), str.size());
                _M_set_length(str.size());
            }
            else if (_Alloc_traits::_S_propagate_on_move_assign()
                     || _Alloc_traits::_S_always_equal()
                     || _M_get_allocator() == str._M_get_allocator())
            {
                // Just move the allocated pointer, our allocator can free it.
                pointer data = nullptr;
                size_type capacity;
                if (!_M_is_local())
                {
                    if (_Alloc_traits::_S_always_equal())
                    {
                        // str can reuse our existing storage.
                        data = _M_data();
                        capacity = allocated_capacity;
                    }
                    else // str can't use it, so free it.
                        _M_destroy(allocated_capacity);
                }

                _M_data(str._M_data());
                _M_length(str.length());
                _M_capacity(str.allocated_capacity);
                if (data)
                {
                    str._M_data(data);
                    str._M_capacity(capacity);
                }
                else
                    str._M_data(str.local_buf);
            }
            else // Need to do a deep copy
                assign(str);
            str.clear();
            return *this;
        }

        /**
         *  @brief  Set value to string constructed from initializer %list.
         *  @param  l  stl::initializer_list.
         */
        ustring&
        operator=(stl::initializer_list<value_type> l)
        {
            this->assign(l.begin(), l.size());
            return *this;
        }

        /**
         *  @brief  Set value to string constructed from a string_view.
         *  @param  svt  An object convertible to string_view.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        operator=(const _Tp& svt)
        { return this->assign(svt); }

        /**
         *  @brief  Convert to a string_view.
         *  @return A string_view.
         */
        operator string_view_type() const noexcept
        { return string_view_type(data(), size()); }

        // Iterators:
        /**
         *  Returns a read/write iterator that points to the first character in
         *  the %string.
         */
        iterator
        begin() noexcept
        { return iterator(_M_data()); }

        /**
         *  Returns a read-only (constant) iterator that points to the first
         *  character in the %string.
         */
        const_iterator
        begin() const noexcept
        { return const_iterator(_M_data()); }

        /**
         *  Returns a read/write iterator that points one past the last
         *  character in the %string.
         */
        iterator
        end() noexcept
        { return iterator(_M_data() + this->size()); }

        /**
         *  Returns a read-only (constant) iterator that points one past the
         *  last character in the %string.
         */
        const_iterator
        end() const noexcept
        { return const_iterator(_M_data() + this->size()); }

        /**
         *  Returns a read/write reverse iterator that points to the last
         *  character in the %string.  Iteration is done in reverse element
         *  order.
         */
        reverse_iterator
        rbegin() noexcept
        { return reverse_iterator(this->end()); }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to the last character in the %string.  Iteration is done in
         *  reverse element order.
         */
        const_reverse_iterator
        rbegin() const noexcept
        { return const_reverse_iterator(this->end()); }

        /**
         *  Returns a read/write reverse iterator that points to one before the
         *  first character in the %string.  Iteration is done in reverse
         *  element order.
         */
        reverse_iterator
        rend() noexcept
        { return reverse_iterator(this->begin()); }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to one before the first character in the %string.  Iteration
         *  is done in reverse element order.
         */
        const_reverse_iterator
        rend() const noexcept
        { return const_reverse_iterator(this->begin()); }

        /**
         *  Returns a read-only (constant) iterator that points to the first
         *  character in the %string.
         */
        const_iterator
        cbegin() const noexcept
        { return const_iterator(this->_M_data()); }

        /**
         *  Returns a read-only (constant) iterator that points one past the
         *  last character in the %string.
         */
        const_iterator
        cend() const noexcept
        { return const_iterator(this->_M_data() + this->size()); }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to the last character in the %string.  Iteration is done in
         *  reverse element order.
         */
        const_reverse_iterator
        crbegin() const noexcept
        { return const_reverse_iterator(this->end()); }

        /**
         *  Returns a read-only (constant) reverse iterator that points
         *  to one before the first character in the %string.  Iteration
         *  is done in reverse element order.
         */
        const_reverse_iterator
        crend() const noexcept
        { return const_reverse_iterator(this->begin()); }

      public:
        // Capacity:
        ///  Returns the number of characters in the string, not including any
        ///  null-termination.
        size_type
        size() const noexcept
        { return _M_string_length; }

        ///  Returns the number of characters in the string, not including any
        ///  null-termination.
        size_type
        length() const noexcept
        { return _M_string_length; }

        ///  Returns the size() of the largest possible %string.
        size_type
        max_size() const noexcept
        { return (_Alloc_traits::max_size(_M_get_allocator()) - 1) / 2; }

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
        void
        resize(size_type n, value_type c);

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
        void
        resize(size_type n)
        { this->resize(n, value_type()); }

        ///  A non-binding request to reduce capacity() to size().
        void
        shrink_to_fit() noexcept
        {
#if cpp_exceptions
            if (capacity() > size())
            {
                try
                { reserve(0); }
                catch(...)
                { }
            }
#endif
        }

        /**
         *  Returns the total number of characters that the %string can hold
         *  before needing to allocate more memory.
         */
        size_type
        capacity() const noexcept
        {
            return _M_is_local() ? size_type(local_capacity)
                                 : allocated_capacity;
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
        void
        reserve(size_type res_arg = 0);

        /**
         *  Erases the string, making it empty.
         */
        void
        clear() noexcept
        { _M_set_length(0); }

        /**
         *  Returns true if the %string is empty.  Equivalent to 
         *  <code>*this == ""</code>.
         */
        _GLIBCXX_NODISCARD bool
        empty() const noexcept
        { return this->size() == 0; }

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
        const_reference
        operator[] (size_type pos) const noexcept
        {
            glibcxx_assert(pos <= size());
            return _M_data()[pos];
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
        reference
        operator[](size_type pos)
        {
            // Allow pos == size() both in C++98 mode, as v3 extension,
            // and in C++11 mode.
            glibcxx_assert(pos <= size());
            // In pedantic mode be strict in C++98 mode.
            _GLIBCXX_DEBUG_PEDASSERT(cplusplus >= 201103L || pos < size());
            return _M_data()[pos];
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
        const_reference
        at(size_type n) const
        {
            if (n >= this->size())
                throw_out_of_range_fmt(N("ustring::at: n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         n, this->size());
            return _M_data()[n];
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
        reference
        at(size_type n)
        {
            if (n >= size())
                throw_out_of_range_fmt(N("ustring::at: n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         n, this->size());
            return _M_data()[n];
        }

        /**
         *  Returns a read/write reference to the data at the first
         *  element of the %string.
         */
        reference
        front() noexcept
        {
            glibcxx_assert(!empty());
            return operator[](0);
        }

        /**
         *  Returns a read-only (constant) reference to the data at the first
         *  element of the %string.
         */
        const_reference
        front() const noexcept
        {
            glibcxx_assert(!empty());
            return operator[](0);
        }

        /**
         *  Returns a read/write reference to the data at the last
         *  element of the %string.
         */
        reference
        back() noexcept
        {
            glibcxx_assert(!empty());
            return operator[](this->size() - 1);
        }

        /**
         *  Returns a read-only (constant) reference to the data at the
         *  last element of the %string.
         */
        const_reference
        back() const noexcept
        {
            glibcxx_assert(!empty());
            return operator[](this->size() - 1);
        }

        // Modifiers:
        /**
         *  @brief  Append a string to this string.
         *  @param str  The string to append.
         *  @return  Reference to this string.
         */
        ustring&
        operator+=(const ustring& str)
        { return this->append(str); }

        /**
         *  @brief  Append a C string.
         *  @param s  The C string to append.
         *  @return  Reference to this string.
         */
        ustring&
        operator+=(const value_type* s)
        { return this->append(s); }

        /**
         *  @brief  Append a character.
         *  @param c  The character to append.
         *  @return  Reference to this string.
         */
        ustring&
        operator+=(value_type c)
        {
            this->push_back(c);
            return *this;
        }

        /**
         *  @brief  Append an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to be appended.
         *  @return  Reference to this string.
         */
        ustring&
        operator+=(stl::initializer_list<value_type> l)
        { return this->append(l.begin(), l.size()); }

        /**
         *  @brief  Append a string_view.
         *  @param svt  An object convertible to string_view to be appended.
         *  @return  Reference to this string.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        operator+=(const _Tp& svt)
        { return this->append(svt); }

        /**
         *  @brief  Append a string to this string.
         *  @param str  The string to append.
         *  @return  Reference to this string.
         */
        ustring&
        append(const ustring& str)
        { return _M_append(str._M_data(), str.size()); }

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
        ustring&
        append(const ustring& str, size_type pos, size_type n = npos)
        { return _M_append(str._M_data()
                           + str._M_check(pos, "ustring::append"),
                           str._M_limit(pos, n)); }

        /**
         *  @brief  Append a C substring.
         *  @param s  The C string to append.
         *  @param n  The number of characters to append.
         *  @return  Reference to this string.
         */
        ustring&
        append(const value_type* s, size_type n)
        {
            glibcxx_requires_string_len(s, n);
            _M_check_length(size_type(0), n, "ustring::append");
            return _M_append(s, n);
        }

        /**
         *  @brief  Append a C string.
         *  @param s  The C string to append.
         *  @return  Reference to this string.
         */
        ustring&
        append(const value_type* s)
        {
            glibcxx_requires_string(s);
            const size_type n = traits_type::length(s);
            _M_check_length(size_type(0), n, "ustring::append");
            return _M_append(s, n);
        }

        /**
         *  @brief  Append multiple characters.
         *  @param n  The number of characters to append.
         *  @param c  The character to use.
         *  @return  Reference to this string.
         *
         *  Appends n copies of c to this string.
         */
        ustring&
        append(size_type n, value_type c)
        { return _M_replace_aux(this->size(), size_type(0), n, c); }

        /**
         *  @brief  Append an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to append.
         *  @return  Reference to this string.
         */
        ustring&
        append(stl::initializer_list<value_type> l)
        { return this->append(l.begin(), l.size()); }

        /**
         *  @brief  Append a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Appends characters in the range [first,last) to this string.
         */
        template<class _InputIterator,
          typename = stl::_RequireInputIter<_InputIterator>>
        ustring&
        append(_InputIterator first, _InputIterator last)
        { return this->replace(end(), end(), first, last); }

        /**
         *  @brief  Append a string_view.
         *  @param svt  An object convertible to string_view to be appended.
         *  @return  Reference to this string.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        append(const _Tp& svt)
        {
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        append(const _Tp& svt, size_type pos, size_type n = npos)
        {
            string_view_type sv = svt;
            return _M_append(sv.data()
                             + stl::sv_check(sv.size(), pos, "ustring::append"),
                             stl::sv_limit(sv.size(), pos, n));
        }

        /**
         *  @brief  Append a single character.
         *  @param c  Character to append.
         */
        void
        push_back(value_type c)
        {
            const size_type size = this->size();
            if (size + 1 > this->capacity())
                this->_M_mutate(size, size_type(0), 0, size_type(1));
            traits_type::assign(this->_M_data()[size], c);
            this->_M_set_length(size + 1);
        }

        /**
         *  @brief  Set value to contents of another string.
         *  @param  str  Source string to use.
         *  @return  Reference to this string.
         */
        ustring&
        assign(const ustring& str)
        {
            if (_Alloc_traits::_S_propagate_on_copy_assign())
            {
                if (!_Alloc_traits::_S_always_equal() && !_M_is_local()
                    && _M_get_allocator() != str._M_get_allocator())
                {
                    // Propagating allocator cannot free existing storage so must
                    // deallocate it before replacing current allocator.
                    if (str.size() <= local_capacity)
                    {
                        _M_destroy(allocated_capacity);
                        _M_data(_M_local_data());
                        _M_set_length(0);
                    }
                    else
                    {
                        const auto len = str.size();
                        auto alloc = str._M_get_allocator();
                        // If this allocation throws there are no effects:
                        auto ptr = _Alloc_traits::allocate(alloc, len + 1);
                        _M_destroy(allocated_capacity);
                        _M_data(ptr);
                        _M_capacity(len);
                        _M_set_length(len);
                    }
                }
                stl::alloc_on_copy(_M_get_allocator(), str._M_get_allocator());
            }
            this->_M_assign(str);
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
        ustring&
        assign(ustring&& str)
        noexcept(_Alloc_traits::_S_nothrow_move())
        {
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
        ustring&
        assign(const ustring& str, size_type pos, size_type n = npos)
        { return _M_replace(size_type(0), this->size(), str._M_data()
                                                        + str._M_check(pos, "ustring::assign"),
                            str._M_limit(pos, n)); }

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
        ustring&
        assign(const value_type* s, size_type n)
        {
            glibcxx_requires_string_len(s, n);
            return _M_replace(size_type(0), this->size(), s, n);
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
        ustring&
        assign(const value_type* s)
        {
            glibcxx_requires_string(s);
            return _M_replace(size_type(0), this->size(), s,
                              traits_type::length(s));
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
        ustring&
        assign(size_type n, value_type c)
        { return _M_replace_aux(size_type(0), this->size(), n, c); }

        /**
         *  @brief  Set value to a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Sets value of string to characters in the range [first,last).
        */
        template<class _InputIterator,
          typename = stl::_RequireInputIter<_InputIterator>>
        ustring&
        assign(_InputIterator first, _InputIterator last)
        { return this->replace(begin(), end(), first, last); }

        /**
         *  @brief  Set value to an stl::initializer_list of characters.
         *  @param l  The stl::initializer_list of characters to assign.
         *  @return  Reference to this string.
         */
        ustring&
        assign(stl::initializer_list<value_type> l)
        { return this->assign(l.begin(), l.size()); }

        /**
         *  @brief  Set value from a string_view.
         *  @param svt  The source object convertible to string_view.
         *  @return  Reference to this string.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        assign(const _Tp& svt)
        {
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        assign(const _Tp& svt, size_type pos, size_type n = npos)
        {
            string_view_type sv = svt;
            return _M_replace(size_type(0), this->size(),
                              sv.data()
                              + stl::sv_check(sv.size(), pos, "ustring::assign"),
                              stl::sv_limit(sv.size(), pos, n));
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
        iterator
        insert(const_iterator p, size_type n, value_type c)
        {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            this->replace(p, p, n, c);
            return iterator(this->_M_data() + pos);
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
        template<class _InputIterator,
          typename = stl::_RequireInputIter<_InputIterator>>
        iterator
        insert(const_iterator p, _InputIterator beg, _InputIterator end)
        {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            this->replace(p, p, beg, end);
            return iterator(this->_M_data() + pos);
        }
        /**
         *  @brief  Insert an stl::initializer_list of characters.
         *  @param p  Iterator referencing location in string to insert at.
         *  @param l  The stl::initializer_list of characters to insert.
         *  @throw  stl::length_error  If new length exceeds @c max_size().
         */
        iterator
        insert(const_iterator p, stl::initializer_list<value_type> l)
        { return this->insert(p, l.begin(), l.end()); }

        // See PR libstdc++/83328
      void
      insert(iterator p, stl::initializer_list<value_type> l)
      {
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
        ustring&
        insert(size_type pos1, const ustring& str)
        { return this->replace(pos1, size_type(0),
                               str._M_data(), str.size()); }

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
        ustring&
        insert(size_type pos1, const ustring& str,
               size_type pos2, size_type n = npos)
        { return this->replace(pos1, size_type(0), str._M_data()
                                                     + str._M_check(pos2, "ustring::insert"),
                               str._M_limit(pos2, n)); }

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
        ustring&
        insert(size_type pos, const value_type* s, size_type n)
        { return this->replace(pos, size_type(0), s, n); }

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
        ustring&
        insert(size_type pos, const value_type* s)
        {
            glibcxx_requires_string(s);
            return this->replace(pos, size_type(0), s,
                                 traits_type::length(s));
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
        ustring&
        insert(size_type pos, size_type n, value_type c)
        { return _M_replace_aux(_M_check(pos, "ustring::insert"),
                                size_type(0), n, c); }

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
        iterator
        insert(const_iterator p, value_type c)
        {
            _GLIBCXX_DEBUG_PEDASSERT(p >= begin() && p <= end());
            const size_type pos = p - begin();
            _M_replace_aux(pos, size_type(0), size_type(1), c);
            return iterator(_M_data() + pos);
        }

        /**
         *  @brief  Insert a string_view.
         *  @param pos  Position in string to insert at.
         *  @param svt  The object convertible to string_view to insert.
         *  @return  Reference to this string.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        insert(size_type pos, const _Tp& svt)
        {
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        insert(size_type pos1, const _Tp& svt,
               size_type pos2, size_type n = npos)
        {
            string_view_type sv = svt;
            return this->replace(pos1, size_type(0),
                                 sv.data()
                                 + stl::sv_check(sv.size(), pos2, "ustring::insert"),
                                 stl::sv_limit(sv.size(), pos2, n));
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
        ustring&
        erase(size_type pos = 0, size_type n = npos)
        {
            _M_check(pos, "ustring::erase");
            if (n == npos)
                this->_M_set_length(pos);
            else if (n != 0)
                this->_M_erase(pos, _M_limit(pos, n));
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
        iterator
        erase(const_iterator position)
        {
            _GLIBCXX_DEBUG_PEDASSERT(position >= begin()
                                     && position < end());
            const size_type pos = position - begin();
            this->_M_erase(pos, size_type(1));
            return iterator(_M_data() + pos);
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
        iterator
        erase(const_iterator first, const_iterator last)
        {
            _GLIBCXX_DEBUG_PEDASSERT(first >= begin() && first <= last
                                     && last <= end());
            const size_type pos = first - begin();
            if (last == end())
                this->_M_set_length(pos);
            else
                this->_M_erase(pos, last - first);
            return iterator(this->_M_data() + pos);
        }

        /**
         *  @brief  Remove the last character.
         *
         *  The string must be non-empty.
         */
        void
        pop_back() noexcept
        {
            glibcxx_assert(!empty());
            _M_erase(size() - 1, 1);
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
        ustring&
        replace(size_type pos, size_type n, const ustring& str)
        { return this->replace(pos, n, str._M_data(), str.size()); }

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
        replace(size_type pos1, size_type n1, const ustring& str,
                size_type pos2, size_type n2 = npos)
        { return this->replace(pos1, n1, str._M_data()
                                             + str._M_check(pos2, "ustring::replace"),
                               str._M_limit(pos2, n2)); }

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
        ustring&
        replace(size_type pos, size_type n1, const value_type* s,
                size_type n2)
        {
            glibcxx_requires_string_len(s, n2);
            return _M_replace(_M_check(pos, "ustring::replace"),
                              _M_limit(pos, n1), s, n2);
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
        ustring&
        replace(size_type pos, size_type n1, const value_type* s)
        {
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
        ustring&
        replace(size_type pos, size_type n1, size_type n2, value_type c)
        { return _M_replace_aux(_M_check(pos, "ustring::replace"),
                                _M_limit(pos, n1), n2, c); }

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
        ustring&
        replace(const_iterator i1, const_iterator i2,
                const ustring& str)
        { return this->replace(i1, i2, str._M_data(), str.size()); }

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
        ustring&
        replace(const_iterator i1, const_iterator i2,
                const value_type* s, size_type n)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
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
        ustring&
        replace(const_iterator i1, const_iterator i2, const value_type* s)
        {
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
        ustring&
        replace(const_iterator i1, const_iterator i2, size_type n,
                value_type c)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            return _M_replace_aux(i1 - begin(), i2 - i1, n, c);
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
        template<class _InputIterator,
          typename = stl::_RequireInputIter<_InputIterator>>
        ustring&
        replace(const_iterator i1, const_iterator i2,
                _InputIterator k1, _InputIterator k2)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->_M_replace_dispatch(i1, i2, k1, k2,
                                             stl::false_type());
        }
#ifdef _GLIBCXX_DISAMBIGUATE_REPLACE_INST
        typename enable_if_not_native_iterator<_InputIterator>::type
#else
        ustring&
#endif
        replace(iterator i1, iterator i2,
		_InputIterator k1, _InputIterator k2)
        {
	  _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
				   && i2 <= end());
	  glibcxx_requires_valid_range(k1, k2);
	  typedef typename stl::is_integer<_InputIterator>::type _Integral;
	  return _M_replace_dispatch(i1, i2, k1, k2, _Integral());
	}

        // Specializations for the common case of pointer and iterator:
        // useful to avoid the overhead of temporary buffering in _M_replace.
        ustring&
        replace(const_iterator i1, const_iterator i2,
                value_type* k1, value_type* k2)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1,
                                 k1, k2 - k1);
        }

        ustring&
        replace(const_iterator i1, const_iterator i2,
                const value_type* k1, const value_type* k2)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1,
                                 k1, k2 - k1);
        }

        ustring&
        replace(const_iterator i1, const_iterator i2,
                iterator k1, iterator k2)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1,
                                 k1.base(), k2 - k1);
        }

        ustring&
        replace(const_iterator i1, const_iterator i2,
                const_iterator k1, const_iterator k2)
        {
            _GLIBCXX_DEBUG_PEDASSERT(begin() <= i1 && i1 <= i2
                                     && i2 <= end());
            glibcxx_requires_valid_range(k1, k2);
            return this->replace(i1 - begin(), i2 - i1,
                                 k1.base(), k2 - k1);
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
        ustring& replace(const_iterator i1, const_iterator i2,
                              stl::initializer_list<value_type> l)
        { return this->replace(i1, i2, l.begin(), l.size()); }

        /**
         *  @brief  Replace range of characters with string_view.
         *  @param pos  The position to replace at.
         *  @param n    The number of characters to replace.
         *  @param svt  The object convertible to string_view to insert.
         *  @return  Reference to this string.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        replace(size_type pos, size_type n, const _Tp& svt)
        {
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        replace(size_type pos1, size_type n1, const _Tp& svt,
                size_type pos2, size_type n2 = npos)
        {
            string_view_type sv = svt;
            return this->replace(pos1, n1,
                                 sv.data()
                                 + stl::sv_check(sv.size(), pos2, "ustring::replace"),
                                 stl::sv_limit(sv.size(), pos2, n2));
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) ustring&
        replace(const_iterator i1, const_iterator i2, const _Tp& svt)
        {
            string_view_type sv = svt;
            return this->replace(i1 - begin(), i2 - i1, sv);
        }

      private:
        template<class _Integer>
        ustring&
        _M_replace_dispatch(const_iterator i1, const_iterator i2,
                            _Integer n, _Integer val, stl::true_type)
        { return _M_replace_aux(i1 - begin(), i2 - i1, n, val); }

        template<class _InputIterator>
        ustring&
        _M_replace_dispatch(const_iterator i1, const_iterator i2,
                            _InputIterator k1, _InputIterator k2,
                            stl::false_type);

        ustring&
        _M_replace_aux(size_type pos1, size_type n1, size_type n2,
                       value_type c);

        ustring&
        _M_replace(size_type pos, size_type len1, const value_type* s,
                   const size_type len2);

        ustring&
        _M_append(const value_type* s, size_type n);

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
        size_type
        copy(value_type* s, size_type n, size_type pos = 0) const;

        /**
         *  @brief  Swap contents with another string.
         *  @param s  String to swap with.
         *
         *  Exchanges the contents of this string with that of @a s in constant
         *  time.
        */
        void
        swap(ustring& s) noexcept;

        // String operations:
        /**
         *  @brief  Return const pointer to null-terminated contents.
         *
         *  This is a handle to internal data.  Do not modify or dire things may
         *  happen.
        */
        const value_type*
        c_str() const noexcept
        { return _M_data(); }

        /**
         *  @brief  Return const pointer to contents.
         *
         *  This is a pointer to internal data.  It is undefined to modify
         *  the contents through the returned pointer. To get a pointer that
         *  allows modifying the contents use @c &str[0] instead,
         *  (or in C++17 the non-const @c str.data() overload).
        */
        const value_type*
        data() const noexcept
        { return _M_data(); }

        /**
         *  @brief  Return non-const pointer to contents.
         *
         *  This is a pointer to the character sequence held by the string.
         *  Modifying the characters in the sequence is allowed.
        */
        value_type*
        data() noexcept
        { return _M_data(); }

        /**
         *  @brief  Return copy of allocator used to construct this string.
        */
        allocator_type
        get_allocator() const noexcept
        { return _M_get_allocator(); }

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
        size_type
        find(const value_type* s, size_type pos, size_type n) const
        noexcept;

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
        size_type
        find(const ustring& str, size_type pos = 0) const
        noexcept
        { return this->find(str.data(), pos, str.size()); }

        /**
         *  @brief  Find position of a string_view.
         *  @param svt  The object convertible to string_view to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of start of first occurrence.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        find(const _Tp& svt, size_type pos = 0) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        find(const value_type* s, size_type pos = 0) const noexcept
        {
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
        size_type
        find(value_type c, size_type pos = 0) const noexcept;

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
        size_type
        rfind(const ustring& str, size_type pos = npos) const
        noexcept
        { return this->rfind(str.data(), pos, str.size()); }

        /**
         *  @brief  Find last position of a string_view.
         *  @param svt  The object convertible to string_view to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of start of last occurrence.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        rfind(const _Tp& svt, size_type pos = npos) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        rfind(const value_type* s, size_type pos, size_type n) const
        noexcept;

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
        size_type
        rfind(const value_type* s, size_type pos = npos) const
        {
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
        size_type
        rfind(value_type c, size_type pos = npos) const noexcept;

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
        size_type
        find_first_of(const ustring& str, size_type pos = 0) const
        noexcept
        { return this->find_first_of(str.data(), pos, str.size()); }

        /**
         *  @brief  Find position of a character of a string_view.
         *  @param svt  An object convertible to string_view containing
         *                characters to locate.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        find_first_of(const _Tp& svt, size_type pos = 0) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        find_first_of(const value_type* s, size_type pos, size_type n) const
        noexcept;

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
        size_type
        find_first_of(const value_type* s, size_type pos = 0) const
        noexcept
        {
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
        size_type
        find_first_of(value_type c, size_type pos = 0) const noexcept
        { return this->find(c, pos); }

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
        size_type
        find_last_of(const ustring& str, size_type pos = npos) const
        noexcept
        { return this->find_last_of(str.data(), pos, str.size()); }

        /**
         *  @brief  Find last position of a character of string.
         *  @param svt  An object convertible to string_view containing
         *                characters to locate.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
        */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        find_last_of(const _Tp& svt, size_type pos = npos) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        find_last_of(const value_type* s, size_type pos, size_type n) const
        noexcept;

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
        size_type
        find_last_of(const value_type* s, size_type pos = npos) const
        noexcept
        {
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
        size_type
        find_last_of(value_type c, size_type pos = npos) const noexcept
        { return this->rfind(c, pos); }

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
        size_type
        find_first_not_of(const ustring& str, size_type pos = 0) const
        noexcept
        { return this->find_first_not_of(str.data(), pos, str.size()); }

        /**
         *  @brief  Find position of a character not in a string_view.
         *  @param svt  A object convertible to string_view containing
         *                characters to avoid.
         *  @param pos  Index of character to search from (default 0).
         *  @return  Index of first occurrence.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        find_first_not_of(const _Tp& svt, size_type pos = 0) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        find_first_not_of(const value_type* s, size_type pos,
                          size_type n) const noexcept;

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
        size_type
        find_first_not_of(const value_type* s, size_type pos = 0) const
        noexcept
        {
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
        size_type
        find_first_not_of(value_type c, size_type pos = 0) const
        noexcept;

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
        size_type
        find_last_not_of(const ustring& str, size_type pos = npos) const
        noexcept
        { return this->find_last_not_of(str.data(), pos, str.size()); }

        /**
         *  @brief  Find last position of a character not in a string_view.
         *  @param svt  An object convertible to string_view containing
         *                characters to avoid.
         *  @param pos  Index of character to search back from (default end).
         *  @return  Index of last occurrence.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) size_type
        find_last_not_of(const _Tp& svt, size_type pos = npos) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        size_type
        find_last_not_of(const value_type* s, size_type pos,
                         size_type n) const noexcept;
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
        size_type
        find_last_not_of(const value_type* s, size_type pos = npos) const
        noexcept
        {
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
        size_type
        find_last_not_of(value_type c, size_type pos = npos) const
        noexcept;

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
        ustring
        substr(size_type pos = 0, size_type n = npos) const
        { return ustring(*this,
                              _M_check(pos, "ustring::substr"), n); }

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
        int
        compare(const ustring& str) const
        {
            const size_type size = this->size();
            const size_type osize = str.size();
            const size_type len = stl::min(size, osize);

            int r = traits_type::compare(_M_data(), str.data(), len);
            if (!r)
                r = _S_compare(size, osize);
            return r;
        }

        /**
         *  @brief  Compare to a string_view.
         *  @param svt An object convertible to string_view to compare against.
         *  @return  Integer < 0, 0, or > 0.
         */
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) int
        compare(const _Tp& svt) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
            string_view_type sv = svt;
            const size_type size = this->size();
            const size_type osize = sv.size();
            const size_type len = stl::min(size, osize);

            int r = traits_type::compare(_M_data(), sv.data(), len);
            if (!r)
                r = _S_compare(size, osize);
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) int
        compare(size_type pos, size_type n, const _Tp& svt) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
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
        template<typename _Tp>
        requires (convertible_to_ustring_view<_Tp>) int
        compare(size_type pos1, size_type n1, const _Tp& svt,
                size_type pos2, size_type n2 = npos) const
        noexcept(is_same<_Tp, string_view_type>::value)
        {
            string_view_type sv = svt;
            return string_view_type(*this)
              .substr(pos1, n1).compare(sv.substr(pos2, n2));
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
        int
        compare(size_type pos, size_type n, const ustring& str) const;

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
        int
        compare(size_type pos1, size_type n1, const ustring& str,
                size_type pos2, size_type n2 = npos) const;

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
        int
        compare(const value_type* s) const noexcept;

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
        int
        compare(size_type pos, size_type n1, const value_type* s) const;

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
        int
        compare(size_type pos, size_type n1, const value_type* s,
                size_type n2) const;

        bool
        starts_with(ustring_view<value_type, traits_type> x) const noexcept
        { return string_view_type(this->data(), this->size()).starts_with(x); }

        bool
        starts_with(value_type x) const noexcept
        { return string_view_type(this->data(), this->size()).starts_with(x); }

        bool
        starts_with(const value_type* x) const noexcept
        { return string_view_type(this->data(), this->size()).starts_with(x); }

        bool
        ends_with(ustring_view<value_type, traits_type> x) const noexcept
        { return string_view_type(this->data(), this->size()).ends_with(x); }

        bool
        ends_with(value_type x) const noexcept
        { return string_view_type(this->data(), this->size()).ends_with(x); }

        bool
        ends_with(const value_type* x) const noexcept
        { return string_view_type(this->data(), this->size()).ends_with(x); }

        // Allow ustringbuf::xfer_bufptrs to call _M_length:
        template<typename, typename, typename> friend class ustringbuf;
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    };


    using utf8  = ustring<utf8_glyph>;
    using utf16 = ustring<utf16_glyph>;
    using utf32 = ustring<utf32_glyph>;

    namespace pmr {
        using namespace stl::pmr;

        using utf8 = ustring<utf8_glyph, unicode_char_traits<utf8_glyph>, polymorphic_allocator<utf8_glyph>>;
        using utf16 =
          ustring<utf16_glyph, unicode_char_traits<utf16_glyph>, polymorphic_allocator<utf16_glyph>>;
        using utf32 =
          ustring<utf32_glyph, unicode_char_traits<utf32_glyph>, polymorphic_allocator<utf32_glyph>>;
    } // namespace pmr

} // namespace webpp


#endif // WEBPP_USTRING_HPP
