#ifndef WEBPP_STORAGE_EMBEDDED_FILE_HPP
#define WEBPP_STORAGE_EMBEDDED_FILE_HPP

#include <filesystem>
#include <optional>
#include <string_view>

#ifdef WEBPP_EMBEDDED_FILES
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    endif
#endif

namespace webpp {

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
        constexpr embedded_file(string_view_type const file_path, string_view_type const content) noexcept
          : _file_path(file_path),
            _content(content) {}

        [[nodiscard]] constexpr string_view_type path() const noexcept {
            return _file_path;
        }

        [[nodiscard]] constexpr string_view_type content() const noexcept {
            return _content;
        }

        [[nodiscard]] static constexpr std::optional<embedded_file> search(
          [[maybe_unused]] string_view_type filepath) noexcept {
#ifdef WEBPP_EMBEDDED_FILES
            for (auto const& [file, content] : webpp_embedded_files) {
                if (filepath == file) {
                    return content;
                }
            }
#endif
            return std::nullopt;
        }


#ifdef WEBPP_EMBEDDED_FILES
        [[nodiscard]] static std::optional<embedded_file> search(std::filesystem::path const& filepath) {
            auto const filename = filepath.filename().string();
            return search(string_view_type{filename.data(), filename.size()});
        }
#else
        [[nodiscard]] static constexpr std::optional<embedded_file> search(
          [[maybe_unused]] std::filesystem::path const& inp_path) noexcept {
            return std::nullopt;
        }
#endif
    };

} // namespace webpp

#endif // WEBPP_STORAGE_EMBEDDED_FILE_HPP
