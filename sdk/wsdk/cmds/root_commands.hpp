// Created by moisrex on 8/22/23.

#ifndef WEBPP_ROOT_COMMANDS_HPP
#define WEBPP_ROOT_COMMANDS_HPP

#include <webpp/std/memory.hpp>
#include <webpp/strings/string_tokenizer.hpp>

namespace webpp::sdk {

    struct global_options {
        enum verbosity {
            normal,   // default
            moderate, // moderate amount of information
            debug     // all the information you can give
        } verbose = normal;
    };



    static constexpr charset WHITESPACES{' ', '\t', '\n', '\r'};

    struct command_options {
        using tokenizer_type = string_tokenizer<>;

        command_options(stl::string_view command) : m_tokenizer{command} {}
        command_options(command_options&&) noexcept            = default;
        command_options(command_options const&)                = delete;
        command_options& operator=(command_options const&)     = delete;
        command_options& operator=(command_options&&) noexcept = default;
        ~command_options()                                     = default;

        [[nodiscard]] global_options& options() noexcept {
            return m_options;
        }

        [[nodiscard]] tokenizer_type& tokenizer() noexcept {
            return m_tokenizer;
        }



      private:
        global_options m_options;
        tokenizer_type m_tokenizer;
    };

} // namespace webpp::sdk

#endif // WEBPP_ROOT_COMMANDS_HPP
