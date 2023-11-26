// Created by moisrex on 8/29/23.


#include "./project.hpp"

#include <webpp/json/defaultjson.hpp>

using namespace webpp::sdk;
using namespace webpp::json;

class project::impl {
  private:
    friend struct project;

    fs::path   path{};
    document<> conf_doc;
};

project::project() : pimpl{stl::make_unique<impl>()} {
    // todo: read the config file
}

project::project(project&& other) noexcept            = default;
project& project::operator=(project&& other) noexcept = default;
webpp::sdk::project::~project()                       = default;

std::filesystem::path project::path() const {
    return pimpl->path;
}

void project::path(std::filesystem::path new_path) {
    pimpl->path = stl::move(new_path);
}

std::string_view project::name() const noexcept {
    return pimpl->conf_doc["name"].as_string_view();
}

void project::name(std::string_view new_name) {
    pimpl->conf_doc["name"] = stl::string{new_name.data(), new_name.size()};
}
