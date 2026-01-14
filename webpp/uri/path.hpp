// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../strings/peek.hpp"
#include "./parser/parse_path.hpp"

#include <compare>
#include <numeric>

namespace webpp::uri {

    /// Serialize path
    template <typename StorageType, istl::String StrT>
    static constexpr void render_path(StorageType const& storage, StrT& out, bool const is_opaque = false) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#url-path-serializer
        if (is_opaque) {
            out += storage.front();
        } else {
            for (auto const& seg : storage) {
                out += '/';
                out += seg;
            }
        }
    }

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::StringLike<T>;

    /**
     * @brief Basic Structured URI Path
     * @tparam SlugType The type of each segment of the path to use in the vector
     *                  If the slug type is a string view (and not a string), some of the parsing features
     *                  will be disabled, and that might be a security problem for you. So if the input is
     *                  from an untrusted source, make sure to slug type is a modifiable string.
     */
    template <Slug SlugType = stl::string>
    struct basic_path : SlugType {
        using string_type      = SlugType;
        using value_type       = string_type;
        using char_type        = istl::char_type_of_t<string_type>;
        using string_view_type = istl::string_view_type_of<string_type>;
        using path_type        = basic_path;
        using size_type        = typename string_type::size_type;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well

        // except slash char
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>;

      private:
        bool m_is_opaque = false; // todo

      public:
        using SlugType::SlugType;

        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !storage.empty() && storage.front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        constexpr void normalize(bool const remove_empty_segments = false) {
            remove_dot_segments(is_absolute(), remove_empty_segments);
        }

        constexpr void set_opaque(bool const value = false) noexcept {
            m_is_opaque = value;
        }

        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool const remove_leading, bool const remove_empty_segments = false) {
            if (storage.empty()) {
                return;
            }

            auto pos = storage.begin();

            // handle the first part
            while (pos != storage.end()) {
                if (remove_empty_segments && pos->empty()) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == current_dir) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == parent_dir) {
                    if (pos != storage.begin()) {
                        auto const last_el = std::prev(pos);
                        if (last_el->empty()) {
                            // remove just this one
                            pos = storage.erase(pos);
                            continue;
                        }
                        if (*last_el != parent_dir) {
                            // remove the previous one and this one
                            pos = storage.erase(last_el, std::next(pos));
                            if (pos == storage.begin()) {
                                return;
                            }
                            --pos;
                            continue;
                        }
                    } else if (remove_leading) {
                        pos = storage.erase(pos);
                        continue;
                    }
                }
                ++pos;
            }
        }

        [[nodiscard]] constexpr bool is_opaque() const noexcept {
            return m_is_opaque;
        }

        template <istl::String NStrT = string_type>
        constexpr void to_string(NStrT& out) const {
            render_path(storage_ref(), out, is_opaque());
        }

        template <istl::String NStrT = string_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] constexpr stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            // http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/transform_iterator.html
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& slug : *this) {
                    sum += slug.size();
                }
                return sum;
            }() + storage.size() - 1;
        }

        constexpr void trim() {
            // remove the last empty string
            if (!storage.empty() && storage.back().empty()) {
                static_cast<void>(storage.pop_back());
            }
        }

        /// Attention: this does not parse the input, it's raw emplace back
        template <typename... Args>
        constexpr decltype(auto) emplace_back(Args&&... args) {
            return storage.emplace_back(stl::forward<Args>(args)...);
        }

        /// Equality check.
        /// https://url.spec.whatwg.org/#url-equivalence
        /// https://url.spec.whatwg.org/#url-path-serializer
        template <uri_options Options = {}, istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT && inp_str) const noexcept {
            return *this == clone<Options>(stl::forward<NStrT>(inp_str));
        }

        /// Equality check.
        /// https://url.spec.whatwg.org/#url-equivalence
        /// https://url.spec.whatwg.org/#url-path-serializer
        [[nodiscard]] constexpr bool operator==(basic_path const& inp_str) const noexcept {
            if (inp_str.storage.size() != storage.size()) {
                return false;
            }
            auto       lhs     = this->storage.begin();
            auto const lhs_end = this->storage.end();
            auto       rhs     = this->storage.begin();
            for (; lhs != lhs_end; ++lhs, ++rhs) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
