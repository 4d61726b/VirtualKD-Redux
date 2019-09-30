 /*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#define __PUT_STATIC_DATA_MEMBERS_HERE
#define _STLP_EXPOSE_GLOBALS_IMPLEMENTATION

#include "stlport_prefix.h"

#if !defined (_STLP_DEBUG) && ! defined (_STLP_ASSERTIONS)
#  if !defined (__APPLE__) || !defined (__GNUC__) || (__GNUC__ < 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ < 3))
/* dums: Please if the following code was being uncommented please explain why
 * as for the moment it only looks like a source of inconsistency in the way
 * STLport different translation units are compiled.
 */
//#    define _STLP_ASSERTIONS 1
#  endif
#endif

#include <utility>
#include <memory>
#include <vector>
#include <set>
#include <list>
#include <slist>
#include <deque>
#include <hash_map>
#include <limits>
#include <string>
#include <stdexcept>
#include <bitset>
#include <locale>

#if (_STLP_STATIC_TEMPLATE_DATA < 1) || defined (__DMC__)
// for rope static members
#  include <rope>
#endif

// boris : this piece of code duplicated from _range_errors.h
#undef _STLP_THROW_MSG
#if defined(_STLP_THROW_RANGE_ERRORS)
#  ifndef _STLP_STDEXCEPT
#    include <stdexcept>
#  endif
#  ifndef _STLP_STRING
#    include <string>
#  endif
#  define _STLP_THROW_MSG(ex,msg)  throw ex(string(msg))
#else
#  if defined (_STLP_RTTI_BUG)
#    define _STLP_THROW_MSG(ex,msg)  TerminateProcess(GetCurrentProcess(), 0)
#  else
#    include <cstdlib>
#    include <cstdio>
#    define _STLP_THROW_MSG(ex,msg)  puts(msg),_STLP_ABORT()
#  endif
#endif

#if defined (_STLP_MSVC) && (_STLP_MSVC < 1310)
#  pragma optimize("g", off)
#endif

_STLP_BEGIN_NAMESPACE

void _STLP_DECLSPEC _STLP_CALL __stl_throw_runtime_error(const char* __msg)
{ _STLP_THROW_MSG(runtime_error, __msg); }

void _STLP_DECLSPEC _STLP_CALL __stl_throw_range_error(const char* __msg)
{ _STLP_THROW_MSG(range_error, __msg); }

void _STLP_DECLSPEC _STLP_CALL __stl_throw_out_of_range(const char* __msg)
{ _STLP_THROW_MSG(out_of_range, __msg); }

void _STLP_DECLSPEC _STLP_CALL __stl_throw_length_error(const char* __msg)
{ _STLP_THROW_MSG(length_error, __msg); }

void _STLP_DECLSPEC _STLP_CALL __stl_throw_invalid_argument(const char* __msg)
{ _STLP_THROW_MSG(invalid_argument, __msg); }

void _STLP_DECLSPEC _STLP_CALL __stl_throw_overflow_error(const char* __msg)
{ _STLP_THROW_MSG(overflow_error, __msg); }

#if defined (_STLP_NO_EXCEPTION_HEADER) || defined (_STLP_BROKEN_EXCEPTION_CLASS)
exception::exception() _STLP_NOTHROW {}
exception::~exception() _STLP_NOTHROW {}
bad_exception::bad_exception() _STLP_NOTHROW {}
bad_exception::~bad_exception() _STLP_NOTHROW {}
const char* exception::what() const _STLP_NOTHROW { return "class exception"; }
const char* bad_exception::what() const _STLP_NOTHROW { return "class bad_exception"; }
#endif

#if defined (_STLP_OWN_STDEXCEPT)
__Named_exception::__Named_exception(const string& __str) {
#if !defined (_STLP_USE_SAFE_STRING_FUNCTIONS)
  strncpy(_M_name, __str.c_str(), _S_bufsize);
  _M_name[_S_bufsize - 1] = '\0';
#else
  strncpy_s(_STLP_ARRAY_AND_SIZE(_M_name), __str.c_str(), _TRUNCATE);
#endif
}

const char* __Named_exception::what() const _STLP_NOTHROW_INHERENTLY { return _M_name; }

// boris : those are needed to force typeinfo nodes to be created in here only
__Named_exception::~__Named_exception() _STLP_NOTHROW_INHERENTLY {}

