// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "details/constants.hpp"
#include "encoding.hpp"

#include <compare>
#include <numeric>

namespace webpp::uri {

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::String<T>;

    template <Slug SlugType = stl::string>
    struct basic_path
      : public stl::vector<stl::remove_cvref_t<SlugType>,
                           rebind_allocator<typename stl::remove_cvref_t<SlugType>::allocator_type,
                                            stl::remove_cvref_t<SlugType>>> {
        using slug_type           = stl::remove_cvref_t<SlugType>;
        using slug_allocator_type = typename slug_type::allocator_type;
        using allocator_type      = rebind_allocator<slug_allocator_type, slug_type>; // vector
        using container_type      = stl::vector<slug_type, allocator_type>;
        using value_type          = slug_type;
        using char_type           = istl::char_type_of<slug_type>;
        using string_type         = stl::
          conditional_t<istl::String<value_type>, value_type, stl::basic_string<char_type, allocator_type>>;
        using string_view_type = istl::string_view_type_of<value_type>;
        using path_type        = basic_path;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

        template <typename... T>
            requires(stl::is_constructible_v<container_type, T...>)
        constexpr basic_path(T&&... args) : container_type{stl::forward<T>(args)...} {}

        template <typename T>
            requires(!stl::is_same_v<stl::remove_cvref_t<T>, basic_path> && istl::StringViewifiable<T>)
        constexpr basic_path(T&& str, allocator_type const& alloc = allocator_type{})
          : container_type{alloc} {
            parse(istl::string_viewify_of<string_view_type>(stl::forward<T>(str)));
        }

        constexpr bool parse(istl::StringifiableOf<string_view_type> auto&& str) {
            auto path = istl::string_viewify_of<string_view_type>(stl::forward<decltype(str)>(str));
            if (path.empty())
                return true;

            for (;;) {
                const stl::size_t slash_start = path.find(separator);
                const stl::size_t the_size    = stl::min(slash_start, path.size());
                value_type        val{this->get_allocator()};
                if (!decode_uri_component(path.substr(0, the_size), val, allowed_chars)) {
                    // error: invalid string passed as a path
                    this->clear();
                    // val = path.substr(0, the_size); // put the non-decoded value
                    return false;
                }
                this->push_back(stl::move(val));
                if (slash_start == string_view_type::npos) {
                    break;
                }
                path.remove_prefix(slash_start + 1);
            }
            return true;
        }

        template <istl::StringViewifiable SegStrT>
        constexpr basic_path& operator/=(SegStrT&& seg_str) {
            auto path = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(seg_str));
            if (path.empty())
                return *this;

            for (;;) {
                const stl::size_t slash_start = path.find(separator);
                const stl::size_t the_size    = stl::min(slash_start, path.size());
                value_type        val{this->get_allocator()};
                if (!decode_uri_component(path.substr(0, the_size), val, allowed_chars)) {
                    // error: invalid string passed as a path
                    val = path.substr(0, the_size); // put the non-decoded value
                }
                this->push_back(stl::move(val));
                if (slash_start == string_view_type::npos) {
                    break;
                }
                path.remove_prefix(slash_start + 1);
            }
            return *this;
        }

        constexpr basic_path& operator=(value_type str) {
            parse(stl::move(str));
            return *this;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(basic_path const& rhs) const noexcept {
            const auto lhs_size = this->size();
            const auto rhs_size = rhs.size();
            if (lhs_size != rhs_size)
                return stl::compare_partial_order_fallback(lhs_size, rhs_size);
            if (stl::equal(this->begin(), this->end(), rhs.begin(), rhs.end())) {
                return stl::partial_ordering::equivalent;
            } else {
                return stl::partial_ordering::unordered;
            }
        }


        template <istl::StringViewifiable SegStrT>
            requires(!stl::same_as<stl::remove_cvref_t<SegStrT>, basic_path>)
        [[nodiscard]] constexpr auto operator<=>(SegStrT&& rhs) const {
            // todo: optimize this
            auto const      path_str = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(rhs));
            path_type const rhs_path{path_str, this->get_allocator()};
            return *this <=> rhs_path;
        }


        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !this->empty() && this->front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        constexpr void normalize() {
            remove_dot_segments(is_absolute());
        }


        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool remove_leading) {
            if (this->empty())
                return;

            auto it = this->begin();

            // handle the first part
            while (it < this->end()) {
                if (*it == current_dir) {
                    this->erase(it);
                    continue;
                } else if (*it == parent_dir) {
                    if (it != this->begin()) {
                        const auto p = std::prev(it);
                        if (p->empty()) {
                            // remove just this one
                            this->erase(it);
                            continue;
                        } else if (*p != parent_dir) {
                            // remove the previous one and this one
                            this->erase(p, std::next(it));
                            --it;
                            continue;
                        }
                    } else if (remove_leading) {
                        this->erase(it);
                        continue;
                    }
                }
                it++;
            }
        }

        constexpr basic_path const& append_to(istl::String auto& str) const {
            if (this->size() == 0)
                return *this;

            auto it = this->cbegin();

            // handling empty this special path: "/"
            if (it->empty() && this->size() == 1) {
                str.append(separator);
                return *this;
            }

            str.append(*it++);
            for (; it != this->cend(); ++it) {
                str.append(separator);
                encode_uri_component(*it, str, allowed_chars);
            }
            return *this;
        }


        [[nodiscard]] constexpr string_type to_string() const {
            string_type str{this->get_allocator()};
            append_to(str);
            return str;
        }


        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] constexpr stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            // http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/transform_iterator.html
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& slug : *this)
                    sum += slug.size();
                return sum;
            }() + this->size() - 1;
        }


        constexpr void fix() {
            // remove the last empty string
            if (this->size() && this->back().empty()) {
                (void) this->pop_back();
            }
        }
    };


    template <istl::Stringifiable S>
    basic_path(S&& str) -> basic_path<decltype(istl::stringify(stl::forward<S>(str)))>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
