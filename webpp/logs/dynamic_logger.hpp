// Created by moisrex on 8/23/23.

#ifndef WEBPP_DYNAMIC_LOGGER_HPP
#define WEBPP_DYNAMIC_LOGGER_HPP

#include "../memory/allocators.hpp"
#include "../std/memory.hpp"
#include "../std/string_view.hpp"
#include "log_concepts.hpp"
#include "logger.hpp"
#include "void_logger.hpp"

namespace webpp {

    namespace details {
        struct dynamic_logger_interface {};
    } // namespace details

    template <typename>
    struct [[nodiscard]] basic_dynamic_logger;

    /// dynamic interface
    template <>
    struct [[nodiscard]] basic_dynamic_logger<details::dynamic_logger_interface> {
        using string_view_type = stl::string_view;

        basic_dynamic_logger() noexcept                                       = default;
        basic_dynamic_logger(basic_dynamic_logger const&) noexcept            = default;
        basic_dynamic_logger(basic_dynamic_logger&&) noexcept                 = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger const&) noexcept = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger&&) noexcept      = default;
        virtual ~basic_dynamic_logger()                                       = default;

        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_DEFINE_METHOD(method_name)                                                             \
    virtual void method_name(string_view_type msg)                                              = 0; \
    virtual void method_name(string_view_type cat, string_view_type msg)                        = 0; \
    virtual void method_name(string_view_type cat, string_view_type msg, stl::error_code)       = 0; \
    virtual void method_name(string_view_type cat, string_view_type msg, stl::exception const&) = 0; \
    virtual void method_name(string_view_type msg, stl::error_code)                             = 0; \
    virtual void method_name(string_view_type msg, stl::exception const&)                       = 0;

        WEBPP_DEFINE_METHOD(error)
        WEBPP_DEFINE_METHOD(info)
        WEBPP_DEFINE_METHOD(warning)
        WEBPP_DEFINE_METHOD(critical)
#undef WEBPP_DEFINE_METHOD
    };

    /// implementation of each logger
    template <Logger LoggerType>
    struct [[nodiscard]] basic_dynamic_logger<LoggerType> final
      : basic_dynamic_logger<details::dynamic_logger_interface> {
        using logger_type      = LoggerType;
        using logger_interface = basic_dynamic_logger<details::dynamic_logger_interface>;
        using string_view_type = typename logger_interface::string_view_type;

        basic_dynamic_logger(basic_dynamic_logger const&)                = default;
        basic_dynamic_logger(basic_dynamic_logger&&) noexcept            = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger const&)     = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger&&) noexcept = default;
        ~basic_dynamic_logger() override                                 = default;

        explicit basic_dynamic_logger(logger_type&& inp_logger) noexcept : actual_logger{stl::move(inp_logger)} {}

        explicit basic_dynamic_logger(logger_type const& inp_logger) noexcept : actual_logger{inp_logger} {}

        template <typename... Args>
            requires(stl::is_constructible_v<logger_type, Args...>)
        explicit basic_dynamic_logger(Args&&... args) noexcept(stl::is_nothrow_constructible_v<logger_type, Args...>)
          : actual_logger{stl::forward<Args>(args)...} {}

        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_DEFINE_METHOD(method_name)                                                               \
    void method_name(string_view_type msg) override {                                                  \
        actual_logger.method_name(msg);                                                                \
    }                                                                                                  \
    void method_name(string_view_type cat, string_view_type msg) override {                            \
        actual_logger.method_name(cat, msg);                                                           \
    }                                                                                                  \
    void method_name(string_view_type cat, string_view_type msg, stl::error_code ecode) override {     \
        actual_logger.method_name(cat, msg, ecode);                                                    \
    }                                                                                                  \
    void method_name(string_view_type cat, string_view_type msg, stl::exception const& exp) override { \
        actual_logger.method_name(cat, msg, exp);                                                      \
    }                                                                                                  \
    void method_name(string_view_type msg, stl::error_code ecode) override {                           \
        actual_logger.method_name(msg, ecode);                                                         \
    }                                                                                                  \
    void method_name(string_view_type msg, stl::exception const& exp) override {                       \
        actual_logger.method_name(msg, exp);                                                           \
    }

        WEBPP_DEFINE_METHOD(error)
        WEBPP_DEFINE_METHOD(info)
        WEBPP_DEFINE_METHOD(warning)
        WEBPP_DEFINE_METHOD(critical)
