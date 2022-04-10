// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../memory/object.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string_view.hpp"
#include "../../traits/traits.hpp"
#include "../response_concepts.hpp"
#include "../routes/context_concepts.hpp"

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


        template <Traits TraitsType>
        struct string_response_body_extension {
            using traits_type      = TraitsType;
            using string_view_type = traits::string_view<traits_type>;
            using string_type      = traits::general_string<traits_type>;
            using allocator_type   = typename string_type::allocator_type;
            using char_type        = traits::char_type<traits_type>;
            using ifstream_type    = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

          protected: // the file extension will use the "content" directly
            using alloc_type    = allocator_type const&;
            string_type content = "";

          public:
            constexpr string_response_body_extension() = default;

            constexpr string_response_body_extension(string_view_type str,
                                                     alloc_type       alloc = allocator_type{})
              : content{str, alloc} {}

            template <typename... Args>
                requires(sizeof...(Args) > 0 &&
                         requires(Args... args) { string_type{stl::forward<Args>(args)...}; }) // string args
            constexpr string_response_body_extension(Args&&... args) : content{stl::forward<Args>(args)...} {}

            /**
             * @brief Get a reference to the body's string
             * @return string
             */
            [[nodiscard]] string_type const& str() const noexcept {
                return content;
            }

            constexpr operator string_type() const noexcept {
                return content;
            }

            [[nodiscard]] bool operator==(string_view_type str) const noexcept {
                return str == content;
            }

            [[nodiscard]] bool operator!=(string_view_type str) const noexcept {
                return str != content;
            }

            [[nodiscard]] auto get_allocator() const noexcept {
                return content.get_allocator();
            }

            template <typename Arg>
            constexpr auto& operator=(Arg arg) {
                content.operator=(stl::forward<Arg>(arg));
                return *this;
            }




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
                    const auto        size          = in.tellg();
                    const stl::size_t reserved_size = static_cast<stl::size_t>(size);
                    auto              result        = object::make_general<string_type>(*this);
                    result.resize(reserved_size);
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

        template <Traits TraitsType>
        [[nodiscard]] bool operator==(typename TraitsType::string_view_type             str,
                                      string_response_body_extension<TraitsType> const& strbody) noexcept {
            return strbody.str() == str;
        }

        template <Traits TraitsType>
        [[nodiscard]] bool operator!=(typename TraitsType::string_view_type             str,
                                      string_response_body_extension<TraitsType> const& strbody) noexcept {
            return strbody.str() != str;
        }

        /**
         * This extension helps the user to create a response with the help of the context
         *
         *   ctx.string_type{"this is a response"}
         *   ctx.str_t{"this is nice"}
         *   ctx.string("hello world")
         *
         * The reason for preferring "string" over "string_type" is that the allocator is handled correctly.
         */

        template <Traits TraitsType, Context ContextType>
        struct string_context_extension : public ContextType {
            using context_type  = ContextType;
            using traits_type   = TraitsType;
            using response_type = typename context_type::response_type;
            using body_type     = typename response_type::body_type;
            using char_type     = traits::char_type<traits_type>;
            using string_type   = typename body_type::string_type;
            using ifstream_type = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;
            // ::template apply_extensions_type<details::string_response_body_extension>;

            using context_type::context_type; // inherit the constructors

            template <typename... Args>
            constexpr HTTPResponse auto string(Args&&... args) const {
                // check if there's an allocator in the args:
                constexpr bool has_allocator = (istl::Allocator<Args> || ...);
                if constexpr (!has_allocator &&
                              requires {
                                  response_type::with_body(
                                    stl::forward<Args>(args)...,
                                    this->alloc_pack.template general_allocator<char_type>());
                              }) {
                    return response_type::with_body(stl::forward<Args>(args)...,
                                                    this->alloc_pack.template general_allocator<char_type>());
                } else {
                    return response_type::with_body(stl::forward<Args>(args)...);
                }
            }



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
                    // stl::unique_ptr<char_type[]> result(static_cast<char_type*>(
                    //  this->alloc_pack.template local_allocator<char_type[]>().allocate(size)));
                    auto result = object::make_general<string_type>(*this);
                    in.seekg(0, in.end);
                    const auto size = in.tellg();
                    result.resize(static_cast<stl::size_t>(
                      size)); // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                    in.seekg(0);
                    in.read(result.data(), size);
                    // todo: cache the results
                    return response_type{body_type{result}};
                    // return body_type{string_type{result.get(),
                    //                              static_cast<stl::string_view::size_type>(size),
                    //                              this->get_allocator()}};
                } else {
                    this->logger.error("Response/File",
                                       fmt::format("Cannot load the specified file: {}", filepath.string()));
                    // todo: retry feature
                    if constexpr (context_type::is_debug()) {
                        return this->error(500u);
                    } else {
                        return this->error(
                          500u,
                          fmt::format("We're not able to load the specified file: {}", filepath.string()));
                    }
                }
            }
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     *
     * This includes these extensions:
     *   - response body    : 1 extension (adds .str())
     *   - response         : 1 extension (adds string_view support to response)
     *   - context          : 1 extension (adds .string(...))
     */
    struct string_response {
        using response_body_extensions =
          extension_pack<as_extension<details::string_response_body_extension>>;
        using context_extensions = extension_pack<as_extension<details::string_context_extension>>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_H
