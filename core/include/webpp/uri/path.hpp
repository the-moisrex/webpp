// Created by moisrex on 10/30/20.

#ifndef WEBPP_PATH_HPP
#define WEBPP_PATH_HPP

#include "../std/string_view.hpp"
#include "../std/string.hpp"
#include "../std/vector.hpp"

namespace webpp::uri {

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = requires (T slug) {
        typename T::value_type;
      slug.empty();
      T{""};
      slug.size();
    };

    template <Slug SlugType = stl::string, typename AllocType = stl::allocator<SlugType>>
    struct basic_path : public stl::vector<SlugType, AllocType> {
        using container_type = stl::vector<SlugType, AllocType>;
        using allocator_type = AllocType;
        using char_type = typename SlugType::value_type;
        using string_type = stl::conditional_t<istl::String<SlugType>, SlugType, stl::basic_string<char_type, allocator_type>>;

        template <typename ...T>
        constexpr basic_path(T&&...args) :
            container_type{stl::forward<T>(args)...} {}

        [[nodiscard]] bool is_absolute() const noexcept {
            return !this->empty() && this->front().empty();
        }

        [[nodiscard]] bool is_relative() const noexcept {
            return !is_absolute();
        }

        void normalize() {
            remove_dot_segments(is_absolute());
        }


        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         */
        void remove_dot_segments(bool remove_leading) {
            if (this->empty()) return;

            auto the_end = this->end();
            auto it = this->begin();

            // handle the first part
            while (it < the_end) {
                if (*it == ".") {
                    // remove it (move it to the last element)
                    the_end = stl::rotate(it, stl::next(it), the_end);
                    continue;
                } else if (*it == "..") {
                    if (it != this->begin()) {
                        const auto p = stl::prev(it);
                        if (p->empty()) {
                            // remove just this one
                            the_end = stl::rotate(it, stl::next(it), the_end);
                            continue;
                        } else if (*p != "..") {
                            // remove the previous one and this one
                            the_end = stl::rotate(p, stl::next(it), the_end);
                            --it;
                            continue;
                        }
                    } else if (remove_leading) {
                        the_end = stl::rotate(it, stl::next(it), the_end);
                        continue;
                    }
                }
                it++;
            }
            this->erase(the_end, this->end()); // actually remove them from the vector
        }

