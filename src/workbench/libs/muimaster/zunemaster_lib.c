/*
    Copyright (C) 2002-2020, The AROS Development Team.
    All rights reserved.
    

    Library code for AmigaOS, based on Dirk Stöckers Libary
    Example
*/

#define BASE_GLOBAL
#define __USE_SYSBASE

struct ExecBase *SysBase;

#include <proto/exec.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <intuition/intuitionbase.h>
#include <exec/execbase.h>

LONG ReturnError2(void)
{
  return -1;
}

#include "muimaster_intern.h"

/* Function declarations for MUI functions - matching muimaster_lib.sfd */
extern Object *MUI_NewObjectA(CONST_STRPTR classname, struct TagItem *tags);
extern Object *MUI_NewObject(CONST_STRPTR classname, ...);
extern VOID MUI_DisposeObject(Object *obj);
extern LONG MUI_RequestA(APTR app, APTR win, LONGBITS flags, CONST_STRPTR title, CONST_STRPTR gadgets, CONST_STRPTR format, APTR params);
extern LONG MUI_Request(APTR app, APTR win, LONGBITS flags, CONST_STRPTR title, CONST_STRPTR gadgets, CONST_STRPTR format, ...);
extern APTR MUI_AllocAslRequest(ULONG reqType, struct TagItem *tagList);
extern APTR MUI_AllocAslRequestTags(ULONG reqType, ...);
extern BOOL MUI_AslRequest(APTR requester, struct TagItem *tagList);
extern BOOL MUI_AslRequestTags(APTR requester, ...);
extern VOID MUI_FreeAslRequest(APTR requester);
extern LONG MUI_Error(void);
extern LONG MUI_SetError(LONG num);
extern struct IClass *MUI_GetClass(CONST_STRPTR classname);
extern VOID MUI_FreeClass(struct IClass *classptr);
extern VOID MUI_RequestIDCMP(Object *obj, ULONG flags);
extern VOID MUI_RejectIDCMP(Object *obj, ULONG flags);
extern VOID MUI_Redraw(Object *obj, ULONG flags);
extern struct MUI_CustomClass *MUI_CreateCustomClass(struct Library *base, CONST_STRPTR supername, struct MUI_CustomClass *supermcc, ULONG datasize, APTR dispatcher);
extern BOOL MUI_DeleteCustomClass(struct MUI_CustomClass *mcc);
extern Object *MUI_MakeObjectA(LONG type, IPTR *params);
extern Object *MUI_MakeObject(LONG type, ...);
extern BOOL MUI_Layout(Object *obj, LONG left, LONG top, LONG width, LONG height, ULONG flags);
extern LONG MUI_ObtainPen(struct MUI_RenderInfo *mri, struct MUI_PenSpec *spec, ULONG flags);
extern VOID MUI_ReleasePen(struct MUI_RenderInfo *mri, LONG pen);
extern APTR MUI_AddClipping(struct MUI_RenderInfo *mri, WORD left, WORD top, WORD width, WORD height);
extern VOID MUI_RemoveClipping(struct MUI_RenderInfo *mri, APTR handle);
extern APTR MUI_AddClipRegion(struct MUI_RenderInfo *mri, struct Region *r);
extern VOID MUI_RemoveClipRegion(struct MUI_RenderInfo *mri, APTR handle);
extern BOOL MUI_BeginRefresh(struct MUI_RenderInfo *mri, ULONG flags);
extern VOID MUI_EndRefresh(struct MUI_RenderInfo *mri, ULONG flags);

#include "native_version.h"
#define VERSION MUSHIN_LIBRARY_VERSION
#define REVISION MUSHIN_LIBRARY_REVISION
#define DATETXT MUSHIN_LIBRARY_DATE
#define VERSTXT MUSHIN_LIBRARY_VERSION_STRING
/*
 * The name exec puts in the RomTag, which is also the name callers have to
 * pass to OpenLibrary().  These cannot be allowed to disagree: OpenLibrary()
 * loads LIBS:<requested name> and then looks the *RomTag* name up in the
 * system library list, so if the two differ every open fails even though the
 * file loaded perfectly.  It must therefore match both the filename the
 * smakefile produces and MUIMASTER_NAME in mui.h.
 *
 * It used to read "muimaster.library" while the smakefile built a file called
 * zunemaster.library and mui.h told callers to ask for zunemaster.library.
 *
 * The default is zunemaster.library so this library can be installed and
 * tested alongside a real muimaster.library.  "smake muimaster.library"
 * recompiles this one file with MUIMASTER_DROPIN set to produce the drop-in
 * replacement; no other source file depends on the name, because everything
 * internal reads MUIMasterBase->lib_Node.ln_Name at runtime instead.
 */
