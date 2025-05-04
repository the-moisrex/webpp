// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../../std/collection.hpp"
#include "../../std/map.hpp"
#include "../../std/string_like.hpp"
#include "../../std/vector.hpp"
#include "../../strings/to_case.hpp"
#include "../uri_status.hpp"
#include "./special_schemes.hpp"

#include <cstdint>
#include <limits>

namespace webpp::uri {

    /**
     * This is the concept that validates the URI contexts for parsing
     */
    template <typename T>
    concept ParsingURIContext = requires(T ctx) {
        typename T::iterator;
        typename T::out_type;
        typename T::out_seg_type;
        typename T::base_seg_type;
        typename T::base_type;
        typename T::char_type;
        typename T::vec_iterator;

        T::is_nothrow;
        T::is_modifiable;
        T::is_segregated;

        ctx.pos;
        ctx.beg;
        ctx.end;
        ctx.base;
        ctx.status;
    };

    /**
     * An output type that is like a vector or a map
     */
    template <typename T>
    concept SegregatedOutput =
      (istl::LinearContainer<T> && !istl::String<T>) || requires { requires T::is_segregated; };

    /// Path can be a vector
    template <typename T>
    concept VectorOutput = istl::LinearContainer<T> && !istl::MapContainer<T> && !istl::String<T>;

    /**
     * This is the output type that the URI parser will be able to put the results of components into.
     */
    template <typename T>
    concept ParsingOutput =
      istl::StringLike<T> || SegregatedOutput<T> || istl::cvref_as<T, istl::nothing_type>;


    template <typename SegType = stl::uint32_t, typename Iter = char const*>
    struct uri_components;

    template <typename T>
    struct is_uri_component : stl::false_type {};

    template <typename SegType, typename Iter>
    struct is_uri_component<uri_components<SegType, Iter>> : stl::true_type {};

    /// == nothing_type
    /// Iterator type has no effect
    template <typename Iter>
    struct uri_components<void, Iter> {};

    /**
     * URL Components
     * @tparam SegType defines the size of each component, for example if you specify uint32_t, then this
     * class will only support up to 4GiB of URL.
     *
     * RFC: https://tools.ietf.org/html/rfc3986
     *
     *    foo://example.com:8042/over/there?name=ferret#nose
     *    \_/   \______________/\_________/ \_________/ \__/
     *     |           |            |            |        |
     *  scheme     authority       path        query   fragment
     *     |   _____________________|__
     *    / \ /                        \
     *    urn:example:animal:ferret:nose
     *
     *  [protocol"://"[username[":"password]"@"]hostname[":"port]"/"?][path]["?"querystring]["#"fragment]
     */
    template <stl::integral SegType, typename Iter>
    struct uri_components<SegType, Iter> {
        using seg_type     = SegType;
        using iterator     = Iter;
        using vec_iterator = seg_type*;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<seg_type>::max() - 1;

        static constexpr seg_type omitted = stl::numeric_limits<seg_type>::max();

        /// resetting the values of integer types are always noexcept
        static constexpr bool is_nothrow = true;

        static constexpr bool is_modifiable = false;

        static constexpr bool is_segregated = false;

      private:
        iterator uri_beg{};
        seg_type scheme_end      = omitted; // excluding :// stuff
        seg_type authority_start = omitted; // username/host start
        seg_type password_start  = omitted;
        seg_type host_start      = omitted;
        seg_type port_start      = omitted;
        seg_type authority_end   = omitted; // path start
        seg_type queries_start   = omitted; // path end
        seg_type fragment_start  = omitted; // query end
        seg_type uri_end         = omitted; // string end

        constexpr void set_min_authority_end(seg_type end) noexcept {
            if (authority_end != omitted) {
                authority_end = stl::max(authority_end, end);
            } else {
                authority_end = end;
            }
        }

        constexpr void set_min_uri_end(seg_type end) noexcept {
            if (uri_end != omitted) {
                uri_end = stl::max(uri_end, end);
            } else {
                uri_end = end;
            }
        }

