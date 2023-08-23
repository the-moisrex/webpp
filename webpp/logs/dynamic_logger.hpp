// Created by moisrex on 8/23/23.

#ifndef WEBPP_DYNAMIC_LOGGER_HPP
#define WEBPP_DYNAMIC_LOGGER_HPP

#include "../std/memory.hpp"
#include "default_logger.hpp"
#include "log_concepts.hpp"

namespace webpp {

    namespace details {
        struct dynamic_logger_interface {};
    } // namespace details

    template <typename>
    struct basic_dynamic_logger;


    /// dynamic interface
    template <>
    struct basic_dynamic_logger<details::dynamic_logger_interface> {
        virtual ~basic_dynamic_logger() = default;
    };


    /// implementation of each logger
    template <Logger LoggerType>
    struct basic_dynamic_logger<LoggerType> final : basic_dynamic_logger<details::dynamic_logger_interface> {
        using logger_type = LoggerType;

        basic_dynamic_logger(basic_dynamic_logger const&)                = default;
        basic_dynamic_logger(basic_dynamic_logger&&) noexcept            = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger const&)     = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger&&) noexcept = default;
        ~basic_dynamic_logger()                                          = default;

        basic_dynamic_logger(logger_type&& logger) noexcept : actual_logger{stl::move(logger)} {}
        basic_dynamic_logger(logger_type const& logger) noexcept : actual_logger{logger} {}


      private:
        logger_type actual_logger;
    };


    /**
     * Dynamic logger
     * This logger is designed to be cheap enough so you can copy it instead of passing it around using
     * references.
     */
    template <>
    struct basic_dynamic_logger<void> {

        // Copying the dynamic logger is easy, and you don't need to pass it by reference to use it
        basic_dynamic_logger(basic_dynamic_logger const&)                = default;
        basic_dynamic_logger(basic_dynamic_logger&&) noexcept            = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger const&)     = default;
        basic_dynamic_logger& operator=(basic_dynamic_logger&&) noexcept = default;
        ~basic_dynamic_logger()                                          = default;

        /// construct the specified logger with the specified allocator and pass in the arguments
        template <Logger LoggerType, typename Allocator, typename... Args>
            requires(stl::is_constructible_v<LoggerType, Args...>)
        constexpr basic_dynamic_logger(stl::type_identity<LoggerType>,
                                       Allocator const& alloc = {},
                                       Args&&... args)
          : logger_ptr{
              stl::allocate_shared<basic_dynamic_logger<LoggerType>>(alloc, stl::forward<Args>(args)...)} {}


        /// default logger
        template <typename Allocator, typename... Args>
        constexpr basic_dynamic_logger(Allocator const& alloc = {}, Args&&... args)
          : basic_dynamic_logger{stl::type_identity<default_logger>{}, alloc, stl::forward<Args>(args)...} {}

        /// change the logger, use the specified allocator
        template <Logger LoggerType, typename Allocator>
        void set_logger(LoggerType&& logger, Allocator const& alloc = {}) {
            using new_logger_type = basic_dynamic_logger<LoggerType>;
            logger_ptr = stl::allocate_shared<new_logger_type>(alloc, stl::forward<LoggerType>(logger));
        }

        /// construct a new logger with the specified arguments in-place using the specified allocator
        template <Logger LoggerType, typename Allocator, typename... Args>
            requires(stl::is_constructible_v<LoggerType, Args...>)
        void emplace_logger(Allocator const& alloc = {}, Args&&... args) {
            using new_logger_type = basic_dynamic_logger<LoggerType>;
            logger_ptr            = stl::allocate_shared<new_logger_type>(alloc, stl::forward<Args>(args)...);
        }

      private:
        using dynamic_logger_type = basic_dynamic_logger<details::dynamic_logger_interface>;
        using default_logger_type = basic_dynamic_logger<default_logger>;

        stl::shared_ptr<dynamic_logger_type> logger_ptr;
    };

    using dynamic_logger = basic_dynamic_logger<void>;
} // namespace webpp

#endif // WEBPP_DYNAMIC_LOGGER_HPP
