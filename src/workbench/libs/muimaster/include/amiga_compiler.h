#ifndef AMIGA_COMPILER_H
#define AMIGA_COMPILER_H

/*
**    $Id$
**
**    Compiler specific macros for function parameter passing
**    and code generation
**
**    Copyright (C) 1985-2005 Hyperion Entertainment VOF and Amiga, Inc.
**        All Rights Reserved
*/

/* Register parameters and __saveds/__stdargs come from the NDK. */
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

/****************************************************************************/

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

#ifndef INTERRUPT
#define INTERRUPT __interrupt
#endif

#ifndef FAR
#define FAR __far
#endif

#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif

#ifndef APICALL
#define APICALL
#endif

#ifndef VARARGS68K
#define VARARGS68K
#endif

#ifndef USED
#define USED
#endif

#ifndef UNUSED
#define UNUSED
#endif

#ifndef USED_VAR
#define USED_VAR
#endif

#ifndef BREL_RESTORE
#define BREL_RESTORE
#endif

#ifndef DEPRECATED
#define DEPRECATED
#endif

/* GCC compatibility */
#define __attribute__(dummy)

/* Do not use these definitions in new code: internal use only! */
#ifndef __REGA0
#define __LIBBASE(arg)  __REG__(a6, arg)
#define __REGA0(arg)    __REG__(a0, arg)
#define __REGA1(arg)    __REG__(a1, arg)
#define __REGA2(arg)    __REG__(a2, arg)
#define __REGA3(arg)    __REG__(a3, arg)
#define __REGA4(arg)    __REG__(a4, arg)
#define __REGA5(arg)    __REG__(a5, arg)
#define __REGA6(arg)    __REG__(a6, arg)
#define __REGA7(arg)    __REG__(a7, arg)
#define __REGD0(arg)    __REG__(d0, arg)
#define __REGD1(arg)    __REG__(d1, arg)
#define __REGD2(arg)    __REG__(d2, arg)
#define __REGD3(arg)    __REG__(d3, arg)
#define __REGD4(arg)    __REG__(d4, arg)
#define __REGD5(arg)    __REG__(d5, arg)
#define __REGD6(arg)    __REG__(d6, arg)
#define __REGD7(arg)    __REG__(d7, arg)
#endif /* __REGA0 */

#endif /* __SASC */

/****************************************************************************/

#ifdef __GNUC__

#ifndef ASM
#define ASM
#endif

#ifndef INLINE
#define INLINE __inline__
#endif

#ifndef FAR
#define FAR
#endif

#ifndef USED
#define USED __attribute__((used))
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifndef USED_VAR
#if (__GNUC__ > 3  || (__GNUC__ == 3 && __GNUC_MINOR__ > 0))
#define USED_VAR USED
#else
#define USED_VAR
#endif
#endif

#ifndef DEPRECATED
#define DEPRECATED  __attribute__((deprecated))
#endif

/****************************************************************************/

#ifdef mc68000

/****************************************************************************/

#ifndef REG
#define REG(reg, arg) __REG__(reg, arg)
#endif

#ifndef REGARGS
#define REGARGS __regargs
#endif

#ifndef STDARGS
#define STDARGS __STDARGS__
#endif

#ifndef INTERRUPT
#define INTERRUPT __interrupt
#endif

#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif

#ifndef APICALL
#define APICALL
#endif

#ifndef VARARGS68K
#define VARARGS68K
#endif

#ifndef BREL_RESTORE
#define BREL_RESTORE
#endif

/* Do not use these definitions in new code: internal use only! */
#ifndef __REGA0
#define __LIBBASE(arg)  __REG__(a6, arg)
#define __REGA0(arg)    __REG__(a0, arg)
#define __REGA1(arg)    __REG__(a1, arg)
#define __REGA2(arg)    __REG__(a2, arg)
#define __REGA3(arg)    __REG__(a3, arg)
#define __REGA4(arg)    __REG__(a4, arg)
#define __REGA5(arg)    __REG__(a5, arg)
#define __REGA6(arg)    __REG__(a6, arg)
#define __REGA7(arg)    __REG__(a7, arg)
#define __REGD0(arg)    __REG__(d0, arg)
#define __REGD1(arg)    __REG__(d1, arg)
#define __REGD2(arg)    __REG__(d2, arg)
#define __REGD3(arg)    __REG__(d3, arg)
#define __REGD4(arg)    __REG__(d4, arg)
#define __REGD5(arg)    __REG__(d5, arg)
#define __REGD6(arg)    __REG__(d6, arg)
#define __REGD7(arg)    __REG__(d7, arg)
#endif /* __REGA0 */

/****************************************************************************/

