// Created by moisrex on 8/29/23.


#include "./imprinter.hpp"

using namespace webpp::sdk;
using webpp::stl::shared_ptr;

proj_imprinter::proj_imprinter(std::shared_ptr<progress_bars>   inp_bars,
                               std::shared_ptr<command_options> inp_opts)
  : bars{stl::move(inp_bars)},
    options{stl::move(inp_opts)} {}

shared_ptr<project> proj_imprinter::get_project() const noexcept {
    return proj;
}

shared_ptr<proj_template> proj_imprinter::get_template() const noexcept {
    return tmpl;
}

void proj_imprinter::set_project(project&& new_proj) {
    proj = stl::make_shared<project>(stl::move(new_proj));
}

void proj_imprinter::set_template(proj_template&& new_tmpl) {
    tmpl = stl::make_shared<proj_template>(stl::move(new_tmpl));
}

// Start imprinting
void proj_imprinter::start() {}
