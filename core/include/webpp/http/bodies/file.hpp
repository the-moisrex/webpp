#ifndef WEBPP_HTTP_FILE_H
#define WEBPP_HTTP_FILE_H

#include "../../extensions/extension.hpp"
#include "../../memory/object.hpp"
#include "../../traits/traits.hpp"
#include "string.hpp"

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
                response_type file(stl::filesystem::path const&         filepath,
                                   [[maybe_unused]] file_options const& options = {}) noexcept {
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
                        const auto size = in.tellg();
                        // stl::unique_ptr<char_type[]> result(static_cast<char_type*>(
                        //  this->alloc_pack.template local_allocator<char_type[]>().allocate(size)));
                        auto result = object::make_general<string_type>(*this);
                        result.reserve(static_cast<stl::size_t>(size));
                        in.seekg(0);
                        in.read(result.data(), size);
                        // todo: cache the results
                        return response_type{body_type{result}};
                        // return body_type{string_type{result.get(),
                        //                              static_cast<stl::string_view::size_type>(size),
                        //                              this->get_allocator()}};
                    } else {
                        this->logger.error(
                          "Response/File",
                          fmt::format("Cannot load the specified file: {}", filepath.string()));
                        // todo: retry feature
                        if constexpr (Mother::is_debug()) {
                            return this->error(500u);
                        } else {
                            return this->error(500u,
                                               fmt::format("We're not able to load the specified file: {}",
                                                           filepath.string()));
                        }
                    }
                }
            };
        };


        struct response_body_file_extension {

            template <Traits TraitsType, typename Mother>
            struct type : public Mother {
                using traits_type   = TraitsType;
                using body_type     = Mother;
                using string_type   = typename body_type::string_type;
                using char_type     = traits::char_type<traits_type>;
                using ifstream_type = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

                // ctor
                using Mother::Mother;


              public:
                bool load(stl::filesystem::path const&         filepath,
                          [[maybe_unused]] file_options const& options = {}) noexcept {
#ifdef WEBPP_EMBEDDED_FILES
                    if (auto content = ::get_static_file(filepath); !content.empty()) {
                        *this = string_type{this->content, alloc};
                        return true;
                    }
#endif

                    // todo: cache

                    // TODO: performance tests
                    // todo: add unix specializations for performance and having fun reasons
                    // TODO: change the replace_string with replace_string_view if the file is cached
                    // checkout this implementation: https://stackoverflow.com/a/17925143/4987470

                    if (auto in = ifstream_type(filepath.c_str(), stl::ios::binary | stl::ios::ate);
                        in.is_open()) {
                        // details on this matter:
                        // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                        // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                        // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I think
                        const auto size   = in.tellg();
                        auto       result = object::make_general<string_type>(*this);
                        result.reserve(static_cast<stl::size_t>(size));
                        in.seekg(0l);
                        in.read(result.data(), size);
                        // todo: cache the results
                        *this = result;
                        return true;
                    } else {
                        return false;
                    }
                }
            };
        };
    } // namespace details


    struct file_response {
        using response_body_extensions =
          extension_pack<details::string_response_body_extension, details::response_body_file_extension>;
        using context_extensions = extension_pack<details::file_context_extension>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_FILE_H
