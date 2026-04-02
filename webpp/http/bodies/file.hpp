// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_FILE_HPP
#define WEBPP_HTTP_BODIES_FILE_HPP

#include "../../memory/allocators.hpp"
#include "../../storage/file.hpp"
#include "./string.hpp"

#include <filesystem>

namespace webpp::http {


    ////////////////////////////// Body Serializer ( Object into Body ) //////////////////////////////

    template <typename T, HTTPBody BodyType>
        requires(stl::same_as<stl::remove_cvref_t<T>, stl::filesystem::path>)
    constexpr void tag_invoke(serialize_body_tag, T&& path, BodyType& body) {
        using body_type   = stl::remove_cvref_t<BodyType>;
        using string_type = typename body_type::string_type;
        string_type file_content{alloc};
        file::get_to(path, file_content);
        body = file_content;
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_FILE_HPP
