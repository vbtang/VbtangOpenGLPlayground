/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#include <stdio.h>
#include <assert.h>

//#include "c99_compat.h"
//#include "c11_compat.h"

/* Compute the size of an array */
#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* For compatibility with Clang's __has_builtin() */
#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

#ifndef __has_attribute
#  define __has_attribute(x) 0
#endif

/**
 * __builtin_expect macros
 */
#if !defined(HAVE___BUILTIN_EXPECT)
#  define __builtin_expect(x, y) (x)
#endif

#ifndef likely
#  ifdef HAVE___BUILTIN_EXPECT
#    define likely(x)   __builtin_expect(!!(x), 1)
#    define unlikely(x) __builtin_expect(!!(x), 0)
#  else
#    define likely(x)   (x)
#    define unlikely(x) (x)
#  endif
#endif

/**
 * __builtin_types_compatible_p compat
 */
#if defined(__cplusplus) || !defined(HAVE___BUILTIN_TYPES_COMPATIBLE_P)
#  define __builtin_types_compatible_p(type1, type2) (1)
#endif

/**
 * Static (compile-time) assertion.
 */
#if defined(_MSC_VER)
   /* MSVC doesn't like VLA's, but it also dislikes zero length arrays
    * (which gcc is happy with), so we have to define STATIC_ASSERT()
    * slightly differently.
    */
#  define STATIC_ASSERT(COND) do {         \
      (void) sizeof(char [(COND) != 0]);   \
   } while (0)
#elif defined(__GNUC__)
   /* This version of STATIC_ASSERT() relies on VLAs.  If COND is
    * false/zero, the array size will be -1 and we'll get a compile
    * error
    */
#  define STATIC_ASSERT(COND) do {         \
      (void) sizeof(char [1 - 2*!(COND)]); \
   } while (0)
#else
#  define STATIC_ASSERT(COND) do { } while (0)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 */
#ifndef __GNUC__
   /* a grown-up compiler is required for the extra type checking: */
#  define container_of(ptr, type, member)                               \
      (type*)((uint8_t *)ptr - offsetof(type, member))
#else
#  define __same_type(a, b) \
      __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
#  define container_of(ptr, type, member) ({                            \
         uint8_t *__mptr = (uint8_t *)(ptr);                            \
         STATIC_ASSERT(__same_type(*(ptr), ((type *)0)->member) ||      \
                       __same_type(*(ptr), void) ||                     \
                       !"pointer type mismatch in container_of()");     \
         ((type *)(__mptr - offsetof(type, member)));                   \
      })
#endif

/**
 * Unreachable macro. Useful for suppressing "control reaches end of non-void
 * function" warnings.
 */
#if defined(HAVE___BUILTIN_UNREACHABLE) || __has_builtin(__builtin_unreachable)
#define unreachable(str)    \
do {                        \
   assert(!str);            \
   __builtin_unreachable(); \
} while (0)
#elif defined (_MSC_VER)
#define unreachable(str)    \
do {                        \
   assert(!str);            \
   __assume(0);             \
} while (0)
#else
#define unreachable(str) assert(!str)
#endif

/**
 * Assume macro. Useful for expressing our assumptions to the compiler,
 * typically for purposes of silencing warnings.
 */
#if __has_builtin(__builtin_assume)
#define assume(expr)       \
do {                       \
   assert(expr);           \
   __builtin_assume(expr); \
} while (0)
#elif defined HAVE___BUILTIN_UNREACHABLE
#define assume(expr) ((expr) ? ((void) 0) \
                             : (assert(!"assumption failed"), \
                                __builtin_unreachable()))
#elif defined (_MSC_VER)
#define assume(expr) __assume(expr)
#else
#define assume(expr) assert(expr)
#endif

