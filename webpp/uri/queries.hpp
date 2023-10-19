// Created by moisrex on 11/4/20.

#ifndef WEBPP_QUERIES_HPP
#define WEBPP_QUERIES_HPP

#include "../memory/allocators.hpp"
#include "../std/map.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components.hpp"
#include "encoding.hpp"

namespace webpp::uri {


    template <typename... T>
    static constexpr void
    parse_queries(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // todo
    }


    template <typename StringType = stl::string,
              typename AllocType  = typename stl::remove_cvref_t<StringType>::allocator_type>
    struct basic_queries : public istl::map_of_strings<StringType, AllocType> {
        using super                         = istl::map_of_strings<StringType, AllocType>;
        using string_type                   = stl::remove_cvref_t<StringType>;
        using key_type                      = typename super::key_type;
        using mapped_type                   = typename super::mapped_type;
        using char_type                     = typename string_type::value_type;
        using value_type                    = stl::pair<key_type, mapped_type>;
        using allocator_type                = typename super::allocator_type;
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename... Args>
            requires requires(Args... args) { super{stl::forward<Args>(args)...}; }
        constexpr basic_queries(Args&&... args) noexcept(noexcept(super(stl::forward<Args>(args)...)))
          : super{stl::forward<Args>(args)...} {}


        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& [key, value] : *this) {
                    sum += key.size();
                    sum += value.size();
                }
                return sum;
            }() + (this->size() * 2) - 2;
        }

        void append_to(istl::String auto& str) const {
            str.append("?");
            for (const auto& [key, value] : *this) {
                encode_uri_component(key, str, allowed_chars);
                str.append("=");
                encode_uri_component(value, str, allowed_chars);
                str.append("&");
            }
            str.pop_back(); // remove the "&" (or remove the unneeded "?" if it's empty)
        }
    };

} // namespace webpp::uri

#endif // WEBPP_QUERIES_HPP
