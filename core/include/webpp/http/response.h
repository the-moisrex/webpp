#ifndef WEBPP_HTTP_RESPONSE_H
#define WEBPP_HTTP_RESPONSE_H

#include "../traits/traits_concepts.h"
#include "./response_concepts.h"
#include "body.h"
#include "header.h"

#include <filesystem>
#include <fstream>
#include <memory>

namespace webpp {

    /**
     * This class owns its data.
     */
    template <Traits TraitsType>
    class basic_response {

      public:
        using traits_type = TraitsType;
        using body_type   = webpp::body;
        using header_type =
          webpp::headers<TraitsType, true, header_type::response>;
        using str_view_t = typename traits_type::string_view_type;
        using str_t      = typename traits_type::string_type;

        body_type   body;
        header_type header;

        basic_response() noexcept                          = default;
        basic_response(basic_response const& res) noexcept = default;
        basic_response(basic_response&& res) noexcept      = default;
        basic_response(str_t const& b) noexcept : body(b) {
        }
        basic_response(str_t&& b) noexcept : body(::std::move(b)) {
        }


        basic_response& operator=(basic_response const&) = default;
        basic_response& operator=(basic_response&& res) noexcept = default;
        basic_response& operator=(str_t const& str) noexcept {
            body.replace_string(str);
            return *this;
        }
        basic_response& operator=(str_t&& str) noexcept {
            body.replace_string(::std::move(str));
            return *this;
        }

        [[nodiscard]] bool
        operator==(basic_response const& res) const noexcept {
            return body == res.body && header == res.header;
        }
        [[nodiscard]] bool
        operator!=(basic_response const& res) const noexcept {
            return body != res.body || header != res.header;
        }

        void calculate_default_headers() noexcept {
            if (!header.contains("Content-Type"))
                header.emplace("Content-Type", "text/html; charset=utf-8");

            if (!header.contains("Content-Length"))
                header.emplace(
                  "Content-Length",
                  ::std::to_string(body.str().size() * sizeof(char)));
        }


        // static methods:
        /*
        static response_t file(::std::filesystem::path const& file) noexcept;
        static response_t image(::std::string_view const& file) noexcept;
        static response_t json_file(::std::string_view const& file) noexcept;
         */
    };


    /*

    #ifdef CONFIG_FILE
    #    if CONFIG_FILE != ""
    #        include CONFIG_FILE
    #    else
        extern std::string_view get_static_file(std::string_view const&)
    noexcept; #    endif #endif

        response_t response_t::file(std::filesystem::path const& _file) noexcept
    { response_t res; #ifdef CONFIG_FILE if (auto content =
    ::get_static_file(filepath); !content.empty()) {
                res.body.replace_string(content);
                return res;
            }
    #endif

            // TODO: performance tests
            // TODO: change the replace_string with replace_string_view if the file is cached

            if (std::ifstream in{_file.c_str(), std::ios::binary |
    std::ios::ate}) {
                // details on this matter:
                // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                // std::unique_ptr<char[]> buffer{new char[buffer_size]};
                // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost,
                // I think
                auto                    size = in.tellg();
                std::unique_ptr<char[]> result(new char[size]);
                in.seekg(0);
                in.read(result.get(), size);
                res.body.replace_string(std::string{
                  result.get(),
    static_cast<std::string_view::size_type>(size)});
            }

            return res;
        }
    */

} // namespace webpp
#endif // WEBPP_HTTP_RESPONSE_H
