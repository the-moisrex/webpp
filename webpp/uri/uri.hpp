// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "../std/string_view.hpp"
#include "../strings/to_case.hpp"
#include "./parser/parse_uri.hpp"
#include "credentials.hpp"
#include "fragment.hpp"
#include "host.hpp"
#include "path.hpp"
#include "port.hpp"
#include "queries.hpp"
#include "scheme.hpp"

namespace webpp::uri {

    /**
     * @brief Basic Structured URI
     */
    template <URIComponents CompT>
    struct [[nodiscard]] basic_uri {
        using component_type   = CompT;
        using string_type      = typename component_type::string_type;
        using char_type        = istl::char_type_of_t<string_type>;
        using allocator_type   = allocator_type_of<component_type>;
        using string_view_type = istl::string_view_type_of<string_type>;

        static constexpr bool is_modifiable = component_type::is_modifiable;
        static constexpr bool is_nothrow    = component_type::is_nothrow;
        static constexpr bool is_structured = URIStructuredComponents<component_type>;

        /// same as string_type if it's modifiable, otherwise, std::string
        using modifiable_string_type = stl::basic_string<char_type, stl::char_traits<char_type>, allocator_type>;

        using scheme_type   = basic_scheme<char_type>;
        using username_type = string_view_type;
        using password_type = string_view_type;
        using host_type     = basic_host<char_type>;
        using path_type     = basic_path<string_type>;
        using queries_type  = basic_queries<char_type>;
        using fragment_type = string_view_type;
        using status_type   = uri_status_type;

      private:
        component_type components;
        status_type    m_status = +uri_status::unparsed;

        template <uri_options Options, typename Iter>
        [[nodiscard]] constexpr uri_status_type parse_step(Iter beg, Iter end, uri_status const status)
          noexcept(is_nothrow) {
            using enum uri_status;
            using context_type = uri_context<component_type, void>;

            auto ctx   = create<context_type>(beg, end, get_allocator());
            // todo: ctx.out = ;
            ctx.status = +status | info_of(m_status);
            details::parse_uri_step<Options | state_override>(ctx);
            set_flags(m_status, flags_of(ctx.status));
            return m_status;
        }

      public:
        template <uri_options Options, typename Iter>
        constexpr void parse(Iter beg, Iter end) noexcept(is_nothrow) {
            using context_type = uri_context<component_type, void>;
            auto ctx           = create<context_type>(beg, end, get_allocator());
            // todo: ctx.out = ;
            parse_uri<Options>(ctx);
            m_status = ctx.status;
        }

        template <uri_options Options = {}>
        constexpr uri_status_type parse(string_view_type const str) noexcept(is_nothrow) {
            return parse<Options>(str.begin(), str.end());
        }

        template <stl::random_access_iterator IterT>
        constexpr basic_uri(IterT const beg, IterT const end, allocator_type const& alloc = {}) noexcept(is_nothrow)
          : components{create<component_type>(beg, end, alloc)} {
            parse(beg, end);
        }

        explicit constexpr basic_uri(string_view_type const uri_str,
                                     allocator_type const&  alloc = {}) // NOLINT(*-explicit-*)
          noexcept(is_nothrow)
          : basic_uri{uri_str, alloc} {}

        template <stl::size_t N>
        explicit(false) constexpr basic_uri(
          char_type const (&uri_str)[N],
          allocator_type const& alloc =
            allocator_type{
        }) noexcept(is_nothrow)
          : basic_uri{string_view_type{uri_str, N}, alloc} {}

        constexpr basic_uri()
            requires(stl::is_default_constructible_v<string_type>)
        = default;
        constexpr basic_uri(basic_uri const&)                = default;
        constexpr basic_uri(basic_uri&&) noexcept            = default;
        constexpr basic_uri& operator=(basic_uri const&)     = default;
        constexpr basic_uri& operator=(basic_uri&&) noexcept = default;
        constexpr ~basic_uri()                               = default;

        [[nodiscard]] constexpr decltype(auto) get_allocator() const noexcept {
            return allocator_from(components);
        }

        [[nodiscard]] constexpr bool valid() const noexcept {
            return is_valid(m_status);
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return valid();
        }

        [[nodiscard]] constexpr string_view_type scheme_view() const noexcept {
            return uri::scheme(components);
        }

        constexpr basic_scheme<char_type> scheme() const noexcept {
            return basic_scheme<char_type>{uri::scheme(components)};
        }

        [[nodiscard]] constexpr string_view_type hostname_view() const noexcept {
            return uri::hostname(components);
        }

        constexpr basic_host<char_type> hostname() const noexcept {
            return basic_host<char_type>{uri::hostname(components)};
        }

        [[nodiscard]] constexpr string_view_type port_view() const noexcept {
            return uri::port(components);
        }

        [[nodiscard]] constexpr string_view_type username() const noexcept {
            return uri::username(components);
        }

        [[nodiscard]] constexpr string_view_type password() const noexcept {
            return uri::password(components);
        }