      public:
        constexpr void set_scheme(iterator beg, iterator end) noexcept {
            uri_beg    = beg;
            scheme_end = static_cast<seg_type>(end - beg);
            set_min_uri_end(scheme_end);
        }

        constexpr void set_path(seg_type beg, seg_type end) noexcept {
            authority_end = beg;
            set_min_uri_end(end);
        }

        constexpr void set_path(iterator beg, iterator end) noexcept {
            set_path(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_username(seg_type beg, seg_type end) noexcept {
            authority_start = beg;
            set_min_authority_end(end);
            set_min_uri_end(end);
        }

        constexpr void set_username(iterator beg, iterator end) noexcept {
            set_username(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_password(seg_type beg, seg_type end) noexcept {
            password_start = beg;
            set_min_authority_end(end);
            set_min_uri_end(end);
        }

        constexpr void set_password(iterator beg, iterator end) noexcept {
            set_password(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_hostname(seg_type beg, seg_type end) noexcept {
            host_start = beg;
            set_min_authority_end(end);
            set_min_uri_end(end);
        }

        constexpr void set_hostname(iterator beg, iterator end) noexcept {
            set_hostname(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_port(seg_type start, seg_type end) noexcept {
            port_start = start;
            set_min_authority_end(end);
            set_min_uri_end(end);
        }

        constexpr void set_port(iterator beg, iterator end) noexcept {
            set_port(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_queries(seg_type start, seg_type end) noexcept {
            queries_start = start;
            set_min_uri_end(end);
        }

        constexpr void set_queries(iterator beg, iterator end) noexcept {
            set_queries(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_fragment(seg_type start, seg_type end) noexcept {
            fragment_start = start;
            set_min_uri_end(end);
        }

        constexpr void set_fragment(iterator beg, iterator end) noexcept {
            set_fragment(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

      private:
        constexpr void clean_authority_end() noexcept {
            if (authority_start == omitted && password_start == omitted && port_start == omitted &&
                host_start == omitted)
            {
                authority_end = omitted;
            }
        }

      public:
        constexpr void clear_scheme() noexcept {
            scheme_end = omitted;
        }

        constexpr void clear_hostname() noexcept {
            host_start = omitted;
            clean_authority_end();
        }

        constexpr void clear_port() noexcept {
            port_start = omitted;
            clean_authority_end();
        }

        constexpr void clear_username() noexcept {
            authority_start = omitted;
            clean_authority_end();
        }

        constexpr void clear_password() noexcept {
            password_start = omitted;
            clean_authority_end();
        }

        constexpr void clear_path() noexcept {
            if (queries_start == omitted && fragment_start == omitted) {
                uri_end = stl::min(authority_end, uri_end);
            }
            authority_end = stl::min(authority_end, uri_end);
        }

        constexpr void clear_queries() noexcept {
            if (fragment_start == omitted) {
                uri_end = stl::min(fragment_start, uri_end);
            }
            queries_start = omitted;
        }

        constexpr void clear_fragment() noexcept {
            uri_end        = stl::min(fragment_start, uri_end);
            fragment_start = omitted;
        }

        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            return scheme_end != omitted;
        }

        [[nodiscard]] constexpr bool has_username() const noexcept {
            return authority_start != omitted;
        }

        [[nodiscard]] constexpr bool has_password() const noexcept {
            return password_start != omitted;
        }

        [[nodiscard]] constexpr bool has_hostname() const noexcept {
            return host_start != omitted;
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return authority_start != omitted;
        }

        [[nodiscard]] constexpr bool has_port() const noexcept {
            return port_start != omitted;
        }

        [[nodiscard]] constexpr bool has_path() const noexcept {
            return authority_end != omitted;
        }

        [[nodiscard]] constexpr bool has_queries() const noexcept {
            return queries_start != omitted;
        }

        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            return fragment_start != omitted;
        }

      private:
        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT view(seg_type beg, seg_type size) const noexcept {
            using str_iterator = typename StrT::const_iterator;
            if constexpr (
              !stl::same_as<str_iterator, iterator> && requires { StrT{uri_beg.base() + beg, size}; })
            {
                return StrT{uri_beg.base() + beg, size};
            } else {
                return StrT{uri_beg + beg, size};
            }
        }

      public:
        [[nodiscard]] constexpr seg_type size() const noexcept {
            return uri_end == omitted ? 0 : uri_end;
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_scheme() const noexcept(!istl::ModifiableString<StrT>) {
            if (scheme_end == omitted) {
                return {};
            }
            return view<StrT>(0, stl::min(scheme_end, size()));
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_username() const noexcept(!istl::ModifiableString<StrT>) {
            if (authority_start == omitted) {
                return {};
            }
            return view<StrT>(authority_start,
                              stl::min(password_start - 1, host_start - 1) - authority_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_password() const noexcept(!istl::ModifiableString<StrT>) {
            if (password_start == omitted) {
                return {};
            }
            return view<StrT>(password_start,
                              stl::min(stl::min(host_start, port_start), authority_end) - 1 - password_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_hostname() const noexcept(!istl::ModifiableString<StrT>) {
            if (host_start == omitted) {
                return {};
            }
            return view<StrT>(host_start,
                              stl::min(stl::min(port_start - 1, authority_end), size()) - host_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_port() const noexcept(!istl::ModifiableString<StrT>) {
            if (port_start == omitted) {
                return {};
            }
            return view<StrT>(port_start, stl::min(authority_end, size()) - port_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_path() const noexcept(!istl::ModifiableString<StrT>) {
            if (authority_end == omitted) {
                return {};
            }
            return view<StrT>(
              authority_end,
              stl::min(stl::min(queries_start - 1, fragment_start - 1), size()) - authority_end);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_queries() const noexcept(!istl::ModifiableString<StrT>) {
            if (queries_start == omitted) {
                return {};
            }
            return view<StrT>(queries_start, stl::min(fragment_start - 1, size()) - queries_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_fragment() const noexcept(!istl::ModifiableString<StrT>) {
            if (fragment_start == omitted) {
                return {};
            }
            return view<StrT>(fragment_start, uri_end - fragment_start);
        }

        template <istl::StringLike StrT = stl::string_view>
        [[nodiscard]] constexpr StrT hostname() const noexcept(!istl::ModifiableString<StrT>) {
            return get_hostname<StrT>();
        }
    };

    template <istl::StringLike StrT, typename Iter>
    struct uri_components<StrT, Iter> {
        using string_type  = StrT;
        using iterator     = Iter;
        using seg_type     = string_type;
        using char_type    = typename string_type::value_type;
        using size_type    = typename string_type::size_type;
        using vec_iterator = seg_type*;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = stl::is_nothrow_assignable_v<string_type, char_type const*>;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        static constexpr bool is_segregated = false;

      private:
        string_type m_scheme{};
        string_type m_username{};
        string_type m_password{};
        string_type m_hostname{};
        string_type m_port{};
        string_type m_path{};
        string_type m_queries{};
        string_type m_fragment{};

      public:
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                                         \
    template <istl::StringLike StrVT = stl::basic_string_view<char_type>>                        \
    [[nodiscard]] constexpr StrVT get_##field() const noexcept(!istl::ModifiableString<StrVT>) { \
        return {m_##field.data(), m_##field.size()};                                             \
    }                                                                                            \
                                                                                                 \
    constexpr void clear_##field() noexcept {                                                    \
        istl::clear(m_##field);                                                                  \
    }                                                                                            \
                                                                                                 \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                                  \
        return !m_##field.empty();                                                               \
    }                                                                                            \
                                                                                                 \
    template <typename InpIter = iterator>                                                       \
    constexpr void set_##field(InpIter beg, InpIter end) noexcept(is_nothrow) {                  \
        istl::assign(m_##field, beg, end);                                                       \
    }                                                                                            \
                                                                                                 \
    constexpr void set_##field(string_type str) noexcept(is_nothrow) {                           \
        m_##field = stl::move(str);                                                              \
    }                                                                                            \
                                                                                                 \
    [[nodiscard]] constexpr string_type& field() noexcept {                                      \
        return m_##field;                                                                        \
    }                                                                                            \
                                                                                                 \
    [[nodiscard]] constexpr string_type const& field() const noexcept {                          \
        return m_##field;                                                                        \
    }



        webpp_def(scheme)
        webpp_def(username)
        webpp_def(password)
        webpp_def(hostname)
        webpp_def(port)
        webpp_def(path)
        webpp_def(queries)
        webpp_def(fragment)
#undef webpp_def

        // NOLINTEND(*-macro-usage)

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username() || has_password();
        }
    };

    /**
     * URI Components fully separated
     *   - Domains are split into its subdomains
     *   - Path are split into its segments
     *   - Queries are mapped
     */
    template <istl::LinearContainer VecOfStr, istl::MapContainer MapOfStr>
    struct uri_components<VecOfStr, MapOfStr> {
        using vec_type     = VecOfStr;
        using map_type     = MapOfStr;
        using string_type  = typename vec_type::value_type;
        using seg_type     = string_type;
        using iterator     = typename string_type::const_iterator;
        using char_type    = typename string_type::value_type;
        using size_type    = typename string_type::size_type;
        using vec_iterator = typename vec_type::iterator;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = false;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        static constexpr bool is_segregated = true;

      private:
        string_type m_scheme{};
        string_type m_username{};
        string_type m_password{};
        string_type m_hostname{};
        string_type m_port{};
        vec_type    m_path{};
        map_type    m_queries{};
        string_type m_fragment{};

      public:
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                                            \
    static constexpr bool is_##field##_modifiable = stl::same_as<decltype(m_##field), string_type>; \
    static constexpr bool is_##field##_vec        = stl::same_as<decltype(m_##field), vec_type>;    \
                                                                                                    \
    template <istl::StringView StrVT = stl::string_view>                                            \
        requires(is_##field##_modifiable)                                                           \
    [[nodiscard]] constexpr StrVT get_##field() const noexcept(!istl::ModifiableString<StrVT>) {    \
        return {m_##field.data(), m_##field.size()};                                                \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept                                                         \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        istl::clear(m_##field);                                                                     \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept                                                         \
        requires(!is_##field##_modifiable)                                                          \
    {                                                                                               \
        istl::clear(m_##field);                                                                     \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                                     \
        if constexpr (is_##field##_vec) {                                                           \
            return !m_##field.empty() && !(m_##field.size() == 1 && m_##field.front().empty());     \
        } else {                                                                                    \
            return !m_##field.empty();                                                              \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    template <typename Iter = iterator>                                                             \
    constexpr void set_##field(Iter beg, Iter end) noexcept(is_nothrow)                             \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        istl::assign(m_##field, beg, end);                                                          \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(decltype(m_##field)&& str) noexcept(is_nothrow)                      \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        m_##field = stl::move(str);                                                                 \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr auto& field() noexcept {                                                \
        return m_##field;                                                                           \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr auto const& field() const noexcept {                                    \
        return m_##field;                                                                           \
    }



        webpp_def(scheme)
        webpp_def(username)
        webpp_def(password)
        webpp_def(hostname)
        webpp_def(port)
        webpp_def(path)
        webpp_def(queries)
        webpp_def(fragment)
#undef webpp_def

        // NOLINTEND(*-macro-usage)

        constexpr void set_path(iterator beg, iterator end) {
            istl::clear(m_path);
            if constexpr (is_modifiable) {
                istl::emplace_one(m_path, beg, end, m_path.get_allocator());
            } else {
                istl::emplace_one(m_path, beg, end);
            }
        }

        constexpr void set_queries(iterator beg, iterator end) {
            using pack_type  = typename map_type::value_type;
            using key_type   = typename map_type::key_type;
            using value_type = typename map_type::mapped_type;

            // first "name" is chosen for the whole value, because of the algorithm that gets the queries will
            // be correct that way

            istl::clear(m_queries);
            if constexpr (is_modifiable) {
                istl::emplace_one(m_queries,
                                  pack_type{
                                    key_type{beg, end, m_queries.get_allocator()},
                                    value_type{m_queries.get_allocator()}
                });
            } else {
                istl::emplace_one(m_queries,
                                  pack_type{
                                    key_type{beg, end},
                                    value_type{}
                });
            }
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_hostname(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_hostname.empty()) {
                return out;
            }
            for (auto pos = m_hostname.begin();;) {
                out += *pos;
                if (++pos == m_hostname.end()) {
                    break;
                }
                out += '.';
            }
            return out;
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_path(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_path.empty()) {
                return out;
            }
            auto seg = m_path.begin();
            for (;;) {
                out += *seg;
                if (++seg == m_path.end()) {
                    break;
                }
                out += '/';
            }
            return out;
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_queries(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_queries.empty()) {
                return out;
            }
            for (auto pos = m_queries.begin();;) {
                auto const [name, value]  = *pos;
                out                      += name;
                if (!value.empty()) {
                    out += '=';
                    out += value;
                }
                if (++pos == m_queries.end()) {
                    break;
                }
                out += '&';
            }
            return out;
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username() || has_password();
        }
    };

    using uri_components_view = uri_components<stl::string_view, stl::string_view::const_iterator>;
    using uri_components_u32  = uri_components<stl::uint32_t, char const*>;

    /**
     * A class used during parsing a URI
     */
    template <typename OutSegType  = stl::uint32_t,
              typename OutIter     = char const*,
              typename BaseSegType = void,
              typename BaseIter    = OutIter>
    struct parsing_uri_context {
        using out_seg_type   = OutSegType;
        using base_seg_type  = BaseSegType;
        using out_type       = stl::conditional_t<is_uri_component<stl::remove_pointer_t<OutSegType>>::value,
                                                  OutSegType,
                                                  uri_components<out_seg_type, OutIter>>;
        using clean_out_type = stl::remove_pointer_t<out_type>;
        using base_type      = stl::conditional_t<is_uri_component<stl::remove_pointer_t<BaseSegType>>::value,
                                                  BaseSegType,
                                                  uri_components<base_seg_type, BaseIter>>;

        // this might be different from OutSegType
        using seg_type        = typename clean_out_type::seg_type;
        using iterator        = typename clean_out_type::iterator;
        using char_type       = stl::iter_value_t<iterator>;
        using state_type      = uri_status_type;
        using vec_iterator    = typename clean_out_type::vec_iterator;

        static constexpr bool is_nothrow    = clean_out_type::is_nothrow;
        static constexpr bool has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool is_modifiable = clean_out_type::is_modifiable;
        static constexpr bool is_segregated = clean_out_type::is_segregated;

        iterator beg{}; // the beginning of the string, not going to change during parsing
        iterator pos{}; // current position
        iterator end{}; // the end of the string
        out_type out{}; // the output uri components
        [[no_unique_address]] base_type base{};
        state_type                      status = stl::to_underlying(uri_status::unparsed);
    };

    enum struct components : stl::uint8_t {
        scheme,
        host,
        username,
        password,
        port,
        path,
        queries,
        fragment
    };

    namespace details {
        template <typename>
        struct is_modifiable_string_if_vector {
            static constexpr bool value = false;
        };

        template <typename T>
            requires istl::LinearContainer<T>
        struct is_modifiable_string_if_vector<T> {
            static constexpr bool value = istl::ModifiableString<T>;
        };

        template <typename T>
        concept ModifiableVectorOfStrings = is_modifiable_string_if_vector<T>::value;

        template <typename T>
        struct extract_types_from_out_type {
            using vec_iterator = T*;
        };

        template <typename T>
            requires istl::LinearContainer<T>
        struct extract_types_from_out_type<T> {
            using vec_iterator = typename T::iterator;
        };

        template <typename T>
            requires istl::MapContainer<T>
        struct extract_types_from_out_type<T> {
            using vec_iterator = T*;
        };
    } // namespace details

    /**
     * Same as parsing_uri_component, but the output is only one single component
     */
    template <components Comp,
              typename OutType,
              typename Iter,
              typename BaseSegType = void,
              typename BaseIter    = void>
        requires(stl::is_pointer_v<OutType> && !stl::integral<OutType>)
    struct parsing_uri_component_context
      : details::extract_types_from_out_type<stl::remove_pointer_t<OutType>> {
        using base_seg_type   = BaseSegType;
        using out_type        = OutType;
        using seg_type        = OutType;
        using base_type       = uri_components<base_seg_type, BaseIter>;
        using iterator        = Iter;
        using char_type       = stl::iter_value_t<iterator>;
        using state_type      = uri_status_type;

        using out_container_type = stl::remove_pointer_t<out_type>;
        using out_seg_type       = out_container_type;

        static constexpr components component     = Comp;
        static constexpr bool       is_nothrow    = stl::is_nothrow_copy_assignable_v<out_container_type>;
        static constexpr bool       has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool       is_segregated = SegregatedOutput<out_container_type>;
        static constexpr bool       is_modifiable = istl::ModifiableString<out_container_type> ||
                                              details::ModifiableVectorOfStrings<out_container_type>;

        iterator beg{}; // the beginning of the string, not going to change during parsing
        iterator pos{};
        iterator end{};
        out_type out{}; // it's a pointer to string, string_view, vector, or a map
        [[no_unique_address]] base_type base{};
        state_type                      status = stl::to_underlying(uri_status::unparsed);
    };

    using parsing_uri_context_u32 = parsing_uri_context<stl::uint32_t, char const*>;

    template <typename StrT = stl::string>
    using parsing_uri_context_string = parsing_uri_context<StrT, typename StrT::const_iterator>;

    template <typename CharT = char>
    using parsing_uri_context_view = parsing_uri_context_string<stl::basic_string_view<CharT>>;

    template <istl::StringLike StrT = stl::string, typename Allocator = typename StrT::allocator_type>
    using parsing_uri_context_segregated = parsing_uri_context<
      stl::vector<StrT, typename stl::allocator_traits<Allocator>::template rebind_alloc<StrT>>,
      istl::map_of_strings<StrT, Allocator>>;

    template <typename Allocator = stl::allocator<char>>
    using parsing_uri_context_segregated_view = parsing_uri_context_segregated<stl::string_view, Allocator>;

    template <typename CtxT>
    concept single_component = ParsingURIContext<CtxT> && requires { CtxT::component; };

    template <components Comp, typename CtxT>
    concept has_component =
      ParsingURIContext<CtxT> && (requires { requires CtxT::component == Comp; } || !single_component<CtxT>);

    namespace details {

        template <components Comp>
        [[nodiscard]] constexpr decltype(auto) get_component(auto& out) noexcept {
            using enum components;
            if constexpr (scheme == Comp) {
                return out.scheme();
            } else if constexpr (username == Comp) {
                return out.username();
            } else if constexpr (password == Comp) {
                return out.password();
            } else if constexpr (port == Comp) {
                return out.port();
            } else if constexpr (host == Comp) {
                return out.hostname();
            } else if constexpr (path == Comp && requires { out.path(); }) {
                return out.path();
            } else if constexpr (queries == Comp) {
                return out.queries();
            } else if constexpr (fragment == Comp) {
                return out.fragment();
            } else {
                return istl::unmove(out);
            }
        }

        template <components Comp>
        [[nodiscard]] constexpr decltype(auto) get_output_value_from(auto& out) noexcept {
            using enum components;
            if constexpr (scheme == Comp) {
                return out.get_scheme();
            } else if constexpr (username == Comp) {
                return out.get_username();
            } else if constexpr (password == Comp) {
                return out.get_password();
            } else if constexpr (port == Comp) {
                return out.get_port();
            } else if constexpr (host == Comp) {
                return out.get_hostname();
            } else if constexpr (path == Comp) {
                return out.get_path();
            } else if constexpr (queries == Comp) {
                return out.get_queries();
            } else if constexpr (fragment == Comp) {
                return out.get_fragment();
            }
        }

        template <components Comp, typename StrVT>
        [[nodiscard]] constexpr auto get_output_view_from(auto& out) noexcept {
            using enum components;
            if constexpr (scheme == Comp) {
                return out.template get_scheme<StrVT>();
            } else if constexpr (username == Comp) {
                return out.template get_username<StrVT>();
            } else if constexpr (password == Comp) {
                return out.template get_password<StrVT>();
            } else if constexpr (port == Comp) {
                return out.template get_port<StrVT>();
            } else if constexpr (host == Comp) {
                return out.template get_hostname<StrVT>();
            } else if constexpr (path == Comp) {
                return out.template get_path<StrVT>();
            } else if constexpr (queries == Comp) {
                return out.template get_queries<StrVT>();
            } else if constexpr (fragment == Comp) {
                return out.template get_fragment<StrVT>();
            }
        }

        template <components Comp, typename... Args>
        constexpr void set_value_to(auto& out, Args&&... args) {
            using enum components;
            if constexpr (scheme == Comp) {
                out.set_scheme(stl::forward<Args>(args)...);
            } else if constexpr (username == Comp) {
                out.set_username(stl::forward<Args>(args)...);
            } else if constexpr (password == Comp) {
                out.set_password(stl::forward<Args>(args)...);
            } else if constexpr (port == Comp) {
                out.set_port(stl::forward<Args>(args)...);
            } else if constexpr (host == Comp) {
                out.set_hostname(stl::forward<Args>(args)...);
            } else if constexpr (path == Comp) {
                out.set_path(stl::forward<Args>(args)...);
            } else if constexpr (queries == Comp) {
                out.set_queries(stl::forward<Args>(args)...);
            } else if constexpr (fragment == Comp) {
                out.set_fragment(stl::forward<Args>(args)...);
            }
        }

        template <components Comp>
        constexpr void clear_from(auto& out) noexcept {
            using enum components;
            if constexpr (scheme == Comp) {
                out.clear_scheme();
            } else if constexpr (username == Comp) {
                out.clear_username();
            } else if constexpr (password == Comp) {
                out.clear_password();
            } else if constexpr (port == Comp) {
                out.clear_port();
            } else if constexpr (host == Comp) {
                out.clear_hostname();
            } else if constexpr (path == Comp) {
                out.clear_path();
            } else if constexpr (queries == Comp) {
                out.clear_queries();
            } else if constexpr (fragment == Comp) {
                out.clear_fragment();
            }
        }

        template <components Comp>
        [[nodiscard]] constexpr bool has_value_from(auto& out) noexcept {
            using enum components;
            if constexpr (scheme == Comp) {
                return out.has_scheme();
            } else if constexpr (username == Comp) {
                return out.has_username();
            } else if constexpr (password == Comp) {
                return out.has_password();
            } else if constexpr (port == Comp) {
                return out.has_port();
            } else if constexpr (host == Comp) {
                return out.has_hostname();
            } else if constexpr (path == Comp) {
                return out.has_path();
            } else if constexpr (queries == Comp) {
                return out.has_queries();
            } else if constexpr (fragment == Comp) {
                return out.has_fragment();
            } else {
                return true;
            }
        }

    } // namespace details

    template <components Comp, ParsingURIContext CtxT>
    [[nodiscard]] constexpr decltype(auto) get_component(CtxT& ctx) noexcept {
        if constexpr (single_component<CtxT>) {
            if constexpr (Comp == CtxT::component) {
                return istl::deptr(ctx.out);
            } else {
                return istl::nothing;
            }
        } else {
            return details::get_component<Comp>(istl::deptr(ctx.out));
        }
    }

    template <typename OutT>
    [[nodiscard]] constexpr auto& get_storage(OutT& out) noexcept {
        if constexpr (requires { out.storage_ref(); }) {
            return out.storage_ref();
        } else {
            return out;
        }
    }

    template <components Comp, ParsingURIContext CtxT>
    [[nodiscard]] constexpr auto& get_storage(CtxT& ctx) noexcept {
        if constexpr (single_component<CtxT>) {
            if constexpr (Comp == CtxT::component) {
                return get_storage(istl::deptr(ctx.out));
            } else {
                return istl::nothing;
            }
        } else if constexpr (!CtxT::is_segregated && !CtxT::is_modifiable) {
            return istl::nothing;
        } else {
            return get_storage(details::get_component<Comp>(istl::deptr(ctx.out)));
        }
    }

    /// Default buffer which is nothing
    /// Specialize it for those you need.
    template <typename OutT>
    [[nodiscard]] static constexpr istl::nothing_type get_buffer([[maybe_unused]] OutT const& out) noexcept {
        return {};
    }

    /// returns end(), start_segment will set it to the right iterator
    template <VectorOutput OutT>
    [[nodiscard]] static constexpr auto get_buffer(OutT& out) noexcept {
        return out.end();
    }

    template <components Comp, ParsingURIContext CtxT>
    [[nodiscard]] static constexpr decltype(auto) get_buffer(CtxT& ctx) noexcept {
        if constexpr (CtxT::is_modifiable) {
            return get_buffer(get_component<Comp>(ctx));
        } else {
            return istl::nothing_type{};
        }
    }

    template <components Comp, ParsingURIContext CtxT>
    [[nodiscard]] constexpr auto get_output_view(CtxT& ctx) noexcept {
        using char_type        = typename CtxT::char_type;
        using string_view_type = stl::basic_string_view<char_type>;
        if constexpr (single_component<CtxT>) {
            if constexpr (Comp == CtxT::component) {
                return string_view_type{ctx.out->data(), ctx.out->data() + ctx.out->size()};
            } else {
                return string_view_type{};
            }
        } else {
            return details::get_output_view_from<Comp, string_view_type>(istl::deptr(ctx.out));
        }
    }

    template <components Comp, ParsingURIContext CtxT, typename... Args>
    constexpr void set_value(CtxT& ctx, Args&&... args) noexcept(CtxT::is_nothrow) {
        if constexpr (single_component<CtxT>) {
            // works for strings only
            if constexpr (Comp == CtxT::component) {
                istl::deptr(ctx.out).assign(stl::forward<Args>(args)...);
            }
        } else {
            details::set_value_to<Comp>(istl::deptr(ctx.out), stl::forward<Args>(args)...);
        }

        if constexpr (components::host == Comp) {
            set_flag(ctx.status, uri_status::has_non_empty_host);
        } else if constexpr (components::port == Comp) {
            set_flag(ctx.status, uri_status::has_non_null_port);
        }
    }

    template <components Comp, ParsingURIContext CtxT>
    constexpr void clear(CtxT& ctx) noexcept {
        using istl::clear;

        if constexpr (single_component<CtxT>) {
            // won't work with the integers
            if constexpr (Comp == CtxT::component) {
                clear(istl::deptr(ctx.out));
            }
        } else {
            details::clear_from<Comp>(istl::deptr(ctx.out));
        }

        if constexpr (components::host == Comp) {
            unset_flag(ctx.status, uri_status::has_non_empty_host);
        } else if constexpr (components::port == Comp) {
            unset_flag(ctx.status, uri_status::has_non_null_port);
        }
    }

    template <components Comp, ParsingURIContext CtxT>
    [[nodiscard]] constexpr bool has_value(CtxT& ctx) noexcept {
        if constexpr (components::host == Comp) {
            return has_flag(ctx.status, uri_status::has_non_empty_host);
        } else if constexpr (components::port == Comp) {
            return has_flag(ctx.status, uri_status::has_non_null_port);
        } else if constexpr (single_component<CtxT>) {
            if constexpr (Comp == CtxT::component) {
                if constexpr (requires { istl::deptr(ctx.out).has_value(); }) {
                    return istl::deptr(ctx.out).has_value();
                } else {
                    return istl::deptr(ctx.out).empty();
                }
            }
            return true;
            // else return true, just to get rid of the warnings
        } else {
            return details::has_value_from<Comp>(istl::deptr(ctx.out));
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