        string_type to_string() const {
            string_type str{this->get_allocator()};
            if (this->size() == 0)
                return str;

            auto it = this->cbegin();
            str.append(*it++);
            for (; it != this->cend(); ++it) {
                str.append("/");
                str.append(*it);
            }
            return str;
        }
    };


//    struct uri_path {
//        basic_uri& self;
//
//        static constexpr auto LEGAL_PATH_CHARS =
//          charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 1>('/'));
//
//        auto& operator=(
//          istl::StringViewifiable auto&& new_path) { // NOLINT(misc-unconventional-assign-operator)
//            set(stl::forward<decltype(new_path)>(new_path));
//            return *this;
//        }
//
//
//
//        /**
//         * @brief check if the URI has a path or not
//         * @return bool true if the URI doesn't have a path
//         */
//        [[nodiscard]] bool empty() const noexcept {
//            self.parse_path();
//            return self.authority_end == self.data.size();
//        }
//
//        /**
//         * @brief get path in non-decoded, string format
//         * @return
//         */
//        [[nodiscard]] str_view_t raw() const noexcept {
//            if (empty())
//                return {};
//            return self.substr(self.authority_end,
//                               stl::min(self.query_start, self.fragment_start) - self.authority_end);
//        }
//
//        /**
//         * @brief decoded path as a string
//         * @return std::optional<string> the string might not have the right format
//         */
//        [[nodiscard]] stl::optional<str_t> decoded() const noexcept {
//            str_t res{self.get_allocator()};
//            if (!decode_uri_component(raw(), res, LEGAL_PATH_CHARS)) {
//                return stl::nullopt;
//            }
//            return res;
//        }
//
//        /**
//         * Get the decoded slugs with the specified container type
//         * We know how to get the allocator, don't worry.
//         * The specified container's string type should be a string and not a string_view
//         */
//        template <typename Container = path<str_t, allocator_type>>
//        [[nodiscard]] Container slugs() const noexcept {
//            Container container(self.get_allocator());
//            extract_slugs_to<Container>(container);
//            return container;
//        }
//
//        /**
//         * Get the non-decoded slugs.
//         * You can use string_view as the underlying string type of the container since we don't
//         * decode the string. As long as the class has access to the string_view, this method has too.
//         */
//        template <typename Container = istl::vector<traits_type, str_view_t>>
//        [[nodiscard]] Container raw_slugs() const noexcept {
//            Container container(self.get_allocator());
//            extract_raw_slugs_to<Container>(container);
//            return container;
//        }
//
//        /**
//         * @brief get the path as the specified type
//         * @details this method will returns a vector/list/... of string/string_views
//         * this method does not just response to the fact that Container should
//         * be an std container, but if string/string_view is presented as a
//         * container, it will return the whole path.
//         */
//        template <typename Container = istl::vector<traits_type, str_view_t>>
//        basic_uri& extract_raw_slugs_to(Container& container) const noexcept {
//            auto _path = raw();
//            if (_path.empty())
//                return self;
//            stl::size_t slash_start      = 0;
//            stl::size_t last_slash_start = 0;
//            auto        _path_size       = _path.size();
//            if (_path.front() == '/')
//                container.emplace_back(); // empty string
//            do {
//                slash_start = _path.find('/', self.last_slash_start + 1);
//                container.emplace_back(_path.data() + self.last_slash_start + 1,
//                                       stl::min(self.slash_start, _path_size) - self.last_slash_start -
//                                       1);
//                // if (slash_start != str_view_t::npos)
//                // _path.remove_prefix(slash_start + 1);
//                // else
//                // _path.remove_prefix(_path.size());
//                self.last_slash_start = self.slash_start;
//            } while (self.slash_start != str_view_t::npos);
//            return self;
//        }
//
//        /**
//         * @brief this one will return a container containing decoded strings of the path.
//         * @attention do not use string_view or any alternatives for this method
//         * as this method should own its data.
//         */
//        template <typename Container = istl::vector<traits_type, str_t>>
//        [[nodiscard]] bool extract_slugs_to(Container& container) const noexcept {
//            using vec_str_t = typename Container::value_type;
//            static_assert(istl::String<vec_str_t>,
//                          "The specified container doesn't hold a value type that we can understand.");
//            for (auto it = container.begin(); it != container.end(); ++it) {
//                vec_str_t tmp(container.get_allocator());
//                if (!decode_uri_component(*it, tmp, PCHAR_NOT_PCT_ENCODED)) {
//                    return false;
//                }
//                it->swap(tmp);
//            }
//            return true;
//        }
//
//        /**
//         * @brief set path
//         */
//        template <typename Container>
//        basic_uri& set_from(const Container& m_path) noexcept {
//            return set(m_path.begin(), m_path.end());
//        }
//
//        /**
//         * Set path by begin and end of an iterator
//         */
//        template <typename Iter>
//        basic_uri& set_from(const Iter& _start, const Iter& _end) noexcept {
//            const auto almost_end = stl::prev(_end);
//            str_t      new_path{self.get_allocator()};
//            for (auto it = _start; it != almost_end; ++it) {
//                encode_uri_component(*it, new_path, PCHAR_NOT_PCT_ENCODED);
//                new_path.append('/');
//            }
//            // append the last slug
//            encode_uri_component(*almost_end, new_path, PCHAR_NOT_PCT_ENCODED);
//            return set(stl::move(new_path));
//        }
//
//        /**
//         * @brief set the path for the uri
//         * @param _path
//         * @return
//         */
//        basic_uri& set(istl::StringViewifiable auto&& m_path) noexcept {
//            self.parse_path();
//            str_t str(self.get_allocator());
//            encode_uri_component(m_path, str, charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 1>('/')));
//            auto _encoded_path = (ascii::starts_with(m_path, '/') ? "" : "/") + str;
//            self.replace_value(self.authority_end, self.query_start - self.authority_end, _encoded_path);
//            return *this;
//        }
//
//        /**
//         * @brief clear path from the URI
//         */
//        basic_uri& clear() noexcept {
//            return set("");
//        }
//
//        /**
//         * @brief checks if the path is an absolute path or relative path
//         * @return bool
//         */
//        [[nodiscard]] bool is_absolute() const noexcept {
//            return ascii::starts_with(raw(), '/');
//        }
//
//        /**
//         * @brief checks if the path is a relative path or an absolute one
//         * @return bool
//         */
//        [[nodiscard]] bool is_relative() const noexcept {
//            return !is_absolute();
//        }
//
//        /**
//         * @brief checks if the uri path is normalized or not (contains relative
//         * . or .. paths)
//         * @return
//         */
//        [[nodiscard]] bool is_normalized() const noexcept {
//            auto m_path = raw_slugs();
//            return m_path.cend() != stl::find_if(m_path.cbegin(), m_path.cend(), [](auto const& p) {
//              return p == "." || p == "..";
//            });
//        }
//
//        /**
//         * @details This method applies the "remove_dot_segments" routine talked
//         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
//         * segments of the URI, in order to normalize the path
//         * (apply and remove "." and ".." segments).
//         */
//        basic_uri& normalize_path() noexcept {
//            // TODO
//            return *this;
//        }
//    };
}

#endif // WEBPP_PATH_HPP
