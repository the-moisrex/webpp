// Created by moisrex on 9/23/20.

#ifndef WEBPP_COMMON_URILS_PCH_H
#define WEBPP_COMMON_URILS_PCH_H

#include <string>

static std::string str_generator(std::size_t size = 1000) {
  std::string_view data = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string str = "";
  str.reserve(size);
  for (std::size_t i = 0 ; i < size / data.size(); i++) {
      str.append(data);
  }
  std::random_shuffle(str.begin(), str.end());
  return str.substr(0, size);
}

#endif // WEBPP_COMMON_URILS_PCH_H
