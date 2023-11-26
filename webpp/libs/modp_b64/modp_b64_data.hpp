#ifndef WEBPP_MODP_B64_DATA_HPP
#define WEBPP_MODP_B64_DATA_HPP

#include <array>
#include <bit>
#include <cstdint>

namespace webpp::modp_b64 {
    static constexpr auto CHAR62  = '+';
    static constexpr auto CHAR63  = '/';
    static constexpr auto CHARPAD = '=';

    static constexpr char e0[256] = {
      'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D', 'D', 'E', 'E', 'E', 'E',
      'F', 'F', 'F', 'F', 'G', 'G', 'G', 'G', 'H', 'H', 'H', 'H', 'I', 'I', 'I', 'I', 'J', 'J', 'J', 'J',
      'K', 'K', 'K', 'K', 'L', 'L', 'L', 'L', 'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
      'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R', 'S', 'S', 'S', 'S', 'T', 'T', 'T', 'T',
      'U', 'U', 'U', 'U', 'V', 'V', 'V', 'V', 'W', 'W', 'W', 'W', 'X', 'X', 'X', 'X', 'Y', 'Y', 'Y', 'Y',
      'Z', 'Z', 'Z', 'Z', 'a', 'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
      'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g', 'h', 'h', 'h', 'h', 'i', 'i', 'i', 'i',
      'j', 'j', 'j', 'j', 'k', 'k', 'k', 'k', 'l', 'l', 'l', 'l', 'm', 'm', 'm', 'm', 'n', 'n', 'n', 'n',
      'o', 'o', 'o', 'o', 'p', 'p', 'p', 'p', 'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
      't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v', 'w', 'w', 'w', 'w', 'x', 'x', 'x', 'x',
      'y', 'y', 'y', 'y', 'z', 'z', 'z', 'z', '0', '0', '0', '0', '1', '1', '1', '1', '2', '2', '2', '2',
      '3', '3', '3', '3', '4', '4', '4', '4', '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
      '8', '8', '8', '8', '9', '9', '9', '9', '+', '+', '+', '+', '/', '/', '/', '/'};

    static constexpr char e1[256] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
      'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
      'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

    static constexpr char e2[256] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
      'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
      'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};





    static constexpr auto d0 = []() constexpr noexcept -> std::array<std::uint32_t, 256> {
        if constexpr (std::endian::native == std::endian::big) {
            /* SPECIAL DECODE TABLES FOR BIG ENDIAN (IBM/MOTOROLA/SUN) CPUS */
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x00f8'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x00fc'0000, 0x00d0'0000,
              0x00d4'0000, 0x00d8'0000, 0x00dc'0000, 0x00e0'0000, 0x00e4'0000, 0x00e8'0000, 0x00ec'0000,
              0x00f0'0000, 0x00f4'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0004'0000, 0x0008'0000, 0x000c'0000, 0x0010'0000,
              0x0014'0000, 0x0018'0000, 0x001c'0000, 0x0020'0000, 0x0024'0000, 0x0028'0000, 0x002c'0000,
              0x0030'0000, 0x0034'0000, 0x0038'0000, 0x003c'0000, 0x0040'0000, 0x0044'0000, 0x0048'0000,
              0x004c'0000, 0x0050'0000, 0x0054'0000, 0x0058'0000, 0x005c'0000, 0x0060'0000, 0x0064'0000,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0068'0000,
              0x006c'0000, 0x0070'0000, 0x0074'0000, 0x0078'0000, 0x007c'0000, 0x0080'0000, 0x0084'0000,
              0x0088'0000, 0x008c'0000, 0x0090'0000, 0x0094'0000, 0x0098'0000, 0x009c'0000, 0x00a0'0000,
              0x00a4'0000, 0x00a8'0000, 0x00ac'0000, 0x00b0'0000, 0x00b4'0000, 0x00b8'0000, 0x00bc'0000,
              0x00c0'0000, 0x00c4'0000, 0x00c8'0000, 0x00cc'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        } else { // little endian
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0000'00f8, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'00fc, 0x0000'00d0,
              0x0000'00d4, 0x0000'00d8, 0x0000'00dc, 0x0000'00e0, 0x0000'00e4, 0x0000'00e8, 0x0000'00ec,
              0x0000'00f0, 0x0000'00f4, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0000'0004, 0x0000'0008, 0x0000'000c, 0x0000'0010,
              0x0000'0014, 0x0000'0018, 0x0000'001c, 0x0000'0020, 0x0000'0024, 0x0000'0028, 0x0000'002c,
              0x0000'0030, 0x0000'0034, 0x0000'0038, 0x0000'003c, 0x0000'0040, 0x0000'0044, 0x0000'0048,
              0x0000'004c, 0x0000'0050, 0x0000'0054, 0x0000'0058, 0x0000'005c, 0x0000'0060, 0x0000'0064,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'0068,
              0x0000'006c, 0x0000'0070, 0x0000'0074, 0x0000'0078, 0x0000'007c, 0x0000'0080, 0x0000'0084,
              0x0000'0088, 0x0000'008c, 0x0000'0090, 0x0000'0094, 0x0000'0098, 0x0000'009c, 0x0000'00a0,
              0x0000'00a4, 0x0000'00a8, 0x0000'00ac, 0x0000'00b0, 0x0000'00b4, 0x0000'00b8, 0x0000'00bc,
              0x0000'00c0, 0x0000'00c4, 0x0000'00c8, 0x0000'00cc, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        }
    }();