/* Attribute const is used for functions that have no effects other than their
 * return value, and only rely on the argument values to compute the return
 * value.  As a result, calls to it can be CSEed.  Note that using memory
 * pointed to by the arguments is not allowed for const functions.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_CONST
#define ATTRIBUTE_CONST __attribute__((__const__))
#else
#define ATTRIBUTE_CONST
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_FLATTEN
#define FLATTEN __attribute__((__flatten__))
#else
#define FLATTEN
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_FORMAT
#if defined (__MINGW_PRINTF_FORMAT)
# define PRINTFLIKE(f, a) __attribute__ ((format(__MINGW_PRINTF_FORMAT, f, a)))
#else
# define PRINTFLIKE(f, a) __attribute__ ((format(__printf__, f, a)))
#endif
#else
#define PRINTFLIKE(f, a)
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_MALLOC
#define MALLOCLIKE __attribute__((__malloc__))
#else
#define MALLOCLIKE
#endif

/* Forced function inlining */
/* Note: Clang also sets __GNUC__ (see other cases below) */
#ifndef ALWAYS_INLINE
#  if defined(__GNUC__)
#    define ALWAYS_INLINE inline __attribute__((always_inline))
#  elif defined(_MSC_VER)
#    define ALWAYS_INLINE __forceinline
#  else
#    define ALWAYS_INLINE inline
#  endif
#endif

/* Used to optionally mark structures with misaligned elements or size as
 * packed, to trade off performance for space.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_PACKED
#define PACKED __attribute__((__packed__))
#else
#define PACKED
#endif

/* Attribute pure is used for functions that have no effects other than their
 * return value.  As a result, calls to it can be dead code eliminated.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_PURE
#define ATTRIBUTE_PURE __attribute__((__pure__))
#else
#define ATTRIBUTE_PURE
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_RETURNS_NONNULL
#define ATTRIBUTE_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#else
#define ATTRIBUTE_RETURNS_NONNULL
#endif

#ifndef NORETURN
#  ifdef _MSC_VER
#    define NORETURN __declspec(noreturn)
#  elif defined HAVE_FUNC_ATTRIBUTE_NORETURN
#    define NORETURN __attribute__((__noreturn__))
#  else
#    define NORETURN
#  endif
#endif

#ifdef _MSC_VER
#define ALIGN16 __declspec(align(16))
#else
#define ALIGN16 __attribute__((aligned(16)))
#endif

#ifdef __cplusplus
/**
 * Macro function that evaluates to true if T is a trivially
 * destructible type -- that is, if its (non-virtual) destructor
 * performs no action and all member variables and base classes are
 * trivially destructible themselves.
 */
#   if (defined(__clang__) && defined(__has_feature))
#      if __has_feature(has_trivial_destructor)
#         define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#      endif
#   elif defined(__GNUC__)
#      if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)))
#         define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#      endif
#   elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#      define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   endif
#   ifndef HAS_TRIVIAL_DESTRUCTOR
       /* It's always safe (if inefficient) to assume that a
        * destructor is non-trivial.
        */
#      define HAS_TRIVIAL_DESTRUCTOR(T) (false)
#   endif
#endif

/**
 * PUBLIC/USED macros
 *
 * If we build the library with gcc's -fvisibility=hidden flag, we'll
 * use the PUBLIC macro to mark functions that are to be exported.
 *
 * We also need to define a USED attribute, so the optimizer doesn't
 * inline a static function that we later use in an alias. - ajax
 */
#ifndef PUBLIC
#  if defined(_WIN32)
#    define PUBLIC __declspec(dllexport)
#    define USED
#  elif defined(__GNUC__)
#    define PUBLIC __attribute__((visibility("default")))
#    define USED __attribute__((used))
#  else
#    define PUBLIC
#    define USED
#  endif
#endif

/**
 * UNUSED marks variables (or sometimes functions) that have to be defined,
 * but are sometimes (or always) unused beyond that. A common case is for
 * a function parameter to be used in some build configurations but not others.
 * Another case is fallback vfuncs that don't do anything with their params.
 *
 * Note that this should not be used for identifiers used in `assert()`;
 * see ASSERTED below.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_UNUSED
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

/**
 * Use ASSERTED to indicate that an identifier is unused outside of an `assert()`,
 * so that assert-free builds don't get "unused variable" warnings.
 */
#ifdef NDEBUG
#define ASSERTED UNUSED
#else
#define ASSERTED
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_WARN_UNUSED_RESULT
#define MUST_CHECK __attribute__((warn_unused_result))
#else
#define MUST_CHECK
#endif

#if defined(__GNUC__)
#define ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define ATTRIBUTE_NOINLINE
#endif


/**
 * Check that STRUCT::FIELD can hold MAXVAL.  We use a lot of bitfields
 * in Mesa/gallium.  We have to be sure they're of sufficient size to
 * hold the largest expected value.
 * Note that with MSVC, enums are signed and enum bitfields need one extra
 * high bit (always zero) to ensure the max value is handled correctly.
 * This macro will detect that with MSVC, but not GCC.
 */
