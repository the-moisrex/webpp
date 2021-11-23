//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CONCEPTS_COPYABLE_H
#define _LIBCPP___CONCEPTS_COPYABLE_H

#include "./assignable.h"
#include "./constructible.h"
#include "./movable.h"
#include "../libcxx-config.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

// [concepts.object]

template<class _Tp>
concept copyable =
  copy_constructible<_Tp> &&
  movable<_Tp> &&
  assignable_from<_Tp&, _Tp&> &&
  assignable_from<_Tp&, const _Tp&> &&
  assignable_from<_Tp&, const _Tp>;

#endif // _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___CONCEPTS_COPYABLE_H
