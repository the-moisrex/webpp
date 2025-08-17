#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../common/meta.hpp"
#include "../std/algorithm.hpp"
#include "../std/string_like.hpp"
#include "../std/tuple.hpp"
#include "size.hpp"
#include "strings_concepts.hpp"

#include <cassert>

namespace webpp::strings {



    // string vector: same as above, but you can add to it
    /*
    template <istl::CharType CharT = char, Allocator AllocType = stl::allocator<CharT>>
    struct basic_string_vector : stl::vector<string_piece<CharT>, AllocType> {};

    using string_vector = basic_string_vector<>;
    */


    /**
     * The logic of finding the next piece will be placed here.
     *
     * I'd love to make this a coroutine!
     */
    template <typename T>
    struct splitter_iterator {
        using splitter_type    = stl::add_const_t<T>;
        using splitter_ptr     = stl::add_pointer_t<splitter_type>;
        using string_view_type = typename splitter_type::string_view_type;
        using iterator         = splitter_iterator;

        // iterator traits
        using difference_type   = stl::size_t;
        using value_type        = string_view_type;
        using pointer           = stl::add_pointer_t<value_type>;
        using reference         = value_type;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

        constexpr explicit splitter_iterator(
          splitter_ptr          ptr,
          difference_type const start_pos_val   = 0,
          difference_type const finish_pos_val  = 0,
          stl::size_t const     delim_index_val = 0) noexcept
          : spltr{ptr},
            start_pos{start_pos_val},
            finish_pos{finish_pos_val},
            delim_index{delim_index_val} {}

        constexpr splitter_iterator() noexcept                                    = default; // .end()
        constexpr splitter_iterator(splitter_iterator&&) noexcept                 = default;
        constexpr splitter_iterator(splitter_iterator const&) noexcept            = default;
        constexpr splitter_iterator& operator=(splitter_iterator&&) noexcept      = default;
        constexpr splitter_iterator& operator=(splitter_iterator const&) noexcept = default;
        constexpr ~splitter_iterator() noexcept                                   = default;

        constexpr iterator& operator++() {
            assert(spltr != nullptr);
            if (finish_pos == spltr->string_size()) {
                // finished
                spltr = nullptr;
                return *this;
            }
            spltr->on_delimiter(delim_index++, [this]<Delimiter DT>(DT delim) {
                if (finish_pos != 0) {
                    finish_pos += ascii::size(delim);
                }
                start_pos = finish_pos;
                if constexpr (istl::CharType<DT> || istl::StringView<DT>) {
                    finish_pos = stl::min(spltr->string_size(), spltr->find(delim, finish_pos));
                } else if constexpr (istl::StringViewifiable<DT>) {
                    finish_pos = stl::min(spltr->string_size(),
                                          spltr->find(istl::string_viewify_of<string_view_type>(delim), finish_pos));
                    // todo: add array support
                    // todo: add functor support
                } else {
                    static_assert_false(DT, "Unknown delimiter type");
                }
            });
            return *this;
        }

        constexpr iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        [[nodiscard]] constexpr bool operator==(iterator other) const noexcept {
            if (spltr == nullptr && other.spltr == nullptr) {
                return true;
            }
            return start_pos == other.start_pos && finish_pos == other.finish_pos && spltr == other.spltr &&
                   delim_index == other.delim_index;
        }

        [[nodiscard]] constexpr bool operator!=(iterator const other) const noexcept {
            return !this->operator==(other);
        }

        [[nodiscard]] constexpr value_type operator*() noexcept {
            return value();
        }

        template <istl::StringView StrV = value_type>
        [[nodiscard]] constexpr StrV value() noexcept {
            // can't dereference an iterator that points to nothing
            assert(spltr != nullptr);
            return spltr->template substr<StrV>(start_pos, finish_pos - start_pos);
        }


      private:
        splitter_ptr    spltr       = nullptr;
        difference_type start_pos   = 0;
        difference_type finish_pos  = 0;
        stl::size_t     delim_index = 0;
    };

    /**
     * String splitter struct.
     *
     * This class will help to split a string
     */
    template <stl::random_access_iterator IterT = char const*, Delimiter... DelimT>
        requires(sizeof...(DelimT) > 0) // we must have at least one delimiter
    struct [[nodiscard]] splitter {
        static constexpr auto delim_count = sizeof...(DelimT);

        using src_iterator            = IterT;
        using char_type               = istl::char_type_of_t<src_iterator>;
        using string_view_type        = stl::basic_string_view<char_type>;
        using delimiter_type          = stl::tuple<DelimT...>;
        using self_type               = splitter;
        using iterator_type           = splitter_iterator<self_type>;
        using default_collection_type = stl::vector<string_view_type>;
        using default_array_type      = stl::array<string_view_type, delim_count + 1>;
        using size_type               = stl::size_t;

        friend iterator_type;

      private:
        src_iterator   beg{};
        src_iterator   endp{};
        delimiter_type delims; // todo: add whitespaces as default delimiters


      public:
        template <istl::StringLike StrV = string_view_type, Delimiter... InpDelimT>
            requires(stl::convertible_to<typename StrV::iterator, src_iterator>)
        constexpr explicit splitter(StrV str_val, InpDelimT&&... delims_input) noexcept
          : beg{str_val.begin()},
            endp{str_val.end()},
            delims{stl::forward<InpDelimT>(delims_input)...} {}

