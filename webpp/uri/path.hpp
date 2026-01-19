// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./parser/parse_path.hpp"

#include <numeric>

namespace webpp::uri {

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::StringLike<T>;

    /// Serialize path
    template <Slug SlugType, typename CharT, typename AllocT>
    static constexpr void render_path(
      stl::span<SlugType const>         storage,
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& out,
      bool const                        is_opaque = false) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#url-path-serializer
        if (is_opaque) {
            out += storage.front();
        } else {
            if (storage.empty()) {
                return;
            }
            auto seg = storage.begin();
            for (;;) {
                out += *seg;
                if (++seg == storage.end()) {
                    break;
                }
                out += '/';
            }
        }
    }

    /// Serialize path from string view
    template <typename CharT, typename AllocT>
    static constexpr void render_path(stl::basic_string_view<CharT> const storage,
                                      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>&   out) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#url-path-serializer
        out += storage;
    }

    /**
     * @brief Basic Structured URI Path
     */
    template <Slug SlugType = stl::string>
    struct basic_path : stl::span<SlugType const> {
        using string_type      = SlugType;
        using char_type        = istl::char_type_of_t<string_type>;
        using string_view_type = istl::string_view_type_of<string_type>;
        using path_type        = basic_path;
        using size_type        = typename string_type::size_type;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well

        // except slash char
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>;

        using stl::span<SlugType>::span; // ctor

        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !this->empty() && this->front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        /**
         * Get the raw string encoded size
         */
        [[nodiscard]] constexpr stl::size_t raw_string_size() const noexcept {
            stl::size_t sum = this->size() - 1;
            for (auto const& slug : *this) {
                sum += slug.size();
            }
            return sum;
        }

        /// Equality check.
        /// https://url.spec.whatwg.org/#url-equivalence
        /// https://url.spec.whatwg.org/#url-path-serializer
        [[nodiscard]] constexpr bool operator==(basic_path const inp_str) const noexcept {
            if (inp_str.size() != this->size()) {
                return false;
            }
            auto       lhs     = this->begin();
            auto const lhs_end = this->end();
            auto       rhs     = this->begin();
            for (; lhs != lhs_end; ++lhs, ++rhs) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };

    /**
     * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
     * Refer to uri_normalize_benchmark for more related algorithms of this
     */
    template <istl::LinearContainer VecT>
    constexpr void
    remove_dot_segments(VecT& path, bool const remove_leading, bool const remove_empty_segments = false) {
        if (path.empty()) {
            return;
        }

        auto pos = path.begin();

        // handle the first part
        while (pos != path.end()) {
            if (remove_empty_segments && pos->empty()) {
                pos = path.erase(pos);
                continue;
            }
            if (*pos == ".") {
                pos = path.erase(pos);
                continue;
            }
            if (*pos == "..") {
                if (pos != path.begin()) {
                    auto const last_el = std::prev(pos);
                    if (last_el->empty()) {
                        // remove just this one
                        pos = path.erase(pos);
                        continue;
                    }
                    if (*last_el != "..") {
                        // remove the previous one and this one
                        pos = path.erase(last_el, std::next(pos));
                        if (pos == path.begin()) {
                            return;
                        }
                        --pos;
                        continue;
                    }
                } else if (remove_leading) {
                    pos = path.erase(pos);
                    continue;
                }
            }
            ++pos;
        }
    }

    template <Slug T>
    [[nodiscard]] static constexpr bool is_absolute(stl::span<T const> const path) noexcept {
        return !path.empty() && path.front().empty();
    }

    template <istl::LinearContainer VecT>
    static constexpr void normalize(VecT& path, bool const remove_empty_segments = false) {
        remove_dot_segments(is_absolute(path), remove_empty_segments);
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
