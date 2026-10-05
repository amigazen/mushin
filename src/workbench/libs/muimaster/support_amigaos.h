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

/*
 * m68k-amigaos-gcc defines __AMIGA__ more often than the bare AMIGA token
 * compiler-specific.h tests.  Set it so __REG__ takes the Amiga GNU branch.
 */
#if defined(__GNUC__) && !defined(AMIGA) && !defined(__MORPHOS__) && !defined(__AROS__)
#if defined(__mc68000__) || defined(__AMIGA__) || defined(__amigaos__)
#define AMIGA 1
#endif
#endif

/*
 * This GCC library is not base-relative.  Defining __SAVE_DS__ first keeps
 * the NDK from turning it into __saveds (that would demand an A4 base).
 */
#if defined(__GNUC__) && defined(MUSHIN_GCC_NATIVE)
#ifndef __SAVE_DS__
#define __SAVE_DS__
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
#ifndef VARARGS68K
#define VARARGS68K
#endif

/*
 * Public LVOs.  SAS/C jumps straight at these bodies, so they are register
 * entries via the NDK macros.  The GNU build points the jump table at sfdc
 * gatestubs, which call the bodies with stack arguments; those parameters
 * stay plain C or the gate and the prototype disagree.
 */
#if defined(__GNUC__)
#define MUI_LIB_ENTRY __ASM__
#define MUI_LIB_ARG(reg, arg) arg
#else
#define MUI_LIB_ENTRY __ASM__ __SAVE_DS__
#define MUI_LIB_ARG(reg, arg) __REG__(reg, arg)
#endif

#if !defined(__GNUC__)
#ifndef AMIGA_COMPILER_H
#include <amiga_compiler.h>
#endif
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
#define ZUNE_BUILTIN_FLOATTEXT 1
#define ZUNE_BUILTIN_DIRLIST 1
#define ZUNE_BUILTIN_DTPIC 1
#define ZUNE_BUILTIN_FLOATTEXT 1
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
int snprintf(char *buf, int size, const char *fmt, ...);      /* PRIV */
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
#       define __inline
#       define SAVEDS
#       define const
#   else
#       define SAVEDS __SAVE_DS__
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

/*
 * AROS spells registers in uppercase (A0).  __REG__ pastes its register
 * token, so map to a lowercase literal before the NDK macro sees it.
 */
#define MUI_REGPARM_D0(type, name) __REG__(d0, type name)
#define MUI_REGPARM_D1(type, name) __REG__(d1, type name)
#define MUI_REGPARM_D2(type, name) __REG__(d2, type name)
#define MUI_REGPARM_D3(type, name) __REG__(d3, type name)
#define MUI_REGPARM_D4(type, name) __REG__(d4, type name)
#define MUI_REGPARM_D5(type, name) __REG__(d5, type name)
#define MUI_REGPARM_D6(type, name) __REG__(d6, type name)
#define MUI_REGPARM_D7(type, name) __REG__(d7, type name)
#define MUI_REGPARM_A0(type, name) __REG__(a0, type name)
#define MUI_REGPARM_A1(type, name) __REG__(a1, type name)
#define MUI_REGPARM_A2(type, name) __REG__(a2, type name)
#define MUI_REGPARM_A3(type, name) __REG__(a3, type name)
#define MUI_REGPARM_A4(type, name) __REG__(a4, type name)
#define MUI_REGPARM_A5(type, name) __REG__(a5, type name)
#define MUI_REGPARM_A6(type, name) __REG__(a6, type name)
#define MUI_REGPARM_A7(type, name) __REG__(a7, type name)

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

#define AROS_LHA(type, name, reg) MUI_REGPARM_##reg(type, name)

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

#define AROS_UFHA(type, name, reg) MUI_REGPARM_##reg(type, name)

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
