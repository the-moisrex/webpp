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
           {"css", "text/css"}, {"gif", "image/gif"},
           {"htm", "text/html"}, {"html", "text/html"},
           {"jpg", "image/jpeg"}, {"png", "image/png"},
           {"js", "text/javascript"}, {"wasm", "application/wasm"},
           {"oga", "audio/ogg"}, {"ogg", "audio/ogg"},
           {"ogv", "video/ogg"}, {"swf", "application/x-shockwave-flash"},
           {"mp4", "video/mp4"}, {"mv4", "video/mp4"},
           {"mp3", "audio/mp3"}, {"svg", "image/svg+xml"},
           {"webm", "video/webm"}, {"xml", "application/xml"},
           {"pdf", "application/pdf"}, {"txt", "text/plain"},
           }
        };
    } // namespace details

    [[nodiscard]] static constexpr stl::string_view mime_type_of(stl::string_view inp_extension) noexcept {
        // todo: optimize this based on first character of the extension
        for (auto const [extension, mime_type] : details::mappings) {
            if (ascii::iequals<ascii::char_case_side::second_lowered>(extension, inp_extension)) {
                return mime_type;
            }
        }

        return {"application/octet-stream"};
    }

    [[nodiscard]] static constexpr stl::string_view mime_type_for(stl::string_view file_name) noexcept {
        if (auto const pos = file_name.find_last_of('.'); pos != stl::string_view::npos) {
            return mime_type_of(file_name.substr(pos + 1));
        }
        return {"application/octet-stream"};
    }

} // namespace webpp::http

#endif // WEBPP_MIME_TYPES_HPP
