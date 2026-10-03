/*
    Copyright (C) 2003, The AROS Development Team. All rights reserved.
*/

#ifndef _MUIMASTER_SUPPORT_AMIGAOS_H_
#define _MUIMASTER_SUPPORT_AMIGAOS_H_

#include <stddef.h>

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#if defined(__GNUC__) && defined(__mc68000__)
#define ASM
#define REG(reg, arg) arg __asm(#reg)
#define SAVEDS
#define STDARGS __stdargs
#define VARARGS68K
#else
#ifndef AMIGA_COMILER_H
#include <amiga_compiler.h>
#endif
#endif

/* GCC uses generated register gates; SAS/C enters these bodies directly. */
#ifdef __GNUC__
#define MUI_LIB_ENTRY
#define MUI_LIB_ARG(reg, arg) arg
#else
#define MUI_LIB_ENTRY __asm __saveds
#define MUI_LIB_ARG(reg, arg) register __##reg arg
#endif

#ifndef PROTO_UTILITY_H
#include <proto/utility.h>
#endif

/* These are the identity function under AmigaOS */
#define AROS_LONG2BE(x) (x)
#define AROS_BE2LONG(x) (x)

#define IMSPEC_EXTERNAL_PREFIX "MUI:Images/"

/* Function declarations */
LONG HexToIPTR(CONST_STRPTR s, ULONG *val);
LONG HexToLong(CONST_STRPTR s, ULONG *val);

/*
 * Which classes support_classes.c puts into builtins[].
 *
 * On AROS these come from mmakefile.src; on AmigaOS the list is maintained by
 * hand here, and every entry support_classes.h tests MUST appear below.  An
 * undefined macro is 0 under ANSI #if, so a missing entry silently drops the
 * class from builtins[] *and* from the guarded descriptor in its own .c file -
 * no warning, no link error, just MUI_GetClass() failing at runtime.  Fourteen
 * entries were missing (this is still the case upstream), which is why the
 * whole list is now spelled out explicitly, zeros included.
 *
 * Anything set to 0 below is deliberate; see the note against each one.
 */
#define ZUNE_BUILTIN_ABOUTMUI 1
#define ZUNE_BUILTIN_BALANCE 1
#define ZUNE_BUILTIN_BOOPSI 1
#define ZUNE_BUILTIN_COLORADJUST 1
#define ZUNE_BUILTIN_COLORFIELD 1
#define ZUNE_BUILTIN_CRAWLING 1
#define ZUNE_BUILTIN_DIRLIST 1
#define ZUNE_BUILTIN_DTPIC 1
#define ZUNE_BUILTIN_FRAMEADJUST 1
#define ZUNE_BUILTIN_FRAMEDISPLAY 1
#define ZUNE_BUILTIN_GAUGE 1
/* No iconlist.c / iconlistview.c in this tree. */
#define ZUNE_BUILTIN_ICONLISTVIEW 0
#define ZUNE_BUILTIN_IMAGEADJUST 1
#define ZUNE_BUILTIN_IMAGEDISPLAY 1
#define ZUNE_BUILTIN_KNOB 1
#define ZUNE_BUILTIN_LEVELMETER 1
#define ZUNE_BUILTIN_NUMERICBUTTON 1
/* No DRAGHANDLE / PANEL / PANELGROUP / PANELTITLE entries: those four classes
   are an AROS desktop extension that no MUI release provides, and AROS' Panel
   even collides with the name of a real, unrelated MUI class.  See the note in
   mui.h where their headers used to be included. */
#define ZUNE_BUILTIN_PALETTE 1
#define ZUNE_BUILTIN_PENADJUST 1
#define ZUNE_BUILTIN_PENDISPLAY 1
#define ZUNE_BUILTIN_POPASL 1
#define ZUNE_BUILTIN_POPFRAME 1
#define ZUNE_BUILTIN_POPIMAGE 1
#define ZUNE_BUILTIN_POPLIST 1
#define ZUNE_BUILTIN_POPPEN 1
#define ZUNE_BUILTIN_POPSCREEN 1
#define ZUNE_BUILTIN_RADIO 1
#define ZUNE_BUILTIN_SCALE 1
#define ZUNE_BUILTIN_SCROLLGROUP 1
#define ZUNE_BUILTIN_SETTINGS 1
#define ZUNE_BUILTIN_SETTINGSGROUP 1
#define ZUNE_BUILTIN_VIRTGROUP 1
#define ZUNE_BUILTIN_VOLUMELIST 1

