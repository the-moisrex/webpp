#include "validators.h"
#include "../std/string_view.h"
#include "../utils/casts.h"
#include "../utils/charset.h"
#include "../utils/strings.h"
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
            if (is::digit(c)) {
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
            } else if (is::digit(c)) {
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
    // I don't think this way is the best way too. but it's lots of code.
    // So I'll just wait for free time to make it better.
    // TODO

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
            if (is::digit(c)) {
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
            } else if (is::digit(c)) {
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
            } else if (is::digit(c)) {
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
bool is::name_color(std::string str) noexcept {

    // converting to lower case
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    static constexpr auto names = {"algae green",
                                   "aliceblue",
                                   "alien green",
                                   "antiquewhite",
                                   "aquamarine",
                                   "army brown",
                                   "ash gray",
                                   "avocado green",
                                   "aztech purple",
                                   "azure",
                                   "baby blue",
                                   "bashful pink",
                                   "basket ball orange",
                                   "battleship gray",
                                   "bean red",
                                   "bee yellow",
                                   "beer",
                                   "beetle green",
                                   "beige",
                                   "black",
                                   "black cat",
                                   "black cow",
                                   "black eel",
                                   "blanchedalmond",
                                   "blonde",
                                   "blood red",
                                   "blossom pink",
                                   "blue angel",
                                   "blue diamond",
                                   "blue dress",
                                   "blue eyes",
                                   "blue gray",
                                   "blue green",
                                   "blue hosta",
                                   "blue ivy",
                                   "blue jay",
                                   "blue koi",
                                   "blue lagoon",
                                   "blue lotus",
                                   "blue orchid",
                                   "blue ribbon",
                                   "blue whale",
                                   "blue zircon",
                                   "blueberry blue",
                                   "blush pink",
                                   "blush red",
                                   "brass",
                                   "bright gold",
                                   "bright neon pink",
                                   "bronze",
                                   "brown bear",
                                   "brown sugar",
                                   "bullet shell",
                                   "burgundy",
                                   "burlywood",
                                   "burnt pink",
                                   "butterfly blue",
                                   "cadillac pink",
                                   "camel brown",
                                   "camouflage green",
                                   "cantaloupe",
                                   "caramel",
                                   "carbon gray",
                                   "carnation pink",
                                   "celeste",
                                   "champagne",
                                   "charcoal",
                                   "chartreuse",
                                   "cherry red",
                                   "chestnut",
                                   "chestnut red",
                                   "chilli pepper",
                                   "chocolate",
                                   "cinnamon",
                                   "cloudy gray",
                                   "clover green",
                                   "cobalt blue",
                                   "coffee",
                                   "columbia blue",
                                   "construction cone orange",
                                   "cookie brown",
                                   "copper",
                                   "coral",
                                   "coral blue",
                                   "corn yellow",
                                   "cornflower blue",
                                   "cornsilk",
                                   "cotton candy",
                                   "cranberry",
                                   "cream",
                                   "crimson",
                                   "crocus purple",
                                   "crystal blue",
                                   "cyan opaque",
                                   "cyan or aqua",
                                   "dark carnation pink",
                                   "dark forest green",
                                   "dark goldenrod",
                                   "dark orange",
                                   "dark orchid",
                                   "dark salmon",
                                   "dark sea green",
                                   "dark slate blue",
                                   "dark slate grey",
                                   "dark turquoise",
                                   "dark violet",
                                   "day sky blue",
                                   "deep peach",
                                   "deep pink",
                                   "deep sky blue",
                                   "denim blue",
                                   "denim dark blue",
                                   "desert sand",
                                   "dimorphotheca magenta",
                                   "dodger blue",
                                   "dollar bill green",
                                   "dragon green",
                                   "dull purple",
                                   "earth blue",
                                   "eggplant",
                                   "electric blue",
                                   "emerald green",
                                   "fall leaf brown",
                                   "fern green",
                                   "ferrari red",
                                   "fire engine red",
                                   "firebrick",
                                   "flamingo pink",
                                   "forest green",
                                   "frog green",
                                   "ginger brown",
                                   "glacial blue ice",
                                   "golden brown",
                                   "goldenrod",
                                   "granite",
                                   "grape",
                                   "grapefruit",
                                   "gray",
                                   "gray cloud",
                                   "gray dolphin",
                                   "gray goose",
                                   "gray wolf",
                                   "grayish turquoise",
                                   "green",
                                   "green apple",
                                   "green onion",
                                   "green peas",
                                   "green snake",
                                   "green thumb",
                                   "green yellow",
                                   "greenish blue",
                                   "gunmetal",
                                   "halloween orange",
                                   "harvest gold",
                                   "hazel green",
                                   "heliotrope purple",
                                   "hot pink",
                                   "hummingbird green",
                                   "iceberg",
                                   "iguana green",
                                   "indigo",
                                   "iridium",
                                   "jade green",
                                   "jasmine purple",
                                   "jeans blue",
                                   "jellyfish",
                                   "jet gray",
                                   "jungle green",
                                   "kelly green",
                                   "khaki",
                                   "khaki rose",
                                   "lapis blue",
                                   "lava red",
                                   "lavender blue",
                                   "lavender pinocchio",
                                   "lawn green",
                                   "lemon chiffon",
                                   "light aquamarine",
                                   "light blue",
                                   "light coral",
                                   "light cyan",
                                   "light jade",
                                   "light pink",
                                   "light salmon",
                                   "light sea green",
                                   "light sky blue",
                                   "light slate",
                                   "light slate blue",
                                   "light slate gray",
                                   "light steel blue",
                                   "lilac",
                                   "lime green",
                                   "lipstick pink",
                                   "love red",
                                   "lovely purple",
                                   "macaroni and cheese",
                                   "macaw blue green",
                                   "magenta",
                                   "mahogany",
                                   "mango orange",
                                   "marble blue",
                                   "maroon",
                                   "mauve",
                                   "medium aquamarine",
                                   "medium forest green",
                                   "medium orchid",
                                   "medium purple",
                                   "medium sea green",
                                   "medium spring green",
                                   "medium turquoise",
                                   "medium violet red",
                                   "metallic silver",
                                   "midnight",
                                   "midnight blue",
                                   "milk white",
                                   "mint green",
                                   "mist blue",
                                   "misty rose",
                                   "moccasin",
                                   "mocha",
                                   "mustard",
                                   "navy blue",
                                   "nebula green",
                                   "neon pink",
                                   "night",
                                   "northern lights blue",
                                   "oak brown",
                                   "ocean blue",
                                   "oil",
                                   "orange gold",
                                   "orange salmon",
                                   "pale blue lily",
                                   "pale violet red",
                                   "papaya orange",
                                   "parchment",
                                   "pastel blue",
                                   "peach",
                                   "pearl",
                                   "periwinkle",
                                   "pig pink",
                                   "pine green",
                                   "pink",
                                   "pink bow",
                                   "pink bubble gum",
                                   "pink cupcake",
                                   "pink daisy",
                                   "pink lemonade",
                                   "pink rose",
                                   "pistachio green",
                                   "platinum",
                                   "plum",
                                   "plum pie",
                                   "plum purple",
                                   "plum velvet",
                                   "powder blue",
                                   "puce",
                                   "pumpkin orange",
                                   "purple",
                                   "purple amethyst",
                                   "purple daffodil",
                                   "purple dragon",
                                   "purple flower",
                                   "purple haze",
                                   "purple iris",
                                   "purple jam",
                                   "purple mimosa",
                                   "purple monster",
                                   "purple sage bush",
                                   "red",
                                   "red dirt",
                                   "red fox",
                                   "red wine",
                                   "robin egg blue",
                                   "rogue pink",
                                   "rose",
                                   "rose gold",
                                   "rosy brown",
                                   "rosy finch",
                                   "royal blue",
                                   "rubber ducky yellow",
                                   "ruby red",
                                   "rust",
                                   "saffron",
                                   "sage green",
                                   "salad green",
                                   "sand",
                                   "sandstone",
                                   "sandy brown",
                                   "sangria",
                                   "sapphire blue",
                                   "scarlet",
                                   "school bus yellow",
                                   "sea blue",
                                   "sea green",
                                   "sea turtle green",
                                   "seashell",
                                   "seaweed green",
                                   "sedona",
                                   "sepia",
                                   "shamrock green",
                                   "shocking orange",
                                   "sienna",
                                   "silk blue",
                                   "sky blue",
                                   "slate blue",
                                   "slate gray",
                                   "slime green",
                                   "smokey gray",
                                   "spring green",
                                   "steel blue",
                                   "stoplight go green",
                                   "sun yellow",
                                   "sunrise orange",
                                   "tan brown",
                                   "tangerine",
                                   "taupe",
                                   "tea green",
                                   "teal",
                                   "thistle",
                                   "tiffany blue",
                                   "tiger orange",
                                   "tron blue",
                                   "tulip pink",
                                   "turquoise",
                                   "tyrian purple",
                                   "valentine red",
                                   "vampire gray",
                                   "vanilla",
                                   "velvet maroon",
                                   "venom green",
                                   "viola purple",
                                   "violet",
                                   "violet red",
                                   "water",
                                   "watermelon pink",
                                   "white",
                                   "windows blue",
                                   "wisteria purple",
                                   "wood",
                                   "yellow",
                                   "yellow green",
                                   "zombie green"};

    auto it =
        std::lower_bound(std::cbegin(names), std::end(names), str,
                         [](const auto& l, const auto& r) { return l < r; });
    return it != std::end(names) && *it == str;
}

bool is::rgb_color(std::string_view sstr) noexcept {
    // TODO: there are better ways to do it, check performance

    constexpr auto numbers = "0123456789";

    trim(sstr);
    if (!sstr.starts_with("rgb(") || !sstr.starts_with("RGB("))
        return false;
    sstr.remove_prefix(4);
    sstr.remove_suffix(1);
    rtrim(sstr);
    auto it = sstr.find_first_not_of(numbers);
    if (!is::uint8(sstr.substr(0, it)))
        return false;
    sstr.remove_suffix(it);
    ltrim(sstr);
    if (sstr.starts_with(','))
        return false;
    sstr.remove_prefix(1);
    ltrim(sstr);
    it = sstr.find_first_not_of(numbers);
    if (!is::uint8(sstr.substr(0, it)))
        return false;
    sstr.remove_prefix(it);
    ltrim(sstr);
    if (!sstr.starts_with(','))
        return false;
    sstr.remove_prefix(1);
    ltrim(sstr);
    it = sstr.find_first_not_of(numbers);
    if (!is::uint8(sstr.substr(0, it)))
        return false;
    sstr.remove_prefix(it);
    ltrim(sstr);
    return sstr.empty();
}

bool is::rgba_color(std::string_view str) noexcept {
    // TODO: there are better ways to do it, check performance

    constexpr auto numbers = "0123456789";

    return true; // TODO: I'm just gonna make it compilable
}
