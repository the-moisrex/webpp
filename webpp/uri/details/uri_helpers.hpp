// Created by moisrex on 3/1/24.

#ifndef URI_HELPER_HPP
#define URI_HELPER_HPP

#include "../../std/array.hpp"
#include "../../strings/peek.hpp"
#include "../uri_status.hpp"
#include "./uri_components.hpp"

namespace webpp::uri::details {

    template <uri_parsing_options Options, ParsingURIContext CtxT, typename Iter, typename... ValT>
    [[nodiscard]] static constexpr bool
    safely_inc_if(Iter& pos, Iter const& end, CtxT& ctx, ValT... val) noexcept {
        if constexpr (Options.ignore_tabs_or_newlines) {
            using ctx_type  = CtxT;
            using char_type = typename ctx_type::char_type;

            stl::array<char_type, sizeof...(ValT)> const arr{static_cast<char_type>(val)...};

            stl::size_t index = 0;
            for (; pos != end; ++pos) {
                if (index == sizeof...(ValT)) {
                    return true;
                }
                switch (*pos) {
                    // ignoring tabs and newlines
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            continue;
                        }
                        [[fallthrough]];

                        [[likely]] default : {
                            if (*pos != arr[index]) {
                                return false;
                            }
                            ++index;
                            continue;
                        }
                }
                break;
            }
            return false;
        } else {
            return ascii::inc_if(pos, end, val...);
        }
    }

    template <uri_parsing_options Options, ParsingURIContext CtxT, typename... ValT>
    [[nodiscard]] static constexpr bool safely_inc_if(CtxT& ctx, ValT... val) noexcept {
        return safely_inc_if<Options>(ctx.pos, ctx.end, ctx, stl::forward<ValT>(val)...);
    }

} // namespace webpp::uri::details

#endif // URI_HELPER_HPP