#ifdef __SASC
#include <dos.h>
#endif

/*
 * PI used to be defined here as 3.1415 and M_PI as PI.  PI itself is gone:
 * nothing referred to it - classes/knob.c and classes/levelmeter.c write
 * 3.14159265358979323846 out in full - and SAS/C's own <math.h> defines PI
 * without an #ifndef guard, so whichever header came second produced a
 * redefinition warning.  It also reached the generated <libraries/mui.h>,
 * where a four-digit PI would have shadowed the real one for applications.
 *
 * M_PI has to stay: imspec_gradientdraw.c uses it for the gradient angle, and
 * SAS/C's <math.h> does not provide it.  Written out in full rather than
 * derived from PI, so the gradient maths no longer silently depends on a
 * constant that was only accurate to four decimal places.  It is marked
 * private so buildincludes.c keeps it out of the generated <libraries/mui.h>:
 * it is the library's own business, and an application that includes both that
 * header and <math.h> should get whichever of the two it asked for.
 */
#ifndef M_PI                            /* PRIV */
#define M_PI 3.14159265358979323846     /* PRIV */
#endif                                  /* PRIV */

#define AROS_STACKSIZE 65536

char *StrDup(const char *x);
#ifdef __GNUC__
int stricmp(const char *left, const char *right);
#endif
#if defined(__SASC) || defined(__GNUC__) /* PRIV */
size_t strlcat(char *buf, const char *src, size_t len); /* PRIV */
#endif /* PRIV */
Object *DoSuperNewTagList(struct IClass *cl, Object *obj,void *dummy, struct TagItem *tags);
Object *VARARGS68K DoSuperNewTags(struct IClass *cl, Object *obj, void *dummy, ...);
int VARARGS68K SPrintf(char *buf, const char *fmt, ...);

/*
 * Declared here because support_amigaos.c supplies both of these on m68k
 * (SAS/C 6.x has no C99 snprintf, and its sprintf would drag stdio into a
 * library that has no startup code).  Without the prototypes IGNORE=63 in the
 * smakefile silently accepted every call as an undeclared function.
 * See the comment at the head of support_amigaos.c: these implement the
 * RawDoFmt format dialect, so all integer conversions need the 'l' modifier.
 */
#ifndef __amigaos4__                                          /* PRIV */
int snprintf(char *buf, size_t size, const char *fmt, ...);      /* PRIV */
int sprintf(char *buf, const char *fmt, ...);                 /* PRIV */
#endif                                                        /* PRIV */


#ifdef __amigaos4__       /* PRIV */
#ifndef WritePixelArrayAlpha /* PRIV */
#define WritePixelArrayAlpha(srcRect, SrcX, SrcY, SrcMod, RastPort, DestX, DestY, SizeX, SizeY, globalAlpha) ICyberGfx->WritePixelArrayAlpha(srcRect, SrcX, SrcY, SrcMod, RastPort, DestX, DestY, SizeX, SizeY, globalAlpha) /* PRIV */
#endif /* PRIV */
#endif /* PRIV */


/*** HookEntry for OS4 (is only a dummy) ************************************/
#ifdef __amigaos4__
ASM ULONG HookEntry(REG(a0, struct Hook *hook),REG(a2, APTR obj), REG(a1, APTR msg));
#endif

/*** OS4 Exec Interface support *********************************************/
#ifdef __amigaos4__
#define EXEC_INTERFACE_DECLARE(x) x
#define EXEC_INTERFACE_GET_MAIN(interface,libbase) (interface = (void*)GetInterface(libbase,"main",1,NULL))
#define EXEC_INTERFACE_DROP(interface) DropInterface((struct Interface*)interface)
#define EXEC_INTERFACE_ASSIGN(a,b) (a = b)
#else
#define EXEC_INTERFACE_DECLARE(x)
#define EXEC_INTERFACE_GET_MAIN(interface,libbase) 1
#define EXEC_INTERFACE_DROP(interface)
#define EXEC_INTERFACE_ASSIGN(a,b)
#endif

/*** AROS Exec extensions ***************************************************/
#ifndef __amigaos4__
APTR AllocVecPooled(APTR pool, ULONG size);
VOID FreeVecPooled(APTR pool, APTR memory);
#endif

/*** AROS Intuition extensions **********************************************/
#define DeinitRastPort(rp)      
#define CloneRastPort(rp) (rp)  
#define FreeRastPort(rp)        