        /// @returns string/string_view
        [[nodiscard]] constexpr auto authority() const
          noexcept(URIRelativeComponents<component_type> || URIHrefComponents<component_type>) {
            if constexpr (URIRelativeComponents<component_type> || URIHrefComponents<component_type>) {
                // For relative components, we can extract the authority directly from the original string
                // For href-based components, we can calculate the start and end of it.
                return uri::authority(components);
            } else {
                // For other component types, we need to construct the authority string
                // This returns a temporary string, not a string_view
                modifiable_string_type out;
                out.reserve(64); // Reserve reasonable size for authority
                render_authority(
                  uri::username(components),
                  uri::password(components),
                  uri::hostname(components),
                  uri::port(components),
                  out);
                return out;
            }
        }

        [[nodiscard]] constexpr string_view_type path_view() const noexcept {
            return uri::path(components);
        }

        [[nodiscard]] constexpr string_view_type queries_view() const noexcept {
            return uri::queries(components);
        }

        constexpr basic_path<string_type> path() const noexcept {
            return basic_path<string_type>{uri::path(components)};
        }

        constexpr basic_queries<char_type> queries() const noexcept {
            return basic_queries<char_type>{uri::queries(components)};
        }

        [[nodiscard]] constexpr string_view_type fragment() const noexcept {
            return uri::fragment(components);
        }

        constexpr void clear_scheme() noexcept(is_nothrow) {
            clear_scheme(components);
        }

        constexpr void clear_username() noexcept(is_nothrow) {
            clear_username(components);
        }

        constexpr void clear_password() noexcept(is_nothrow) {
            clear_password(components);
        }

        constexpr void clear_hostname() noexcept(is_nothrow) {
            clear_hostname(components);
        }

        constexpr void clear_port() noexcept(is_nothrow) {
            clear_port(components);
        }

        constexpr void clear_path() noexcept(is_nothrow) {
            clear_path(components);
        }

        constexpr void clear_queries() noexcept(is_nothrow) {
            clear_queries(components);
        }

        constexpr void clear_fragment() noexcept(is_nothrow) {
            clear_fragment(components);
        }

        constexpr void clear_authority() noexcept(is_nothrow) {
            clear_username();
            clear_password();
            clear_hostname();
            clear_port();
        }

        constexpr void clear() noexcept(is_nothrow) {
            clear_scheme();
            clear_authority();
            clear_path();
            clear_queries();
            clear_fragment();
        }

        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            return uri::has_scheme(components);
        }

        [[nodiscard]] constexpr bool has_hostname() const noexcept {
            return uri::has_hostname(components);
        }

        [[nodiscard]] constexpr bool has_username() const noexcept {
            return uri::has_username(components);
        }

        [[nodiscard]] constexpr bool has_password() const noexcept {
            return uri::has_password(components);
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username() || has_password();
        }

        [[nodiscard]] constexpr bool has_port() const noexcept {
            return uri::has_port(components);
        }

        [[nodiscard]] constexpr bool has_path() const noexcept {
            return uri::has_path(components);
        }

