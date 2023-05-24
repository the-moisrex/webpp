#ifndef WEBPP_VALIDATION_H
#define WEBPP_VALIDATION_H

#include "../convert/casts.hpp"
#include "../libs/ctre.hpp"
#include "../std/concepts.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/to_case.hpp"
#include "../strings/validators.hpp"

#include <algorithm>

namespace webpp::is {

    /**
     * @brief check if these two are equal
     */
    template <typename T1, typename T2>
    [[nodiscard]] constexpr bool equals(T1 const& first, T2 const& second) noexcept {
        return first == second;
    }

    /**
     * @brief check if str contains seed
     * @param str
     * @param seed
     * @return true if it does contain it
     */

    [[nodiscard]] constexpr bool contains(istl::StringViewifiable auto&& _str,
                                          istl::StringViewifiable auto&& _seed) noexcept {
        auto str  = istl::string_viewify(_str);
        auto seed = istl::string_viewify(_seed);
        return str.find(seed) != decltype(str)::npos;
    }

    /**
     * @brief check if the container contains value
     */
    template <typename T>
    [[nodiscard]] constexpr bool contains(stl::initializer_list<T> const& container,
                                          T const&                        value) noexcept {
        return stl::find(stl::cbegin(container), stl::cend(container), value) != stl::cend(container);
    }

    /**
     * @brief check if the container contains key
     */
    template <template <class, class...> class Container, class T1, class... Args>
    [[nodiscard]] constexpr bool contains_key(Container<T1, Args...> const& container,
                                              T1 const&                     key) noexcept {
        return container.find(key) != stl::end(container);
    }

    /**
     * @brief check if the container contains the value
     */
    template <template <class, class, class...> class Container, class T1, class T2, class... Args>
    [[nodiscard]] constexpr bool contains_value(Container<T1, T2, Args...> const& container,
                                                T2 const&                         value) noexcept {
        for (auto pair : container)
            if (pair.second == value)
                return true;
        return false;
    }
    /**
     * @brief check if the specified str is an email or not
     * @param str
     * @return true if the specified str is an email
     */
    [[nodiscard]] bool email(istl::StringViewifiable auto&& _str) noexcept {
        const auto str = istl::string_viewify(stl::forward<decltype(_str)>(_str));
        // constexpr auto pattern =
        //   ctll::fixed_string{R"regex(^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+)regex"};
        // constexpr auto pattern =
        // ctll::fixed_string{"^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+"
        //                                             "(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$"};
        // return ctre::match<pattern>(str);
        return false;
    }


    [[nodiscard]] constexpr bool FQDN(istl::StringViewifiable auto&& _str) noexcept;


    [[nodiscard]] constexpr bool url(istl::StringViewifiable auto&& _str) noexcept;

