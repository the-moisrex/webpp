#ifndef WEBPP_HTTP_FILE_H
#define WEBPP_HTTP_FILE_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits.hpp"
#include "./string.hpp"

#include <filesystem>
#include <fstream>

#ifdef WEBPP_EMBEDDED_FILES
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    else
extern std::string_view get_static_file(std::string_view const&) noexcept;
#    endif
#endif

namespace webpp::http {

    struct file_options {
        bool     cache          = true;
        bool     retry          = true;
        unsigned retry_count    = 2;
        void*    global_storage = nullptr; // todo
    };

    namespace details {

        struct file_context_extension {

            template <Traits TraitsType, typename Mother>
            struct type : public Mother {
                using traits_type   = TraitsType;
                using response_type = typename Mother::response_type;
                using body_type     = typename response_type::body_type;
                using string_type   = typename body_type::string_type;
                using char_type     = traits::char_type<traits_type>;
                using ifstream_type = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

                // ctor
                using Mother::Mother;


              public:
                response_type file(stl::filesystem::path const& filepath,
                                   file_options const&          options = {}) noexcept {
#ifdef WEBPP_EMBEDDED_FILES
                    if (auto content = ::get_static_file(filepath); !content.empty()) {
                        return string_type{this->content, alloc};
                    }
#endif

                    // todo: cache

                    // TODO: performance tests
                    // todo: add unix specializations for performance and having fun reasons
                    // TODO: change the replace_string with replace_string_view if the file is cached

                    if (auto in = ifstream_type(filepath.c_str(), stl::ios::binary | stl::ios::ate);
                        in.is_open()) {
                        // details on this matter:
                        // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                        // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                        // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I think
                        auto                         size = in.tellg();
                        stl::unique_ptr<char_type[]> result(this->get_allocator().allocate(size));
                        in.seekg(0);
                        in.read(result.get(), size);
                        // todo: cache the results
                        return body_type{string_type{result.get(),
                                                     static_cast<stl::string_view::size_type>(size),
                                                     this->get_allocator()}};
                    } else {
                        this->logger.error("Response/File",
                                           "Cannot load the specified file: %s",
                                           filepath.string());
                        // todo: retry feature
                        if constexpr (this->is_debug()) {
                            return this->error(500u);
                        } else {
                            return this->error(500u,
                                               stl::format("We're not able to load the specified file: {}",
                                                           filepath.string()));
                        }
                    }
                }
            };
        };
    } // namespace details


    struct file_response {
        using response_body_extensions = extension_pack<details::string_response_body_extension>;
        using context_extensions       = extension_pack<details::file_context_extension>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_FILE_H
