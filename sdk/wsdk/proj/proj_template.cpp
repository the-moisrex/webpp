// Created by moisrex on 8/29/23.

#include "./proj_template.hpp"

using namespace webpp::sdk;
using webpp::stl::shared_ptr;

class proj_template::impl {
    shared_ptr<command_options> options;

  public:
    impl(shared_ptr<command_options>&& inp_options) : options{stl::move(inp_options)} {}
};

webpp::sdk::proj_template::proj_template(std::shared_ptr<command_options> options)
  : pimpl{stl::make_unique<impl>(stl::move(options))} {}

proj_template::proj_template(proj_template&& other) noexcept : pimpl{stl::move(other.pimpl)} {}

proj_template& webpp::sdk::proj_template::operator=(proj_template&& other) noexcept {
    if (stl::addressof(other) != this) {
        pimpl = stl::move(other.pimpl);
    }
    return *this;
}

webpp::sdk::proj_template::~proj_template() = default;
