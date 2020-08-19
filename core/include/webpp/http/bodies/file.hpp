#ifndef WEBPP_HTTP_FILE_H
#define WEBPP_HTTP_FILE_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits_concepts.hpp"

#include <filesystem>

#ifdef WEBPP_EMBEDDED_FILES
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    else
extern std::string_view get_static_file(std::string_view const&) noexcept;
#    endif
#endif

namespace webpp {

    struct file_body {
        template <Traits TraitsType>
        struct type {
            using traits_type      = TraitsType;
            using string_type      = typename traits_type::string_type;
            using string_view_type = typename traits_type::string_view_type;
            using char_type        = typename string_type::value_type;
            using allocator_type   = typename traits_type::template allocator<char_type>;
            using alloc_type       = allocator_type const&;
            using ifstream_type = typename stl::basic_ifstream<char_type, typename traits_type::char_traits>;

          private:
            string_type     content;
            stl::error_code _error{};

            void load_file(stl::filesystem::path const& filepath, allocator_type alloc) noexcept {
#ifdef WEBPP_EMBEDDED_FILES
                if (auto content = ::get_static_file(filepath); !content.empty()) {
                    content = string_type{content, alloc};
                    _error  = stl::error_code{}; // reset the error_code
                    return;
                }
#endif

                // TODO: performance tests
                // todo: add unix specializations for performance and having fun reasons
                // TODO: change the replace_string with replace_string_view if the file is cached

                if (auto in = ifstream_type{filepath.c_str(), stl::ios::binary | stl::ios::ate};
                    in.is_open()) {
                    // details on this matter:
                    // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                    // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                    // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I think
                    auto                         size = in.tellg();
                    stl::unique_ptr<char_type[]> result(alloc.allocate(size));
                    in.seekg(0);
                    in.read(result.get(), size);
                    // todo: cache the results
                    content =
                      string_type{result.get(), static_cast<stl::string_view::size_type>(size), alloc};
                    return;
                } else {
                    // todo: error code here
                    // todo: retry feature
                }

                return;
            }

          public:
            constexpr type(alloc_type alloc = allocator_type{}) noexcept : content{"", alloc} {}

            constexpr type(string_view_type filename, alloc_type alloc = allocator_type{}) noexcept {
                load_file(filename, alloc);
            }

            // fixme: why? why can't I have non-constexpr constructor?
            //            type(stl::filesystem::path filename, alloc_type alloc = allocator_type{}) noexcept {
            //                load_file(filename, alloc);
            //            }

            constexpr type(type const& fbody) noexcept : content{fbody.content}, _error{fbody._error} {}

            constexpr type(type&& fbody) noexcept
              : content{stl::move(fbody.content)},
                _error{stl::move(fbody._error)} {}

            void load(stl::filesystem::path _file) noexcept {
                load_file(_file, content.get_allocator());
            }

            [[nodiscard]] string_type const& str() const noexcept {
                return content;
            }

            [[nodiscard]] auto const& error() const noexcept {
                return _error;
            }

            [[nodiscard]] bool operator==(string_view_type str) const noexcept {
                return str == content;
            }

            [[nodiscard]] bool operator!=(string_view_type str) const noexcept {
                return str != content;
            }
        };
    };

    template <Traits TraitsType>
    [[nodiscard]] bool operator==(typename TraitsType::string_view_type                str,
                                  typename file_body::template type<TraitsType> const& filebody) noexcept {
        return filebody.str() == str;
    }

    template <Traits TraitsType>
    [[nodiscard]] bool operator!=(typename TraitsType::string_view_type                str,
                                  typename file_body::template type<TraitsType> const& filebody) noexcept {
        return filebody.str() != str;
    }

    struct file_response {
        using response_body_extensions = extension_pack<file_body>;
    };


} // namespace webpp

#endif // FILE_H
