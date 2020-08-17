// Created by moisrex on 8/16/20.

#ifndef WEBPP_FILE_LOGGER_HPP
#define WEBPP_FILE_LOGGER_HPP

#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../traits/traits_concepts.hpp"


namespace webpp {

    struct file_logger {
        using logger_type = file_logger;
        using logger_ref  = file_logger const&;
        using logger_ptr  = file_logger*;


        void log() noexcept {

        }

    };

} // namespace webpp

#endif // WEBPP_FILE_LOGGER_HPP