#ifdef MUIMASTER_DROPIN
#define LIBNAME  "muimaster.library"
#else
#define LIBNAME  "zunemaster.library"
#endif
#define IDSTRING "$VER: " LIBNAME " " VERSTXT " (" DATETXT ")\r\n"

/*
 * kprintf (debug.lib) talks to the serial port.  LibInit / LibOpen run inside
 * OpenLibrary(), which holds a Forbid, so a kprintf that Waits for the serial
 * device deadlocks the machine.  Leave this off until the open path returns.
 */
/* #define MYDEBUG 1 */
#include "debug.h"

typedef BPTR SEGLISTPTR;
#define LC_LIBHEADERTYPEPTR struct Library *

/************************************************************************/

/* First executable routine of this library; must return an error
   to the unsuspecting caller (CLib39x LibStart). */
LONG LibStart(void)
{
  return -1;
}

LONG ReturnError(void)
{
  return LibStart();
}

/************************************************************************/

/* MUI private functions */

/*
 * LVO 0x84 to 0x96, so these are entered straight from an application with
 * nothing but A6 set up, exactly like LibOpen() and friends below, and they
 * need __saveds for the same reason.  D() is empty while MYDEBUG is off, but
 * the attribute has to stay so a later debug rebuild does not reintroduce a
 * near-data access with the caller's A4.  Every other function reachable
 * through the jump table - all of mui_*.c - is declared __asm __saveds already.
 */

ASM SAVEDS void MUI_Priv1(REG(a6, struct Library *MUIMasterBase))
{
        D(bug("MUI_Priv1() called"));
}

ASM SAVEDS void MUI_Priv2(REG(a6, struct Library *MUIMasterBase))
{
        D(bug("MUI_Priv2() called"));
}

ASM SAVEDS void MUI_Priv3(REG(a6, struct Library *MUIMasterBase))
{
        D(bug("MUI_Priv3() called"));
}

ASM SAVEDS void MUI_Priv4(REG(a6, struct Library *MUIMasterBase))
{
        D(bug("MUI_Priv4() called"));
}

/************************************************************************/

/* Some functions which are somewhere else */

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR MUIMasterBase);
ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR MUIMasterBase);
void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR MUIMasterBase);
void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR MUIMasterBase);

/************************************************************************/

/*
 * MakeLibrary() feeds this table to InitStruct() before calling LibInit.
 *
 * The previous form was a struct of UBYTEs mixed with STRPTRs, using the
 * compact 0xA0/0x80/0x90 commands.  InitStruct long-aligns a 0x80 payload,
 * which only matches that struct if the compiler also 4-aligns pointers.
 * SAS/C's default ALIGNMENT=2 puts the name pointer two bytes early, so
 * Kickstart reads the next command as part of ln_Name and then walks off
 * the table into LibVectors[] - OpenLibrary() never returns.
 *
 * CLib39x (and Commodore's exec/initializers.h) use a stream of UWORDs so
 * the compiler cannot insert padding.  INITBYTE/INITWORD are the 0xE000/0xD000
 * commands Kickstart documents.  Fallbacks below match those macros in case
 * an older NDK header only provides OFFSET.
 */
#ifndef INITBYTE
#define INITBYTE(offset,value)  0xe000,(UWORD)(offset),(UWORD)((value)<<8)
#define INITWORD(offset,value)  0xd000,(UWORD)(offset),(UWORD)(value)
#endif

/*
 * Name and id-string pointers stay out of this table: INITLONG splits an
 * address with >>16, and SAS/C will not accept that as a static initializer
 * (Error 20).  LibInit writes both pointers itself, the same way CLib39x
 * InitLib does.
 */
static const UWORD LibInitData[] = {
  INITBYTE(OFFSET(Node,    ln_Type),      NT_LIBRARY),
  INITBYTE(OFFSET(Library, lib_Flags),    LIBF_SUMUSED|LIBF_CHANGED),
  INITWORD(OFFSET(Library, lib_Version),  VERSION),
  INITWORD(OFFSET(Library, lib_Revision), REVISION),
  0
};

/************************************************************************/
extern const ULONG LibInitTable[4]; /* the prototype */

