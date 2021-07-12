#ifndef COMMON_DEFS_HPP
#define COMMON_DEFS_HPP

#define ALWAYS_INLINE       inline __attribute__((always_inline))
#define NEVER_INLINE        inline __attribute__((noinline))

#define LIKELY(x)           __builtin_expect(!!(x), 1)
#define UNLIKELY(x)         __builtin_expect(!!(x), 0)


#endif // COMMON_DEFS_HPP
