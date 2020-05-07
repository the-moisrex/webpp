#ifndef WEBPP_RESPONSE_H
#define WEBPP_RESPONSE_H

#include "body.h"
#include "header.h"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace webpp {

    /**
     * This class owns its data.
     */
    template <typename Traits = std_traits>
    class response_t {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");

      public:
        using traits     = Traits;
        using body_t     = webpp::body;
        using header_t   = webpp::headers<Traits, true, header_type::response>;
        using str_view_t = typename traits::string_view_type;
        using str_t      = typename traits::string_type;

        body_t   body;
        header_t header;

        response_t() noexcept                      = default;
        response_t(response_t const& res) noexcept = default;
        response_t(response_t&& res) noexcept      = default;
        response_t(str_t const& b) noexcept : body(b) {
        }
        response_t(str_t&& b) noexcept : body(std::move(b)) {
        }


        response_t& operator=(response_t const&) = default;
        response_t& operator=(response_t&& res) noexcept = default;
        response_t& operator=(str_t const& str) noexcept {
            body.replace_string(str);
            return *this;
        }
        response_t& operator=(str_t&& str) noexcept {
            body.replace_string(std::move(str));
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
                  std::to_string(body.str().size() * sizeof(char)));
        }


        // static methods:
        static response_t file(std::filesystem::path const& file) noexcept;
        static response_t image(std::string_view const& file) noexcept;
        static response_t json_file(std::string_view const& file) noexcept;
    };


    template <typename ResponseType, typename = void>
    struct is_response : std::false_type {};

    template <typename ResponseType>
    struct is_response<
      ResponseType,
      std::void_t<
        typename ResponseType::body_t, typename ResponseType::header_t,
        typename ResponseType::traits, typename ResponseType::str_t,
        typename ResponseType::str_view_t,
        decltype(
          std::declval<ResponseType>().body,
          std::declval<ResponseType>().header,
          std::declval<ResponseType>().operator typename ResponseType::str_t(),
          std::declval<ResponseType>().
          operator typename ResponseType::str_view_t(),
          (void)0)>> : std::true_type {};

    template <typename ResponseType>
    constexpr bool is_response_v = is_response<ResponseType>::value;

    template <typename ResType>
    concept Response = is_response_v<ResType>;

} // namespace webpp
#endif // WEBPP_RESPONSE_H
