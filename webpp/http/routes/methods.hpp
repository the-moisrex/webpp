#ifndef WEBPP_VALVES_METHODS_HPP
#define WEBPP_VALVES_METHODS_HPP

#include "valve_traits.hpp"
#include "valves.hpp" // method is not templated, so we need valve NOW

namespace webpp::http {

    struct method : valve<method> {
      private:
        // since It's literally just a few characters, we don't need to store the size as well
        char const* method_str;

      public:
        explicit constexpr method(char const* inp_method) noexcept : method_str{inp_method} {}

        constexpr method(method const&) noexcept            = default;
        constexpr method(method&&) noexcept                 = default;
        constexpr method& operator=(method const&) noexcept = default;
        constexpr method& operator=(method&&) noexcept      = default;
        constexpr ~method() noexcept                        = default;

        template <HTTPRequest ReqT>
        [[nodiscard]] constexpr bool operator()(ReqT const& req) const noexcept {
            return req.method() == method_str;
        }

        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType> const& ctx) const noexcept {
            return operator()(ctx.request);
        }

        /**
         * @brief Converts the object to a string representation.
         * This function appends the string representation of the object to the given output string.
         * @param out The output string to which the string representation is appended.
         */
        void to_string(istl::String auto& out) const {
            append_to(out, method_str);
        }
    };

    using method = method;

    constexpr auto get     = method{"GET"};
    constexpr auto post    = method{"POST"};
    constexpr auto head    = method{"HEAD"};
    constexpr auto put     = method{"PUT"};
    constexpr auto patch   = method{"PATCH"};
    constexpr auto del     = method{"DELETE"};
    constexpr auto options = method{"OPTIONS"};

    inline namespace literals {
        constexpr auto operator""_method(char const* str, [[maybe_unused]] std::size_t inp_size) noexcept {
            return method{str};
        }
    } // namespace literals

} // namespace webpp::http


#endif // WEBPP_VALVES_METHODS_HPP
