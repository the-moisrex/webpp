// Created by moisrex on 10/30/20.

#ifndef WEBPP_QUERIES_STRING_HPP
#define WEBPP_QUERIES_STRING_HPP

#include "../std/optional.hpp"
#include "details/parser.hpp"

namespace webpp::uri {

    /**
     * This is designed to separate the queries' methods from other uri methods
     */
    template <typename ...T>
    struct queries_string : public uri_parser<T...> {
        using super = uri_parser<T...>;
        using string_view_type = typename super::string_view_type;
        using string_type = typename super::string_type;


        queries_string& operator=(istl::StringViewifiable auto&& str) {
            set(stl::forward<decltype(str)>(str));
            return *this;
        }

        /**
         * @brief checks if the uri has query or not
         */
        [[nodiscard]] bool empty() const noexcept {
            this->parse_query();
            return this->query_start == this->data.size();
        }

        [[nodiscard]] string_view_type raw() const noexcept {
            this->parse_query();
            if (this->query_start == this->data.size())
                return {};
            return this->substr(this->query_start + 1, this->fragment_start - this->query_start - 1);
        }

        /**
         * Get the query in a decoded string format
         */
        [[nodiscard]] stl::optional<string_type> decoded_string() const noexcept {
            string_type d_queries{this->get_allocator()};
            if (!decode_uri_component(raw(), d_queries, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                return stl::nullopt;
            }
            return d_queries;
        }

        template <typename MapType = istl::map<traits_type, string_type, string_type>>
        [[nodiscard]] auto decoded() const noexcept {
            MapType res(this->get_allocator());
            extract_queries_to(res);
            return res;
        }

        /**
         * Set queries
         */
        auto& set(istl::StringViewifiable auto&& _queries) {
            auto m_query = istl::string_viewify(stl::forward<decltype(_queries)>(_queries));
            if (!is::query(m_query))
                throw stl::invalid_argument("The specified string is not a valid query");

            string_type encoded_query(this->get_allocator());
            if (ascii::starts_with(m_query, '?')) {
                encoded_query.append('?');
            }
            encode_uri_component(m_query, encoded_query, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);

            this->parse_query();

            if (this->query_start != this->data.size()) {
                // we don't have a query
                if (this->fragment_start != this->data.size()) {
                    this->replace_value(this->fragment_start, 0, this->encoded_query);
                } else {
                    this->parse_path();
                    if (this->authority_end == this->data.size()) {
                        // we don't even have authority_end
                        this->parse_scheme();
                        if (this->authority_start == this->data.size()) {
                            // there's no authority_start
                            if (this->scheme_end == this->data.size()) {
                                // it's an empty string
                                this->replace_value(0, 0, "///" + this->encoded_query);
                            } else {
                                this->replace_value(this->scheme_end, 0, "/" + this->encoded_query);
                            }
                        } else {
                            this->replace_value(this->authority_start, 0, "/" + this->encoded_query);
                        }
                    } else {
                        // we have authority_end
                        if (this->data[this->authority_end] == '/') {
                            this->replace_value(this->authority_end + 1, 0, this->encoded_query);
                        } else {
                            this->replace_value(this->authority_end + 1, 0, "/" + this->encoded_query);
                        }
                    }
                }
            } else {
                // we have query
                this->replace_value(this->query_start, this->fragment_start - this->query_start,
                                   this->encoded_query);
            }
            return self;
        }

        /**
         * Append queries from a container like std::map or std::multimap
         */
        template <typename MapIter>
        auto& set_from(MapIter const& _queries_begin, MapIter const& _queries_end) {
            static_assert(is_mutable(), "You can't use this method on a non-modifiable uri struct.");
            using map_key_type   = typename MapIter::first_type;
            using map_value_type = typename MapIter::second_type;
            static_assert(istl::StringViewifiable<map_key_type> &&
                          istl::StringViewifiable<map_value_type>,
                          "The specified map is not valid");

            stl::size_t reserved_size = 0;
            for (auto it = _queries_begin; it != _queries_end; ++it)
                reserved_size += it->first->size() + it->second->size() + 2;
            string_type _query_data(this->get_allocator());
            _query_data.reserve(reserved_size);

            for (auto it = _queries_begin; it != _queries_end; ++it) {
                string_type name(this->get_allocator());
                string_type value(this->get_allocator());
                encode_uri_component(it->first, name, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                encode_uri_component(it->second, value, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);
                if (name.empty()) // when name is empty, we just don't care
                    continue;
                _query_data.append(name);
                if (!value.empty()) {
                    _query_data.append('=');
                    _query_data.append(value);
                }
                if (stl::next(it) != _queries_end) {
                    _query_data.append('&');
                }
            }
            set(stl::move(_query_data));
            return self;
        }

        template <typename MapType>
        auto& set_from(MapType const& _queries) {
            return set_from(_queries.begin(), _queries.end());
        }

        /**
         * @brief clear the query section of the URI
         */
        auto& clear() noexcept {
            return set("");
        }

        /**
         * Get the query in as a map<string, string>
         * It's also in a decoded format
         */
        template <typename MapType = istl::map<traits_type, string_type, string_type>>
        auto& extract_to(MapType& q_structured) const noexcept {
            using map_key_type   = typename MapType::key_value;
            using map_value_type = typename MapType::mapped_type;
            static_assert(istl::String<map_key_type>,
                          "The specified container can't hold the query keys.");
            static_assert(istl::String<map_value_type>,
                          "The specified container can't hold the query values.");
            stl::size_t last_and_sep = 0;
            auto        _query       = raw();
            do {
                auto and_sep = _query.find('&', last_and_sep); // find the delimiter
                auto eq_sep  = _query.find("=", last_and_sep, and_sep - last_and_sep);
                auto name    = _query.substr(last_and_sep + 1, stl::min(eq_sep, and_sep));
                last_and_sep = and_sep;
                if (name.empty()) // a name should not be empty
                    continue;
                string_type d_value(this->get_allocator());
                string_type d_name(this->get_allocator());
                if (and_sep != string_view_type::npos) { // we have a value as well
                    d_value = _query.substr(eq_sep + 1, and_sep);
                }
                if (!decode_uri_component(name, d_name, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                    d_name = name; // just put the non-decoded string there
                }
                if (!d_name.empty()) {
                    map_value_type new_value(q_structured.get_allocator());
                    if (decode_uri_component(d_value, new_value, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                        q_structured[d_name] = stl::move(new_value);
                    } else {
                        q_structured[d_name] = stl::move(d_value); // just put the non-decoded value here
                    }
                }
            } while (last_and_sep != string_view_type::npos);
            return *this;
        }

        auto& append(istl::StringViewifiable auto&& key,
                          istl::StringViewifiable auto&& value) noexcept {
            // todo
        }
    };

}

#endif // WEBPP_QUERIES_STRING_HPP