logic_error::~logic_error() _STLP_NOTHROW_INHERENTLY {}
runtime_error::~runtime_error() _STLP_NOTHROW_INHERENTLY {}
domain_error::~domain_error() _STLP_NOTHROW_INHERENTLY {}
invalid_argument::~invalid_argument() _STLP_NOTHROW_INHERENTLY {}
length_error::~length_error() _STLP_NOTHROW_INHERENTLY {}
out_of_range::~out_of_range() _STLP_NOTHROW_INHERENTLY {}
range_error::~range_error() _STLP_NOTHROW_INHERENTLY {}
overflow_error::~overflow_error() _STLP_NOTHROW_INHERENTLY {}
underflow_error::~underflow_error() _STLP_NOTHROW_INHERENTLY {}

#endif /* _STLP_OWN_STDEXCEPT */

#if !defined(_STLP_WCE_EVC3)
#  if defined (_STLP_NO_BAD_ALLOC)
const nothrow_t nothrow /* = {} */;
#  endif
#endif

#if !defined (_STLP_NO_FORCE_INSTANTIATE)

#  if defined (_STLP_DEBUG) || defined (_STLP_ASSERTIONS)
_STLP_MOVE_TO_PRIV_NAMESPACE
template struct _STLP_CLASS_DECLSPEC __stl_debug_engine<bool>;
_STLP_MOVE_TO_STD_NAMESPACE
#  endif

template class _STLP_CLASS_DECLSPEC __debug_alloc<__node_alloc>;
template class _STLP_CLASS_DECLSPEC __debug_alloc<__new_alloc>;

const int __use_ptr_atomic_swap = sizeof(__stl_atomic_t) == sizeof(void*);
template class _STLP_CLASS_DECLSPEC _Atomic_swap_struct<__use_ptr_atomic_swap>;

//Export of the types used to represent buckets in the hashtable implementation.
/*
 * For the vector class we do not use any MSVC6 workaround even if we export it from
 * the STLport dynamic libraries because we know what methods are called and none is
 * a template method. Moreover the exported class is an instanciation of vector with
 * _Slist_node_base struct that is an internal STLport class that no user should ever
 * use.
 */
#  if !defined (_STLP_USE_PTR_SPECIALIZATIONS)
template class _STLP_CLASS_DECLSPEC allocator<_STLP_PRIV _Slist_node_base*>;

_STLP_MOVE_TO_PRIV_NAMESPACE
template class _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<_Slist_node_base**, _Slist_node_base*,
                                                      allocator<_Slist_node_base*> >;
template class _STLP_CLASS_DECLSPEC _Vector_base<_Slist_node_base*,
                                                 allocator<_Slist_node_base*> >;
_STLP_MOVE_TO_STD_NAMESPACE
#  endif

#  if defined (_STLP_DEBUG)
_STLP_MOVE_TO_PRIV_NAMESPACE
template class _STLP_CLASS_DECLSPEC _STLP_NON_DBG_NAME(vector)<_Slist_node_base*,
                                                               allocator<_Slist_node_base*> >;
_STLP_MOVE_TO_STD_NAMESPACE
#  endif

template class _STLP_CLASS_DECLSPEC vector<_STLP_PRIV _Slist_node_base*,
                                           allocator<_STLP_PRIV _Slist_node_base*> >;
//End of hashtable bucket types export.

//Export of _Locale_impl facets container:
#  if !defined (_STLP_USE_PTR_SPECIALIZATIONS)
template class _STLP_CLASS_DECLSPEC allocator<locale::facet*>;

_STLP_MOVE_TO_PRIV_NAMESPACE
template class _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<locale::facet**, locale::facet*, allocator<locale::facet*> >;
template class _STLP_CLASS_DECLSPEC _Vector_base<locale::facet*, allocator<locale::facet*> >;
_STLP_MOVE_TO_STD_NAMESPACE

#  endif
#  if defined (_STLP_DEBUG)
_STLP_MOVE_TO_PRIV_NAMESPACE
#    define _STLP_NON_DBG_VECTOR _STLP_NON_DBG_NAME(vector)
template class _STLP_CLASS_DECLSPEC __construct_checker<_STLP_PRIV _STLP_NON_DBG_VECTOR<locale::facet*, allocator<locale::facet*> > >;
template class _STLP_CLASS_DECLSPEC _STLP_NON_DBG_VECTOR<locale::facet*, allocator<locale::facet*> >;
#    undef _STLP_NON_DBG_VECTOR
_STLP_MOVE_TO_STD_NAMESPACE
#  endif

