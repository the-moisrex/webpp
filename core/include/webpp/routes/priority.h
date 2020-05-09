// Created by moisrex on 5/1/20.

#ifndef WEBPP_PRIORITY_H
#define WEBPP_PRIORITY_H

#include <cstdint>

namespace webpp::routes {

    /**
     * This struct is designed to be used in the routing system for the purpose
     * of adding priority capabilities to the routing system.
     *
     * leveling system:
     *   - Full scale     : 0 - max(uint16_fast_t)
     *   - Low point      : 1024
     *   - High point     : 10240
     *   - Middle point   : 4608
     * The reason for this strange numbers is that the user should not usually
     * mess around these numbers and should only use automatic or relative
     * leveling and not the manual ways.
     *
     * Entry level route prioritization features:
     *   - [ ] Auto prioritization based on:
     *     - [ ] level that they've been added (default)
     *     - [ ] Stats
     *   - [ ] Manual prioritization
     *     - [ ] Manual numbering
     *   - [ ] Relative prioritization
     *     - [ ] Possible hints
     *       - [ ] Higher than
     *       - [ ] Lower than
     *       - [ ] Equal to
     *     - [ ] Relative to the highest
     *     - [ ] Relative to the lowest
     *     - [ ] Relative to the middle
     *   - [ ] On-The-Fly Re-Prioritization
     */
    struct priority {
      protected:
        uint_fast16_t _level;

      public:
        constexpr auto level() const noexcept {
            return _level;
        }

        /**
         * Changing the level manually. Try not to use this! Use automatic or
         * relative leveling.
         * @param _new_level
         */
        void level(decltype(_level) _new_level) noexcept {
            _level = _new_level;
        }

        constexpr bool operator<(priority __level) const noexcept {
            return this->_level < __level.level();
        }

        constexpr bool operator>(priority __level) const noexcept {
            return this->_level > __level.level();
        }

        constexpr bool operator<=(priority __level) const noexcept {
            return this->_level <= __level.level();
        }

        constexpr bool operator>=(priority __level) const noexcept {
            return this->_level >= __level.level();
        }

        constexpr bool operator!=(priority __level) const noexcept {
            return this->_level != __level.level();
        }

        constexpr bool operator==(priority __level) const noexcept {
            return this->_level == __level.level();
        }

        ///////////////////////////////////////////////////////////////////////

        constexpr bool operator<(decltype(_level) __level) const noexcept {
            return this->_level < __level;
        }

        constexpr bool operator>(decltype(_level) __level) const noexcept {
            return this->_level > __level;
        }

        constexpr bool operator<=(decltype(_level) __level) const noexcept {
            return this->_level <= __level;
        }

        constexpr bool operator>=(decltype(_level) __level) const noexcept {
            return this->_level >= __level;
        }

        constexpr bool operator!=(decltype(_level) __level) const noexcept {
            return this->_level != __level;
        }

        constexpr bool operator==(decltype(_level) __level) const noexcept {
            return this->_level == __level;
        }
    };



    constexpr bool operator<(uint_fast8_t lhs, priority rhs) noexcept {
        return lhs < rhs.level();
    }

    constexpr bool operator>(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs > rhs.level();
    }

    constexpr bool operator<=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs <= rhs.level();
    }

    constexpr bool operator>=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs >= rhs.level();
    }

    constexpr bool operator!=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs != rhs.level();
    }

    constexpr bool operator==(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs == rhs.level();
    }


} // namespace webpp::routes

#endif // WEBPP_PRIORITY_H
