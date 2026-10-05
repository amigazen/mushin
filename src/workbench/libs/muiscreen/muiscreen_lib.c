/*
    Copyright (C) 2009-2025, The AROS Development Team.
    All rights reserved.

    Library startup, resident tag, and LVO function table for
    muiscreen.library on AmigaOS (SAS/C slink build).

    LibVectors[] must match muiscreen_lib.fd exactly so that LVO
    offsets align with muiscreen_pragmas.h (bias 30, first public
    call MUIS_AllocPubScreenDesc at -0x1e).
*/

#define BASE_GLOBAL
#define __USE_SYSBASE

struct ExecBase *SysBase;

#include <proto/exec.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>

LONG ReturnError2(void)
{
  return -1;
}

#include "muiscreen_intern.h"

#define VERSION   1
#define REVISION  1
#define DATETXT   "14.09.2026"
#define VERSTXT   "1.1"
#define LIBNAME   "muiscreen.library"
#define IDSTRING  "$VER: " LIBNAME " " VERSTXT " (" DATETXT ")\r\n"

/* #define MYDEBUG 1 */
#define DEBUG 0
#include <aros/debug.h>

typedef BPTR SEGLISTPTR;
#define LC_LIBHEADERTYPEPTR struct Library *

/************************************************************************/

LONG LibStart(void)
{
  return -1;
}

LONG ReturnError(void)
{
  return LibStart();
}

/************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR MUIScreenBase);
void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR MUIScreenBase);

/************************************************************************/

#ifndef INITBYTE
#define INITBYTE(offset,value)  0xe000,(UWORD)(offset),(UWORD)((value)<<8)
#define INITWORD(offset,value)  0xd000,(UWORD)(offset),(UWORD)(value)
#endif

static const UWORD LibInitData[] = {
  INITBYTE(OFFSET(Node,    ln_Type),      NT_LIBRARY),
  INITBYTE(OFFSET(Library, lib_Flags),    LIBF_SUMUSED|LIBF_CHANGED),
  INITWORD(OFFSET(Library, lib_Version),  VERSION),
  INITWORD(OFFSET(Library, lib_Revision), REVISION),
  0
};

/************************************************************************/
extern const ULONG LibInitTable[4];

const struct Resident RomTag = {
  RTC_MATCHWORD,
  (struct Resident *)&RomTag,
  (struct Resident *)LibInitTable,
  RTF_AUTOINIT,
  VERSION,
  NT_LIBRARY,
  0,
  LIBNAME,
  IDSTRING,
  (APTR)&LibInitTable
};

/************************************************************************/

SAVEDS ULONG LibReserved(void)
{
  return 0;
}

ASM SAVEDS struct Library *LibOpen(REG(a6, struct MUIScreenBase_intern *MUIScreenBase))
{
  MUIScreenBase->library.lib_Flags &= ~LIBF_DELEXP;
  MUIScreenBase->library.lib_OpenCnt++;

  return &MUIScreenBase->library;
}

ASM SAVEDS SEGLISTPTR LibExpunge(REG(a6, struct MUIScreenBase_intern *mb))
{
  if (!mb->library.lib_OpenCnt)
  {
    SEGLISTPTR seglist;

    seglist = mb->seglist;

    L_ExpungeLib(&mb->library);

    Remove((struct Node *)mb);

    FreeMem((STRPTR) mb - mb->library.lib_NegSize,
    mb->library.lib_NegSize +
    mb->library.lib_PosSize);

    return seglist;
  }
  else
    mb->library.lib_Flags |= LIBF_DELEXP;

  return 0;
}

ASM SAVEDS SEGLISTPTR LibClose(REG(a6, struct MUIScreenBase_intern *mb))
{
  if (!(--mb->library.lib_OpenCnt))
    return LibExpunge(mb);

  return 0;
}

#undef SysBase
extern struct ExecBase *SysBase;

ASM SAVEDS struct Library *LibInit(REG(a0, SEGLISTPTR seglist), REG(d0, struct MUIScreenBase_intern *mb), REG(a6, struct ExecBase *sysbase))
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

  mb->library.lib_Node.ln_Type = NT_LIBRARY;
  mb->library.lib_Node.ln_Name = LIBNAME;
  mb->library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
  mb->library.lib_Version = VERSION;
  mb->library.lib_Revision = REVISION;
  mb->library.lib_IdString = IDSTRING;

  D(bug("muiscreen.library base at 0x%lx\n", (ULONG)mb));

  if (L_InitLib(&mb->library))
  {
    return &mb->library;
  }

  FreeMem((STRPTR)mb - mb->library.lib_NegSize,
  mb->library.lib_NegSize +
  mb->library.lib_PosSize);
  return 0;
}

/************************************************************************/
/* LVO jump table: four mandatory vectors, then public muiscreen
   entries matching muiscreen_lib.fd (bias 30). */

static const APTR LibVectors[] = {
  (APTR) LibOpen,
  (APTR) LibClose,
  (APTR) LibExpunge,
  (APTR) LibReserved,
  (APTR) MUIS_AllocPubScreenDesc,
  (APTR) MUIS_FreePubScreenDesc,
  (APTR) MUIS_OpenPubScreen,
  (APTR) MUIS_ClosePubScreen,
  (APTR) MUIS_OpenPubFile,
  (APTR) MUIS_ClosePubFile,
  (APTR) MUIS_ReadPubFile,
  (APTR) MUIS_WritePubFile,
  (APTR) MUIS_AddInfoClient,
  (APTR) MUIS_RemInfoClient,
  (APTR) -1
};

const ULONG LibInitTable[4] = {
  (ULONG)sizeof(struct MUIScreenBase_intern),
  (ULONG)LibVectors,
  (ULONG)LibInitData,
  (ULONG)LibInit
};

void _CXFERR(void)
{
    D(bug("CXFERR\n"));
}

#ifdef __SASC
void REGARGS __chkabort(void) { }
void REGARGS _CXBRK(void)     { }
void __SAVE_DS__ __XCEXIT(void)  { }
#endif
