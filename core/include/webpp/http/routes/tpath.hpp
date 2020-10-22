// Created by moisrex on 4/29/20.

#ifndef WEBPP_TPATH_H
#define WEBPP_TPATH_H

#include "route.hpp"
#include "webpp/uri/uri.hpp"

#include <cassert>
#include <cstddef> // for std::size_t
#include <string_view>


namespace webpp {

    /**
     * TODO: add types to the "{user_id}" to be able to use it as "{int:user_id}"
     */
    stl::map<stl::string_view, stl::string_view>
    parse_vars(stl::string_view const& _templ,
               stl::string_view const& _path) noexcept {
        using namespace webpp;

        stl::map<stl::string_view, stl::string_view> res;

        using ptr_t = decltype(_templ.data());

        ptr_t templ_start = _templ.data();
        ptr_t templ_end   = _templ.data() + _templ.size();
        ptr_t curly_start = templ_end;
        ptr_t colon       = templ_end;
        ptr_t path_char   = _path.data();

        for (ptr_t c = templ_start; *c; c++) {
            // breaks out of the loop when the template and the paths don't
            // match
            if (curly_start == templ_end && *c != '{' && *c != *path_char++)
                break;

            switch (*c) {
                case '{':
                    if (curly_start == templ_end) {
                        colon = curly_start = c;
                    }
                    break;
                case ':':
                    if (curly_start != templ_end) {
                        colon = c;
                    }
                    break;
                case '}':
                    // check if we are in a curly braces
                    if (curly_start != templ_end) {
                        stl::string_view key{
                          colon + 1, static_cast<stl::string_view::size_type>(
                                       c - colon - 1)};

                        // find _ in "{var}_" inside path
                        // this doesn't take the / in account
                        auto next_char_path =
                          stl::string_view{path_char}.find(*(c + 1));

                        stl::string_view value{
                          path_char,
                          stl::min(next_char_path,
                                   _path.size() - (path_char - _path.data()))};
                        /*
                        std::string_view type{curly_start + 1,
                                              static_cast<std::string_view::size_type>(
                                                  colon - curly_start)};
                                                  */
                        res[key] = value;

                        // preparing for the next variable
                        curly_start = colon = templ_end; // reset
                        path_char += next_char_path;
                    }
                    break;
            }
        }

        return res;
    }




    /**
     * Check whether or not the specified URI path is a match for the specified
     * template. This function will be used in "tpath_condition". I didn't
     * implement it there because it's a template method and I'd like to
     * abstract away the implementation details.
     * @param templated_path
     * @param _path
     * @return
     */
    template <typename CharT>
    [[nodiscard]] bool
    tpath_check(stl::basic_string_view<CharT> const& templated_path,
                stl::basic_string_view<CharT> const& _path) noexcept {
        //        auto          tit        = templated_path.data(); // templated
        //        iterator auto          pit        = _path.data();          //
        //        path iterator decltype(tit) seg_start  = nullptr; stl::size_t
        //        seg_starts = 0; decltype(pit) pseg_start = nullptr; for (;;) {
        //            switch (*tit) {
        //                case '\0':
        //
        //                    // error: there's an open segment rule and we can't just
        //                    // leave it unclosed. I should throw an error in the user's
        //                    // face but I'm nice and I'm just gonna assert
        //                    assert(("the specified templated path is not
        //                    valid.",
        //                            seg_start == nullptr));
        //                    if (seg_start) {
        //                        return false;
        //                    }
        //                    break;
        //                case '{': // might be start of a segment
        //
        //                    if (seg_start) {
        //                        // the { char is part of the segment rule here
        //                        ++seg_starts;
        //                        break;
        //                    }
        //
        //                    // pinpoint the segment start for when we reach the end of
        //                    // it
        //                    seg_start  = tit + 1;
        //                    pseg_start = pit;
        //                    break;
        //                case '}':
        //                    { // might be the end of a segment
        //
        //                        // a new } char with no starting { char
        //                        assert(("The specified templated path is not
        //                        valid.",
        //                                seg_starts != 0));
        //                        if (seg_starts == 0)
        //                            return false;
        //
        //                        --seg_starts;
        //                        if (seg_starts != 0)
        //                            break; // this } char is part of the
        //                            segment rule
        //                                   // itself
        //
        //#if CXX20
        //                        stl::basic_string_view<CharT>
        //                        seg_rule(seg_start, tit);
        //#elif CXX17
        //                        std::basic_string_view<CharT>
        //                        seg_rule(seg_start,
        //                                                               tit -
        //                                                               seg_start);
        //#endif
        //
        //                        // going to find the segment in the path string too
        //                        for (auto it = pit;; ++it) {
        //                            switch (*it) {
        //                                case '\0':
        //                                    goto after_loop;
        //                                    //                                case '/':
        //                                    // todo
        //                            }
        //                        }
        //                        //                    after_loop:
        //                        // todo
        //                        //                        parse_seg(seg_rule, );
        //                    }
        //                    break;
        //                case '\\': // escape character
        //                    // we just don't care what the next char is. even if it's {
        //                    // or } which they have meanings in this algorithm.
        //                    ++tit;
        //                    if (*tit != '}' && *tit != '{')
        //                        --tit;
        //                    break;
        //                default:
        //                    // it's not an error, it's normal to return false here.
        //                    if (!seg_start && *tit != *pit)
        //                        return false;
        //            }
        //
        //            ++tit;
        //            pit += seg_start != nullptr; // only add 1 if we're not in
        //            a segment
        //        }
        return false;
    }

    struct tpath_condition {
      protected:
        stl::string_view tpath_str;

      public:
        constexpr tpath_condition(stl::string_view str) noexcept
          : tpath_str(str) {
        }

        constexpr tpath_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] inline bool
        operator()(RequestType const& req) const noexcept {
            return tpath_check(tpath_str, req.request_uri());
        }
    };

    /**
     * Features:
     *   - [ ] Type
     *     - [ ] Default type
     *   - [ ] Validating the segments with a custom method
     *   - [ ] Partial segments: segments that are not between two slashes
     *   - [ ] Naming the segments
     *   - [ ] Variadic segments: segments that contain multiple path segments
     *   - [ ] Default value for segments
     *     - [ ] string as default value
     *     - [ ] integer types as default value
     *     - [ ] custom type as a default value
     *     - [ ] custom object as default value
     *   - [ ] Making a segment optional
     *   - [ ] Custom SegTypes (Segment Types):
     *     - [ ]
     * Examples of tpath:
     *   - /{int:user_id}/profile
     *   - /{username:username}/profile
     *   - /{int:}
     *   - /{email:}
     *   - /page/{uint:page_num}.html
     *   - /product/{product_list:prod_name}/view
     *   - /view/{view_name}
     *   - /{string:one}/{two}
     *   - /{slugs...}/page/{uint:page_num}
     *   - /{string:controller=Home}/{action=Index}/{id?}
     * Attention: getting those segments are the responsibility of the
     * "route" class. We will define the implementation for it here, but the
     * final user should get the data from there; they can use this feature
     * directly here, but it looks nicer if they do it there.
     */
    //    struct tpath : public valve<tpath_condition> {
    //        using valve<tpath_condition>::valve;
    //    };
    //
    //    constexpr tpath operator""_tpath(const char* str, std::size_t len) {
    //        return tpath{std::string_view{str, len}};
    //    }
    //
} // namespace webpp

#endif // WEBPP_TPATH_H