#undef WEBPP_DEFINE_METHOD

      private:
        logger_type actual_logger;
    };

    /**
     * Dynamic logger
     * This logger is designed to be cheap enough so you can copy it instead of passing it around using
     * references.
     */
    template <>
    struct [[nodiscard]] basic_dynamic_logger<void> final {
        using logger_type = basic_dynamic_logger;
        using logger_ref  = basic_dynamic_logger;
        using logger_ptr  = basic_dynamic_logger*;

        using logger_interface = basic_dynamic_logger<details::dynamic_logger_interface>;
        using string_view_type = typename logger_interface::string_view_type;

        static constexpr bool is_debug = is_debug_build;

        // Copying the dynamic logger is easy, and you don't need to pass it by reference to use it
        basic_dynamic_logger(basic_dynamic_logger const&)                = default;
        basic_dynamic_logger(basic_dynamic_logger&&) noexcept            = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger const&)     = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger&&) noexcept = default;
        ~basic_dynamic_logger()                                          = default;

        /// construct the specified logger with the specified allocator and pass in the arguments
        template <Logger    LoggerType,
                  Allocator AllocT = default_allocator_t<basic_dynamic_logger<LoggerType>>,
                  typename... Args>
        explicit constexpr basic_dynamic_logger(
          [[maybe_unused]] stl::type_identity<LoggerType> inp_type,
          AllocT const&                                   inp_alloc = alloc,
          Args&&... args)
            requires(stl::is_constructible_v<LoggerType, Args...>)
          : m_logger_ptr{
              stl::allocate_shared<basic_dynamic_logger<LoggerType>>(inp_alloc, stl::forward<Args>(args)...)} {}

        /// default logger
        template <Allocator AllocT = default_allocator_t<basic_dynamic_logger<default_static_logger>>, typename... Args>
        explicit(false) constexpr basic_dynamic_logger(AllocT const& inp_alloc = alloc, Args&&... args)
          : m_logger_ptr{stl::allocate_shared<basic_dynamic_logger<default_static_logger>>(
              inp_alloc,
              stl::forward<Args>(args)...)} {}

        /// change the logger, use the specified allocator
        template <Logger LoggerType, typename Allocator = default_allocator_t<basic_dynamic_logger<LoggerType>>>
        void set_logger(LoggerType&& inp_logger, Allocator const& inp_alloc = alloc) {
            using new_logger_type = basic_dynamic_logger<LoggerType>;
            m_logger_ptr = stl::allocate_shared<new_logger_type>(inp_alloc, stl::forward<LoggerType>(inp_logger));
        }

        /// construct a new logger with the specified arguments in-place using the specified allocator
        template <Logger LoggerType,
                  typename Allocator = default_allocator_t<basic_dynamic_logger<LoggerType>>,
                  typename... Args>
            requires(stl::is_constructible_v<LoggerType, Args...>)
        void emplace_logger(Allocator const& inp_alloc = alloc, Args&&... args) {
            using new_logger_type = basic_dynamic_logger<LoggerType>;
            m_logger_ptr          = stl::allocate_shared<new_logger_type>(inp_alloc, stl::forward<Args>(args)...);
        }

        /// Disable the logging by setting the logger to use the void logger
        template <typename Allocator = default_allocator_t<basic_dynamic_logger<void_logger>>>
        void disable(Allocator const& inp_alloc = alloc) {
            emplace_logger<void_logger>(inp_alloc);
        }

        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_DEFINE_METHOD(method_name)                                                                   \
                                                                                                           \
    template <istl::StringViewifiable DetStrT>                                                             \
    void method_name(DetStrT&& details) const noexcept {                                                   \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<DetStrT>(details)));        \
    }                                                                                                      \
                                                                                                           \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                            \
    void method_name(CatStrT&& category, DetStrT&& details) const noexcept {                               \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<CatStrT>(category)),        \
                                  istl::view_of<string_view_type>(stl::forward<DetStrT>(details)));        \
    }                                                                                                      \
                                                                                                           \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                            \
    void method_name(CatStrT&& category, DetStrT&& details, stl::error_code const& ecode) const noexcept { \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<CatStrT>(category)),        \
                                  istl::view_of<string_view_type>(stl::forward<DetStrT>(details)),         \
                                  ecode);                                                                  \
    }                                                                                                      \
                                                                                                           \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                            \
    void method_name(CatStrT&& category, DetStrT&& details, stl::exception const& exp) const noexcept {    \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<CatStrT>(category)),        \
                                  istl::view_of<string_view_type>(stl::forward<DetStrT>(details)),         \
                                  exp);                                                                    \
    }                                                                                                      \
                                                                                                           \
    template <istl::StringViewifiable StrT>                                                                \
    void method_name(StrT&& details, stl::error_code const& ecode) const noexcept {                        \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<StrT>(details)), ecode);    \
    }                                                                                                      \
                                                                                                           \
    template <istl::StringViewifiable StrT>                                                                \
    void method_name(StrT&& details, stl::exception const& exp) const noexcept {                           \
        m_logger_ptr->method_name(istl::view_of<string_view_type>(stl::forward<StrT>(details)), exp);      \
    }                                                                                                      \
                                                                                                           \
    template <typename... OptsT>                                                                           \
    void method_name(if_debug_tag, OptsT&&... opts) const noexcept {                                       \
        if constexpr (is_debug) {                                                                          \
            m_logger_ptr->method_name(stl::forward<OptsT>(opts)...);                                       \
        }                                                                                                  \
    }


        WEBPP_DEFINE_METHOD(error)
        WEBPP_DEFINE_METHOD(info)
        WEBPP_DEFINE_METHOD(warning)
        WEBPP_DEFINE_METHOD(critical)
#undef WEBPP_DEFINE_METHOD


      private:
        using dynamic_logger_type = basic_dynamic_logger<details::dynamic_logger_interface>;
        using default_logger_type = basic_dynamic_logger<default_static_logger>;

        stl::shared_ptr<dynamic_logger_type> m_logger_ptr;
    };

    using dynamic_logger = basic_dynamic_logger<void>;
} // namespace webpp

#endif // WEBPP_DYNAMIC_LOGGER_HPP
