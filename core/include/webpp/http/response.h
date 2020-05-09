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
    class response_t {

      public:
        using traits_t = TraitsType;
        using body_t   = webpp::body;
        using header_t =
          webpp::headers<TraitsType, true, header_type::response>;
        using str_view_t = typename traits_t::string_view_type;
        using str_t      = typename traits_t::string_type;

        body_t   body;
        header_t header;

        response_t() noexcept                      = default;
        response_t(response_t const& res) noexcept = default;
        response_t(response_t&& res) noexcept      = default;
        response_t(str_t const& b) noexcept : body(b) {
        }
        response_t(str_t&& b) noexcept : body(::std::move(b)) {
        }


        response_t& operator=(response_t const&) = default;
        response_t& operator=(response_t&& res) noexcept = default;
        response_t& operator=(str_t const& str) noexcept {
            body.replace_string(str);
            return *this;
        }
        response_t& operator=(str_t&& str) noexcept {
            body.replace_string(::std::move(str));
            return *this;
        }

        [[nodiscard]] bool operator==(response_t const& res) const noexcept {
            return body == res.body && header == res.header;
        }
        [[nodiscard]] bool operator!=(response_t const& res) const noexcept {
            return body != res.body || header != res.header;
        }

        response_t& operator<<(str_view_t const& str) noexcept {
            body << str;
            return *this;
        }

        operator str_view_t() const noexcept {
            return body.str();
        }
        operator str_t() const noexcept {
            return str_t{body.str()};
        }

        void calculate_default_headers() noexcept {
            // todo: use C++20 header.contains instead when possible
            if (header.find("Content-Type") == header.cend())
                header.emplace("Content-Type", "text/html; charset=utf-8");

            if (header.find("Content-Length") == header.cend())
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
