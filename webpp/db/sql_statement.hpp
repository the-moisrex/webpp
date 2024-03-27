#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "../common/meta.hpp"
#include "../memory/object.hpp"
#include "../std/tuple.hpp"
#include "../traits/enable_traits.hpp"
#include "sql_cell.hpp"
#include "sql_concepts.hpp"
#include "sql_row.hpp"

namespace webpp::sql {


    /**
     * A wrapper for the driver's statement type. This is what the user uses directly.
     */
    template <Traits TraitsType, SQLStatement StmtType>
    struct sql_statement : StmtType, enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using etraits          = enable_traits<TraitsType>;
        using driver_type      = StmtType;
        using size_type        = typename driver_type::size_type;
        using string_type      = traits::string<traits_type>;
        using row_type         = sql_row<sql_statement>;
        using cell_type        = sql_cell<sql_statement>;
        using char_type        = traits::char_type<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using iterator         = row_iterator<sql_statement>;
        using const_iterator   = row_iterator<sql_statement const>;

        static constexpr auto LOG_CAT = "SQLStmt";

        // should satisfy ItupleOptions
        // These are ituple options which make all elements of ituple a "row".
        template <stl::size_t N = 0>
        struct iterator_options {
            using default_type                = cell_type;
            static constexpr stl::size_t size = N;

            template <stl::size_t NewN = 0>
            using resize = iterator_options<NewN>;

            // get the cell type based on the index
            template <stl::size_t I>
            static constexpr default_type get_default(auto& itup) noexcept {
                // get the stmt from the first tuple element
                auto stmt = stl::get<0>(itup).statement();
                return {I, stmt};
            }

            static constexpr auto ituplify(row_type const& row) noexcept {
                return row.template as_tuple<N, istl::ituple>();
            }
        };

        template <EnabledTraits ET>
        explicit sql_statement(ET&& inp_et) noexcept
          : driver_type{},
            etraits{stl::forward<ET>(inp_et)} {}

        template <EnabledTraits ET>
        sql_statement(driver_type&& driver, ET&& inp_et) noexcept
          : driver_type{stl::move(driver)},
            etraits{stl::forward<ET>(inp_et)} {}

        sql_statement(sql_statement&&) noexcept = default;
        sql_statement(sql_statement const&)     = delete;


        sql_statement& operator=(sql_statement&&) noexcept = default;
        sql_statement& operator=(sql_statement const&)     = delete;

        sql_statement& operator=(driver_type&& new_driver) noexcept {
            driver() = stl::move(new_driver);
            return *this;
        }

        /**
         * Possible values that get passed down:
         *   - string_view
         *   - integral
         */
        template <typename T>
        sql_statement& bind(size_type index, T&& val) noexcept {
            auto errmsg = object::make_object<string_type>(*this);
            if constexpr (requires { this->bind(index, stl::forward<T>(val), errmsg); }) {
                driver().bind(index, stl::forward<T>(val), errmsg);
            } else if constexpr (istl::StringViewifiable<T>) {
                driver().bind(index, istl::string_viewify_of<string_view_type>(stl::forward<T>(val)), errmsg);
            } else {
                static_assert_false(T, "Don't know how to bind the value, unknown type specified.");
            }
            log(errmsg);
            return *this;
        }

        bool step() noexcept {
            auto       errmsg          = object::make_object<string_type>(*this);
            bool const continue_or_not = driver().step(errmsg);
            if (!errmsg.empty()) {
                this->logger.error(LOG_CAT, errmsg);
                return false;
            }
            return continue_or_not;
        }

        sql_statement& execute() noexcept {
            stl::ignore = step();
            return *this;
        }

        sql_statement& reset() noexcept {
            auto errmsg = object::make_object<string_type>(*this);
            driver().reset(errmsg);
            log(errmsg);
            return *this;
        }

        cell_type column(size_type index) noexcept {
            return cell_type{*this, index};
        }

        [[nodiscard]] cell_type operator[](size_type index) noexcept {
            return cell_type{*this, index};
        }

        template <typename T>
        [[nodiscard]] cell_type operator<<(T&& val) noexcept {
            bind(0, stl::forward<T>(val));
            return cell_type{*this, 1};
        }

        [[nodiscard]] driver_type& driver() noexcept {
            return *static_cast<driver_type*>(this);
        }

        template <stl::size_t N>
        [[nodiscard]] auto structured() noexcept {
            return istl::ituple_iterable<sql_statement, iterator_options<N>>{*this};
        }

        template <stl::size_t N>
        [[nodiscard]] auto structured() const noexcept {
            return istl::ituple_iterable<sql_statement, iterator_options<N>>{*this};
        }

        template <stl::size_t N>
        [[nodiscard]] auto&& structured() && noexcept {
            return istl::ituple_iterable<sql_statement, iterator_options<N>>{stl::move(*this)};
        }

        // row iterator
        [[nodiscard]] iterator begin() noexcept {
            execute();
            return iterator{this};
        }

        // end of row iterator
        [[nodiscard]] iterator end() noexcept {
            return iterator{};
        }

        // end of row iterator
        [[nodiscard]] const_iterator end() const noexcept {
            return iterator{};
        }

        [[nodiscard]] row_type first() noexcept {
            execute();
            return row_type{*this};
        }

        void log(istl::String auto const& errmsg) noexcept {
            if (!errmsg.empty()) {
                this->logger.error(LOG_CAT, errmsg);
            }
        }
    };

} // namespace webpp::sql

namespace std {

    template <size_t I, class... T>
    struct tuple_element<I, webpp::sql::sql_statement<T...>> {
        using type = typename webpp::sql::sql_statement<T...>::row_type;
    };

    template <class... T>
    struct tuple_size<webpp::sql::sql_statement<T...>> : integral_constant<size_t, 0> {};

} // namespace std

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
