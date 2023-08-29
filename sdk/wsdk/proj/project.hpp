// Created by moisrex on 8/29/23.

#ifndef WEBPP_PROJECT_HPP
#define WEBPP_PROJECT_HPP

#include <webpp/std/filesystem.hpp>
#include <webpp/std/memory.hpp>
#include <webpp/std/string_view.hpp>

namespace webpp::sdk {

    struct project {
        project();
        project(project const&)            = delete;
        project& operator=(project const&) = delete;
        project(project&&) noexcept;
        project& operator=(project&&) noexcept;
        ~project();

        /// Get and Set the directory
        [[nodiscard]] fs::path path() const;
        void                   path(fs::path);

        [[nodiscard]] stl::string_view name() const noexcept;
        void                           name(stl::string_view);

      private:
        class impl;
        impl* pimpl;
    };


} // namespace webpp::sdk

#endif // WEBPP_PROJECT_HPP
