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

    template <typename ViewType, typename T>
    concept PossibleDataTypes = stl::same_as<typename ViewType::data_type, T> ||
                                stl::same_as<T, istl::nothing_type> ||
                                (istl::ReadOnlyCollection<stl::remove_cvref_t<T>> &&
                                 requires {
                                     typename stl::remove_cvref_t<T>::value_type;
                                     requires requires(typename stl::remove_cvref_t<T>::value_type obj) {
                                                  obj.first;
                                                  obj.second;
                                              };
                                 });

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


    template <typename T>
    concept View = requires(T view) {
                       typename T::data_type;
                       typename T::string_type;
                       typename T::string_view_type;
                       typename T::traits_type;
                       requires Traits<typename T::traits_type>;

                       requires stl::movable<T>;
                       requires stl::copyable<T>;


                       // render with the data passed to it
                       requires requires(typename T::string_type & out,
                                         typename T::string_view_type sv,
                                         typename T::data_type        dt) {
                                    view.scheme(sv); // reparse, and change the scheme
                                    view.render(out, dt);
                                    { view.has_scheme() } -> stl::same_as<bool>;
                                };
                   };

} // namespace webpp::views

#endif // WEBPP_VIEW_CONCEPTS_HPP
