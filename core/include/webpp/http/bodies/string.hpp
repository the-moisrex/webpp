// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../memory/object.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string_view.hpp"
#include "../../storage/file.hpp"
#include "../../traits/traits.hpp"
#include "../http_concepts.hpp"
#include "../routes/router_concepts.hpp"
#include "../status_code.hpp"
#include "./custom_body.hpp"

#include <filesystem>
#include <fstream>

namespace webpp::http {


    namespace details {


        template <Traits TraitsType>
        struct string_body_extension {
            using traits_type      = TraitsType;
            using string_view_type = traits::string_view<traits_type>;
            using string_type      = traits::general_string<traits_type>;
            using allocator_type   = typename string_type::allocator_type;
            using char_type        = traits::char_type<traits_type>;
            using ifstream_type    = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

          private:
            // the file extension will use the "content"'s allocator directly
            using alloc_type    = allocator_type const&;
            string_type content = "";

          public:
            constexpr string_body_extension() = default;

            constexpr string_body_extension(string_view_type str, alloc_type alloc = allocator_type{})
              : content{str, alloc} {}

            template <typename... Args>
                requires(sizeof...(Args) > 0 &&
                         requires(Args... args) { string_type{stl::forward<Args>(args)...}; }) // string args
            constexpr string_body_extension(Args&&... args) : content{stl::forward<Args>(args)...} {}


            // load a file
            constexpr string_body_extension(stl::filesystem::path const& file,
                                            alloc_type                   alloc = allocator_type{})
              : content{alloc} {
                static_cast<void>(load(file));
            }

            /**
             * @brief Get a reference to the body's string
             * @return string
             */
            [[nodiscard]] string_type const& string() const noexcept {
                return content;
            }

            template <typename StrType>
                requires(istl::StringifiableOf<string_type, StrType>)
            [[nodiscard]] string_type const& string(StrType&& str) {
                return content.operator=(stl::forward<StrType>(str));
            }

            constexpr operator string_type() const noexcept {
                return content;
            }

            // todo: add "replace_format"
            // todo: add istring member functions to it
            // todo: operator >> and <<

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
            constexpr auto& operator=(Arg&& arg) {
                content.operator=(stl::forward<Arg>(arg));
                return *this;
            }

            // load a file as a string for the body
            bool load(stl::filesystem::path const& filepath) noexcept {
                auto result = object::make_general<string_type>(*this);

                bool const res = file::get_to(filepath, result);
                if (res) {
                    // read the file successfully
                    *this = stl::move(result);
                }
                return res;
            }


            template <typename StrType>
                requires(istl::StringViewifiableOf<string_view_type, StrType>)
            constexpr auto& operator<<(StrType&& str) {
                auto const str_view = istl::string_viewify_of<string_view_type>(stl::forward<StrType>(str));
                content.append(str_view.data(), str_view.size());
                return *this;
            }
        };

        template <Traits TraitsType>
        [[nodiscard]] bool operator==(typename TraitsType::string_view_type    str,
                                      string_body_extension<TraitsType> const& strbody) noexcept {
            return strbody.string() == str;
        }