#define ASSERT_BITFIELD_SIZE(STRUCT, FIELD, MAXVAL) \
   do { \
      ASSERTED STRUCT s; \
      s.FIELD = (MAXVAL); \
      assert((int) s.FIELD == (MAXVAL) && "Insufficient bitfield size!"); \
   } while (0)


/** Compute ceiling of integer quotient of A divided by B. */
#define DIV_ROUND_UP( A, B )  ( ((A) + (B) - 1) / (B) )

/** Clamp X to [MIN,MAX].  Turn NaN into MIN, arbitrarily. */
#define CLAMP( X, MIN, MAX )  ( (X)>(MIN) ? ((X)>(MAX) ? (MAX) : (X)) : (MIN) )

/* Syntax sugar occuring frequently in graphics code */
#define SATURATE( X ) CLAMP(X, 0.0f, 1.0f)

/** Minimum of two values: */
#define MIN2( A, B )   ( (A)<(B) ? (A) : (B) )

/** Maximum of two values: */
#define MAX2( A, B )   ( (A)>(B) ? (A) : (B) )

/** Minimum and maximum of three values: */
#define MIN3( A, B, C ) ((A) < (B) ? MIN2(A, C) : MIN2(B, C))
#define MAX3( A, B, C ) ((A) > (B) ? MAX2(A, C) : MAX2(B, C))

/** Align a value to a power of two */
#define ALIGN_POT(x, pot_align) (((x) + (pot_align) - 1) & ~((pot_align) - 1))

/**
 * Macro for declaring an explicit conversion operator.  Defaults to an
 * implicit conversion if C++11 is not supported.
 */
#if __cplusplus >= 201103L
#define EXPLICIT_CONVERSION explicit
#elif defined(__cplusplus)
#define EXPLICIT_CONVERSION
#endif

/** Set a single bit */
#define BITFIELD_BIT(b)      (1u << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD_MASK(b)      \
   ((b) == 32 ? (~0u) : BITFIELD_BIT((b) % 32) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD_RANGE(b, count) \
   (BITFIELD_MASK((b) + (count)) & ~BITFIELD_MASK(b))

/** Set a single bit */
#define BITFIELD64_BIT(b)      (1ull << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD64_MASK(b)      \
   ((b) == 64 ? (~0ull) : BITFIELD64_BIT(b) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD64_RANGE(b, count) \
   (BITFIELD64_MASK((b) + (count)) & ~BITFIELD64_MASK(b))

/* TODO: In future we should try to move this to u_debug.h once header
 * dependencies are reorganised to allow this.
 */
enum pipe_debug_type
{
   PIPE_DEBUG_TYPE_OUT_OF_MEMORY = 1,
   PIPE_DEBUG_TYPE_ERROR,
   PIPE_DEBUG_TYPE_SHADER_INFO,
   PIPE_DEBUG_TYPE_PERF_INFO,
   PIPE_DEBUG_TYPE_INFO,
   PIPE_DEBUG_TYPE_FALLBACK,
   PIPE_DEBUG_TYPE_CONFORMANCE,
};

#if !defined(alignof) && !defined(__cplusplus)
#if __STDC_VERSION__ >= 201112L
#define alignof(t) _Alignof(t)
#elif defined(_MSC_VER)
#define alignof(t) __alignof(t)
#else
#define alignof(t) __alignof__(t)
#endif
#endif

/* Macros for static type-safety checking.
 *
 * https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
 */

#if __has_attribute(capability)
typedef int __attribute__((capability("mutex"))) lock_cap_t;

#define guarded_by(l) __attribute__((guarded_by(l)))
#define acquire_cap(l) __attribute((acquire_capability(l), no_thread_safety_analysis))
#define release_cap(l) __attribute((release_capability(l), no_thread_safety_analysis))
#define assert_cap(l) __attribute((assert_capability(l), no_thread_safety_analysis))
#define requires_cap(l) __attribute((requires_capability(l)))
#define disable_thread_safety_analysis __attribute((no_thread_safety_analysis))

#else

typedef int lock_cap_t;

#define guarded_by(l)
#define acquire_cap(l)
#define release_cap(l)
#define assert_cap(l)
#define requires_cap(l)
#define disable_thread_safety_analysis

#endif

#endif /* UTIL_MACROS_H */
