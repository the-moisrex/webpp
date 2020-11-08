// Created by moisrex on 10/6/20.

#ifndef WEBPP_MIME_TYPES_HPP
#define WEBPP_MIME_TYPES_HPP

#include "../std/string_view.hpp"
#include "../strings/iequals.hpp"

namespace webpp {

    namespace details {
        static constexpr struct mapping {
            const char* extension;
            const char* mime_type;
        } mappings[] = {
          {"css", "text/css"},        {"gif", "image/gif"},         {"htm", "text/html"},
          {"html", "text/html"},      {"jpg", "image/jpeg"},        {"png", "image/png"},
          {"js", "text/javascript"},  {"wasm", "application/wasm"}, {"oga", "audio/ogg"},
          {"ogg", "audio/ogg"},       {"ogv", "video/ogg"},         {"swf", "application/x-shockwave-flash"},
          {"mp4", "video/mp4"},       {"mv4", "video/mp4"},         {"mp3", "audio/mp3"},
          {"svg", "image/svg+xml"},   {"webm", "video/webm"},       {"xml", "application/xml"},
          {"pdf", "application/pdf"}, {"txt", "text/plain"},        {0, 0} // Marks end of list.
        };
    }

    auto mime_type_of(istl::StringViewifiable auto &&extension) {
        for (details::mapping const* m = details::mappings; m->extension; ++m) {
            if (ascii::iequals<ascii::char_case_side::second_lowered>(m->extension, extension)) {
                return m->mime_type;
            }
        }

        return "application/octet-stream";
    }


}

#endif // WEBPP_MIME_TYPES_HPP
