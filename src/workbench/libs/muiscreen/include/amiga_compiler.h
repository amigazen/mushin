#ifndef AMIGA_COMPILER_H
#define AMIGA_COMPILER_H

/*
** Compiler specific macros for function parameter passing
** and code generation (subset used by muiscreen.library).
** Register parameters come from the NDK.
*/

#if defined(__GNUC__) && !defined(AMIGA) && !defined(__MORPHOS__) && !defined(__AROS__)
#if defined(__mc68000__) || defined(__AMIGA__) || defined(__amigaos__)
#define AMIGA 1
#endif
#endif
#include <clib/compiler-specific.h>

#ifndef ASM
#define ASM __ASM__
#endif
#ifndef REG
#define REG(reg, arg) __REG__(reg, arg)
#endif
#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif
#ifndef STDARGS
#define STDARGS __STDARGS__
#endif

#ifdef __SASC

#ifndef ASM
#define ASM __ASM__
#endif

#ifndef INLINE
#define INLINE __inline
#endif

#ifndef REG
#define REG(reg, arg) __REG__(reg, arg)
#endif

#ifndef REGARGS
#define REGARGS __regargs
#endif

#ifndef STDARGS
#define STDARGS __STDARGS__
#endif

#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif

#ifndef VARARGS68K
#define VARARGS68K
#endif

#define __attribute__(dummy)

#endif /* __SASC */

#ifdef __GNUC__

#ifndef ASM
#define ASM
#endif

#ifndef INLINE
#define INLINE __inline__
#endif

#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif

#ifndef STDARGS
#define STDARGS __STDARGS__
#endif

#ifdef mc68000
#ifndef REG
#define REG(reg, arg) __REG__(reg, arg)
#endif
#else
#ifndef REG
#define REG(reg,arg) arg
#endif
#endif

#endif /* __GNUC__ */

#endif /* AMIGA_COMPILER_H */