    static constexpr auto d1 = []() constexpr noexcept -> std::array<std::uint32_t, 256> {
        if constexpr (std::endian::native == std::endian::big) {
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0003'e000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0003'f000, 0x0003'4000,
              0x0003'5000, 0x0003'6000, 0x0003'7000, 0x0003'8000, 0x0003'9000, 0x0003'a000, 0x0003'b000,
              0x0003'c000, 0x0003'd000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0000'1000, 0x0000'2000, 0x0000'3000, 0x0000'4000,
              0x0000'5000, 0x0000'6000, 0x0000'7000, 0x0000'8000, 0x0000'9000, 0x0000'a000, 0x0000'b000,
              0x0000'c000, 0x0000'd000, 0x0000'e000, 0x0000'f000, 0x0001'0000, 0x0001'1000, 0x0001'2000,
              0x0001'3000, 0x0001'4000, 0x0001'5000, 0x0001'6000, 0x0001'7000, 0x0001'8000, 0x0001'9000,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0001'a000,
              0x0001'b000, 0x0001'c000, 0x0001'd000, 0x0001'e000, 0x0001'f000, 0x0002'0000, 0x0002'1000,
              0x0002'2000, 0x0002'3000, 0x0002'4000, 0x0002'5000, 0x0002'6000, 0x0002'7000, 0x0002'8000,
              0x0002'9000, 0x0002'a000, 0x0002'b000, 0x0002'c000, 0x0002'd000, 0x0002'e000, 0x0002'f000,
              0x0003'0000, 0x0003'1000, 0x0003'2000, 0x0003'3000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        } else { // little endian
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0000'e003, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'f003, 0x0000'4003,
              0x0000'5003, 0x0000'6003, 0x0000'7003, 0x0000'8003, 0x0000'9003, 0x0000'a003, 0x0000'b003,
              0x0000'c003, 0x0000'd003, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0000'1000, 0x0000'2000, 0x0000'3000, 0x0000'4000,
              0x0000'5000, 0x0000'6000, 0x0000'7000, 0x0000'8000, 0x0000'9000, 0x0000'a000, 0x0000'b000,
              0x0000'c000, 0x0000'd000, 0x0000'e000, 0x0000'f000, 0x0000'0001, 0x0000'1001, 0x0000'2001,
              0x0000'3001, 0x0000'4001, 0x0000'5001, 0x0000'6001, 0x0000'7001, 0x0000'8001, 0x0000'9001,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'a001,
              0x0000'b001, 0x0000'c001, 0x0000'd001, 0x0000'e001, 0x0000'f001, 0x0000'0002, 0x0000'1002,
              0x0000'2002, 0x0000'3002, 0x0000'4002, 0x0000'5002, 0x0000'6002, 0x0000'7002, 0x0000'8002,
              0x0000'9002, 0x0000'a002, 0x0000'b002, 0x0000'c002, 0x0000'd002, 0x0000'e002, 0x0000'f002,
              0x0000'0003, 0x0000'1003, 0x0000'2003, 0x0000'3003, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        }
    }();