        template <istl::StringViewifiable StrV = string_view_type, Delimiter... InpDelimT>
            requires(!istl::StringLike<StrV>)
        constexpr explicit splitter(StrV str_val, InpDelimT&&... delims_input) noexcept
          : beg{istl::string_viewify(stl::forward<StrV>(str_val)).data()},
            endp{istl::string_viewify(stl::forward<StrV>(str_val)).data() +
                 istl::string_viewify(stl::forward<StrV>(str_val)).size()},
            delims{stl::forward<InpDelimT>(delims_input)...} {}

        template <Delimiter... InpDelimT>
        constexpr explicit splitter(src_iterator inp_beg, src_iterator inp_end, InpDelimT&&... delims_input) noexcept
          : beg{inp_beg},
            endp{inp_end},
            delims{stl::forward<InpDelimT>(delims_input)...} {}

        template <Delimiter... InpDelimT>
        constexpr explicit splitter(src_iterator inp_beg, InpDelimT&&... delims_input) noexcept
            requires(istl::CharType<std::iter_value_t<iterator_type>>)
          : beg{inp_beg},
            endp{std::next(inp_beg, std::strlen(inp_beg))},
            delims{stl::forward<InpDelimT>(delims_input)...} {}

        [[nodiscard]] constexpr iterator_type begin() const noexcept {
            return iterator_type{this}.operator++();
        }

        [[nodiscard]] constexpr iterator_type end() const noexcept {
            return {};
        }

        template <typename Vec = default_collection_type>
        Vec& split(Vec& vec) const {
            stl::copy(this->begin(), this->end(), stl::back_inserter(vec));
            return vec;
        }

        /**
         * Split the strings and get a vector
         */
        template <typename Vec = default_collection_type, typename... Args>
        [[nodiscard]] constexpr Vec split(Args&&... args) const {
            Vec vec{stl::forward<Args>(args)...};
            vec.reserve(delim_count + 1UL); // we're gambling here
            split<Vec>(vec);
            return vec;
        }

        template <typename Arr = default_array_type>
        constexpr void split_array(Arr& data) const noexcept {
            constexpr auto array_size = stl::tuple_size_v<Arr>;
            ([&, this]<stl::size_t... I>(stl::index_sequence<I...>) {
                auto pos_finder = [this, last_pos = 0UL]<stl::size_t Index>(istl::value_holder<Index>) mutable {
                    constexpr auto delim_index  = stl::clamp(Index, 0UL, delim_count - 1UL);
                    auto const     delim        = get<delim_index>(delims);
                    auto const     pos          = this->find(delim, last_pos) - last_pos;
                    auto const     ret          = substr(last_pos, pos);
                    last_pos                   += pos;
                    last_pos                   += ascii::size(delim);
                    return ret;
                };
                ((data[I] = pos_finder(istl::value_holder<I>{})), ...); // call the func
            })(stl::make_index_sequence<array_size>());
        }

        template <typename Arr = default_array_type, typename... Args>
        [[nodiscard]] constexpr Arr split_array(Args&&... args) const noexcept {
            Arr arr{stl::forward<Args>(args)...};
            split_array(arr);
            return arr;
        }

        template <stl::size_t N, typename Arr = stl::array<string_view_type, N>, typename... Args>
        [[nodiscard]] constexpr Arr split_into(Args&&... args) const noexcept {
            Arr arr{stl::forward<Args>(args)...};
            split_array(arr);
            return arr;
        }

        template <stl::size_t Index>
        [[nodiscard]] constexpr auto delimiter() const noexcept {
            constexpr stl::size_t delim_index = delimiter_clamp(Index);
            auto const            delim       = stl::get<delim_index>(delims);
            return delim;
        }

        [[nodiscard]] static constexpr auto delimiter_clamp(stl::size_t const index) noexcept {
            return stl::clamp(index, 0UL, sizeof...(DelimT) - 1UL);
        }

        template <typename FuncT>
        constexpr void on_delimiter(stl::size_t const index, FuncT&& functor) const {
            auto const dindex = delimiter_clamp(index);
            istl::for_index(dindex, delims, stl::forward<FuncT>(functor));
        }

        [[nodiscard]] constexpr stl::size_t string_size() const noexcept {
            return static_cast<stl::size_t>(endp - beg);
        }

        template <istl::StringLike StrT = string_view_type>
        [[nodiscard]] constexpr StrT substr(stl::size_t const pos, stl::size_t const count = StrT::npos) const
          noexcept(istl::StringView<StrT>) {
            return StrT{beg, endp}.substr(pos, count);
        }

        template <typename... T>
        [[nodiscard]] constexpr size_type find(T&&... args) const noexcept {
            return string_view_type{beg, endp}.find(std::forward<T>(args)...);
        }

        // todo: add a way to use coroutines here as another way of doing the same thing
    };

    template <typename T, typename... DelimT>
        requires(sizeof...(DelimT) > 0)
    splitter(T&&, DelimT&&...) -> splitter<stl::ranges::iterator_t<T>, DelimT...>;

    template <stl::random_access_iterator T, typename... DelimT>
        requires(sizeof...(DelimT) > 0)
    splitter(T, T, DelimT&&...) -> splitter<T, DelimT...>;

} // namespace webpp::strings


#endif
