#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../common/meta.hpp"
#include "../std/algorithm.hpp"
#include "../std/string.hpp"
#include "size.hpp"
#include "strings_concepts.hpp"

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

        constexpr splitter_iterator() noexcept                         = default; // .end()
        constexpr splitter_iterator(splitter_iterator const&) noexcept = default; // .end()
        constexpr splitter_iterator(splitter_ptr    ptr,
                                    difference_type start_pos_val   = 0ul,
                                    difference_type finish_pos_val  = 0ul,
                                    stl::size_t     delim_index_val = 0ul) noexcept
          : spltr{ptr},
            start_pos{start_pos_val},
            finish_pos{finish_pos_val},
            delim_index{delim_index_val} {}

        iterator& operator++() {
            assert(spltr != nullptr);
            const auto len = spltr->str.size();
            if (finish_pos == len) {
                // finished
                spltr = nullptr;
                return *this;
            }
            spltr->on_delimiter(delim_index++, [this, len]<Delimiter DT>(DT&& delim) {
                if (finish_pos != 0ul)
                    finish_pos += ascii::size(delim);
                start_pos = finish_pos;
                if constexpr (istl::CharType<DT> || istl::StringView<DT>) {
                    finish_pos = stl::min(len, spltr->str.find(delim, finish_pos));
                } else if constexpr (istl::StringViewifiable<DT>) {
                    finish_pos =
                      stl::min(len,
                               spltr->str.find(istl::string_viewify_of<string_view_type>(delim), finish_pos));
                    // todo: add array support
                    // todo: add functor support
                } else {
                    static_assert_false(DT, "Unknown delimiter type");
                }
            });
            return *this;
        }

        iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        [[nodiscard]] constexpr bool operator==(iterator other) const noexcept {
            if (spltr == nullptr && other.spltr == nullptr)
                return true;
            return start_pos == other.start_pos && finish_pos == other.finish_pos && spltr == other.spltr &&
                   delim_index == other.delim_index;
        }

        [[nodiscard]] constexpr bool operator!=(iterator other) const noexcept {
            return !this->operator==(other);
        }

        value_type operator*() {
            // can't dereference an iterator that points to nothing
            assert(spltr != nullptr);
            return spltr->str.substr(start_pos, finish_pos - start_pos);
        }


      private:
        splitter_ptr    spltr       = nullptr;
        difference_type start_pos   = 0ul;
        difference_type finish_pos  = 0ul;
        stl::size_t     delim_index = 0ul;
    };


    /**
     * String splitter struct.
     *
     * This class will help to split a string_splits
     */
    template <typename StrV = stl::string_view, Delimiter... DelimT>
        requires((istl::StringView<StrV> || istl::String<StrV>) && // it can be a string or string view
                 sizeof...(DelimT) > 0ul)                          // we must have at least one delimiter
    struct basic_splitter {

        static constexpr auto delim_count = sizeof...(DelimT);

        using string_view_type        = StrV;
        using delimiter_type          = stl::tuple<DelimT...>;
        using self_type               = basic_splitter;
        using iterator_type           = splitter_iterator<self_type>;
        using default_collection_type = stl::vector<string_view_type>;
        using default_array_type      = stl::array<string_view_type, delim_count + 1ul>;

        friend iterator_type;

      private:
        string_view_type str;
        delimiter_type   delims; // todo: add whitespaces as default delimiters


      public:
        constexpr basic_splitter(string_view_type str_val, DelimT&&... delims_input) noexcept
          : str{str_val},
            delims{stl::forward<DelimT>(delims_input)...} {}

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
        [[nodiscard]] Vec split(Args&&... args) const {
            Vec vec{stl::forward<Args>(args)...};
            vec.reserve(delim_count + 1ul); // we're gambling here
            split<Vec>(vec);
            return vec;
        }

        template <typename Arr = default_array_type>
        constexpr void split_array(Arr& data) const noexcept {
            constexpr auto array_size = stl::tuple_size<Arr>::value;
            auto pos_finder = [this, last_pos = 0ul]<stl::size_t Index>(istl::value_holder<Index>) mutable {
                constexpr auto delim_index = stl::clamp(Index, 0ul, delim_count - 1ul);
                const auto     delim       = stl::get<delim_index>(delims);
                const auto     pos         = stl::min(str.size(), str.find(delim, last_pos));
                const auto     ret         = str.substr(last_pos, pos - last_pos);
                last_pos                   = pos;
                last_pos += ascii::size(delim);
                return ret;
            };
            ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
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
            const auto            delim       = stl::get<delim_index>(delims);
            return delim;
        }

        [[nodiscard]] static constexpr auto delimiter_clamp(stl::size_t index) noexcept {
            return stl::clamp(index, 0ul, sizeof...(DelimT) - 1ul);
        }

        template <typename FuncT>
        constexpr void on_delimiter(stl::size_t index, FuncT&& functor) const {
            auto const dindex = delimiter_clamp(index);
            istl::for_index(dindex, delims, stl::forward<FuncT>(functor));
        }


        [[nodiscard]] constexpr string_view_type substr(stl::size_t start_pos, stl::size_t finish_pos) {
            return str.substr(start_pos, finish_pos);
        }

        // todo: add a way to use coroutines here as another way of doing the same thing
    };


    template <Delimiter... DelimT>
        requires(sizeof...(DelimT) > 0ul) // we must have at least one delimiter
    struct splitter : basic_splitter<stl::string_view, DelimT...> {
        using basic_splitter<stl::string_view, DelimT...>::basic_splitter;
    };


    template <typename T, Delimiter... DelimT>
    splitter(T&&, DelimT&&...) -> splitter<DelimT...>;

    /*
    // split strings with the specified delimiter
    template <typename StringVec = string_vector, istl::StringViewifiable StrV>
    StringVec split(StrV&& str, istl::CharType auto... delims) {}

    template <typename StringVec = string_vector,
              istl::StringViewifiable InpStrV,
              istl::StringViewifiable... StrV>
    StringVec split(InpStrV&& str, StrV&&... delims) {}
    */

} // namespace webpp::strings


#endif
