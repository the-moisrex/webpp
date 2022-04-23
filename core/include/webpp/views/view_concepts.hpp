// Created by moisrex on 16/1/22.

#ifndef WEBPP_VIEW_CONCEPTS_HPP
#define WEBPP_VIEW_CONCEPTS_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/tuple.hpp"

namespace webpp::views {

    template <typename T>
    concept DataView = requires(T item) {
        requires stl::copyable<T>;
        requires stl::movable<T>;
        item.key();
        item.value();
    };

    namespace details {
        template <typename T>
        struct is_data_view {
            static constexpr bool value = DataView<T>;
        };
    } // namespace details

    template <typename T>
    concept DataViews = stl::same_as<T, istl::nothing_type> ||(istl::ReadOnlyCollection<T>&& requires {
        typename T::value_type;
        requires DataView<typename T::value_type>;
    });

    template <typename T>
    concept PossibleDataTypes = DataViews<stl::remove_cvref_t<T>> ||
      (istl::ReadOnlyCollection<stl::remove_cvref_t<T>>&& requires {
          typename stl::remove_cvref_t<T>::value_type;
          requires requires(typename stl::remove_cvref_t<T>::value_type obj) {
              obj.first;
              obj.second;
          };
      });

    template <typename T>
    concept DataViewSettings = requires(T dv) {
        typename T::traits_type;
        Traits<typename T::traits_type>;
        dv.acceptable_types; // of type "view_data_flags"
    };

    /**
     * Features of a view:
     *
     *   - Cache-able. (we don't need the data to be cached, only the rendered scheme)
     *   - Cross-Format Transpiler
     */
    template <typename T>
    concept ViewManager = requires(T man) {
        typename T::traits_type;
        typename T::string_type;
        typename T::string_view_type;

        requires Traits<typename T::traits_type>;
        requires requires(typename T::string_view_type str) {
            { man.view(str) } -> istl::String;
        };
    };

    template <typename T>
    concept ViewManagerInput = ViewManager<T> && stl::is_lvalue_reference_v<T>;

    template <typename V, typename T>
    concept ViewDataInput =
      PossibleDataTypes<T> || stl::same_as<typename V::data_view_type, stl::remove_cvref_t<T>> ||
      stl::same_as<typename V::data_type, stl::remove_cvref_t<T>>;

    template <typename T>
    concept View = requires(T view) {
        typename T::data_view_type;
        typename T::data_type;
        typename T::string_type;
        typename T::string_view_type;
        typename T::traits_type;
        requires Traits<typename T::traits_type>;
        requires DataViews<typename T::data_view_type>;

        requires stl::movable<T>;
        requires stl::copyable<T>;


        // render with the data passed to it
        requires requires(typename T::string_type & out,
                          typename T::string_view_type sv,
                          typename T::data_view_type   dv,
                          typename T::data_type        dt) {
            view.scheme(sv); // reparse, and change the scheme
            view.render(out, dt);
            view.render(out, dv);
        };
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