#else /* PPC */

/****************************************************************************/

#ifndef REG
#define REG(reg,arg) arg
#endif

#ifndef REGARGS
#define REGARGS
#endif

#ifndef STDARGS
#define STDARGS
#endif

#ifndef INTERRUPT
#define INTERRUPT
#endif

#ifndef SAVEDS
#define SAVEDS
#endif

#ifndef APICALL
#define APICALL __attribute__((libcall))
#endif

#ifndef VARARGS68K
#define VARARGS68K __attribute__((linearvarargs))
#endif

#ifndef BREL_RESTORE
#define BREL_RESTORE __attribute__((baserel_restore__))
#endif

/* Do not use these definitions in new code: internal use only! */
#ifndef __REGA0
#define __LIBBASE(arg) arg
#define __REGA0(arg)   arg
#define __REGA1(arg)   arg
#define __REGA2(arg)   arg
#define __REGA3(arg)   arg
#define __REGA4(arg)   arg
#define __REGA5(arg)   arg
#define __REGA6(arg)   arg
#define __REGA7(arg)   arg
#define __REGD0(arg)   arg
#define __REGD1(arg)   arg
#define __REGD2(arg)   arg
#define __REGD3(arg)   arg
#define __REGD4(arg)   arg
#define __REGD5(arg)   arg
#define __REGD6(arg)   arg
#define __REGD7(arg)   arg
#endif /* __REGA0 */

/****************************************************************************/

#endif /* neither 68k nor PPC */

#endif /* __GNUC__ */

/****************************************************************************/

#ifdef __VBCC__

#ifndef ASM
#define ASM
#endif

#ifndef INLINE
#define INLINE static
#endif

#ifndef FAR
#define FAR
#endif

#ifndef SAVEDS
#define SAVEDS __SAVE_DS__
#endif

#ifndef REGARGS
#define REGARGS
#endif

#ifndef STDARGS
#define STDARGS
#endif

/* GCC compatibility */
#define __attribute__(dummy)

#ifndef USED
#define USED __entry
#endif

#ifndef UNUSED
#define UNUSED
#endif

#ifndef USED_VAR
#define USED_VAR USED
#endif

#ifndef DEPRECATED
#define DEPRECATED
#endif

/****************************************************************************/

#ifdef __PPC__

/****************************************************************************/

#ifndef __REGD0
#define __LIBBASE(arg) arg
#define __REGD0(arg)   arg
#define __REGD1(arg)   arg
#define __REGD2(arg)   arg
#define __REGD3(arg)   arg
#define __REGD4(arg)   arg
#define __REGD5(arg)   arg
#define __REGD6(arg)   arg
#define __REGD7(arg)   arg
#define __REGA0(arg)   arg
#define __REGA1(arg)   arg
#define __REGA2(arg)   arg
#define __REGA3(arg)   arg
#define __REGA4(arg)   arg
#define __REGA5(arg)   arg
#define __REGA6(arg)   arg
#define __REGA7(arg)   arg
#endif

#ifndef REG
#define REG(reg,arg) arg
#endif

#ifndef INTERRUPT
#define INTERRUPT
#endif

#ifndef APICALL
#define APICALL __aos4libcall __linearvarargs
#endif

#ifndef VARARGS68K
#define VARARGS68K __linearvarargs
#endif

#ifndef BREL_RESTORE
#define BREL_RESTORE __brel
#endif

/****************************************************************************/

#else /* M68k */

/****************************************************************************/

#ifndef __REGD0
#define __LIBBASE(arg) arg
#define __REGD0(arg)   arg
#define __REGD1(arg)   arg
#define __REGD2(arg)   arg
#define __REGD3(arg)   arg
#define __REGD4(arg)   arg
#define __REGD5(arg)   arg
#define __REGD6(arg)   arg
#define __REGD7(arg)   arg
#define __REGA0(arg)   arg
#define __REGA1(arg)   arg
#define __REGA2(arg)   arg
#define __REGA3(arg)   arg
#define __REGA4(arg)   arg
#define __REGA5(arg)   arg
#define __REGA6(arg)   arg
#define __REGA7(arg)   arg
#endif

#ifndef REG
#define REG(reg, arg) __REG__(reg, arg)
#endif

#ifndef INTERRUPT
#define INTERRUPT __amigainterrupt
#endif

#ifndef APICALL
#define APICALL
#endif

#ifndef VARARGS68K
#define VARARGS68K
#endif

#ifndef BREL_RESTORE
#define BREL_RESTORE
#endif

#endif /* neither 68k nor PPC */

#endif /* __VBCC__ */

/****************************************************************************/

#endif /* AMIGA_COMPILER_H */