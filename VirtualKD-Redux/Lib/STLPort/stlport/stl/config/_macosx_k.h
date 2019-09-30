#ifndef __stl_config__macosx_k_h
#define __stl_config__macosx_k_h

#define _STLP_PLATFORM "MacOS X Kernel"

#define _STLP_UNIX 1

#if defined(_REENTRANT) && !defined(_PTHREADS)
# define _PTHREADS
#endif

#define _STLP_NO_MATH_SUPPORT
#define _STLP_NO_CSTD_FUNCTION_IMPORTS
#define _STLP_NEW_DONT_THROW_BAD_ALLOC
#define _STLP_NO_BAD_ALLOC
#define _STLP_NO_MBSTATE_T

#ifndef WCHAR_MIN
#define WCHAR_MIN 0
#endif

#define _STLP_NATIVE_CPP_C_INCLUDE_PATH kext/c++
#define _STLP_NATIVE_C_INCLUDE_PATH kext

#define errno _errno_not_defined

#ifndef abs
template <class _Ty> _Ty static inline abs(_Ty t)
{
	return (t < 0) ? -t : t;
}

static inline long labs(long t) {return abs(t);}
static inline long long llabs(long long t) {return abs(t);}
#endif

#ifndef _gen_div
template <class _T> struct _gen_div_t
{
	_T quot;
	_T rem;
};

template <class _T> _gen_div_t<_T> static inline _gen_div(_T a, _T b)
{
	 _gen_div_t<_T> r = {a / b, a % b};
	 return r;
}

typedef _gen_div_t<long> ldiv_t;
typedef _gen_div_t<long long> lldiv_t;

static inline ldiv_t ldiv(long a, long b) {return _gen_div(a,b);}
static inline lldiv_t lldiv(long long a, long long b) {return _gen_div(a,b);}

#endif

#include <sys/types.h>
#ifndef NULL
#define NULL 0
#endif

inline const void *memchr (const void *ptr, int value, size_t num)
{
	const unsigned char *p = (const unsigned char *)ptr;
	value &= 0xFF;

	for(size_t i = 0; i < num;i++)
		if (p[i] == value)
			return p + i;
	
	return NULL;
}

inline  void * memchr (void *ptr, int value, size_t num)
{
	return const_cast<void *>(memchr(const_cast<void *>(ptr),value,num));
}

#include <kern/debug.h>
#define _STLP_THROW_MSG(ex,msg)	panic("STLPort exception: %s", msg)
#define __THROW_BAD_ALLOC panic("Kernel is out of memory")


typedef long long streampos;

template <class _T> void *operator new(size_t size, _T* &ptr)
{
	return ptr;
}

//typedef wchar_t wint_t;

extern "C"
{
	static inline void *malloc(size_t size)
	{
		return new char[size];
	}

	static inline void free(void *p)
	{
		return delete (char *)p;
	}
}

#if defined(_PTHREADS)
#  define _STLP_THREADS
#  define _STLP_PTHREADS
/*
#  ifndef __USE_UNIX98
#    define __USE_UNIX98
#  endif
*/
/* This feature exist at least since glibc 2.2.4 */
/* #  define __FIT_XSI_THR */ /* Unix 98 or X/Open System Interfaces Extention */
#  ifdef __USE_XOPEN2K
/* The IEEE Std. 1003.1j-2000 introduces functions to implement spinlocks. */
#   ifndef __UCLIBC__ /* There are no spinlocks in uClibc 0.9.27 */
#     define _STLP_USE_PTHREAD_SPINLOCK
#   else
#     ifndef _STLP_DONT_USE_PTHREAD_SPINLOCK
        /* in uClibc (0.9.26) pthread_spinlock* declared in headers
         * but absent in library */
#       define _STLP_DONT_USE_PTHREAD_SPINLOCK
#     endif
#   endif
/* #   define __FIT_PSHARED_MUTEX */
#  endif
#endif

/* Endiannes */
#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#  error "One of __LITTLE_ENDIAN and __BIG_ENDIAN undefined; Fix me!"
#endif

#if defined (__LITTLE_ENDIAN__)
#  define _STLP_LITTLE_ENDIAN 1
#elif defined (__BIG_ENDIAN__)
#  define _STLP_BIG_ENDIAN 1
#endif

#  define _STLP_NO_NATIVE_WIDE_FUNCTIONS 1
/*
#    define _STLP_NO_NATIVE_WIDE_STREAMS   1
*/

#include <string.h>

static inline void *memcpy_workaround(void *dst, const void *src, size_t len)
{
	memcpy(dst, src, len);
	return dst;
}

#define memcpy memcpy_workaround

#endif
