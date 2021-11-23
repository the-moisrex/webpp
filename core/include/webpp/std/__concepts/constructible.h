//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CONCEPTS_CONSTRUCTIBLE_H
#define _LIBCPP___CONCEPTS_CONSTRUCTIBLE_H

#include "./convertible_to.h"
#include "./destructible.h"
#include "../libcxx-config.hpp"
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

// [concept.constructible]
template<class _Tp, class... _Args>
concept constructible_from =
    destructible<_Tp> && is_constructible_v<_Tp, _Args...>;

// [concept.default.init]

template<class _Tp>
concept __default_initializable = requires { ::new _Tp; };

template<class _Tp>
concept default_initializable = constructible_from<_Tp> &&
    requires { _Tp{}; } && __default_initializable<_Tp>;

// [concept.moveconstructible]
template<class _Tp>
concept move_constructible =
  constructible_from<_Tp, _Tp> && convertible_to<_Tp, _Tp>;

// [concept.copyconstructible]
template<class _Tp>
concept copy_constructible =
  move_constructible<_Tp> &&
  constructible_from<_Tp, _Tp&> && convertible_to<_Tp&, _Tp> &&
  constructible_from<_Tp, const _Tp&> && convertible_to<const _Tp&, _Tp> &&
  constructible_from<_Tp, const _Tp> && convertible_to<const _Tp, _Tp>;

#endif // _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___CONCEPTS_CONSTRUCTIBLE_H