template class _STLP_CLASS_DECLSPEC vector<locale::facet*, allocator<locale::facet*> >;
//End of export of _Locale_impl facets container.

#  if defined (_STLP_USE_PTR_SPECIALIZATIONS)
template class _STLP_CLASS_DECLSPEC allocator<void*>;

typedef _STLP_PRIV _List_node<void*> _VoidPtr_Node;
template class _STLP_CLASS_DECLSPEC allocator<_VoidPtr_Node>;

_STLP_MOVE_TO_PRIV_NAMESPACE

template class _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<void**, void*, allocator<void*> >;
template class _STLP_CLASS_DECLSPEC _Vector_base<void*, allocator<void*> >;
template class _STLP_CLASS_DECLSPEC _STLP_PTR_IMPL_NAME(vector)<void*, allocator<void*> >;

template class _STLP_CLASS_DECLSPEC _List_node<void*>;
template class _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<_List_node_base, _VoidPtr_Node, allocator<_VoidPtr_Node> >;
template class _STLP_CLASS_DECLSPEC _List_base<void*, allocator<void*> >;
template class _STLP_CLASS_DECLSPEC _STLP_PTR_IMPL_NAME(list)<void*, allocator<void*> >;

template class _STLP_CLASS_DECLSPEC _Slist_node<void*>;
template class _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<_Slist_node_base, _Slist_node<void*>, allocator<_Slist_node<void*> > >;
template class _STLP_CLASS_DECLSPEC _Slist_base<void*, allocator<void*> >;
template class _STLP_CLASS_DECLSPEC _STLP_PTR_IMPL_NAME(slist)<void*, allocator<void*> >;

template class  _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<size_t, void*, allocator<void*> >;
template class  _STLP_CLASS_DECLSPEC _STLP_alloc_proxy<void***, void**, allocator<void**> >;
template struct _STLP_CLASS_DECLSPEC _Deque_iterator<void*, _Nonconst_traits<void*> >;
template class  _STLP_CLASS_DECLSPEC _Deque_base<void*, allocator<void*> >;
template class  _STLP_CLASS_DECLSPEC _STLP_PTR_IMPL_NAME(deque)<void*, allocator<void*> >;

_STLP_MOVE_TO_STD_NAMESPACE

#  endif /* _STLP_USE_PTR_SPECIALIZATIONS */

_STLP_MOVE_TO_PRIV_NAMESPACE

template class _STLP_CLASS_DECLSPEC _Rb_global<bool>;
template class _STLP_CLASS_DECLSPEC _List_global<bool>;

template class _STLP_CLASS_DECLSPEC _Sl_global<bool>;
template class _STLP_CLASS_DECLSPEC _Stl_prime<bool>;

template class _STLP_CLASS_DECLSPEC _LimG<bool>;

_STLP_MOVE_TO_STD_NAMESPACE

#endif /* _STLP_NO_FORCE_INSTANTIATE */

_STLP_END_NAMESPACE

#define FORCE_SYMBOL extern

#if defined (_WIN32) && defined (_STLP_USE_DECLSPEC) && !defined (_STLP_USE_STATIC_LIB)
// stlportmt.cpp : Defines the entry point for the DLL application.
//
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#  undef FORCE_SYMBOL
#  define FORCE_SYMBOL APIENTRY

extern "C" {

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls((HINSTANCE)hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
    }
  return TRUE;
}

} /* extern "C" */

#if !defined (_STLP_MSVC) && !defined (__MINGW32__)
_STLP_BEGIN_NAMESPACE

static void FORCE_SYMBOL
force_link() {
  set<int>::iterator iter;
  // _M_increment; _M_decrement instantiation
  ++iter;
  --iter;
}

_STLP_END_NAMESPACE
#endif

#endif /* _WIN32 */

#if defined (__ICL) && (__ICL >= 900) && (_STLP_MSVC_LIB < 1300)
#  undef std

namespace std
{
  void _STLP_CALL unexpected() {
    unexpected_handler hdl;
    set_unexpected(hdl = set_unexpected((unexpected_handler)0));
    hdl();
  }
}
#endif
