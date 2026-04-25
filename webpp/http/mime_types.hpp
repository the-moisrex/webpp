// Created by moisrex on 10/6/20.

#ifndef WEBPP_MIME_TYPES_HPP
#define WEBPP_MIME_TYPES_HPP

#include "../std/array.hpp"
#include "../std/string_view.hpp"
#include "../strings/iequals.hpp"

namespace webpp::http {

    namespace details {
        struct mapping {
            stl::string_view extension;
            stl::string_view mime_type;
        };

        static constexpr stl::array<mapping, 20> mappings{
          {
           {.extension = "css", .mime_type = "text/css"},
           {.extension = "gif", .mime_type = "image/gif"},
           {.extension = "htm", .mime_type = "text/html"},
           {.extension = "html", .mime_type = "text/html"},
           {.extension = "jpg", .mime_type = "image/jpeg"},
           {.extension = "png", .mime_type = "image/png"},
           {.extension = "js", .mime_type = "text/javascript"},
           {.extension = "wasm", .mime_type = "application/wasm"},
           {.extension = "oga", .mime_type = "audio/ogg"},
           {.extension = "ogg", .mime_type = "audio/ogg"},
           {.extension = "ogv", .mime_type = "video/ogg"},
           {.extension = "swf", .mime_type = "application/x-shockwave-flash"},
           {.extension = "mp4", .mime_type = "video/mp4"},
           {.extension = "mv4", .mime_type = "video/mp4"},
           {.extension = "mp3", .mime_type = "audio/mp3"},
           {.extension = "svg", .mime_type = "image/svg+xml"},
           {.extension = "webm", .mime_type = "video/webm"},
           {.extension = "xml", .mime_type = "application/xml"},
           {.extension = "pdf", .mime_type = "application/pdf"},
           {.extension = "txt", .mime_type = "text/plain"},
           }
        };
    } // namespace details

    [[nodiscard]] static constexpr stl::string_view mime_type_of(stl::string_view inp_extension) noexcept {
        // todo: optimize this based on first character of the extension
        for (auto const [extension, mime_type] : details::mappings) {
            if (ascii::iequals_sl(extension, inp_extension)) {
                return mime_type;
            }
        }

        return {"application/octet-stream"};
    }

    [[nodiscard]] static constexpr stl::string_view mime_type_for(stl::string_view const file_name) noexcept {
        if (auto const pos = file_name.find_last_of('.'); pos != stl::string_view::npos) {
            return mime_type_of(file_name.substr(pos + 1));
        }
        return {"application/octet-stream"};
    }

} // namespace webpp::http

#endif // WEBPP_MIME_TYPES_HPP
