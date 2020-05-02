// Created by moisrex on 5/1/20.

#ifndef WEBPP_PRIORITY_H
#define WEBPP_PRIORITY_H

namespace webpp::routes {

    /**
     * This struct is designed to be used in the routing system for the purpose
     * of adding priority capabilities to the routing system.
     *
     * Ordering system:
     *   - Full scale     : 0 - max(uint16_fast_t)
     *   - Low point      : 1024
     *   - High point     : 10240
     *   - Middle point   : 4608
     * The reason for this strange numbers is that the user should not usually
     * mess around these numbers and should only use automatic or relative
     * ordering and not the manual ways.
     *
     * Entry level route prioritization features:
     *   - [ ] Auto prioritization based on:
     *     - [ ] Order that they've been added (default)
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
    struct priority {};
} // namespace webpp::routes

#endif // WEBPP_PRIORITY_H
