// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

#include <tuple>
#include <type_traits>

namespace webpp {

    namespace details {

        template <typename Predicate, typename Operation>
        struct unique_op {
            template <typename Pair, typename T>
            struct apply {
                typedef typename Pair::first  seq_;
                typedef typename Pair::second prior_;
                typedef typename eval_if<
                  and_<is_not_na<prior_>, apply2<Predicate, prior_, T>>,
                  identity<seq_>, apply2<Operation, seq_, T>>::type new_seq_;

                typedef ::std::pair<new_seq_, T> type;
            };
        };

        template <typename Sequence, typename Predicate, typename Inserter>
        struct unique_impl
          : first<typename fold<
              Sequence, pair<typename Inserter::state, na>,
              protect<aux::unique_op<Predicate,
                                     typename Inserter::operation>>>::type> {};

        template <typename Sequence, typename Predicate, typename Inserter>
        struct reverse_unique_impl
          : first<typename reverse_fold<
              Sequence, pair<typename Inserter::state, na>,
              protect<aux::unique_op<Predicate,
                                     typename Inserter::operation>>>::type> {};


    } // namespace details

    template <typename... Extensions>
    struct extension_pack : public ::std::decay_t<Extensions>... {};

    /**
     * Unpack std::tuple into extension pack
     */
    template <typename... Extensions>
    struct extension_pack<::std::tuple<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Unpack the extension pack
     */
    template <typename... Extensions>
    struct extension_pack<extension_pack<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Remove duplicate extensions
     */
    template <typename E1, typename... Extensions>
    struct extension_pack<E1, E1, Extensions...>
      : public extension_pack<E1, Extensions...> {};

    template <typename... Extensions>
    using flatten_extension_list = extension_pack<

      >;

} // namespace webpp

#endif // WEBPP_EXTENSION_H