    /**
     * @brief checks if an string is a valid host based on RFC3986
     * (https://tools.ietf.org/html/rfc3986)
     * @param str
     * @return
     */
    [[nodiscard]] constexpr bool host(istl::StringViewifiable auto&& _str) noexcept {
        auto str = istl::string_viewify(_str);

        using str_view_t = decltype(str);
        using char_type  = typename str_view_t::value_type;

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        constexpr auto UNRESERVED =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        constexpr charset<char_type, 11> SUB_DELIMS{'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};

        /**
         * This is the character set corresponds to the last part of
         * the "IPvFuture" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        constexpr auto IPV_FUTURE_LAST_PART = charset(UNRESERVED, SUB_DELIMS, charset<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the "reg-name" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        constexpr auto REG_NAME_NOT_PCT_ENCODED = charset(UNRESERVED, SUB_DELIMS);

        if (str.empty())
            return false;
        if (ascii::starts_with(str, '[') && ascii::ends_with(str, ']')) {
            if (str[1] == 'v') { // future ip
                if (auto dot_delim = str.find('.'); dot_delim != str_view_t::npos) {
                    auto ipvf_version = str.substr(2, dot_delim);
                    if (!HEXDIG<char_type>.contains(ipvf_version)) {
                        // ERROR: basic_uri is not valid
                        return false;
                    }

                    auto ipvf = str.substr(dot_delim + 1, str.size() - 2);
                    return IPV_FUTURE_LAST_PART.contains(ipvf);
                }

            } else { // ipv6
                return is::ipv6(str.substr(1, str.size() - 2));
            }
        } else if (ascii::is::digit(str[0]) && is::ipv4(str)) { // ipv4
            return true;
        } else {
            constexpr auto ccc = charset(REG_NAME_NOT_PCT_ENCODED, charset<char_type, 1>({'%'}));
            return ccc.contains(str);
        }

        return false;
    }

    /*
     * Check if the specified string is a query (in URI) or not
     * @return bool true if it's a query
     */

    [[nodiscard]] constexpr bool query(istl::StringViewifiable auto&& _str) noexcept {
        auto str        = istl::string_viewify(_str);
        using char_type = istl::char_type_of_t<decltype(str)>;

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        constexpr auto UNRESERVED =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        constexpr charset<char_type, 11> SUB_DELIMS{'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};
        /**
         * This is the character set corresponds to the "pchar" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        constexpr auto PCHAR_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, charset<char_type, 2>{':', '@'});

        /**
         * This is the character set corresponds to the "query" syntax
         * and the "fragment" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
          charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 2>{'/', '?'});

        return QUERY_OR_FRAGMENT_NOT_PCT_ENCODED.contains(str);
    }


    [[nodiscard]] constexpr bool ip_range(istl::StringViewifiable auto&& _str) noexcept;


    [[nodiscard]] constexpr bool ipv4_range(istl::StringViewifiable auto&& _str) noexcept;


    [[nodiscard]] constexpr bool ipv6_range(istl::StringViewifiable auto&& _str) noexcept;
    // bool isImage(something) noexcept;

    /**
     * Check if the specified string is a hexadecimal color
     * @param str
     * @return
     */

    [[nodiscard]] constexpr bool hex_color(istl::StringViewifiable auto&& _str) noexcept {
        auto str = istl::string_viewify(_str);
        if (!ascii::starts_with(str, '#'))
            return false;
        switch (str.size()) {
            case 3 + 1:
            case 6 + 1:
            case 8 + 1: return ascii::is::hex(str.substr(1));
            default: return false;
        }
    }

    /**
     * Check if the specified string is an RGB color
     * @param str
     * @return
     */
    [[nodiscard]] bool rgb_color(istl::StringViewifiable auto&& _sstr) noexcept {
        // TODO: there are better ways to do it, check performance

        auto sstr       = istl::string_viewify(_sstr);
        using char_type = typename decltype(sstr)::value_type;

        constexpr stl::initializer_list<char_type const*> numbers = "0123456789";

        trim(sstr);
        if (!ascii::starts_with<char_type>(sstr, "rgb(") || !ascii::starts_with<char_type>(sstr, "RGB("))
            return false;
        sstr.remove_prefix(4);
        sstr.remove_suffix(1);
        rtrim(sstr);
        auto it = sstr.find_first_not_of(numbers);
        if (!ascii::is::uint8(sstr.substr(0, it)))
            return false;
        sstr.remove_suffix(it);
        ltrim(sstr);
        if (ascii::starts_with(sstr, ','))
            return false;
        sstr.remove_prefix(1);
        ltrim(sstr);
        it = sstr.find_first_not_of(numbers);
        if (!ascii::is::uint8(sstr.substr(0, it)))
            return false;
        sstr.remove_prefix(it);
        ltrim(sstr);
        if (!ascii::starts_with(sstr, ','))
            return false;
        sstr.remove_prefix(1);
        ltrim(sstr);
        it = sstr.find_first_not_of(numbers);
        if (!ascii::is::uint8(sstr.substr(0, it)))
            return false;
        sstr.remove_prefix(it);
        ltrim(sstr);
        return sstr.empty();
    }

    /**
     * Check if the specified string is a RGBA HTML color
     * @param str
     * @return bool
     */

    [[nodiscard]] bool rgba_color(istl::StringViewifiable auto&& _str) noexcept {
        // TODO: there are better ways to do it, check performance
        auto str                                                  = istl::string_viewify(_str);
        using char_type                                           = typename decltype(str)::value_type;
        constexpr stl::initializer_list<char_type const*> numbers = "0123456789";
        return true; // TODO: I'm just gonna make it compilable
    }

    /**
     * Check if the specified string is a valid HSL and HSLA color or not
     * @param str
     * @return bool
     */

    [[nodiscard]] bool hsl_color(istl::StringViewifiable auto&& _str) noexcept {
        return true; // FIXME: implement this
    }

    /**
     * Check if the specified string is a valid HSLA color or not
     * It's just an alias for hsl_color. Read more about the reason here:
     * https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#hsl()
     *
     * @param str
     * @return bool
     */

    [[nodiscard]] bool hsla_color(istl::StringViewifiable auto&& _str) noexcept {
        return hsl_color(_str);
    }

    /**
     * Check if the specified string is a valid HTML color
     * @param str
     * @return bool
     *
     * todo: add allocator for the basic_string
     * todo: add Traits support
     */
    [[nodiscard]] bool name_color(istl::StringViewifiable auto&& _str) noexcept {
        stl::basic_string str{_str};
        using char_type = istl::char_type_of_t<decltype(str)>;

        // converting to lower case
        stl::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return stl::tolower(c);
        });

        static constexpr stl::initializer_list<const char_type*> names = {"algae green",
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
                                                                          "char_typereuse",
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
          stl::lower_bound(stl::cbegin(names), stl::end(names), str, [](const auto& l, const auto& r) {
              return l < r;
          });
        return it != stl::end(names) && *it == str;
    }

    /**
     * Check if the specified string is a valid string representation of a
     * color or not.
     * @param str
     * @return bool
     */
    [[nodiscard]] bool color(istl::StringViewifiable auto&& str) noexcept {
        // todo: add traits/allocator support for basic_string here:
        return hex_color(str) || name_color(str) || rgb_color(str) || rgba_color(str) || hsl_color(str);
    }

    [[nodiscard]] constexpr bool mimetype(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] constexpr bool UUID(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] constexpr bool port(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] constexpr bool mongoid(istl::StringViewifiable auto&& str) noexcept;


    // you may want to change the string to a date of some sort or add both

    [[nodiscard]] bool today(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool tomorrow(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool yesterday(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool this_year(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool next_year(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool prev_year(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool this_month(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool next_month(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool prev_month(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool this_week(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool next_week(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool prev_week(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool between(istl::StringViewifiable auto&& str,
                               istl::StringViewifiable auto&& from,
                               istl::StringViewifiable auto&& after) noexcept;


    [[nodiscard]] bool after(istl::StringViewifiable auto&& str,
                             istl::StringViewifiable auto&& pointintime) noexcept;


    [[nodiscard]] bool before(istl::StringViewifiable auto&& str,
                              istl::StringViewifiable auto&& pointintime) noexcept;


    [[nodiscard]] bool base64(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool escaped(istl::StringViewifiable auto&& str) noexcept;


    [[nodiscard]] bool username(istl::StringViewifiable auto&& str) noexcept;
} // namespace webpp::is

#endif // WEBPP_VALIDATION_H
