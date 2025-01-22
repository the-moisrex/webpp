// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../strings/peek.hpp"
#include "./parser/parse_path.hpp"

#include <compare>
#include <numeric>

namespace webpp::uri {

    /// Serialize path
    template <typename StorageType, istl::String StrT>
    static constexpr void render_path(StorageType const& storage, StrT& out, bool const is_opaque = false) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#url-path-serializer
        if (is_opaque) {
            out += storage.front();
        } else {
            for (auto const& seg : storage) {
                out += '/';
                out += seg;
            }
        }
    }

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::StringLike<T>;

    /**
     * @brief Basic Structured URI Path
     * @tparam SlugType The type of each segment of the path to use in the vector
     *                  If the slug type is a string view (and not a string), some of the parsing features
     *                  will be disabled, and that might be a security problem for you. So if the input is
     *                  from an untrusted source, make sure to slug type is a modifiable string.
     */
    template <Slug SlugType = stl::string, typename AllocT = allocator_type_from_t<SlugType>>
    struct basic_path {
        using slug_type           = SlugType;
        using slug_allocator_type = AllocT;
        using allocator_type      = rebind_allocator<slug_allocator_type, slug_type>; // vector's alloc
        using container_type      = stl::vector<slug_type, allocator_type>;
        using value_type          = slug_type;
        using char_type           = istl::char_type_of_t<slug_type>;
        using string_type         = istl::defaulted_string<slug_type, allocator_type>;
        using string_view_type    = istl::string_view_type_of<slug_type>;
        using path_type           = basic_path;
        using iterator            = typename container_type::iterator;
        using const_iterator      = typename container_type::const_iterator;

        using size_type       = typename container_type::size_type;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using vector_type     = container_type; // used in uri's uri_components

        static constexpr bool is_modifiable = istl::ModifiableString<value_type>;
        static constexpr bool is_segregated = true;
        static constexpr bool is_nothrow    = false;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well

        // except slash char
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>;

      private:
        container_type storage;
        bool           m_is_opaque = false;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            using iterator_type = typename string_view_type::iterator;
            parsing_uri_component_context<components::path, basic_path*, iterator_type> ctx;
            ctx.beg    = beg;
            ctx.end    = end;
            ctx.pos    = beg;
            ctx.out    = this;
            ctx.scheme = scheme_type::special_scheme;
            parse_path<Options>(ctx);
            return ctx.status;
        }

        template <uri_parsing_options Options = uri_parsing_options{}, istl::StringViewifiable StrT>
        constexpr uri_status_type parse(StrT&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
            return parse<Options>(str.begin(), str.end());
        }

        template <typename... T>
            requires(stl::is_constructible_v<container_type, T...>)
        explicit constexpr basic_path(T&&... args) : storage{stl::forward<T>(args)...} {}

        // NOLINTBEGIN(*-forwarding-reference-overload)
        template <istl::StringViewifiable T, typename InpAlloc = allocator_type>
            requires(!istl::cvref_as<T, basic_path>)
        explicit constexpr basic_path(T&& str, InpAlloc const& alloc = {}) : storage{alloc} {
            parse(stl::forward<T>(str));
        }

        template <istl::String T>
            requires(
              !istl::cvref_as<T, basic_path> && istl::cvref_as<typename T::allocator_type, allocator_type>)
        explicit constexpr basic_path(T&& str) : container_type{str.get_allocator()} {
            parse(stl::forward<T>(str));
        }

        // NOLINTEND(*-forwarding-reference-overload)

        template <istl::StringViewifiable SegStrT>
        constexpr basic_path& operator/=(SegStrT&& seg_str) {
            parse(stl::forward<SegStrT>(seg_str));
            return *this;
        }

        constexpr basic_path& operator=(value_type str) {
            storage.clear();
            parse(stl::move(str));
            return *this;
        }

        [[nodiscard]] constexpr allocator_type const& get_allocator() const noexcept {
            return storage.get_allocator();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        [[nodiscard]] constexpr decltype(auto) begin() const noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() const noexcept {
            return storage.end();
        }

        [[nodiscard]] constexpr decltype(auto) begin() noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() noexcept {
            return storage.end();
        }

        [[nodiscard]] constexpr decltype(auto) front() const noexcept {
            return storage.front();
        }

        [[nodiscard]] constexpr decltype(auto) back() const noexcept {
            return storage.front();
        }

        template <typename Arg>
            requires(!stl::integral<Arg>)
        [[nodiscard]] constexpr auto operator[](Arg&& arg) noexcept {
            return storage.operator[](stl::forward<Arg>(arg));
        }

        [[nodiscard]] constexpr auto operator[](size_type index) noexcept {
            return storage.operator[](index);
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(basic_path const& rhs) const noexcept {
            auto const lhs_size = storage.size();
            auto const rhs_size = rhs.size();
            if (lhs_size != rhs_size) {
                return stl::compare_partial_order_fallback(lhs_size, rhs_size);
            }
            if (stl::equal(storage.begin(), storage.end(), rhs.begin(), rhs.end())) {
                return stl::partial_ordering::equivalent;
            }
            return stl::partial_ordering::unordered;
        }

        template <istl::StringViewifiable SegStrT>
            requires(!stl::same_as<stl::remove_cvref_t<SegStrT>, basic_path>)
        [[nodiscard]] constexpr auto operator<=>(SegStrT&& rhs) const {
            // todo: optimize this
            auto const      path_str = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(rhs));
            path_type const rhs_path{path_str, storage.get_allocator()};
            return *this <=> rhs_path;
        }

        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !storage.empty() && storage.front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        constexpr void normalize(bool const remove_empty_segments = false) {
            remove_dot_segments(is_absolute(), remove_empty_segments);
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty() && !(storage.size() == 1 && storage.front().empty());
        }

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) {
            storage.clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(storage, beg, end, storage.get_allocator());
            } else {
                istl::emplace_one(storage, beg, end);
            }
        }

        constexpr void set_opaque(bool const value = false) noexcept {
            m_is_opaque = value;
        }

        constexpr void clear() {
            return storage.clear();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return storage.empty();
        }

        constexpr void pop_back() noexcept {
            return storage.pop_back();
        }

        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool const remove_leading,
                                           bool const remove_empty_segments = false) {
            if (storage.empty()) {
                return;
            }

            auto pos = storage.begin();

            // handle the first part
            while (pos != storage.end()) {
                if (remove_empty_segments && pos->empty()) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == current_dir) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == parent_dir) {
                    if (pos != storage.begin()) {
                        auto const last_el = std::prev(pos);
                        if (last_el->empty()) {
                            // remove just this one
                            pos = storage.erase(pos);
                            continue;
                        }
                        if (*last_el != parent_dir) {
                            // remove the previous one and this one
                            pos = storage.erase(last_el, std::next(pos));
                            if (pos == storage.begin()) {
                                return;
                            }
                            --pos;
                            continue;
                        }
                    } else if (remove_leading) {
                        pos = storage.erase(pos);
                        continue;
                    }
                }
                ++pos;
            }
        }

        [[nodiscard]] constexpr bool is_opaque() const noexcept {
            return m_is_opaque;
        }

        template <istl::String NStrT = string_type>
        constexpr void to_string(NStrT& out) const {
            render_path(storage_ref(), out, is_opaque());
        }

        template <istl::String NStrT = string_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] constexpr stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            // http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/transform_iterator.html
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& slug : *this) {
                    sum += slug.size();
                }
                return sum;
            }() + storage.size() - 1;
        }

        constexpr void trim() {
            // remove the last empty string
            if (!storage.empty() && storage.back().empty()) {
                stl::ignore(storage.pop_back());
            }
        }

        template <typename... Args>
        decltype(auto) emplace_back(Args&&... args) {
            return storage.emplace_back(stl::forward<Args>(args)...);
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            auto str = istl::string_viewify(stl::forward<NStrT>(inp_str));
            for (auto const& piece : storage) {
                if (!str.starts_with(piece)) {
                    return false;
                }
                str.remove_prefix(piece.size() + 1);
            }
            return true;
        }

        [[nodiscard]] constexpr bool operator==(basic_path const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

    template <istl::Stringifiable S>
    basic_path(S&& str) -> basic_path<stl::remove_cvref_t<decltype(istl::stringify(stl::forward<S>(str)))>>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