/*** Miscellanous compiler supprot ******************************************/
#ifndef SAVEDS
#   ifdef __MAXON__
#       define __asm
#       define __inline
#       define SAVEDS
#       define const
#   else
#       define SAVEDS __saveds
#   endif
#endif 

#define __stackparm

/*** Miscellanous AROS macros ***********************************************/
#define AROS_LIBFUNC_INIT
#define AROS_LIBBASE_EXT_DECL(a, b) extern a b;
#define AROS_LIBFUNC_EXIT
#define AROS_ASMSYMNAME(a) a

#define LC_BUILDNAME(x) x
#define LC_LIBHEADERTYPEPTR struct Library *
#define LIBBASETYPEPTR struct Library *

/*** AROS types *************************************************************/
#ifndef __AROS_TYPES_DEFINED__
#   define __AROS_TYPES_DEFINED__
    typedef unsigned long IPTR;
    /*
     * The signed counterpart of IPTR, used wherever a tag value or a method
     * argument carries a number that can go negative - classes/list.c,
     * classes/menuitem.c, classes/popobject.c, classes/scrollgroup.c and
     * classes/palette.c all cast through it.  It was missing here, and those
     * sources only ever compiled because they reached the generated
     * <libraries/mui.h>, which still carries a copy from an older revision of
     * this header.  font.c had worked around it with a local
     * "#define SIPTR LONG".
     */
    typedef signed long   SIPTR;
    typedef long          STACKLONG;
    typedef unsigned long STACKULONG;
    typedef void (*VOID_FUNC)();
#define STACKED 
#endif /* __AROS_TYPES_DEFINED__ */

/*** AROS list macros *******************************************************/
#define ForeachNode(l,n)                       \
for                                            \
(                                              \
    n=(void *)(((struct List *)(l))->lh_Head); \
    ((struct Node *)(n))->ln_Succ;             \
    n=(void *)(((struct Node *)(n))->ln_Succ)  \
)

/* ForeachNodeSafe() was added here for classes/panelgroup.c, the only thing in
   the library that ever used it, and went with that file.  AROS defines it in
   <exec/lists.h> if it is ever needed again. */

/*** AROS register definitions **********************************************/
#ifdef __GNUC__
#define MUI_REG_A0 "a0"
#define MUI_REG_A1 "a1"
#define MUI_REG_A2 "a2"
#define MUI_REG_A3 "a3"
#define MUI_REG_A4 "a4"
#define MUI_REG_A5 "a5"
#define MUI_REG_A6 "a6"
#define MUI_REG_A7 "a7"
#define MUI_REG_D0 "d0"
#define MUI_REG_D1 "d1"
#define MUI_REG_D2 "d2"
#define MUI_REG_D3 "d3"
#define MUI_REG_D4 "d4"
#define MUI_REG_D5 "d5"
#define MUI_REG_D6 "d6"
#define MUI_REG_D7 "d7"
#endif

#define __REG_D0 __d0
#define __REG_D1 __d1
#define __REG_D2 __d2
#define __REG_D3 __d3
#define __REG_D4 __d4
#define __REG_D5 __d5
#define __REG_D6 __d6
#define __REG_D7 __d7
#define __REG_A0 __a0
#define __REG_A1 __a1
#define __REG_A2 __a2
#define __REG_A3 __a3
#define __REG_A4 __a4
#define __REG_A5 __a5
#define __REG_A6 __a6
#define __REG_A7 __a7

/*** AROS library function macros *******************************************/
#define AROS_LH0(rt, fn, bt, bn, lvo, p) \
    ASM rt LIB_##fn (void)
#define AROS_LH1(rt, fn, a1, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1)
#define AROS_LH2(rt, fn, a1, a2, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2)
#define AROS_LH3(rt, fn, a1, a2, a3, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3)
#define AROS_LH4(rt, fn, a1, a2, a3, a4, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3, a4)
#define AROS_LH5(rt, fn, a1, a2, a3, a4, a5, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3, a4, a5)
#define AROS_LH6(rt, fn, a1, a2, a3, a4, a5, a6, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3, a4, a5, a6)
#define AROS_LH7(rt, fn, a1, a2, a3, a4, a5, a6, a7, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3, a4, a5, a6, a7)
#define AROS_LH8(rt, fn, a1, a2, a3, a4, a5, a6, a7, a8, bt, bn, lvo, p) \
    ASM rt LIB_##fn (a1, a2, a3, a4, a5, a6, a7, a8)

#ifdef __SASC
#   define AROS_LHA(type, name, reg) register __REG_##reg type name
#else
#   define AROS_LHA(type, name, reg) type name
#endif

