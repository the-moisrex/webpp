// Created by moisrex on 5/10/20.

#ifndef WEBPP_EXTENSION_PRIORITY_H
#define WEBPP_EXTENSION_PRIORITY_H

namespace webpp::http::inline extensions {

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
        [[nodiscard]] constexpr auto level() const noexcept {
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

        [[nodiscard]] constexpr bool operator<(priority __level) const noexcept {
            return this->_level < __level.level();
        }

        [[nodiscard]] constexpr bool operator>(priority __level) const noexcept {
            return this->_level > __level.level();
        }

        [[nodiscard]] constexpr bool operator<=(priority __level) const noexcept {
            return this->_level <= __level.level();
        }

        [[nodiscard]] constexpr bool operator>=(priority __level) const noexcept {
            return this->_level >= __level.level();
        }

        [[nodiscard]] constexpr bool operator!=(priority __level) const noexcept {
            return this->_level != __level.level();
        }

        [[nodiscard]] constexpr bool operator==(priority __level) const noexcept {
            return this->_level == __level.level();
        }

        ///////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr bool operator<(decltype(_level) __level) const noexcept {
            return this->_level < __level;
        }

        [[nodiscard]] constexpr bool operator>(decltype(_level) __level) const noexcept {
            return this->_level > __level;
        }

        [[nodiscard]] constexpr bool operator<=(decltype(_level) __level) const noexcept {
            return this->_level <= __level;
        }

        [[nodiscard]] constexpr bool operator>=(decltype(_level) __level) const noexcept {
            return this->_level >= __level;
        }

        [[nodiscard]] constexpr bool operator!=(decltype(_level) __level) const noexcept {
            return this->_level != __level;
        }

        [[nodiscard]] constexpr bool operator==(decltype(_level) __level) const noexcept {
            return this->_level == __level;
        }
    };



    [[nodiscard]] constexpr bool operator<(uint_fast8_t lhs, priority rhs) noexcept {
        return lhs < rhs.level();
    }

    [[nodiscard]] constexpr bool operator>(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs > rhs.level();
    }

    [[nodiscard]] constexpr bool operator<=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs <= rhs.level();
    }

    [[nodiscard]] constexpr bool operator>=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs >= rhs.level();
    }

    [[nodiscard]] constexpr bool operator!=(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs != rhs.level();
    }

    [[nodiscard]] constexpr bool operator==(uint_fast16_t lhs, priority rhs) noexcept {
        return lhs == rhs.level();
    }


    struct ex_priority {

        // this madness just fills the array with this: {0, 1, 2, 3, ..., N}
        ::std::array<::std::size_t, sizeof...(RouteType)> priorities =
          ([]<::std::size_t... I>(::std::index_sequence<I...>) {
              return ::std::array<::std::size_t, sizeof...(I)>{I...};
          }) (::std::make_index_sequence<sizeof...(RouteType)>());


        struct iterator {
            using value_type = const_router<RouteType...>;

          private:
            using routes_ptr_t     = decltype(routes)*;
            using priorities_ptr_t = decltype(priorities)*;

            routes_ptr_t     routes_ptr;
            priorities_ptr_t priorities_ptr;
            ::std::size_t    index;

          public:
            constexpr iterator(routes_ptr_t     _routes_ptr     = nullptr,
                               priorities_ptr_t _priorities_ptr = nullptr,
                               ::std::size_t    _index          = sizeof...(RouteType)) noexcept
              : routes_ptr{_routes_ptr},
                priorities_ptr{_priorities_ptr},
                index(_index) {}

            constexpr iterator(iterator const& iter) noexcept
              : routes_ptr{iter.routes_ptr},
                priorities_ptr{iter.priorities_ptr},
                index{iter.index} {}

            auto& operator=(iterator const& iter) noexcept {
                if (iter != *this) {
                    routes_ptr     = iter.routes_ptr;
                    priorities_ptr = iter.priorities_ptr;
                    index          = iter.index;
                }
                return *this;
            }

            ~iterator() noexcept {}

            auto& operator++() noexcept {
                ++index;
                return *this;
            }
            auto& operator--() noexcept {
                --index;
                return *this;
            }
            auto& operator->() noexcept {
                return (*routes_ptr)[(*priorities_ptr)[index]];
            }
            auto& operator*() noexcept {
                return &(*routes_ptr)[(*priorities_ptr)[index]];
            }

            constexpr bool operator==(iterator const& iter) const noexcept {
                return routes_ptr == iter.routes_ptr && priorities_ptr == iter.priorities_ptr &&
                       index == iter.index;
            }

            constexpr bool operator!=(iterator const& iter) const noexcept {
                return routes_ptr != iter.routes_ptr || priorities_ptr != iter.priorities_ptr ||
                       index != iter.index;
            }

            constexpr void swap(iterator& iter) noexcept {
                using ::std::swap;
                swap(routes_ptr, iter.routes_ptr);
                swap(priorities_ptr, iter.priorities_ptr);
                swap(index, iter.index);
            }
        };

        constexpr auto begin() const noexcept {
            return iterator(routes, 0);
        }
        constexpr auto end() const noexcept {
            return iterator(routes);
        }
    };

} // namespace webpp::extensions

#endif // WEBPP_EXTENSION_PRIORITY_H
