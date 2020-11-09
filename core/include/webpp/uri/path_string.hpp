// Created by moisrex on 10/30/20.

#ifndef WEBPP_PATH_STRING_HPP
#define WEBPP_PATH_STRING_HPP

#include "./details/constants.hpp"
#include "./details/parser.hpp"
#include "./path.hpp"

namespace webpp::uri {

    template<typename ...T>
    struct string_path : public uri_parser<T...> {
        using super = uri_parser<T...>;
        using char_type = typename super::char_type;
        using str_t = typename super::string_type;
        using str_view_t = typename super::string_view_type;
        using allocator_type = typename super::allocator_type;

        template <typename ...Args>
        constexpr string_path(Args&& ...args) : super{stl::forward<Args>(args)...} {}

        static constexpr auto LEGAL_PATH_CHARS =
          charset(details::PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/'));

        auto& operator=(
          istl::StringViewifiable auto&& new_path) { // NOLINT(misc-unconventional-assign-operator)
            set(stl::forward<decltype(new_path)>(new_path));
            return *this;
        }



        /**
         * @brief check if the URI has a path or not
         * @return bool true if the URI doesn't have a path
         */
        [[nodiscard]] bool empty() const noexcept {
            this->parse_path();
            return this->authority_end == this->data.size();
        }

        /**
         * @brief get path in non-decoded, string format
         */
        [[nodiscard]] str_view_t raw() const noexcept {
            if (empty())
                return {};
            return this->substr(this->authority_end,
                               stl::min(this->query_start, this->fragment_start) - this->authority_end);
        }

        /**
         * @brief decoded path as a string
         * @return std::optional<string> the string might not have the right format
         */
        [[nodiscard]] stl::optional<str_t> decoded() const noexcept {
            str_t res{this->get_allocator()};
            if (!decode_uri_component(raw(), res, LEGAL_PATH_CHARS)) {
                return stl::nullopt;
            }
            return res;
        }

        /**
         * Get the decoded slugs with the specified container type
         * We know how to get the allocator, don't worry.
         * The specified container's string type should be a string and not a string_view
         */
        template <typename Container = basic_path<str_t, allocator_type>>
        [[nodiscard]] Container slugs() const noexcept {
            Container container(this->get_allocator());
            extract_slugs_to<Container>(container);
            return container;
        }

        /**
         * Get the non-decoded slugs.
         * You can use string_view as the underlying string type of the container since we don't
         * decode the string. As long as the class has access to the string_view, this method has too.
         */
        template <typename Container = basic_path<str_view_t, allocator_type>>
        [[nodiscard]] Container raw_slugs() const noexcept {
            Container container(this->get_allocator());
            extract_raw_slugs_to<Container>(container);
            return container;
        }

        /**
         * @brief get the path as the specified type
         * @details this method will returns a vector/list/... of string/string_views
         * this method does not just response to the fact that Container should
         * be an std container, but if string/string_view is presented as a
         * container, it will return the whole path.
         */
        template <typename Container = basic_path<str_view_t, allocator_type>>
        auto& extract_raw_slugs_to(Container& container) const noexcept {
            auto _path = raw();
            if (_path.empty())
                return *this;
            stl::size_t slash_start      = 0;
            stl::size_t last_slash_start = 0;
            auto        _path_size       = _path.size();
            if (_path.front() == '/')
                container.emplace_back(); // empty string
            do {
                slash_start = _path.find('/', this->last_slash_start + 1);
                container.emplace_back(_path.data() + this->last_slash_start + 1,
                                       stl::min(this->slash_start, _path_size) - this->last_slash_start -
                                       1);
                // if (slash_start != str_view_t::npos)
                // _path.remove_prefix(slash_start + 1);
                // else
                // _path.remove_prefix(_path.size());
                this->last_slash_start = this->slash_start;
            } while (this->slash_start != str_view_t::npos);
            return *this;
        }

        /**
         * @brief this one will return a container containing decoded strings of the path.
         * @attention do not use string_view or any alternatives for this method
         * as this method should own its data.
         */
        template <typename Container = basic_path<str_t, allocator_type>>
        [[nodiscard]] bool extract_slugs_to(Container& container) const noexcept {
            using vec_str_t = typename Container::value_type;
            static_assert(istl::String<vec_str_t>,
                          "The specified container doesn't hold a value type that we can understand.");
            for (auto it = container.begin(); it != container.end(); ++it) {
                vec_str_t tmp(container.get_allocator());
                if (!decode_uri_component(*it, tmp, PCHAR_NOT_PCT_ENCODED)) {
                    return false;
                }
                it->swap(tmp);
            }
            return true;
        }

        /**
         * @brief set path
         */
        template <typename Container>
        auto& set_from(const Container& m_path) noexcept {
            return set(m_path.begin(), m_path.end());
        }

        /**
         * Set path by begin and end of an iterator
         */
        template <typename Iter>
        auto& set_from(const Iter& _start, const Iter& _end) noexcept {
            const auto almost_end = stl::prev(_end);
            str_t      new_path{this->get_allocator()};
            for (auto it = _start; it != almost_end; ++it) {
                encode_uri_component(*it, new_path, PCHAR_NOT_PCT_ENCODED);
                new_path.append('/');
            }
            // append the last slug
            encode_uri_component(*almost_end, new_path, PCHAR_NOT_PCT_ENCODED);
            return set(stl::move(new_path));
        }

        /**
         * @brief set the path for the uri
         */
        auto& set(istl::StringViewifiable auto&& m_path) noexcept {
            this->parse_path();
            str_t str(this->get_allocator());
            encode_uri_component(m_path, str, charset(details::PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/')));
            auto _encoded_path = (ascii::starts_with(m_path, '/') ? "" : "/") + str;
            this->replace_value(this->authority_end, this->query_start - this->authority_end, _encoded_path);
            return *this;
        }

        /**
         * @brief clear path from the URI
         */
        auto& clear() noexcept {
            return set("");
        }

        /**
         * @brief checks if the path is an absolute path or relative path
         * @return bool
         */
        [[nodiscard]] bool is_absolute() const noexcept {
            return ascii::starts_with(raw(), '/');
        }

        /**
         * @brief checks if the path is a relative path or an absolute one
         * @return bool
         */
        [[nodiscard]] bool is_relative() const noexcept {
            return !is_absolute();
        }

        /**
         * @brief checks if the uri path is normalized or not (contains relative
         * . or .. paths)
         * @return bool
         */
        [[nodiscard]] bool is_normalized() const noexcept {
            auto m_path = raw_slugs();
            return m_path.cend() != stl::find_if(m_path.cbegin(), m_path.cend(), [](auto const& p) {
              return p == "." || p == "..";
            });
        }

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        auto& normalize() noexcept {
            // TODO
            return *this;
        }
    };
}

#endif // WEBPP_PATH_STRING_HPP