/* The library loader looks for this marker in the memory
   the library code and data will occupy. It is responsible
   setting up the Library base data structure. */
const struct Resident RomTag = {
  RTC_MATCHWORD,                   /* Marker value. */
  (struct Resident *)&RomTag,      /* This points back to itself. */
#ifdef MUSHIN_GCC_NATIVE
  (struct Resident *)(&RomTag + 1), /* GCC may reorder the static tables. */
#else
  (struct Resident *)LibInitTable, /* This points somewhere behind this marker. */
#endif
  RTF_AUTOINIT,                    /* The Library should be set up according to the given table. */
  VERSION,                         /* The version of this Library. */
  NT_LIBRARY,                      /* This defines this module as a Library. */
  0,                               /* Initialization priority of this Library; unused. */
  LIBNAME,                         /* Points to the name of the Library. */
  IDSTRING,                        /* The identification string of this Library. */
  (APTR)&LibInitTable              /* This table is for initializing the Library. */
};

/************************************************************************/

/*
 * The four vectors below, plus LibInit further down, are the only functions in
 * this library that the system enters directly: exec calls them from
 * OpenLibrary(), CloseLibrary(), RemLibrary() and InitResident().  scoptions
 * selects SMALLDATA, so every reference to a global compiles to an A4-relative
 * access, and exec plainly does not set A4 to this library's data base before
 * jumping through the vector table.  Each one therefore has to be SAVEDS - just
 * like the MUI_* entry points in the mui_*.c files and the BOOPSI dispatchers,
 * which have always been declared that way - so that it reloads A4 from
 * __LinkerDB on entry.
 *
 * These six were the only entry points in the library that were missing it.
 * LibInit's "SysBase = sysbase" is the damaging case: with the caller's A4
 * still in place, that assignment wrote ExecBase to whatever address happened
 * to sit at the same near-data offset from the caller's A4 rather than to this
 * library's own SysBase.  The global was left as the linker set it, so the
 * first OpenLibrary() in L_InitLib() called through a NULL base - which is why
 * OpenLibrary("muimaster.library") never returned.  LibExpunge and LibClose
 * have the same problem with the Remove() and FreeMem() calls they make, and
 * would have taken the machine down on the way out even if init had survived.
 */

/* The mandatory reserved library function */
SAVEDS ULONG LibReserved(void)
{
  return 0;
}

/* Open the library, as called via OpenLibrary() */
ASM SAVEDS struct Library *LibOpen(REG(a6, struct MUIMasterBase_intern * MUIMasterBase))
{
  /* Prevent delayed expunge and increment opencnt */
  MUIMasterBase->library.lib_Flags &= ~LIBF_DELEXP;
  MUIMasterBase->library.lib_OpenCnt++;

  return &MUIMasterBase->library;
}

/* Expunge the library, remove it from memory */
ASM SAVEDS SEGLISTPTR LibExpunge(REG(a6, struct MUIMasterBase_intern *mb))
{
  if (!mb->library.lib_OpenCnt &&
      ZUNE_FreeBuiltinClasses(&mb->library))
  {
    SEGLISTPTR seglist;

    seglist = mb->seglist;

    L_ExpungeLib(&mb->library);

    /* Remove the library from the public list */
    Remove((struct Node *)mb);

    /* Free the vector table and the library data */
    FreeMem((STRPTR) mb - mb->library.lib_NegSize,
    mb->library.lib_NegSize +
    mb->library.lib_PosSize);

    return seglist;
  }
  else
    mb->library.lib_Flags |= LIBF_DELEXP;

  /* Return the segment pointer, if any */
  return 0;
}

/* Close the library, as called by CloseLibrary() */
ASM SAVEDS SEGLISTPTR LibClose(REG(a6, struct MUIMasterBase_intern *mb))
{
  if(!(--mb->library.lib_OpenCnt))
  {
    if (mb->library.lib_Flags & LIBF_DELEXP)
      return LibExpunge(mb);
  }
  return 0;
}

#undef SysBase
extern struct ExecBase *SysBase;

