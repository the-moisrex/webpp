#ifndef WEBPP_STORAGE_FILE_HPP
#define WEBPP_STORAGE_FILE_HPP

#include <array>
#include <exception>
#include <optional>
#include <string_view>
#include <type_traits>

/**
 * @brief The file class
 * @details This class is designed for constant files that will be produced from
 * source files on build-time. This class's only purpose is to work at compile-time.
 */
class embedded_file {
  public:
    using string_view_type = std::string_view;

  private:
    string_view_type _file_path;
    string_view_type _content;

  public:
    constexpr embedded_file(string_view_type file_path, string_view_type content) noexcept
      : _file_path(file_path),
        _content(content) {}

    [[nodiscard]] constexpr string_view_type path() const noexcept {
        return _file_path;
    }

    [[nodiscard]] constexpr string_view_type content() const noexcept {
        return _content;
    }

    [[nodiscard]] static constexpr std::optional<embedded_file> search(string_view_type path) noexcept {
        constexpr std::array<embedded_file, 2> files{embedded_file("file.json", "{\"value\": 10}"),
                                                     embedded_file("config.json", "{}")};

        for (auto const& f : files)
            if (f.path() == path)
                return f;

        return std::nullopt;
    }
};

#endif // WEBPP_STORAGE_FILE_HPP