    static constexpr auto d2 = []() constexpr noexcept -> std::array<std::uint32_t, 256> {
        if constexpr (std::endian::native == std::endian::big) {
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0000'0f80, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'0fc0, 0x0000'0d00,
              0x0000'0d40, 0x0000'0d80, 0x0000'0dc0, 0x0000'0e00, 0x0000'0e40, 0x0000'0e80, 0x0000'0ec0,
              0x0000'0f00, 0x0000'0f40, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0000'0040, 0x0000'0080, 0x0000'00c0, 0x0000'0100,
              0x0000'0140, 0x0000'0180, 0x0000'01c0, 0x0000'0200, 0x0000'0240, 0x0000'0280, 0x0000'02c0,
              0x0000'0300, 0x0000'0340, 0x0000'0380, 0x0000'03c0, 0x0000'0400, 0x0000'0440, 0x0000'0480,
              0x0000'04c0, 0x0000'0500, 0x0000'0540, 0x0000'0580, 0x0000'05c0, 0x0000'0600, 0x0000'0640,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'0680,
              0x0000'06c0, 0x0000'0700, 0x0000'0740, 0x0000'0780, 0x0000'07c0, 0x0000'0800, 0x0000'0840,
              0x0000'0880, 0x0000'08c0, 0x0000'0900, 0x0000'0940, 0x0000'0980, 0x0000'09c0, 0x0000'0a00,
              0x0000'0a40, 0x0000'0a80, 0x0000'0ac0, 0x0000'0b00, 0x0000'0b40, 0x0000'0b80, 0x0000'0bc0,
              0x0000'0c00, 0x0000'0c40, 0x0000'0c80, 0x0000'0cc0, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        } else { // little endian
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0080'0f00, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x00c0'0f00, 0x0000'0d00,
              0x0040'0d00, 0x0080'0d00, 0x00c0'0d00, 0x0000'0e00, 0x0040'0e00, 0x0080'0e00, 0x00c0'0e00,
              0x0000'0f00, 0x0040'0f00, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0040'0000, 0x0080'0000, 0x00c0'0000, 0x0000'0100,
              0x0040'0100, 0x0080'0100, 0x00c0'0100, 0x0000'0200, 0x0040'0200, 0x0080'0200, 0x00c0'0200,
              0x0000'0300, 0x0040'0300, 0x0080'0300, 0x00c0'0300, 0x0000'0400, 0x0040'0400, 0x0080'0400,
              0x00c0'0400, 0x0000'0500, 0x0040'0500, 0x0080'0500, 0x00c0'0500, 0x0000'0600, 0x0040'0600,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0080'0600,
              0x00c0'0600, 0x0000'0700, 0x0040'0700, 0x0080'0700, 0x00c0'0700, 0x0000'0800, 0x0040'0800,
              0x0080'0800, 0x00c0'0800, 0x0000'0900, 0x0040'0900, 0x0080'0900, 0x00c0'0900, 0x0000'0a00,
              0x0040'0a00, 0x0080'0a00, 0x00c0'0a00, 0x0000'0b00, 0x0040'0b00, 0x0080'0b00, 0x00c0'0b00,
              0x0000'0c00, 0x0040'0c00, 0x0080'0c00, 0x00c0'0c00, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        }
    }();


    static constexpr auto d3 = []() constexpr noexcept -> std::array<std::uint32_t, 256> {
        if constexpr (std::endian::native == std::endian::big) {
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x0000'003e, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'003f, 0x0000'0034,
              0x0000'0035, 0x0000'0036, 0x0000'0037, 0x0000'0038, 0x0000'0039, 0x0000'003a, 0x0000'003b,
              0x0000'003c, 0x0000'003d, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0000'0001, 0x0000'0002, 0x0000'0003, 0x0000'0004,
              0x0000'0005, 0x0000'0006, 0x0000'0007, 0x0000'0008, 0x0000'0009, 0x0000'000a, 0x0000'000b,
              0x0000'000c, 0x0000'000d, 0x0000'000e, 0x0000'000f, 0x0000'0010, 0x0000'0011, 0x0000'0012,
              0x0000'0013, 0x0000'0014, 0x0000'0015, 0x0000'0016, 0x0000'0017, 0x0000'0018, 0x0000'0019,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x0000'001a,
              0x0000'001b, 0x0000'001c, 0x0000'001d, 0x0000'001e, 0x0000'001f, 0x0000'0020, 0x0000'0021,
              0x0000'0022, 0x0000'0023, 0x0000'0024, 0x0000'0025, 0x0000'0026, 0x0000'0027, 0x0000'0028,
              0x0000'0029, 0x0000'002a, 0x0000'002b, 0x0000'002c, 0x0000'002d, 0x0000'002e, 0x0000'002f,
              0x0000'0030, 0x0000'0031, 0x0000'0032, 0x0000'0033, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        } else { // little endian
            return {
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x003e'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x003f'0000, 0x0034'0000,
              0x0035'0000, 0x0036'0000, 0x0037'0000, 0x0038'0000, 0x0039'0000, 0x003a'0000, 0x003b'0000,
              0x003c'0000, 0x003d'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x0000'0000, 0x0001'0000, 0x0002'0000, 0x0003'0000, 0x0004'0000,
              0x0005'0000, 0x0006'0000, 0x0007'0000, 0x0008'0000, 0x0009'0000, 0x000a'0000, 0x000b'0000,
              0x000c'0000, 0x000d'0000, 0x000e'0000, 0x000f'0000, 0x0010'0000, 0x0011'0000, 0x0012'0000,
              0x0013'0000, 0x0014'0000, 0x0015'0000, 0x0016'0000, 0x0017'0000, 0x0018'0000, 0x0019'0000,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x001a'0000,
              0x001b'0000, 0x001c'0000, 0x001d'0000, 0x001e'0000, 0x001f'0000, 0x0020'0000, 0x0021'0000,
              0x0022'0000, 0x0023'0000, 0x0024'0000, 0x0025'0000, 0x0026'0000, 0x0027'0000, 0x0028'0000,
              0x0029'0000, 0x002a'0000, 0x002b'0000, 0x002c'0000, 0x002d'0000, 0x002e'0000, 0x002f'0000,
              0x0030'0000, 0x0031'0000, 0x0032'0000, 0x0033'0000, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff,
              0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff, 0x01ff'ffff};
        }
    }();



} // namespace webpp::modp_b64

#endif // WEBPP_MODP_B64_DATA_HPP
