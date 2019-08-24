#include "validators.h"
#include "../std/string_view.h"
#include "../utils/charset.h"
#include <string>

using namespace webpp;

/**
 * This function checks to make sure the given string
 * is a valid rendering of an octet as a decimal number.
 *
 * @param[in] octetString
 *     This is the octet string to validate.
 *
 * @return
 *     An indication of whether or not the given astring
 *     is a valid rendering of an octet as a
 *     decimal number is returned.
 */
constexpr inline bool is_ipv4_octet(std::string_view const& octetString) {
    int octet = 0;
    for (auto c : octetString) {
        if (is::digit(c)) {
            octet *= 10;
            octet += static_cast<int>(c - '0');
        } else {
            return false;
        }
    }
    return (octet <= 255);
}

constexpr bool is::ipv4(const std::string_view& address) noexcept {
#define mohammad_implementation
#undef stackoverflow_implemenation
#undef RichardWalters_implementation

#ifdef stackoverflow_implemenation
    // probabely slow and not constexpr compatible
    static const std::regex pattern{"^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
                                    "\\.){3}(25[0-5]|2[0-4][0-9]|[01]"
                                    "?[0-9][0-9]?)$"};
    return std::regex_match(str, pattern);
#endif

#ifdef mohammad_implementation
    std::size_t dot_pos = 0;
    std::size_t i = 0;
    for (auto const& c : address) {
        if (c == '.' || i == address.size() - 1) {
            if (is_ipv4_octet(std::string_view(address.data() + dot_pos + 1,
                                               i - dot_pos)))
                return false;
            dot_pos = i;
        }
        i++;
    }
    return true;
#endif

#ifdef RichardWalters_implementation
    // Not constexpr compatible
    size_t numGroups = 0;
    size_t state = 0;
    std::string octetBuffer;
    for (auto c : address) {
        switch (state) {
        case 0: { // not in an octet yet
            if (DIGIT.contains(c)) {
                octetBuffer.push_back(c);
                state = 1;
            } else {
                return false;
            }
        } break;

        case 1: { // expect a digit or dot
            if (c == '.') {
                if (numGroups++ >= 4) {
                    return false;
                }
                if (!ValidateOctet(octetBuffer)) {
                    return false;
                }
                octetBuffer.clear();
                state = 0;
            } else if (DIGIT.contains(c)) {
                octetBuffer.push_back(c);
            } else {
                return false;
            }
        } break;
        }
    }
    if (!octetBuffer.empty()) {
        ++numGroups;
        if (!ValidateOctet(octetBuffer)) {
            return false;
        }
    }
    return (numGroups == 4);
#endif
}

constexpr bool is::ipv6(std::string_view const& address) noexcept {
    enum class ValidationState {
        NO_GROUPS_YET,
        COLON_BUT_NO_GROUPS_YET,
        AFTER_COLON_EXPECT_GROUP_OR_IPV4,
        IN_GROUP_NOT_IPV4,
        IN_GROUP_COULD_BE_IPV4,
        COLON_AFTER_GROUP,
    } state = ValidationState::NO_GROUPS_YET;
    size_t numGroups = 0;
    size_t numDigits = 0;
    bool doubleColonEncountered = false;
    size_t potentialIpv4AddressStart = 0;
    size_t position = 0;
    bool ipv4AddressEncountered = false;
    for (auto c : address) {
        switch (state) {
        case ValidationState::NO_GROUPS_YET: {
            if (c == ':') {
                state = ValidationState::COLON_BUT_NO_GROUPS_YET;
            } else if (is::digit(c)) {
                potentialIpv4AddressStart = position;
                numDigits = 1;
                state = ValidationState::IN_GROUP_COULD_BE_IPV4;
            } else if (HEXDIG.contains(c)) {
                numDigits = 1;
                state = ValidationState::IN_GROUP_NOT_IPV4;
            } else {
                return false;
            }
        } break;

        case ValidationState::COLON_BUT_NO_GROUPS_YET: {
            if (c == ':') {
                if (doubleColonEncountered) {
                    return false;
                } else {
                    doubleColonEncountered = true;
                    state = ValidationState::AFTER_COLON_EXPECT_GROUP_OR_IPV4;
                }
            } else {
                return false;
            }
        } break;

        case ValidationState::AFTER_COLON_EXPECT_GROUP_OR_IPV4: {
            if (DIGIT.contains(c)) {
                potentialIpv4AddressStart = position;
                if (++numDigits > 4) {
                    return false;
                }
                state = ValidationState::IN_GROUP_COULD_BE_IPV4;
            } else if (HEXDIG.contains(c)) {
                if (++numDigits > 4) {
                    return false;
                }
                state = ValidationState::IN_GROUP_NOT_IPV4;
            } else {
                return false;
            }
        } break;

        case ValidationState::IN_GROUP_NOT_IPV4: {
            if (c == ':') {
                numDigits = 0;
                ++numGroups;
                state = ValidationState::COLON_AFTER_GROUP;
            } else if (HEXDIG.contains(c)) {
                if (++numDigits > 4) {
                    return false;
                }
            } else {
                return false;
            }
        } break;

        case ValidationState::IN_GROUP_COULD_BE_IPV4: {
            if (c == ':') {
                numDigits = 0;
                ++numGroups;
                state = ValidationState::AFTER_COLON_EXPECT_GROUP_OR_IPV4;
            } else if (c == '.') {
                ipv4AddressEncountered = true;
                break;
            } else if (DIGIT.contains(c)) {
                if (++numDigits > 4) {
                    return false;
                }
            } else if (HEXDIG.contains(c)) {
                if (++numDigits > 4) {
                    return false;
                }
                state = ValidationState::IN_GROUP_NOT_IPV4;
            } else {
                return false;
            }
        } break;

        case ValidationState::COLON_AFTER_GROUP: {
            if (c == ':') {
                if (doubleColonEncountered) {
                    return false;
                } else {
                    doubleColonEncountered = true;
                    state = ValidationState::AFTER_COLON_EXPECT_GROUP_OR_IPV4;
                }
            } else if (DIGIT.contains(c)) {
                potentialIpv4AddressStart = position;
                ++numDigits;
                state = ValidationState::IN_GROUP_COULD_BE_IPV4;
            } else if (HEXDIG.contains(c)) {
                ++numDigits;
                state = ValidationState::IN_GROUP_NOT_IPV4;
            } else {
                return false;
            }
        } break;
        }
        if (ipv4AddressEncountered) {
            break;
        }
        ++position;
    }
    if ((state == ValidationState::IN_GROUP_NOT_IPV4) ||
        (state == ValidationState::IN_GROUP_COULD_BE_IPV4)) {
        // count trailing group
        ++numGroups;
    }
    if ((position == address.length()) &&
        ((state == ValidationState::COLON_BUT_NO_GROUPS_YET) ||
         (state == ValidationState::AFTER_COLON_EXPECT_GROUP_OR_IPV4) ||
         (state == ValidationState::COLON_AFTER_GROUP))) { // trailing
                                                           // single colon
        return false;
    }
    if (ipv4AddressEncountered) {
        if (!is::ipv4(address.substr(potentialIpv4AddressStart))) {
            return false;
        }
        numGroups += 2;
    }
    if (doubleColonEncountered) {
        // A double colon matches one or more groups (of 0).
        return (numGroups <= 7);
    } else {
        return (numGroups == 8);
    }
}
