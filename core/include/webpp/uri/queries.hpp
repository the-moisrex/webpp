// Created by moisrex on 11/4/20.

#ifndef WEBPP_QUERIES_HPP
#define WEBPP_QUERIES_HPP

#include "../std/map.hpp"
#include "../utils/allocators.hpp"

namespace webpp::uri {

    template <typename StringType = stl::string,
              typename AllocType  = rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type,
                                                    stl::pair<stl::add_const_t<stl::remove_cvref_t<StringType>>, stl::remove_cvref_t<StringType>>>>
    struct basic_queries : stl::map<stl::remove_cvref_t<StringType>, stl::remove_cvref_t<StringType>,
                                    stl::less<stl::remove_cvref_t<StringType>>, AllocType> {
        using string_type    = stl::remove_cvref_t<StringType>;
        using allocator_type = AllocType;
        using super          = stl::map<string_type, string_type, stl::less<string_type>, allocator_type>;

        template <typename... Args>
        basic_queries(Args&&... args) : super{stl::forward<Args>(args)...} {}


        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] stl::size_t raw_string_size() const noexcept {
            return stl::reduce(this->cbegin(), this->cend(), 0ull,
                               [](string_type const& item) {
                                   return item.first.size() + item.second.size();
                               }) +
                   (this->size() * 2) - 2;
        }
    };

} // namespace webpp::uri

#endif // WEBPP_QUERIES_HPP
