#ifndef WEBPP_PROPERTY_H
#define WEBPP_PROPERTY_H

/**
 * What is should be able to do:
 *   - [ ] Good enough to be able to be used as a public class member
 *   - [ ] Should require no more than the devs will use
 *   - [ ] We should have watchers
 *     - [ ] Properties with no watchers should not pay for it
 *     - [ ] Do something when:
 *       - [ ] On ctor
 *       - [ ] On dtor
 *       - [ ] On change
 *       - [ ] On read
 *       - [ ] On write
 *       - [ ] On clear
 *       - [ ] On move
 *       - [ ] On copy
 *   - [ ] Should be a regular type
 *     - [ ] Default constructors
 *     - [ ] Assignment operators
 *     - [ ] Move operators
 *     - [ ] Operator overloads
 *       - [ ] = + / - ...
 *   - [ ] Swap functions
 *   - [ ] Thread safe:
 *     - [ ] Read only lock
 *     - [ ] Write only lock
 *     - [ ] Read and write lock
 *   - [ ] Read only capabilities
 *   - [ ] Constexpr compatible as much as possible
 */

#include <ostream>
#include <utility>

namespace webpp {

    template <typename T>
    class property {
      private:
        T data;

      public:
        explicit constexpr property(T const& t) noexcept : data(t) {}

        explicit constexpr property(T&& t) noexcept : data(std::move(t)) {}

        ~property() noexcept = default;

        constexpr void operator=(T const& t) noexcept {
            data = t;
        }

        constexpr void operator=(T&& t) noexcept {
            data = std::move(t);
        }

        // Getting the value
        constexpr T const& value() const noexcept {
            return data;
        }

        // C++ operators:
        template <typename S>
        constexpr auto& operator<<(S& s) {
            s << data;
            return s;
        }

        template <typename S>
        constexpr bool operator==(S const& s) const noexcept {
            return data == s;
        }

        template <typename S>
        constexpr bool operator!=(S const& s) const noexcept {
            return data != s;
        }
    };

    template <typename T>
    constexpr auto& operator<<(std::ostream& s, property<T> const& t) {
        s << t.value();
        return s;
    }

    template <typename TT, typename S>
    constexpr bool operator==(S const& s, property<TT> const& p) noexcept {
        return p.value() == s;
    }

} // namespace webpp

#endif // WEBPP_PROPERTY_H