        template <Traits TraitsType>
        [[nodiscard]] bool operator!=(typename TraitsType::string_view_type    str,
                                      string_body_extension<TraitsType> const& strbody) noexcept {
            return strbody.string() != str;
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

            using context_type::context_type; // inherit the constructors

            template <typename... Args>
            constexpr HTTPResponse auto string(Args&&... args) const {
                // check if there's an allocator in the args:
                constexpr bool has_allocator = (Allocator<Args> || ...);
                if constexpr (!has_allocator && requires {
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

            template <typename StrT, typename... Args>
            constexpr string_type format(StrT&& format_str, Args&&... args) const {
                // todo: it's possible to optimize this for constant expressions
                string_type str{alloc::general_alloc_for<string_type>(*this)};
                fmt::vformat_to(stl::back_inserter(str),
                                istl::to_std_string_view(format_str),
                                fmt::make_format_args(stl::forward<Args>(args)...));
                return str;
            }

            // load a file as a string body and return a response
            [[nodiscard]] response_type file(stl::filesystem::path const& filepath) noexcept {
                auto result = object::make_general<string_type>(*this);

                bool const res = file::get_to(filepath, result);
                if (res) {
                    // read the file successfully
                    return response_type{body_type{result}};
                } else {

                    this->logger.error("Response/File",
                                       fmt::format("Cannot load the specified file: {}", filepath.string()));
                    // todo: retry feature
                    if constexpr (context_type::is_debug()) {
                        return this->error(http::status_code::internal_server_error);
                    } else {
                        return this->error(
                          http::status_code::internal_server_error,
                          fmt::format("We're not able to load the specified file: {}", filepath.string()));
                    }
                }
            }
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     */
    struct string_body {

        // implement TextBasedBodyCommunication concept
        using request_body_extensions  = extension_pack<as_extension<details::string_body_extension>>;
        using response_body_extensions = extension_pack<as_extension<details::string_body_extension>>;

        // Add easy to use member functions to use text as a body and generate text-based responses
        using context_extensions = extension_pack<as_extension<details::string_context_extension>>;
    };





    template <TraitsAccess TraitsType = default_traits>
    struct string_custom_body : public custom_body<string_custom_body<TraitsType>, TraitsType> {
        using custom_body_type = custom_body<string_custom_body, TraitsType>;
        using traits_type      = typename custom_body_type::traits_type;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using allocator_type   = typename string_type::allocator_type;
        using char_type        = traits::char_type<traits_type>;

      private:
        // the file extension will use the "content"'s allocator directly
        using alloc_type = allocator_type const&;
        string_type content{alloc::general_alloc_for<string_type>(*this)};

      public:
        using custom_body<string_custom_body, TraitsType>::custom_body;


        // load a file
        constexpr string_custom_body& operator=(stl::filesystem::path const& file) {
            static_cast<void>(load(file));
            return *this;
        }

        template <typename T>
            requires(istl::StringifiableOf<string_type, T>)
        constexpr string_custom_body& operator=(T&& str) {
            content = istl::stringify_of<string_type>(stl::forward<T>(str), content.get_allocator());
            return *this;
        }

        /**
         * @brief Get a reference to the body's string
         * @return string
         */
        [[nodiscard]] string_type const& string() const noexcept {
            return content;
        }

        template <typename StrType>
            requires(istl::StringifiableOf<string_type, StrType>)
        [[nodiscard]] string_type const& string(StrType&& str) {
            return content.operator=(stl::forward<StrType>(str));
        }

        constexpr operator string_type() const noexcept {
            return content;
        }



        // todo: add "replace_format"
        // todo: add istring member functions to it
        // todo: operator >> and <<

        template <typename StrT>
            requires(istl::StringViewifiableOf<string_view_type, StrT>)
        [[nodiscard]] constexpr bool operator==(StrT&& rhs) const noexcept {
            return content == istl::string_viewify_of<string_view_type>(stl::forward<StrT>(rhs));
        }

        [[nodiscard]] constexpr auto operator==(string_custom_body const& rhs) const noexcept {
            return content == rhs.content;
        }

        [[nodiscard]] constexpr auto operator<=>(string_custom_body const& rhs) const noexcept = default;


        template <typename StrT>
            requires(istl::StringViewifiableOf<string_view_type, StrT>)
        [[nodiscard]] constexpr stl::strong_ordering operator<=>(StrT&& rhs) const noexcept {
            return stl::compare_three_way{}(
              content,
              istl::string_viewify_of<string_view_type>(stl::forward<StrT>(rhs)));
        }


        [[nodiscard]] alloc_type get_allocator() const noexcept {
            return content.get_allocator();
        }

        template <typename Arg>
        constexpr auto& operator=(Arg&& arg) {
            content.operator=(stl::forward<Arg>(arg));
            return *this;
        }

        // load a file as a string for the body
        bool load(stl::filesystem::path const& filepath) noexcept {
            auto result = object::make_general<string_type>(*this);

            bool const res = file::get_to(filepath, result);
            if (res) {
                // read the file successfully
                *this = stl::move(result);
            }
            return res;
        }


        template <typename StrType>
            requires(istl::StringViewifiableOf<string_view_type, StrType>)
        constexpr auto& operator<<(StrType&& str) {
            auto const str_view = istl::string_viewify_of<string_view_type>(stl::forward<StrType>(str));
            content.append(str_view.data(), str_view.size());
            return *this;
        }
    };


    template <EnabledTraits ET>
    string_custom_body(ET&&) -> string_custom_body<ET>;


    template <typename T>
    string_custom_body(T&&) -> string_custom_body<default_traits>;

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_H
