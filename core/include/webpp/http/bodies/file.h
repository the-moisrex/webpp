#ifndef WEBPP_HTTP_FILE_H
#define WEBPP_HTTP_FILE_H

#include "../../traits/traits_concepts.h"

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
            using string_view_type = typename traits_type::stirng_view_type;

          private:
            string_type content;

            static string_type load_file(stl::filesystem::path const& filepath) const noexcept {
#ifdef WEBPP_EMBEDDED_FILES
                if (auto content = ::get_static_file(filepath); !content.empty()) {
                    return content;
                }
#endif

                // TODO: performance tests
                // todo: add unix specializations for performance and having fun reasons
                // TODO: change the replace_string with replace_string_view if the file is cached

                if (stl::ifstream in{filepath.c_str(), stl::ios::binary | stl::ios::ate}) {
                    // details on this matter:
                    // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                    // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                    // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I think
                    auto                    size = in.tellg();
                    stl::unique_ptr<char[]> result(new char[size]);
                    in.seekg(0);
                    in.read(result.get(), size);
                    return string_type{result.get(), static_cast<stl::string_view::size_type>(size)};
                }

                return res;
            }

          public:
            type(stl::filesystem::path filename) noexcept : content{load_file(filename)} {
            }


            [[nodiscard]] string_type const& str() const noexcept {
                return content;
            }
        }
    };

} // namespace webpp

#endif // FILE_H
