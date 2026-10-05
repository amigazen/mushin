/*
    Minimal AmigaOS / SAS/C support for muiscreen.library.
    Mirrors the pieces of muimaster's support_amigaos.h that this
    library actually needs (types, list macros, compiler wrappers).
*/

#ifndef _MUISCREEN_SUPPORT_AMIGAOS_H_
#define _MUISCREEN_SUPPORT_AMIGAOS_H_

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

#ifndef AMIGA_COMPILER_H
#include <amiga_compiler.h>
#endif

#ifdef __SASC
#include <dos.h>
#endif

#ifndef SAVEDS
#define SAVEDS __saveds
#endif

#ifndef ASM
#define ASM __asm
#endif

#ifndef REG
#define REG(reg,arg) register __##reg arg
#endif

#ifndef STDARGS
#define STDARGS __stdargs
#endif

#define LC_BUILDNAME(x) x
#define LC_LIBHEADERTYPEPTR struct Library *
#define LIBBASETYPEPTR struct Library *

#ifndef __AROS_TYPES_DEFINED__
#define __AROS_TYPES_DEFINED__
typedef unsigned long IPTR;
typedef signed long   SIPTR;
#endif

#define ForeachNode(l,n)                       \
for                                            \
(                                              \
    n=(void *)(((struct List *)(l))->lh_Head); \
    ((struct Node *)(n))->ln_Succ;             \
    n=(void *)(((struct Node *)(n))->ln_Succ)  \
)

#define ForeachNodeSafe(l,n,n2)                                 \
for                                                             \
(                                                               \
    n=(void *)(((struct List *)(l))->lh_Head);                  \
    ((n2)=(void *)(((struct Node *)(n))->ln_Succ)) != NULL;     \
    n=(void *)(n2)                                              \
)

#endif /* _MUISCREEN_SUPPORT_AMIGAOS_H_ */
