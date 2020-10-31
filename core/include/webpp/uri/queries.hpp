// Created by moisrex on 10/30/20.

#ifndef WEBPP_QUERIES_HPP
#define WEBPP_QUERIES_HPP

namespace webpp {

    /**
     * This is designed to separate the queries' methods from other uri methods
     */
    struct queries_type {
        basic_uri& self;

        queries_type& operator=(istl::StringViewifiable auto&& str) {
            set(stl::forward<decltype(str)>(str));
            return *this;
        }

        /**
         * @brief checks if the uri has query or not
         */
        [[nodiscard]] bool empty() const noexcept {
            self.parse_query();
            return self.query_start == self.data.size();
        }

        [[nodiscard]] str_view_t raw() const noexcept {
            self.parse_query();
            if (self.query_start == self.data.size())
                return {};
            return self.substr(self.query_start + 1, self.fragment_start - self.query_start - 1);
        }

        /**
         * Get the query in a decoded string format
         */
        [[nodiscard]] stl::optional<str_t> decoded_string() const noexcept {
            str_t d_queries{self.get_allocator()};
            if (!decode_uri_component(raw(), d_queries, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED)) {
                return stl::nullopt;
            }
            return d_queries;
        }

        template <typename MapType = istl::map<traits_type, str_t, str_t>>
        [[nodiscard]] auto decoded() const noexcept {
            MapType res(alloc_holder_type::get_allocator());
            extract_queries_to(res);
            return res;
        }

        /**
         * Set queries
         */
        basic_uri& set(istl::StringViewifiable auto&& _queries) {
            auto m_query = istl::string_viewify(stl::forward<decltype(_queries)>(_queries));
            if (!is::query(m_query))
                throw stl::invalid_argument("The specified string is not a valid query");

            str_t encoded_query(alloc_holder_type::get_allocator());
            if (ascii::starts_with(m_query, '?')) {
                encoded_query.append('?');
            }
            encode_uri_component(m_query, encoded_query, QUERY_OR_FRAGMENT_NOT_PCT_ENCODED);

            self.parse_query();

            if (self.query_start != self.data.size()) {
                // we don't have a query
                if (self.fragment_start != self.data.size()) {
                    self.replace_value(self.fragment_start, 0, self.encoded_query);
                } else {
                    self.parse_path();
                    if (self.authority_end == self.data.size()) {
                        // we don't even have authority_end
                        self.parse_scheme();
                        if (self.authority_start == self.data.size()) {
                            // there's no authority_start
                            if (self.scheme_end == self.data.size()) {
                                // it's an empty string
                                self.replace_value(0, 0, "///" + self.encoded_query);
                            } else {
                                self.replace_value(self.scheme_end, 0, "/" + self.encoded_query);
                            }
                        } else {
                            self.replace_value(self.authority_start, 0, "/" + self.encoded_query);
                        }
                    } else {
                        // we have authority_end
                        if (self.data[self.authority_end] == '/') {
                            self.replace_value(self.authority_end + 1, 0, self.encoded_query);
                        } else {
                            self.replace_value(self.authority_end + 1, 0, "/" + self.encoded_query);
                        }
                    }
                }
            } else {
                // we have query
                self.replace_value(self.query_start, self.fragment_start - self.query_start,
                                   self.encoded_query);
            }
            return self;
        }

        /**
         * Append queries from a container like std::map or std::multimap
         */
        template <typename MapIter>
        basic_uri& set_from(MapIter const& _queries_begin, MapIter const& _queries_end) {
            static_assert(is_mutable(), "You can't use this method on a non-modifiable uri struct.");
            using map_key_type   = typename MapIter::first_type;
            using map_value_type = typename MapIter::second_type;
            static_assert(istl::StringViewifiable<map_key_type> &&
                          istl::StringViewifiable<map_value_type>,
                          "The specified map is not valid");

            stl::size_t reserved_size = 0;
            for (auto it = _queries_begin; it != _queries_end; ++it)
                reserved_size += it->first->size() + it->second->size() + 2;
            str_t _query_data(alloc_holder_type::get_allocator());
            _query_data.reserve(reserved_size);

            for (auto it = _queries_begin; it != _queries_end; ++it) {
                str_t name(alloc_holder_type::get_allocator());
                str_t value(alloc_holder_type::get_allocator());
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
        basic_uri& set_from(MapType const& _queries) {
            return set_from(_queries.begin(), _queries.end());
        }

        /**
         * @brief clear the query section of the URI
         */
        basic_uri& clear() noexcept {
            return set("");
        }

        /**
         * Get the query in as a map<string, string>
         * It's also in a decoded format
         */
        template <typename MapType = istl::map<traits_type, str_t, str_t>>
        basic_uri& extract_to(MapType& q_structured) const noexcept {
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
                str_t d_value(alloc_holder_type::get_allocator());
                str_t d_name(alloc_holder_type::get_allocator());
                if (and_sep != str_view_t::npos) { // we have a value as well
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
            } while (last_and_sep != str_view_t::npos);
            return *this;
        }

        basic_uri& append(istl::StringViewifiable auto&& key,
                          istl::StringViewifiable auto&& value) noexcept {
            // todo
        }
    };

}

#endif // WEBPP_QUERIES_HPP