/* Initialize library */
ASM SAVEDS struct Library *LibInit(REG(a0, SEGLISTPTR seglist), REG(d0, struct MUIMasterBase_intern *mb), REG(a6, struct ExecBase *sysbase))
{
#ifdef _M68060
  if(!(sysbase->AttnFlags & AFF_68060))
    return 0;
#elif defined (_M68040)
  if(!(sysbase->AttnFlags & AFF_68040))
    return 0;
#elif defined (_M68030)
  if(!(sysbase->AttnFlags & AFF_68030))
    return 0;
#elif defined (_M68020)
  if(!(sysbase->AttnFlags & AFF_68020))
    return 0;
#endif

  mb->seglist = seglist;
  mb->sysbase = sysbase;
  SysBase = sysbase;

  /*
   * CLib39x InitLib writes these itself rather than trusting InitStruct
   * alone.  Do the same: if the table is ignored or only partly applied,
   * AddLibrary() still sees a valid name, type and id string.
   */
  mb->library.lib_Node.ln_Type = NT_LIBRARY;
  mb->library.lib_Node.ln_Name = LIBNAME;
  mb->library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
  mb->library.lib_Version = VERSION;
  mb->library.lib_Revision = REVISION;
  mb->library.lib_IdString = IDSTRING;

  D(bug("Librarybase at 0x%p\n",mb));

  if (L_InitLib(&mb->library))
    return &mb->library;

  FreeMem((STRPTR)mb - mb->library.lib_NegSize,
  mb->library.lib_NegSize +
  mb->library.lib_PosSize);
  return 0;
}

/************************************************************************/
/* LVO jump table (CLib39x FuncTab[] pattern): four mandatory library
   vectors, then muimaster_lib.fd entries through MUI_EndRefresh, -1.
   Slot indices 4..32 map to LVO 0x1e..0xc6 (bias 30); Priv1-4 occupy
   0x84..0x96.  No trailing mui38dev/MUI 5 slots until implemented. */

#ifdef __GNUC__
#include "gates.h"
#define MUI_VECTOR(name) (APTR) Gate_##name
#else
#define MUI_VECTOR(name) (APTR) name
#endif

static const APTR LibVectors[] = {
  (APTR) LibOpen,
  (APTR) LibClose,
  (APTR) LibExpunge,
  (APTR) LibReserved,
  MUI_VECTOR(MUI_NewObjectA),
  MUI_VECTOR(MUI_DisposeObject),
  MUI_VECTOR(MUI_RequestA),
  MUI_VECTOR(MUI_AllocAslRequest),
  MUI_VECTOR(MUI_AslRequest),
  MUI_VECTOR(MUI_FreeAslRequest),
  MUI_VECTOR(MUI_Error),
  MUI_VECTOR(MUI_SetError),
  MUI_VECTOR(MUI_GetClass),
  MUI_VECTOR(MUI_FreeClass),
  MUI_VECTOR(MUI_RequestIDCMP),
  MUI_VECTOR(MUI_RejectIDCMP),
  MUI_VECTOR(MUI_Redraw),
  MUI_VECTOR(MUI_CreateCustomClass),
  MUI_VECTOR(MUI_DeleteCustomClass),
  MUI_VECTOR(MUI_MakeObjectA),
  MUI_VECTOR(MUI_Layout),
  (APTR) MUI_Priv1,
  (APTR) MUI_Priv2,
  (APTR) MUI_Priv3,
  (APTR) MUI_Priv4,
  MUI_VECTOR(MUI_ObtainPen),
  MUI_VECTOR(MUI_ReleasePen),
  MUI_VECTOR(MUI_AddClipping),
  MUI_VECTOR(MUI_RemoveClipping),
  MUI_VECTOR(MUI_AddClipRegion),
  MUI_VECTOR(MUI_RemoveClipRegion),
  MUI_VECTOR(MUI_BeginRefresh),
  MUI_VECTOR(MUI_EndRefresh),
  (APTR) -1
};

/* The following data structures and data are responsible for
   setting up the Library base data structure and the library
   function vector.
*/
const ULONG LibInitTable[4] = {
  (ULONG)sizeof(struct MUIMasterBase_intern), /* Size of the base data structure */
  (ULONG)LibVectors,             /* Points to the LVO jump table above */
  (ULONG)LibInitData,            /* Library base data structure setup table */
  (ULONG)LibInit                 /* The address of the routine to do the setup */
};

void _CXFERR(void)
{
    D(bug("CXFERR\n"));
}

#ifdef __SASC
/* Stubs required when linking sc.lib into a shared library (CLib39x LibInit.c). */
void __regargs __chkabort(void) { }
void __regargs _CXBRK(void)     { }
void __saveds __XCEXIT(void)  { }
#endif
