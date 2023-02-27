// Created by moisrex on 11/4/20.

#ifndef WEBPP_PATH_HPP
#define WEBPP_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/expected.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "details/constants.hpp"
#include "encoding.hpp"

#include <numeric>

namespace webpp::uri {

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = requires(T slug) {
                       typename T::value_type;
                       slug.empty();
                       T{""};
                       slug.size();
                   };

    enum struct path_error : stl::uint_fast8_t {
        none,          // no errors
        invalid_string // Parts of the specified string cannot be decoded to a valid path.
    };

    template <Slug SlugType      = stl::string,
              typename AllocType = typename stl::remove_cvref_t<SlugType>::allocator_type>
    struct basic_path
      : public istl::expected<stl::vector<stl::remove_cvref_t<SlugType>,
                                          rebind_allocator<AllocType, stl::remove_cvref_t<SlugType>>>,
                              path_error> {
        using container_type         = stl::vector<stl::remove_cvref_t<SlugType>,
                                           rebind_allocator<AllocType, stl::remove_cvref_t<SlugType>>>;
        using char_type              = typename SlugType::value_type;
        using value_type             = typename container_type::value_type;
        using pointer                = typename container_type::pointer;
        using const_pointer          = typename container_type::const_pointer;
        using reference              = typename container_type::reference;
        using const_reference        = typename container_type::const_reference;
        using iterator               = typename container_type::itereator;
        using const_iterator         = typename container_type::const_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;
        using reverse_iterator       = typename container_type::reverse_iterator;
        using size_type              = typename container_type::size_type;
        using difference_type        = typename container_type::different_type;
        using allocator_type         = typename container_type::allocator_type;

        using string_type = stl::
          conditional_t<istl::String<value_type>, value_type, stl::basic_string<char_type, allocator_type>>;
        using string_view_type = istl::string_view_type_of<value_type>;



        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

        template <typename... T>
            requires(stl::is_constructible_v<container_type, T...>)
        constexpr basic_path(T&&... args) : container_type{stl::forward<T>(args)...} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename T>
            requires(!stl::is_same_v<stl::remove_cvref_t<T>, basic_path> && istl::StringViewifiable<T>)
        constexpr basic_path(T&& str, allocator_type const& alloc = allocator_type{})
          : container_type{alloc} {
            parse(istl::string_viewify_of<string_view_type>(stl::forward<T>(str)));
        }
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr void parse(istl::StringifiableOf<string_view_type> auto&& str) {
            auto path = istl::string_viewify_of<string_view_type>(stl::forward<decltype(str)>(str));
            if (path.empty())
                return;

            for (;;) {
                const stl::size_t slash_start = path.find(separator);
                const stl::size_t the_size    = stl::min(slash_start, path.size());
                value_type        val{this->get_allocator()}; // todo: how to get another allocator here?
                if (!decode_uri_component(path.substr(0, the_size), val, allowed_chars)) {
                    this->clear();
                    this->emplace(path_error::invalid_string); // set the error
                    val = path.substr(0, the_size);            // put the non-decoded value
                    // todo: should we just "return" here and stop doing the work?
                }
                this->push_back(stl::move(val));
                if (slash_start == string_view_type::npos) {
                    break;
                }
                path.remove_prefix(slash_start + 1);
            }
        }

        constexpr basic_path& operator=(value_type str) {
            parse(stl::move(str));
            return *this;
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

        constexpr void append_to(istl::String auto& str) const {
            if (this->size() == 0)
                return;

            auto it = this->cbegin();

            // handling empty this special path: "/"
            if (it->empty() && this->size() == 1) {
                str.append(separator);
                return;
            }

            str.append(*it++);
            for (; it != this->cend(); ++it) {
                str.append(separator);
                encode_uri_component(*it, str, allowed_chars);
            }
        }


        constexpr string_type to_string() const {
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



        [[nodiscard]] constexpr size_type size() const noexcept {
            return this->has_value() ? this->size() : 0;
        }

        [[nodiscard]] constexpr iterator begin() noexcept {
            return this->has_value() ? this->begin() : iterator{};
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept {
            return this->has_value() ? this->begin() : const_iterator{};
        }

        [[nodiscard]] constexpr iterator end() noexcept {
            return this->has_value() ? this->end() : iterator{};
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept {
            return this->has_value() ? this->end() : const_iterator{};
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
            return this->has_value() ? this->rbegin() : reverse_iterator{};
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
            return this->has_value() ? this->rbegin() : const_reverse_iterator{};
        }

        [[nodiscard]] constexpr reverse_iterator rend() noexcept {
            return this->has_value() ? this->rend() : reverse_iterator{};
        }

        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
            return this->has_value() ? this->rend() : const_reverse_iterator{};
        }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
            return this->has_value() ? this->cbegin() : const_iterator{};
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept {
            return this->has_value() ? this->cend() : const_iterator{};
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
            return this->has_value() ? this->crbegin() : const_reverse_iterator{};
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
            return this->has_value() ? this->crend() : const_reverse_iterator{};
        }
    };


    template <istl::Stringifiable S>
    basic_path(S&& str) -> basic_path<decltype(istl::stringify(stl::forward<S>(str)))>;

} // namespace webpp::uri

#endif // WEBPP_PATH_HPP
