/*
 * Created by moisrex on 11/04/22.
 * Initial source from: https://github.com/kainjow/Mustache.git
 *
 * Boost Software License - Version 1.0
 *
 * Mustache
 * Copyright 2015-2020 Kevin Wojniak
 * Copyright 2022      Mohammad Bahoosh
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef WEBPP_MUSTACHE_VIEW_HPP
#define WEBPP_MUSTACHE_VIEW_HPP


#include "../strings/trim.hpp"
#include "../traits/enable_traits.hpp"
#include "../utils/functional.hpp"
#include "data_view.hpp"
#include "html.hpp"
#include "view_concepts.hpp"
#include "webpp/traits/traits.hpp"

#include <array>
#include <variant>


namespace webpp::views {

    template <Traits TraitsType>
    struct mustache_data_view_settings {
        using traits_type = TraitsType;
        static constexpr view_data_flags acceptable_types{data_views::boolean,
                                                          data_views::lambda,
                                                          data_views::string,
                                                          data_views::list,
                                                          data_views::variant};
    };

    template <Traits TraitsType>
    using mustache_data_view_types = data_view<mustache_data_view_settings<TraitsType>{}>;

    template <Traits TraitsType>
    using mustache_data_view = typename mustache_data_view_types<TraitsType>::type;

    template <typename TraitsType>
    struct basic_renderer {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;

        using type1 = stl::function<string_type(string_view_type)>;
        using type2 = stl::function<string_type(string_view_type, bool escaped)>;

        string_type operator()(const string_type& text) const {
            return func1(text);
        }

        string_type operator()(const string_type& text, bool escaped) const {
            return func2(text, escaped);
        }

      private:
        basic_renderer(const type1& t1, const type2& t2) : func1(t1), func2(t2) {}

        const type1& func1;
        const type2& func2;

        template <typename StringType>
        friend class mustache_view;
    };



    /**
     * A pair of delimiter type; in mustache you can change the delimiter you're using for variables and
     * stuff.
     */
    template <Traits TraitsType>
    struct delimiter_set {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;

        static constexpr auto default_begin = "{{";
        static constexpr auto default_end   = "}}";

        string_view_type begin = default_begin;
        string_view_type end   = default_end;

        [[nodiscard]] constexpr bool is_default() const noexcept {
            return begin == default_begin && end == default_end;
        }
    };


    template <Traits TraitsType>
    struct context {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using data_view_type   = mustache_data_view<traits_type>;
        using data_value_type  = typename data_view_type::value_type;
        using items_type = traits::generalify_allocators<traits_type, stl::vector<const data_view_type*>>;
        using items_value_type = typename items_type::value_type;

      private:
        items_type items;

      public:
        template <EnabledTraits ET>
        context(ET&& et, const data_view_type* data)
          : items{et.alloc_pack.template general_allocator<items_value_type>()} {
            push(data);
        }


        void push(const data_view_type* data) {
            items.insert(items.begin(), data);
        }

        void pop() {
            items.erase(items.begin());
        }

        const data_value_type* get(string_view_type name) const {
            // process {{.}} name
            if (name.size() == 1 && name.at(0) == '.') {
                return items.front();
            }
            if (name.find('.') == string_view_type::npos) {
                // process normal name without having to split which is slower
                for (const auto& item : items) {
                    const auto var = item->get(name);
                    if (var) {
                        return var;
                    }
                }
                return nullptr;
            }
            // process x.y-like name
            const auto names = split(name, '.');
            for (const auto& item : items) {
                auto var = item;
                for (const auto& n : names) {
                    var = var->get(n);
                    if (!var) {
                        break;
                    }
                }
                if (var) {
                    return var;
                }
            }
            return nullptr;
        }

        const data_value_type* get_partial(string_view_type name) const {
            for (const auto& item : items) {
                const auto var = item->get(name);
                if (var) {
                    return var;
                }
            }
            return nullptr;
        }

        context(const context&) = delete;
        context& operator=(const context&) = delete;
    };




    template <Traits TraitsType>
    struct line_buffer_state {
        using traits_type = TraitsType;
        using string_type = traits::general_string<traits_type>;

        string_type data;
        bool        contained_section_tag = false;

        [[nodiscard]] constexpr bool is_empty_or_contains_only_whitespace() const noexcept {
            for (const auto ch : data) {
                // don't look at newlines
                if (ch != ' ' && ch != '\t') {
                    return false;
                }
            }
            return true;
        }

        constexpr void clear() {
            data.clear();
            contained_section_tag = false;
        }
    };

    template <Traits TraitsType>
    struct context_internal {
        using traits_type = TraitsType;

        context<traits_type>&          ctx;
        delimiter_set<traits_type>     delim_set{};
        line_buffer_state<traits_type> line_buffer{};
    };

    enum class tag_type {
        text,
        variable,
        unescaped_variable,
        section_begin,
        section_end,
        section_begin_inverted,
        comment,
        partial,
        set_delimiter
    };

    template <Traits TraitsType>
    struct mstch_tag /* gcc doesn't allow "tag tag;" so rename the class :( */ {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;

        string_view_type                name;
        tag_type                        type = tag_type::text;
        stl::optional<string_view_type> section_text;
        delimiter_set<traits_type>      delim_set;

        [[nodiscard]] constexpr bool is_section_begin() const noexcept {
            return type == tag_type::section_begin || type == tag_type::section_begin_inverted;
        }
        [[nodiscard]] constexpr bool is_section_end() const noexcept {
            return type == tag_type::section_end;
        }
    };

    template <Traits TraitsType>
    struct context_pusher {
        using traits_type    = TraitsType;
        using data_view_type = mustache_data_view<traits_type>;

        context_pusher(context_internal<traits_type>& ctx, const data_view_type* data) : ctx_(ctx) {
            ctx.ctx.push(data);
        }
        ~context_pusher() {
            ctx_.ctx.pop();
        }
        context_pusher(const context_pusher&) = delete;
        context_pusher& operator=(const context_pusher&) = delete;

      private:
        context_internal<traits_type>& ctx_;
    };

    template <Traits TraitsType>
    struct component {
        using traits_type      = TraitsType;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using string_size_type = typename string_view_type::size_type;

        using children_type = traits::generalify_allocators<traits_type, stl::vector<component>>;

        string_view_type       text;
        mstch_tag<traits_type> tag;
        children_type          children;
        string_size_type       position = string_view_type::npos;

        enum class walk_control {
            walk, // "continue" is reserved :/
            stop,
            skip,
        };


        template <EnabledTraits ET>
        constexpr component(ET&& et, string_view_type t = "", string_size_type p = string_view_type::npos)
          : text(t),
            children{et.alloc_pack.template general_allocator<component>()},
            position(p) {}

        [[nodiscard]] constexpr bool is_text() const noexcept {
            return tag.type == tag_type::text;
        }

        [[nodiscard]] constexpr bool is_newline() const noexcept {
            return is_text() && ((text.size() == 2 && text[0] == '\r' && text[1] == '\n') ||
                                 (text.size() == 1 && (text[0] == '\n' || text[0] == '\r')));
        }

        [[nodiscard]] constexpr bool is_non_newline_whitespace() const noexcept {
            return is_text() && !is_newline() && text.size() == 1 && (text[0] == ' ' || text[0] == '\t');
        }

        template <typename WalkCallback>
        constexpr void walk_children(WalkCallback&& callback) {
            for (auto& child : children) {
                if (child.walk(callback) != walk_control::walk) {
                    break;
                }
            }
        }

      private:
        template <typename WalkCallback>
        constexpr walk_control walk(WalkCallback&& callback) {
            walk_control control{callback(*this)};
            if (control == walk_control::stop) {
                return control;
            } else if (control == walk_control::skip) {
                return walk_control::walk;
            }
            for (auto& child : children) {
                control = child.walk(callback);
                if (control == walk_control::stop) {
                    return control;
                }
            }
            return control;
        }
    };

    template <Traits TraitsType>
    struct mustache_view : enable_traits<TraitsType> {
        using etraits          = enable_traits<TraitsType>;
        using traits_type      = TraitsType;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using char_type        = typename string_type::value_type;

        using string_size_type  = typename string_type::size_type;
        using escape_handler    = stl::function<string_type(string_view_type)>;
        using component_type    = component<traits_type>;
        using walk_control_type = typename component_type::walk_control;

        using lambda_type = stl::variant<stl::function<string_type(string_view_type)>,
                                         stl::function<string_type(string_view_type, bool)>>;
        using map_type    = traits::generalify_allocators<traits_type, stl::map<string_type, string_type>>;
        using value_type  = stl::variant<bool, string_type, lambda_type, map_type>;
        using data_type   = traits::generalify_allocators<traits_type, stl::map<string_type, value_type>>;
        using data_view_types = mustache_data_view_types<traits_type>;
        using data_view_type  = mustache_data_view<traits_type>;
        using data_value_type = typename data_view_type::value_type;

        static_assert(DataView<data_value_type>, "data_view_type should be a span<data_value>");

        using lambda_view_type = typename data_view_types::lambda;

        using render_handler = stl::function<void(string_view_type)>;

        static constexpr auto MUSTACHE_CAT = "MustacheView";

      private:
        string_type    error_msg{this->alloc_pack.template general_allocator<char_type>()};
        component_type root_component;
        escape_handler escaper = [this](auto&& val) {
            string_type out{this->alloc_pack.template general_allocator<char_type>()};
            html_escape(val, out);
            return out;
        };

      public:
        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, mustache_view>)
        constexpr mustache_view(ET&& et) noexcept : etraits{et}, root_component{et} {}

        constexpr mustache_view(mustache_view const&)     = default;
        constexpr mustache_view(mustache_view&&) noexcept = default;

        constexpr mustache_view& operator=(mustache_view const&) = default;
        constexpr mustache_view& operator=(mustache_view&&) noexcept = default;

        constexpr void scheme(string_view_type input) {
            delimiter_set<traits_type> delim_set;
            parse(input, delim_set);
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return error_msg.empty();
        }

        [[nodiscard]] constexpr string_view_type error_message() const noexcept {
            return error_msg;
        }

        constexpr void set_custom_escape(const escape_handler& escape_fn) {
            escaper = escape_fn;
        }

        template <typename stream_type>
        constexpr stream_type& render(data_view_type const& data, stream_type& stream) {
            render(data, [&stream](string_view_type str) {
                stream << str;
            });
            return stream;
        }

        constexpr string_type render(data_view_type const& data) {
            stl::basic_ostringstream<typename string_type::value_type> ss;
            return render(data, ss).str();
        }

        template <typename stream_type>
        constexpr stream_type& render(context<traits_type>& ctx, stream_type& stream) {
            context_internal<traits_type> context{ctx};
            render(
              [&stream](string_view_type str) {
                  stream << str;
              },
              context);
            return stream;
        }

        constexpr string_type render(context<traits_type>& ctx) {
            stl::basic_ostringstream<typename string_type::value_type> ss;
            return render(ctx, ss).str();
        }

        constexpr void render(data_view_type const& data, const render_handler& handler) {
            if (!is_valid()) {
                return;
            }
            context<traits_type>          ctx{&data};
            context_internal<traits_type> context{ctx};
            render(handler, context);
        }


        template <PossibleDataTypes DT = data_type>
        constexpr void render(string_type& out, DT&& data) {
            if (!is_valid()) {
                return;
            }
            if constexpr (stl::same_as<stl::remove_cvref_t<DT>, data_view_type>) {
                context<traits_type>          ctx{*this, &data};
                context_internal<traits_type> context{.ctx = ctx};
                // todo: optimization chance: out::reserve
                render(
                  [&]<typename ContentT>(ContentT&& content) {
                      out += stl::forward<ContentT>(content);
                  },
                  context);
            } else if constexpr (stl::same_as<DT, data_type> || istl::Collection<DT>) {
                using data_view_value_type = typename data_view_type::value_type;
                auto data_vec = object::make_general<stl::vector<data_view_value_type>>(this->alloc_pack);
                data_vec.reserve(data.size());
                stl::transform(stl::begin(data),
                               stl::end(data),
                               stl::back_inserter(data_vec),
                               [this](auto&& item) -> data_view_value_type {
                                   auto const& [key, value] = item;
                                   return {*this, key, value};
                               });
                render<data_view_type>(out, data_view_type{data_vec.begin(), data_vec.end()});
            } else {
                this->logger.error(
                  MUSTACHE_CAT,
                  "We don't understand the data you passed the mustache renderer, we're gonna ignore them.");
            }
        }

      private:
        template <EnabledTraits ET>
        constexpr mustache_view(ET&& et, string_view_type input, context_internal<traits_type>& ctx)
          : mustache_view(et) {
            parser(input, ctx);
        }



        /////// Parser


        constexpr static stl::array<string_view_type, 5> whitespace{{"\r\n", "\n", "\r", " ", "\t"}};

        constexpr void parse(string_view_type input, delimiter_set<traits_type>& delim_set) {
            using streamstring = stl::basic_ostringstream<typename string_type::value_type>;

            const string_view_type brace_delimiter_end_unescaped("}}}");
            const string_size_type input_size{input.size()};

            bool current_delimiter_is_brace{delim_set.is_default()};

            using sections_type       = stl::vector<component_type*>;
            using section_starts_type = stl::vector<string_size_type>;

            auto sections       = object::make_local<sections_type>(this->alloc_pack, &root_component);
            auto section_starts = object::make_local<section_starts_type>(this->alloc_pack);

            auto             current_text          = object::make_local<string_type>(this->alloc_pack);
            string_size_type current_text_position = string_type::npos;

            current_text.reserve(input_size);

            const auto process_current_text = [this, &current_text, &current_text_position, &sections]() {
                if (!current_text.empty()) {
                    const component_type comp{*this, current_text, current_text_position};
                    sections.back()->children.push_back(comp);
                    current_text.clear();
                    current_text_position = string_type::npos;
                }
            };


            for (string_size_type input_position = 0; input_position != input_size;) {
                bool parse_tag = false;

                if (input.compare(input_position, delim_set.begin.size(), delim_set.begin) == 0) {
                    process_current_text();

                    // Tag start delimiter
                    parse_tag = true;
                } else {
                    bool parsed_whitespace = false;
                    for (const auto& whitespace_text : whitespace) {
                        if (input.compare(input_position, whitespace_text.size(), whitespace_text) == 0) {
                            process_current_text();

                            const component_type comp{*this, whitespace_text, input_position};
                            sections.back()->children.push_back(comp);
                            input_position += whitespace_text.size();

                            parsed_whitespace = true;
                            break;
                        }
                    }

                    if (!parsed_whitespace) {
                        if (current_text.empty()) {
                            current_text_position = input_position;
                        }
                        current_text.append(1, input[input_position]);
                        input_position++;
                    }
                }

                if (!parse_tag) {
                    continue;
                }

                // Find the next tag start delimiter
                const string_size_type tag_location_start = input_position;

                // Find the next tag end delimiter
                string_size_type       tag_contents_location{tag_location_start + delim_set.begin.size()};
                const bool             tag_is_unescaped_var{current_delimiter_is_brace &&
                                                tag_location_start != (input_size - 2) &&
                                                input.at(tag_contents_location) == delim_set.begin.at(0)};
                const string_view_type current_tag_delimiter_end{
                  tag_is_unescaped_var ? brace_delimiter_end_unescaped : delim_set.end};
                const auto current_tag_delimiter_end_size = current_tag_delimiter_end.size();
                if (tag_is_unescaped_var) {
                    ++tag_contents_location;
                }
                const string_size_type tag_location_end{
                  input.find(current_tag_delimiter_end, tag_contents_location)};
                if (tag_location_end == string_view_type::npos) {
                    streamstring ss;
                    ss << "Unclosed tag at " << tag_location_start;
                    error_msg.assign(ss.str());
                    return;
                }

                // Parse tag
                string_view_type tag_contents =
                  input.substr(tag_contents_location, tag_location_end - tag_contents_location);
                ascii::trim(tag_contents);
                component_type comp{*this};
                if (!tag_contents.empty() && tag_contents[0] == '=') {
                    if (!parse_set_delimiter_tag(tag_contents, delim_set)) {
                        streamstring ss;
                        ss << "Invalid set delimiter tag at " << tag_location_start;
                        error_msg.assign(ss.str());
                        return;
                    }
                    current_delimiter_is_brace = delim_set.is_default();
                    comp.tag.type              = tag_type::set_delimiter;
                    comp.tag.delim_set         = delim_set;
                }
                if (comp.tag.type != tag_type::set_delimiter) {
                    parse_tag_contents(tag_is_unescaped_var, tag_contents, comp.tag);
                }
                comp.position = tag_location_start;
                sections.back()->children.push_back(comp);

                // Start next search after this tag
                input_position = tag_location_end + current_tag_delimiter_end_size;

                // Push or pop sections
                if (comp.tag.is_section_begin()) {
                    sections.push_back(&sections.back()->children.back());
                    section_starts.push_back(input_position);
                } else if (comp.tag.is_section_end()) {
                    if (sections.size() == 1) {
                        streamstring ss;
                        ss << "Unopened section \"" << comp.tag.name << "\" at " << comp.position;
                        error_msg.assign(ss.str());
                        return;
                    }
                    sections.back()->tag.section_text =
                      input.substr(section_starts.back(), tag_location_start - section_starts.back());
                    sections.pop_back();
                    section_starts.pop_back();
                }
            }

            process_current_text();

            // Check for sections without an ending tag
            root_component.walk_children([this](component_type& comp) -> walk_control_type {
                if (!comp.tag.is_section_begin()) {
                    return walk_control_type::walk;
                }
                if (comp.children.empty() || !comp.children.back().tag.is_section_end() ||
                    comp.children.back().tag.name != comp.tag.name) {
                    streamstring ss;
                    ss << "Unclosed section \"" << comp.tag.name << "\" at " << comp.position;
                    error_msg.assign(ss.str());
                    return walk_control_type::stop;
                }
                comp.children.pop_back(); // remove now useless end section component
                return walk_control_type::walk;
            });

            // fixme: what am I looking here?
            if (!error_msg.empty()) {
                return;
            }
        }

        constexpr bool is_set_delimiter_valid(string_view_type delimiter) const {
            // "Custom delimiters may not contain whitespace or the equals sign."
            // todo: optimize-able
            for (const auto ch : delimiter) {
                if (ch == '=' || stl::isspace(ch)) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] constexpr bool
        parse_set_delimiter_tag(string_view_type contents, delimiter_set<traits_type>& delimiter_set) const {
            // Smallest legal tag is "=X X="
            if (contents.size() < 5) {
                return false;
            }
            if (contents.back() != '=') {
                return false;
            }
            contents = contents.substr(1, contents.size() - 2); // todo: use remove_prefix and remove_suffix
            ascii::trim(contents);
            const auto spacepos = contents.find(' ');
            if (spacepos == string_view_type::npos) {
                return false;
            }
            const auto nonspace = contents.find_first_not_of(' ', spacepos + 1);
            assert(nonspace != string_type::npos);
            const auto begin = contents.substr(0, spacepos);
            const auto end   = contents.substr(nonspace, contents.size() - nonspace);
            if (!is_set_delimiter_valid(begin) || !is_set_delimiter_valid(end)) {
                return false;
            }
            delimiter_set.begin = begin;
            delimiter_set.end   = end;
            return true;
        }

        constexpr void parse_tag_contents(bool                    is_unescaped_var,
                                          string_view_type        contents,
                                          mstch_tag<traits_type>& tag) const {
            if (is_unescaped_var) {
                tag.type = tag_type::unescaped_variable;
                tag.name = contents;
            } else if (contents.empty()) {
                tag.type = tag_type::variable;
                tag.name = {};
            } else {
                switch (contents.at(0)) {
                    case '#': tag.type = tag_type::section_begin; break;
                    case '^': tag.type = tag_type::section_begin_inverted; break;
                    case '/': tag.type = tag_type::section_end; break;
                    case '>': tag.type = tag_type::partial; break;
                    case '&': tag.type = tag_type::unescaped_variable; break;
                    case '!': tag.type = tag_type::comment; break;
                    default: tag.type = tag_type::variable; break;
                }
                if (tag.type == tag_type::variable) {
                    tag.name = contents;
                } else {
                    string_type name{contents};
                    name.erase(name.begin());
                    ascii::trim(name);
                    tag.name = name;
                }
            }
        }



        ////// Renderer





        constexpr string_type render(context_internal<traits_type>& ctx) {
            stl::basic_ostringstream<typename string_type::value_type> ss;
            render(
              [&ss](string_view_type str) {
                  ss << str;
              },
              ctx);
            return ss.str();
        }

        constexpr void
        render(const render_handler& handler, context_internal<traits_type>& ctx, bool root_renderer = true) {
            root_component.walk_children([&handler, &ctx, this](component_type& comp) -> walk_control_type {
                return render_component(handler, ctx, comp);
            });
            // process the last line, but only for the top-level renderer
            if (root_renderer) {
                render_current_line(handler, ctx.line_buffer, nullptr);
            }
        }

        constexpr void render_current_line(const render_handler&           handler,
                                           line_buffer_state<traits_type>& line_buffer,
                                           const component_type*           comp) const {
            // We're at the end of a line, so check the line buffer state to see
            // if the line had tags in it, and also if the line is now empty or
            // contains whitespace only. if this situation is true, skip the line.
            if (!line_buffer.contained_section_tag || !line_buffer.is_empty_or_contains_only_whitespace()) {
                handler(line_buffer.data);
                if (comp) {
                    handler(comp->text);
                }
            }
            line_buffer.clear();
        }

        constexpr walk_control_type render_component(const render_handler&          handler,
                                                     context_internal<traits_type>& ctx,
                                                     component_type&                comp) {
            if (comp.is_text()) {
                if (comp.is_newline()) {
                    render_current_line(handler, ctx.line_buffer, &comp);
                } else {
                    ctx.line_buffer.data.append(comp.text);
                }
                return walk_control_type::walk;
            }

            const mstch_tag<traits_type>& tag{comp.tag};
            const data_value_type*        var = nullptr;
            switch (tag.type) {
                case tag_type::variable:
                case tag_type::unescaped_variable:
                    if ((var = ctx.ctx.get(tag.name)) != nullptr) {
                        if (!render_variable(handler, var, ctx, tag.type == tag_type::variable)) {
                            return walk_control_type::stop;
                        }
                    }
                    break;
                case tag_type::section_begin:
                    if ((var = ctx.ctx.get(tag.name)) != nullptr) {
                        if (stl::holds_alternative<lambda_view_type>(var->value_ptr())) {
                            if (!render_lambda(handler,
                                               var,
                                               ctx,
                                               render_lambda_escape::optional,
                                               *comp.tag.section_text,
                                               true)) {
                                return walk_control_type::stop;
                            }
                        } else if (!var->is_false() && !var->is_empty_list()) {
                            render_section(handler, ctx, comp, var);
                        }
                    }
                    return walk_control_type::skip;
                case tag_type::section_begin_inverted:
                    if ((var = ctx.ctx.get(tag.name)) == nullptr || var->is_false() || var->is_empty_list()) {
                        render_section(handler, ctx, comp, var);
                    }
                    return walk_control_type::skip;
                case tag_type::partial:
                    if ((var = ctx.ctx.get_partial(tag.name)) != nullptr &&
                        (var->is_partial() || var->is_string())) {
                        const auto& partial_result =
                          var->is_partial() ? var->partial_value()() : var->string_value();
                        mustache_view tmpl{partial_result};
                        tmpl.set_custom_escape(escaper);
                        if (!tmpl.is_valid()) {
                            error_msg = tmpl.error_message();
                        } else {
                            tmpl.render(handler, ctx, false);
                            if (!tmpl.is_valid()) {
                                error_msg = tmpl.error_message();
                            }
                        }
                        if (!tmpl.is_valid()) {
                            return walk_control_type::stop;
                        }
                    }
                    break;
                case tag_type::set_delimiter: ctx.delim_set = *comp.tag.delim_set; break;
                default: break;
            }

            return walk_control_type::walk;
        }

        enum class render_lambda_escape {
            escape,
            unescape,
            optional,
        };

        constexpr bool render_lambda(const render_handler&          handler,
                                     const data_view_type*          var,
                                     context_internal<traits_type>& ctx,
                                     render_lambda_escape           escape,
                                     string_view_type               text,
                                     bool                           parse_with_same_context) {
            const typename basic_renderer<traits_type>::type2 render2 =
              [this, &ctx, parse_with_same_context, escape](string_view_type txt, bool escaped) {
                  const auto process_template =
                    [this, &ctx, escape, escaped](mustache_view& tmpl) -> string_type {
                      if (!tmpl.is_valid()) {
                          error_msg = tmpl.error_message();
                          return {};
                      }
                      context_internal<traits_type> render_ctx{ctx.ctx}; // start a new line_buffer
                      const auto                    str = tmpl.render(render_ctx);
                      if (!tmpl.is_valid()) {
                          error_msg = tmpl.error_message();
                          return {};
                      }
                      bool do_escape = false;
                      switch (escape) {
                          case render_lambda_escape::escape: do_escape = true; break;
                          case render_lambda_escape::unescape: do_escape = false; break;
                          case render_lambda_escape::optional: do_escape = escaped; break;
                      }
                      return do_escape ? escaper(str) : str;
                  };
                  if (parse_with_same_context) {
                      mustache_view tmpl{txt, ctx};
                      tmpl.set_custom_escape(escaper);
                      return process_template(tmpl);
                  }
                  mustache_view tmpl{txt};
                  tmpl.set_custom_escape(escaper);
                  return process_template(tmpl);
              };
            const typename basic_renderer<traits_type>::type1 render = [&render2](string_view_type txt) {
                return render2(txt, false);
            };
            if (var->is_lambda2()) {
                const basic_renderer<traits_type> renderer{render, render2};
                ctx.line_buffer.data.append(var->lambda2_value()(text, renderer));
            } else {
                render_current_line(handler, ctx, nullptr);
                ctx.line_buffer.data.append(render(var->lambda_value()(text)));
            }
            return error_msg.empty();
        }

        constexpr bool render_variable(const render_handler&          handler,
                                       const data_value_type*         var,
                                       context_internal<traits_type>& ctx,
                                       bool                           escaped) {
            if (auto val_str = stl::get_if<string_view_type>(var->value_ptr())) {
                ctx.line_buffer.data.append(escaped ? escaper(val_str) : val_str);
            } else if (auto val_lambda = stl::get_if<lambda_view_type>(var->value_ptr())) {
                const render_lambda_escape escape_opt =
                  escaped ? render_lambda_escape::escape : render_lambda_escape::unescape;
                return render_lambda(handler, val_lambda, ctx, escape_opt, {}, false);
                {
                    using streamstring = stl::basic_ostringstream<typename string_type::value_type>;
                    streamstring ss;
                    ss << "Lambda with render argument is not allowed for regular variables";
                    error_msg = ss.str();
                    return false;
                }
            }
            return true;
        }

        constexpr void render_section(const render_handler&          handler,
                                      context_internal<traits_type>& ctx,
                                      component_type&                incomp,
                                      const data_view_type*          var) {
            const auto callback = [&handler, &ctx, this](component_type& comp) -> walk_control_type {
                return render_component(handler, ctx, comp);
            };
            if (var && var->is_non_empty_list()) {
                for (const auto& item : var->list_value()) {
                    // account for the section begin tag
                    ctx.line_buffer.contained_section_tag = true;

                    const context_pusher<string_type> ctxpusher{ctx, &item};
                    incomp.walk_children(callback);

                    // ctx may have been cleared. account for the section end tag
                    ctx.line_buffer.contained_section_tag = true;
                }
            } else if (var) {
                // account for the section begin tag
                ctx.line_buffer.contained_section_tag = true;

                const context_pusher<string_type> ctxpusher{ctx, var};
                incomp.walk_children(callback);

                // ctx may have been cleared. account for the section end tag
                ctx.line_buffer.contained_section_tag = true;
            } else {
                // account for the section begin tag
                ctx.line_buffer.contained_section_tag = true;

                incomp.walk_children(callback);

                // ctx may have been cleared. account for the section end tag
                ctx.line_buffer.contained_section_tag = true;
            }
        }
    };

} // namespace webpp::views

#endif // WEBPP_MUSTACHE_VIEW_HPP