        [[nodiscard]] constexpr bool has_queries() const noexcept {
            return uri::has_queries(components);
        }

        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            return uri::has_fragment(components);
        }

        /**
         * This method returns an indication of whether the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] constexpr bool has_authority() const noexcept {
            return has_hostname() || has_credentials() || has_port();
        }

        constexpr basic_uri& operator=(string_view_type const str) noexcept(is_nothrow) {
            clear();
            parse(str);
            return *this;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return is_valid(m_status) && m_status != +uri_status::unparsed;
        }

        /// Return the length of the URI if available
        [[nodiscard]] constexpr stl::size_t length() const noexcept
            requires(!is_structured)
        {
            // todo: store the length in structured components for fast retrieval
            return length(components);
        }

        constexpr void to_string(modifiable_string_type& out) const {
            // https://url.spec.whatwg.org/#concept-url-serializer
            if constexpr (is_structured) {
                // todo: make this better:
                out.reserve(128);
            } else {
                out.reserve(length());
            }
            render_scheme(uri::scheme(components), out, true);
            if (has_hostname()) {
                out.append('/');
                out.append('/');
                if (has_credentials()) {
                    render_username(uri::username(components), out);
                    if (has_password()) {
                        out.append(':');
                        render_password(uri::password(components), out);
                    }
                    out.append('@');
                }
                render_hostname(uri::hostname(components), out);
                if (!port().is_default_port(scheme())) {
                    render_port(uri::port(components), out, true);
                }
            } else if (!is_opaque() && path().size() > 1 && path().front().empty()) {
                // If url’s host is null, url does not have an opaque path, url’s path’s size is greater than
                // 1, and url’s path[0] is the empty string, then append U+002F (/) followed by U+002E (.) to
                // output.c
                // This prevents web+demo:/.//not-a-host/ or web+demo:/path/..//not-a-host/, when parsed and
                // then serialized, from ending up as web+demo://not-a-host/ (they end up as
                // web+demo:/.//not-a-host/).
                out.append('/');
                out.append('.');
            }

            render_path(uri::path(components), out);
            render_queries(uri::queries(components), out, true);
            render_fragment(uri::fragment(components), out, true);
        }

        template <typename... Args>
        [[nodiscard]] constexpr modifiable_string_type as_string(Args&&... args) const {
            modifiable_string_type out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        [[nodiscard]] constexpr modifiable_string_type href() const {
            return as_string<modifiable_string_type>();
        }

        template <uri_options Options = {}>
        constexpr void href(string_view_type const str) noexcept(is_nothrow) {
            parse<Options>(str);
        }

        // /**
        //  * This method resolves the given relative reference, based on the given
        //  * base URI, returning the resolved target URI.
        //  *
        //  * @param[in] relative_uri
        //  *     This describes how to get to the target starting at the base.
        //  *
        //  * @return
        //  *     The resolved target URI is returned.
        //  *
        //  * @note
        //  *     It only makes sense to call this method on an absolute URI
        //  *     (in which I mean, the base URI should be absolute,
        //  *     as in IsRelativeReference() should return false).
        //  */
        // [[nodiscard]] constexpr basic_uri resolve(basic_uri const& relative_uri) const noexcept {
        //     // Resolve the reference by following the algorithm
        //     // from section 5.2.2 in
        //     // RFC 3986 (https://tools.ietf.org/html/rfc3986).
        //     auto target = create(relative_uri.get_allocator());
        //     if (relative_uri.has_scheme()) {
        //         target = relative_uri;
        //         target.path.normalize();
        //     } else {
        //         target.scheme   = this->scheme;
        //         target.fragment = relative_uri.fragment;
        //         if (!relative_uri.host.empty()) {
        //             target.host     = relative_uri.host;
        //             target.port     = relative_uri.port;
        //             target.username = relative_uri.username;
        //             target.password = relative_uri.password;
        //             target.path     = relative_uri.path;
        //             target.queries  = relative_uri.queries;
        //             target.path.normalize();
        //         } else {
        //             target.host     = this->host;
        //             target.username = this->username;
        //             target.password = this->password;
        //             target.port     = this->port;
        //             if (relative_uri.path.empty()) {
        //                 target.path = this->path;
        //                 if (!relative_uri.queries.empty()) {
        //                     target.queries = relative_uri.queries;
        //                 } else {
        //                     target.queries = this->queries;
        //                 }
        //             } else {
        //                 target.queries = relative_uri.queries;
        //                 // RFC describes this as:
        //                 // "if (R.path starts-with "/") then"
        //                 if (relative_uri.path.is_path_absolute()) {
        //                     target.path = relative_uri.path;
        //                     target.path.normalize();
        //                 } else {
        //                     // RFC describes this as:
        //                     // "T.path = merge(Base.path, R.path);"
        //                     target.path = this->path;
        //                     if (target.path.size() > 1) {
        //                         target.path.pop_back();
        //                     }
        //                     stl::copy(relative_uri.path.cbegin(),
        //                               relative_uri.path.cend(),
        //                               stl::back_inserter(target.path));
        //                     target.path.normalize();
        //                 }
        //             }
        //         }
        //     }
        //
        //     return target;
        // }


        /// Check if the path is an opaque path
        [[nodiscard]] constexpr bool is_opaque() const noexcept {
            return has_flag(m_status, uri_status::opaque_path);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type scheme(string_view_type const str) noexcept(is_nothrow) {
            auto status_res = parse_step<Options>(str.begin(), str.end(), uri_status::unparsed);
            if (is_valid(status_res) && port() == known_port(scheme())) {
                // From https://url.spec.whatwg.org/#scheme-state
                // If url’s port is url’s scheme’s default port, then set url’s port to null.
                port().clear();
                unset_flag(status_res, uri_status::has_non_null_port);
                unset_flag(m_status, uri_status::has_non_null_port);
            }
            return status_res;
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type authority(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_authority);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type username(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_authority);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type password(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_authority);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type hostname(string_view_type const str) noexcept(is_nothrow) {
            // https://url.spec.whatwg.org/#dom-url-hostname
            // If this’s URL has an opaque path, then return.
            if (this->path().is_opaque()) [[unlikely]] {
                return +uri_status::setting_hostname_on_opaque_path;
            }
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_authority);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type port(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_port);
        }

        template <stl::integral T = stl::uint16_t>
            requires is_modifiable
        constexpr bool port(T port_num) {
            return this->port().assign(port_num);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type path(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_path);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type queries(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_queries);
        }

        template <uri_options Options = {}>
            requires is_modifiable
        constexpr uri_status_type fragment(string_view_type const str) noexcept(is_nothrow) {
            return parse_step<Options>(str.begin(), str.end(), uri_status::valid_fragment);
        }
    };

    using uri = basic_uri<uri_components_owning<char>>;
} // namespace webpp::uri

#endif // WEBPP_URI_HPP
