// Created by moisrex on 8/28/23.

#ifndef WEBPP_PROGRESS_INDICATOR_HPP
#define WEBPP_PROGRESS_INDICATOR_HPP

#include <webpp/std/string.hpp>
#include <webpp/std/unordered_map.hpp>

namespace webpp::sdk {

    struct progress_status {
        stl::string description;
        int         percent;
    };

    /**
     * Shows one single progress bar
     */
    struct progress_indicator {
        virtual void update(stl::string_view desc, int percent);
        void         update(int percent);
        void         update(stl::string_view desc);
    };

    /**
     * Provides multiple progress bars and handles all of them together
     */
    struct progress_bars {
        virtual void update(int id, stl::string_view desc, int percent);
        void         update(int id, stl::string_view desc);
        void         update(int id, int percent);

      private:
        stl::unordered_map<int, progress_indicator> bars;
    };

} // namespace webpp::sdk

#endif // WEBPP_PROGRESS_INDICATOR_HPP
