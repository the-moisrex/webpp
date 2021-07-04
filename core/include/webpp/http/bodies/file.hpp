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

    namespace details {
        struct file_body_extension {

            using dependencies = extension_pack<string_response_body_extension>;

            template <Traits TraitsType, typename StringBody>
            struct type : StringBody {
              private:
                using super = StringBody;

              public:
                using traits_type      = TraitsType;
                using string_view_type = typename super::string_view_type;
                using allocator_type   = typename super::allocator_type;
                using string_type      = typename super::string_type;
                using alloc_type       = allocator_type const&;
                using char_type        = typename super::char_type;
                using ifstream_type    = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

              private:
                stl::error_code _error{};

                void load_file(stl::filesystem::path const& filepath) noexcept {
#ifdef WEBPP_EMBEDDED_FILES
                    if (auto content = ::get_static_file(filepath); !content.empty()) {
                        this->content = string_type{this->content, alloc};
                        _error        = stl::error_code{}; // reset the error_code
                        return;
                    }
#endif

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
                        this->content = string_type{result.get(),
                                                    static_cast<stl::string_view::size_type>(size),
                                                    this->get_allocator()};
                        return;
                    } else {
                        // todo: error code here
                        // todo: retry feature
                    }
                }

              public:
                constexpr type(alloc_type alloc = allocator_type{}) noexcept : super{"", alloc} {}

                constexpr type(string_view_type filename, alloc_type alloc = allocator_type{}) noexcept
                  : super{"", alloc} {
                    load_file(filename);
                }

                // fixme: why? why can't I have non-constexpr constructor?
                //            type(stl::filesystem::path filename, alloc_type alloc = allocator_type{})
                //            noexcept {
                //                load_file(filename, alloc);
                //            }

                constexpr type(type const& fbody) noexcept : super{fbody}, _error{fbody._error} {}

                constexpr type(type&& fbody) noexcept
                  : super{stl::move(fbody)},
                    _error{stl::move(fbody._error)} {}

                void load(stl::filesystem::path _file) noexcept {
                    load_file(_file, this->content.get_allocator());
                }

                [[nodiscard]] auto const& error() const noexcept {
                    return _error;
                }

                [[nodiscard]] bool operator==(string_view_type str) const noexcept {
                    return str == this->content;
                }

                [[nodiscard]] bool operator!=(string_view_type str) const noexcept {
                    return str != this->content;
                }
            };
        };


        struct file_context_extension {

            template <Traits TraitsType, typename Mother>
            struct type : public Mother {
                using response_type = typename Mother::response_type;

                // ctor
                using Mother::Mother;


                response_type file(stl::filesystem::path _file) noexcept {
                    return response_type{_file};
                }
            };
        };
    } // namespace details




    struct file_response {
        using dependencies             = extension_pack<string_response>;
        using response_body_extensions = extension_pack<details::file_body_extension>;
        using context_extensions       = extension_pack<details::file_context_extension>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_FILE_H
