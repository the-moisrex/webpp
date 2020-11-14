// Created by moisrex on 11/4/20.

#ifndef WEBPP_QUERIES_HPP
#define WEBPP_QUERIES_HPP

#include "../std/map.hpp"
#include "../utils/allocators.hpp"
#include "./encoding.hpp"
#include "./details/constants.hpp"

namespace webpp::uri {

    template <typename StringType = stl::string,
              typename AllocType  = rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type,
                                                    stl::pair<stl::add_const_t<stl::remove_cvref_t<StringType>>, stl::remove_cvref_t<StringType>>>>
    struct basic_queries : stl::map<stl::remove_cvref_t<StringType>, stl::remove_cvref_t<StringType>,
                                    stl::less<stl::remove_cvref_t<StringType>>, AllocType> {
        using string_type    = stl::remove_cvref_t<StringType>;
        using char_type = typename string_type::value_type;
        using allocator_type = AllocType;
        using value_type = stl::pair<const string_type, string_type>;
        using super          = stl::map<string_type, string_type, stl::less<string_type>, allocator_type>;
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename... Args>
        constexpr basic_queries(Args&&... args) : super{stl::forward<Args>(args)...} {}


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

        void append_to(istl::String auto&str) const {
            for (const auto [key, value] : *this) {
                encode_uri_component(key, str, allowed_chars);
                encode_uri_component(value, str, allowed_chars);
            }
        }

    };

} // namespace webpp::uri

#endif // WEBPP_QUERIES_HPP
