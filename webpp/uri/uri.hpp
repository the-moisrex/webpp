// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "../std/string_view.hpp"
#include "../strings/to_case.hpp"
#include "authority.hpp"
#include "fragment.hpp"
#include "host.hpp"
#include "path.hpp"
#include "port.hpp"
#include "queries.hpp"
#include "scheme.hpp"
#include "uri_string.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void continue_parsing_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        while (!has_error(ctx.status)) {
            switch (get_value(ctx.status)) {
                using enum uri_status;
                case valid:                  // we're done parsing
                case valid_punycode: return; // todo?
                case valid_authority: parse_authority<Options>(ctx); break;
                case valid_file_host:
                    if constexpr (Options.allow_file_hosts) {
                        parse_file_host<Options>(ctx);
                    } else {
                        stl::unreachable(); // should be impossible.
                    }
                    break;
                case valid_port: parse_port<Options>(ctx); break;
                case valid_authority_end: parse_authority_end<Options>(ctx); break;
                case valid_opaque_path: parse_opaque_path<Options>(ctx); break;
                case valid_path: parse_path<Options>(ctx); break;
                case valid_queries: parse_queries<Options>(ctx); break;
                case valid_fragment: parse_fragment<Options>(ctx); break;
                case unparsed: parse_scheme<Options>(ctx); break; // start from the beginning
                default: stl::unreachable(); break;               // should be impossible
            }
        }
    }

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        continue_parsing_uri<Options>(ctx);
    }

    template <uri_parsing_options Options = uri_parsing_options{}, istl::StringView StrV = stl::string_view>
    static constexpr auto parse_uri(StrV str) noexcept {
        using iterator     = typename StrV::const_iterator;
        using context_type = parsing_uri_context<StrV, iterator>;
        context_type context{.beg = str.begin(),
                             .pos = str.begin(), // current position is start
                             .end = str.end()};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_parsing_options Options = uri_parsing_options{}, istl::StringLike StrT, typename SegType>
    static constexpr auto parse_uri(
      StrT const&                    the_url,
      uri_components<SegType> const& origin_context) noexcept(istl::StringView<StrT>) {
        using iterator             = typename StrT::const_iterator;
        using base_components_type = uri_components<SegType>;
        using base_seg_type        = typename base_components_type::seg_type;
        using context_type         = parsing_uri_context<StrT, iterator, base_seg_type>;

        context_type context{.beg  = the_url.begin(),
                             .pos  = the_url.begin(), // current position is start
                             .end  = the_url.end(),
                             .base = origin_context};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_parsing_options     Options = uri_parsing_options{},
              istl::StringLike        StrT,
              istl::StringViewifiable OStrV>
    static constexpr auto parse_uri(StrT const& the_url, OStrV&& base_uri) noexcept(istl::StringView<StrT>) {
        using iterator = typename StrT::const_iterator;
        static_assert(
          stl::same_as<iterator, typename OStrV::const_iterator>,
          "Origin's string's char type must be the same as the specified URI's string's char type.");
        auto const base         = istl::string_viewify(stl::forward<OStrV>(base_uri));
        using base_context_type = parsing_uri_context<stl::uint32_t, iterator>;

        base_context_type origin_context{.beg = base.begin(), .pos = base.begin(), .end = base.end()};
        parse_uri<Options>(origin_context);

        return parse_uri<Options>(the_url, origin_context.out);
    }

    /**
     * @brief Customization of uri components that holds all of the URI components with all the bells and the
     * whistles
     * @tparam StrT String or String View type
     * @tparam AllocT Allocator type (we're not extractinig it from StrT, because you may pass a string view)
     */
    template <istl::StringLike StrT, Allocator AllocT>
    struct uri_components<StrT, AllocT> {
        using string_type    = StrT;
        using allocator_type = AllocT;

        using scheme_type   = basic_scheme<string_type>;
        using host_type     = basic_host<string_type, allocator_type>;
        using username_type = basic_username<string_type>;
        using password_type = basic_password<string_type>;
        using port_type     = basic_port<string_type>;
        using path_type     = basic_path<string_type, allocator_type>;
        using fragment_type = basic_fragment<string_type>;
        using queries_type  = basic_queries<string_type, allocator_type>;

        using vec_type     = typename path_type::vector_type;
        using map_type     = typename queries_type::map_type;
        using seg_type     = string_type;
        using iterator     = typename string_type::const_iterator;
        using char_type    = typename string_type::value_type;
        using size_type    = typename string_type::size_type;
        using map_iterator = typename map_type::iterator;
        using vec_iterator = typename vec_type::iterator;

        // map_type::value_type is const, we need a modifiable name
        using map_value_type = stl::pair<typename map_type::key_type, typename map_type::mapped_type>;


        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = false;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        static constexpr bool is_segregated = true;

      private:
        scheme_type   m_scheme{};
        username_type m_username{};
        password_type m_password{};
        host_type     m_hostname{};
        port_type     m_port{};
        path_type     m_path{};
        queries_type  m_queries{};
        fragment_type m_fragment{};

      public:
        constexpr uri_components() = default;

        explicit constexpr uri_components(allocator_type const& alloc)
          : m_scheme{alloc},
            m_username{alloc},
            m_password{alloc},
            m_hostname{alloc},
            m_port{alloc},
            m_path{alloc},
            m_queries{alloc},
            m_fragment{alloc} {}

        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                                            \
    template <istl::StringLike NStrT = stl::basic_string_view<char_type>>                           \
    [[nodiscard]] constexpr NStrT field##_view() const noexcept                                     \
        requires(!is_modifiable)                                                                    \
    {                                                                                               \
        return m_##field.template view<NStrT>();                                                    \
    }                                                                                               \
                                                                                                    \
    template <istl::StringLike NStrT = typename decltype(m_##field)::string_type, typename... Args> \
    [[nodiscard]] constexpr NStrT get_##field(Args&&... args)                                       \
      const noexcept(!istl::ModifiableString<NStrT>) {                                              \
        return m_##field.template as_string<NStrT>(stl::forward<Args>(args)...);                    \
    }                                                                                               \
                                                                                                    \
    template <uri_parsing_options     Options = uri_parsing_options{},                              \
              istl::StringViewifiable NStrT   = stl::basic_string_view<char_type>>                  \
    constexpr uri_status_type field(NStrT&& inp_str) noexcept(!istl::ModifiableString<NStrT>) {     \
        auto const str = istl::string_viewify(stl::forward<NStrT>(inp_str));                        \
        return m_##field.template parse<Options>(str.begin(), str.end());                           \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept {                                                       \
        m_##field.clear();                                                                          \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                                     \
        return m_##field.has_value();                                                               \
    }                                                                                               \
                                                                                                    \
    template <typename Iter = iterator>                                                             \
    constexpr void set_##field(Iter beg, Iter end) noexcept(is_nothrow) {                           \
        m_##field.assign(beg, end);                                                                 \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(decltype(m_##field)&& str) noexcept(is_nothrow) {                    \
        m_##field = stl::move(str);                                                                 \
    }                                                                                               \
                                                                                                    \
    template <typename Iter = iterator>                                                             \
    constexpr void set_lowered_##field(Iter beg, Iter end) noexcept(is_nothrow) {                   \
        if constexpr (is_modifiable) {                                                              \
            ascii::lower_to(field##_ref(), beg, end);                                               \
        } else {                                                                                    \
            set_##field(beg, end);                                                                  \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    constexpr void set_lowered_##field(string_type str) noexcept(is_nothrow) {                      \
        if constexpr (is_modifiable) {                                                              \
            ascii::lower_to(m_##field, str.begin(), str.end());                                     \
        } else {                                                                                    \
            set_##field(stl::move(str));                                                            \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr auto const& field() const noexcept {                                    \
        return m_##field;                                                                           \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr auto& field##_ref() noexcept {                                          \
        return m_##field.storage_ref();                                                             \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr auto const& field##_ref() const noexcept {                              \
        return m_##field.storage_ref();                                                             \
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
     * A class used during parsing a URI
     */
    template <typename OutComponentType, typename Iter, typename BaseSegType = void, typename BaseIter = void>
    struct parsing_structured_uri_context {
        using out_seg_type   = OutComponentType;
        using base_seg_type  = BaseSegType;
        using out_type       = OutComponentType;
        using clean_out_type = stl::remove_pointer_t<out_type>;
        using base_type      = stl::conditional_t<is_uri_component<stl::remove_pointer_t<BaseSegType>>::value,
                                             BaseSegType,
                                             uri_components<base_seg_type, BaseIter>>;
        using seg_type       = typename clean_out_type::seg_type; // this might be different than
                                                                  // OutSegType
        using iterator       = Iter;
        using iterator_traits = stl::iterator_traits<iterator>;
        using char_type       = istl::char_type_of_t<typename iterator_traits::pointer>;
        using state_type      = stl::underlying_type_t<uri_status>;

        using map_iterator   = typename clean_out_type::map_iterator;
        using vec_iterator   = typename clean_out_type::vec_iterator;
        using map_value_type = typename clean_out_type::map_value_type;


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
        scheme_type                     scheme = scheme_type::not_special;
    };

    /**
     * @brief Basic Structured URI
     * @tparam StringType Storage Type
     * @tparam AllocT Allocator type
     */
    template <istl::StringLike StringType, Allocator AllocT = allocator_type_from_t<StringType>>
    struct basic_uri : uri_components<StringType, rebind_allocator<AllocT, typename StringType::value_type>> {
        using string_type     = StringType;
        using char_type       = istl::char_type_of_t<string_type>;
        using allocator_type  = rebind_allocator<AllocT, char_type>;
        using components_type = uri_components<string_type, allocator_type>;
        using iterator        = typename string_type::const_iterator;
        using size_type       = typename string_type::size_type;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow    = false;

        /// same as string_type if it's modifiable, otherwise, std::string
        using modifiable_string_type = istl::defaulted_string<string_type, allocator_type>;

        using scheme_type   = basic_scheme<string_type>;
        using username_type = basic_username<string_type>;
        using password_type = basic_password<string_type>;
        using host_type     = basic_host<string_type, allocator_type>;
        using port_type     = basic_port<string_type>;
        using path_type     = basic_path<string_type, allocator_type>;
        using queries_type  = basic_queries<string_type, allocator_type>;
        using fragment_type = basic_fragment<string_type>;

        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_structured_uri_context<components_type*, Iter> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = static_cast<components_type*>(this);
            parse_uri<Options>(ctx);
            return ctx.status;
        }

        template <uri_parsing_options Options = uri_parsing_options{}, istl::StringViewifiable StrT>
        constexpr uri_status_type parse(StrT&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
            return parse<Options>(str.begin(), str.end());
        }

        template <istl::StringViewifiable T, Allocator InpAllocT = allocator_type>
        explicit(false) constexpr basic_uri(T&& uri_str, InpAllocT const& alloc = {}) // NOLINT(*-explicit-*)
          noexcept(is_nothrow)
          : components_type{alloc} {
            parse(stl::forward<T>(uri_str));
        }

        template <Allocator InpAllocT = allocator_type>
        constexpr explicit basic_uri(InpAllocT const& alloc) : components_type{alloc} {}

        constexpr basic_uri()
            requires(stl::is_default_constructible_v<string_type>)
        = default;
        constexpr basic_uri(basic_uri const&)                = default;
        constexpr basic_uri(basic_uri&&) noexcept            = default;
        constexpr basic_uri& operator=(basic_uri const&)     = default;
        constexpr basic_uri& operator=(basic_uri&&) noexcept = default;
        constexpr ~basic_uri()                               = default;

        [[nodiscard]] constexpr auto get_allocator() const noexcept {
            return this->get_hostname().get_allocator();
        }

        constexpr void clear_authority() noexcept(is_nothrow) {
            this->clear_username();
            this->clear_password();
            this->clear_hostname();
            this->clear_port();
        }

        constexpr void clear() noexcept(is_nothrow) {
            this->clear_scheme();
            this->clear_authority();
            this->clear_path();
            this->clear_queries();
            this->clear_fragment();
        }

        template <istl::StringViewifiable InpStr = stl::basic_string_view<char_type>>
        constexpr basic_uri& operator=(InpStr&& inp_str) noexcept(is_nothrow) {
            this->clear();
            parse(stl::forward<InpStr>(inp_str));
            return *this;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return this->has_scheme() || this->has_authority() || this->has_path() || this->has_queries() ||
                   this->has_fragment();
        }

        /// Get the total string size WITHOUT DELIMITTERS, and not considering the the encoding bloat
        ///
        /// Attention: The size will be different based on the string type used because non-modifiable string
        ///            types won't be able to hold decoded/encoded values.
        [[nodiscard]] constexpr size_type size() const noexcept {
            // todo: queries, host, and path's sizes are not string sizes
            return this->scheme_ref().size() + this->username_ref().size() + this->password_ref().size() +
                   this->hostname_ref().size() + this->port_ref().size() + this->path_ref().size() +
                   this->queries_ref().size() + this->fragment_ref().size();
        }

        template <istl::String NStrT = modifiable_string_type>
        constexpr void to_string(NStrT& out) const {
            out.reserve(size());
            this->scheme().to_string(out, true);
            this->username().to_string(out);
            this->password().to_string(out, true);
            this->hostname().to_string(out);
            if (!this->port().is_default_port(this->scheme().view())) {
                this->port().to_string(out, true);
            }
            this->path().to_string(out);
            this->queries().to_string(out, true);
            this->fragment().to_string(out, true);
        }

        template <istl::String NStrT = modifiable_string_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        template <uri_parsing_options     Options = uri_parsing_options{},
                  istl::StringViewifiable NStrT   = stl::basic_string_view<char_type>>
        constexpr uri_status_type href(NStrT&& inp_str) {
            return parse<Options>(stl::forward<NStrT>(inp_str));
        }

        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] constexpr bool has_authority() const noexcept {
            return this->has_hostname() || this->has_port() || this->has_username() || this->has_password();
        }

        /**
         * This method resolves the given relative reference, based on the given
         * base URI, returning the resolved target URI.
         *
         * @param[in] relative_uri
         *     This describes how to get to the target starting at the base.
         *
         * @return
         *     The resolved target URI is returned.
         *
         * @note
         *     It only makes sense to call this method on an absolute URI
         *     (in which I mean, the base URI should be absolute,
         *     as in IsRelativeReference() should return false).
         */
        [[nodiscard]] constexpr basic_uri resolve(basic_uri const& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            auto target = create(relative_uri.get_allocator());
            if (relative_uri.has_scheme()) {
                target = relative_uri;
                target.path.normalize();
            } else {
                target.scheme   = this->scheme;
                target.fragment = relative_uri.fragment;
                if (!relative_uri.host.empty()) {
                    target.host     = relative_uri.host;
                    target.port     = relative_uri.port;
                    target.username = relative_uri.username;
                    target.password = relative_uri.password;
                    target.path     = relative_uri.path;
                    target.queries  = relative_uri.queries;
                    target.path.normalize();
                } else {
                    target.host     = this->host;
                    target.username = this->username;
                    target.password = this->password;
                    target.port     = this->port;
                    if (relative_uri.path.empty()) {
                        target.path = this->path;
                        if (!relative_uri.queries.empty()) {
                            target.queries = relative_uri.queries;
                        } else {
                            target.queries = this->queries;
                        }
                    } else {
                        target.queries = relative_uri.queries;
                        // RFC describes this as:
                        // "if (R.path starts-with "/") then"
                        if (relative_uri.path.is_path_absolute()) {
                            target.path = relative_uri.path;
                            target.path.normalize();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path = this->path;
                            if (target.path.size() > 1) {
                                target.path.pop_back();
                            }
                            stl::copy(relative_uri.path.cbegin(),
                                      relative_uri.path.cend(),
                                      stl::back_inserter(target.path));
                            target.path.normalize();
                        }
                    }
                }
            }

            return target;
        }
    };

    using uri = basic_uri<stl::string>;
} // namespace webpp::uri

#endif // WEBPP_URI_HPP
