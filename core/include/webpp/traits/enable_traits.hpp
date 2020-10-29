// Created by moisrex on 8/30/20.

#ifndef WEBPP_ENABLE_TRAITS_HPP
#define WEBPP_ENABLE_TRAITS_HPP

#include "../std/concepts.hpp"
#include "../std/std.hpp"
#include "traits.hpp"
#include "traits_concepts.hpp"

#include <type_traits>

namespace webpp {

    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <Traits TraitsType>
    struct enable_traits {
        using traits_type      = stl::remove_cvref_t<TraitsType>;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using char_type        = typename traits_type::char_type;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename traits_type::template allocator<char_type>;

      private:
        [[no_unique_address]] allocator_type alloc{};

      public:
        [[no_unique_address]] logger_ref logger{};

        constexpr explicit enable_traits(logger_ref  logger_obj = logger_type{},
                                         auto const& alloc_obj  = allocator_type{}) noexcept
          : logger{logger_obj},
            alloc{alloc_obj} {}

        constexpr enable_traits()                         = default;
        constexpr enable_traits(enable_traits const&)     = default;
        constexpr enable_traits(enable_traits&&) noexcept = default;


        template <typename AllocType = typename allocator_type::value_type>
        auto const& get_allocator() const noexcept {
            using new_alloc_type = stl::remove_cvref_t<AllocType>;
            if constexpr (stl::same_as<new_alloc_type, typename allocator_type::value_type>) {
                return alloc;
            } else {
                return to_alloc<allocator_type, new_alloc_type>{alloc};
            }
        }

        /**
         * This is designed to pass the allocator and logger and other necessary stuff from this traits
         * enabled struct and create an object of the specified type and pass those traits to it.
         * This is there to make sure we don't have to pass all these stuff around ourselves and this can
         * manage those passings.
         */
        template <EnabledTraits T, typename ...Args>
        [[nodiscard]] constexpr auto instantiate(Args&& ...args) noexcept {
            return T{stl::forward<Args>(args)..., logger, get_allocator()};
        }

        void swap(EnabledTraits auto& other) {
            using stl::swap;
            swap(alloc, other.alloc);
            swap(logger, other.logger);
        }

    };

} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
