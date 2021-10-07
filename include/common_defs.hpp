#ifndef WEAK_COMMON_DEFS_HPP
#define WEAK_COMMON_DEFS_HPP

//#define DONT_USE_FORCE_INLINE

#ifndef DONT_USE_FORCE_INLINE
#  define ALWAYS_INLINE inline __attribute__((always_inline))
#  define NEVER_INLINE inline __attribute__((noinline))
#else
#  define ALWAYS_INLINE inline
#  define NEVER_INLINE inline
#endif

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#ifndef UNUSED
#  define UNUSED(x) (void)x
#endif// UNUSED

#endif// WEAK_COMMON_DEFS_HPP
