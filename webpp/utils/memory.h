#ifndef WEBPP_MEMORY_H
#define WEBPP_MEMORY_H

// Created by moisrex on 12/8/19.

/**
 * Get the available memory
 * This method will calculate the available memory every time you call it.
 * Try to use timed_available_memory if you need a more performant version of
 * this function.
 * @return the amount of available memory or 0 if the info is not available
 */
unsigned long long available_memory() noexcept;

unsigned long long timed_available_memory() noexcept;

#endif // WEBPP_MEMORY_H
