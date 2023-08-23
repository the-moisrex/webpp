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

    struct command_options {
        command_options() = default;

      private:
        stl::shared_ptr<global_options> options;
    };

} // namespace webpp::sdk

#endif // WEBPP_ROOT_COMMANDS_HPP