/*** AROS user function macros **********************************************/
#define AROS_USERFUNC_INIT
#define AROS_USERFUNC_EXIT

#define AROS_UFH0(rt, fn) \
    ASM rt fn (void)
#define AROS_UFH1(rt, fn, a1) \
    ASM rt fn (a1)
#define AROS_UFH2(rt, fn, a1, a2) \
    ASM rt fn (a1, a2)
#define AROS_UFH3(rt, fn, a1, a2, a3) \
    ASM rt fn (a1, a2, a3)
#define AROS_UFH4(rt, fn, a1, a2, a3, a4) \
    ASM rt fn (a1, a2, a3, a4)
#define AROS_UFH5(rt, fn, a1, a2, a3, a4, a5) \
    ASM rt fn (a1, a2, a3, a4, a5)
#define AROS_UFH6(rt, fn, a1, a2, a3, a4, a5, a6) \
    ASM rt fn (a1, a2, a3, a4, a5, a6)
#define AROS_UFH7(rt, fn, a1, a2, a3, a4, a5, a6, a7) \
    ASM rt fn (a1, a2, a3, a4, a5, a6, a7)
#define AROS_UFH8(rt, fn, a1, a2, a3, a4, a5, a6, a7, a8) \
    ASM rt fn (a1, a2, a3, a4, a5, a6, a7, a8)

#define AROS_UFH0S(rt, fn) \
    ASM static rt fn (void)
#define AROS_UFH1S(rt, fn, a1) \
    ASM static rt fn (a1)
#define AROS_UFH2S(rt, fn, a1, a2) \
    ASM static rt fn (a1, a2)
#define AROS_UFH3S(rt, fn, a1, a2, a3) \
    ASM static rt fn (a1, a2, a3)
#define AROS_UFH4S(rt, fn, a1, a2, a3, a4) \
    ASM static rt fn (a1, a2, a3, a4)
#define AROS_UFH5S(rt, fn, a1, a2, a3, a4, a5) \
    ASM static rt fn (a1, a2, a3, a4, a5)
#define AROS_UFH6S(rt, fn, a1, a2, a3, a4, a5, a6) \
    ASM static rt fn (a1, a2, a3, a4, a5, a6)
#define AROS_UFH7S(rt, fn, a1, a2, a3, a4, a5, a6, a7) \
    ASM static rt fn (a1, a2, a3, a4, a5, a6, a7)
#define AROS_UFH8S(rt, fn, a1, a2, a3, a4, a5, a6, a7, a8) \
    ASM static rt fn (a1, a2, a3, a4, a5, a6, a7, a8)

#ifdef __SASC
#   define AROS_UFHA(type, name, reg) register __REG_##reg type name
#elif defined(__GNUC__) && defined(__mc68000__)
#   define AROS_UFHA(type, name, reg) type name __asm(MUI_REG_##reg)
#else
#   define AROS_UFHA(type, name, reg) type name
#endif

#define AROS_UFP0 AROS_UFH0
#define AROS_UFP1 AROS_UFH1
#define AROS_UFP2 AROS_UFH2
#define AROS_UFP3 AROS_UFH3
#define AROS_UFP4 AROS_UFH4
#define AROS_UFP5 AROS_UFH5
#define AROS_UFP6 AROS_UFH6
#define AROS_UFP7 AROS_UFH7
#define AROS_UFP8 AROS_UFH8

#define AROS_UFPA AROS_UFHA

/* 
    With the following define a typical dispatcher will looks like this:
    BOOPSI_DISPATCHER(IPTR,IconWindow_Dispatcher,cl,obj,msg)
*/
#define BOOPSI_DISPATCHER(rettype,name,cl,obj,msg) \
    AROS_UFH3(SAVEDS rettype, name,\
        AROS_UFHA(Class  *, cl,  A0),\
        AROS_UFHA(Object *, obj, A2),\
        AROS_UFHA(Msg     , msg, A1)) {AROS_USERFUNC_INIT
#define BOOPSI_DISPATCHER_END AROS_USERFUNC_EXIT}
#define BOOPSI_DISPATCHER_PROTO(rettype,name,cl,obj,msg) \
    AROS_UFP3(SAVEDS rettype, name,\
        AROS_UFPA(Class  *, cl,  A0),\
        AROS_UFPA(Object *, obj, A2),\
        AROS_UFPA(Msg     , msg, A1))


#endif /* _MUIMASTER_SUPPORT_AMIGAOS_H_ */
