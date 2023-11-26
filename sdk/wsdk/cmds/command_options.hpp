// Created by moisrex on 8/22/23.

#ifndef WEBPP_COMMAND_OPTIONS_HPP
#define WEBPP_COMMAND_OPTIONS_HPP

#include <webpp/logs/dynamic_logger.hpp>
#include <webpp/std/memory.hpp>
#include <webpp/std/span.hpp>
#include <webpp/strings/string_tokenizer.hpp>

namespace webpp::sdk {

    struct global_options {
        enum verbosity {
            normal,   // default
            moderate, // moderate amount of information
            debug     // all the information you can give
        } verbose = normal;
    };

    using row_view = stl::span<const stl::pair<stl::string_view, stl::string_view>>;

    struct output_port {
        output_port()                                       = default;
        output_port(output_port const&) noexcept            = default;
        output_port(output_port&&) noexcept                 = default;
        output_port& operator=(output_port const&) noexcept = default;
        output_port& operator=(output_port&&) noexcept      = default;

        virtual ~output_port() = default;

        virtual void notify(stl::string_view)                    = 0;
        virtual void send_table(stl::string_view name, row_view) = 0;

        template <istl::StringViewifiable StrT>
        output_port& operator<<(StrT&& str) {
            notify(istl::string_viewify(stl::forward<StrT>(str)));
            return *this;
        }
    };

    struct stdout_output_port final : output_port {
        void notify(stl::string_view) override;
        void send_table(stl::string_view name, row_view rows) override;
    };

    static constexpr charset WHITESPACES{' ', '\t', '\n', '\r'};

    struct command_options {
        using tokenizer_type = string_tokenizer<>;

        command_options(stl::string_view             command,
                        stl::shared_ptr<output_port> inp_out    = stl::make_shared<stdout_output_port>(),
                        webpp::dynamic_logger        inp_logger = {})
          : m_tokenizer{command},
            m_output{stl::move(inp_out)},
            m_logger{stl::move(inp_logger)} {}

        command_options(command_options&&) noexcept            = default;
        command_options(command_options const&)                = delete;
        command_options& operator=(command_options const&)     = delete;
        command_options& operator=(command_options&&) noexcept = default;
        ~command_options()                                     = default;

        [[nodiscard]] global_options& options() noexcept {
            return m_options;
        }

        [[nodiscard]] tokenizer_type& tokenizer() noexcept {
            return m_tokenizer;
        }

        output_port& output() noexcept {
            return *m_output;
        }

        [[nodiscard]] dynamic_logger logger() const noexcept {
            return m_logger;
        }

      private:
        global_options               m_options;
        tokenizer_type               m_tokenizer;
        stl::shared_ptr<output_port> m_output;
        webpp::dynamic_logger        m_logger;
    };

} // namespace webpp::sdk

#endif // WEBPP_COMMAND_OPTIONS_HPP
