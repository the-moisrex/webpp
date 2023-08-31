// Created by moisrex on 8/29/23.


#include "./project.hpp"

using namespace webpp::sdk;


class project::impl {
  private:
    friend struct project;

    stl::string name{};
    fs::path    path{};
};

webpp::sdk::project::project() : pimpl{stl::make_unique<impl>()} {}

std::filesystem::path webpp::sdk::project::path() const {
    return pimpl->path;
}
void webpp::sdk::project::path(std::filesystem::path new_path) {
    pimpl->path = stl::move(new_path);
}
std::string_view webpp::sdk::project::name() const noexcept {
    return {pimpl->name.data(), pimpl->name.size()};
}
void webpp::sdk::project::name(std::string_view new_name) {
    pimpl->name = stl::string{new_name.data(), new_name.size()};
}
webpp::sdk::project::project(project&& other) noexcept : pimpl{stl::exchange(other.pimpl, nullptr)} {}
project& webpp::sdk::project::operator=(project&& other) noexcept {
    if (stl::addressof(other) != this) {
        pimpl = stl::move(other.pimpl);
    }
    return *this;
}
webpp::sdk::project::~project() = default;
