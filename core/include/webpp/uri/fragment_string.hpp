// Created by moisrex on 11/6/20.

#ifndef WEBPP_FRAGMENTS_STRING_HPP
#define WEBPP_FRAGMENTS_STRING_HPP

namespace webpp::uri {


    struct fragment_string {

        /**
         * @brief get fragment
         */
        [[nodiscard]] str_view_t fragment() const noexcept {
            parse_fragment();
            return substr(fragment_start + 1);
        }

        /**
         * @brief an indication of whether the URI has fragment or not.
         * @return
         */
        [[nodiscard]] bool has_fragment() const noexcept {
            parse_fragment();
            return fragment_start != data.size();
        }

        /**
         * @brief clear the fragment part of the uri
         * @return
         */
        uri_string& clear_fragment() noexcept {
            return fragment({});
        }

    };

}

#endif // WEBPP_FRAGMENTS_STRING_HPP
