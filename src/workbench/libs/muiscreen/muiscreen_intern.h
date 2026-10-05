#ifndef MUISCREEN_INTERN_H
#define MUISCREEN_INTERN_H

/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <intuition/intuitionbase.h>
#include <libraries/muiscreen.h>

#ifndef __AROS__
#include "support_amigaos.h"
#endif

struct MUIScreenBase_intern
{
    struct Library          library;
#ifndef __AROS__
    /* On AROS these fields are handled by the system / autoopened libs. */
    struct ExecBase        *sysbase;
    BPTR                    seglist;
    struct DosLibrary      *dosbase;
    struct Library         *utilitybase;
    struct GfxBase         *gfxbase;
    struct IntuitionBase   *intuibase;
    struct Library         *iffparsebase;
#endif
    const char             *muisb_def;
    struct Task            *muisb_closeTask;
    struct MsgPort         *muisb_taskMsgPort;
    struct SignalSemaphore  muisb_acLock;
    struct List             muisb_autocScreens;
    struct List             clients;
};

/* Registerised LVO prototypes for intra-library calls.
 * a6 is the library base on every jump-table entry (Amiga convention);
 * it is listed only where the implementation reads it. */
#ifdef __SASC
extern __asm __saveds struct MUI_PubScreenDesc *MUIS_AllocPubScreenDesc(
    register __a0 struct MUI_PubScreenDesc *src);
extern __asm __saveds BOOL MUIS_FreePubScreenDesc(
    register __a0 struct MUI_PubScreenDesc *psd);
extern __asm __saveds char *MUIS_OpenPubScreen(
    register __a0 struct MUI_PubScreenDesc *desc,
    register __a6 struct MUIScreenBase_intern *MUIScreenBase);
extern __asm __saveds BOOL MUIS_ClosePubScreen(
    register __a0 char *name,
    register __a6 struct MUIScreenBase_intern *MUIScreenBase);
extern __asm __saveds APTR MUIS_OpenPubFile(
    register __a0 char *name,
    register __d0 ULONG mode);
extern __asm __saveds void MUIS_ClosePubFile(
    register __a0 APTR pf);
extern __asm __saveds struct MUI_PubScreenDesc *MUIS_ReadPubFile(
    register __a0 APTR pf);
extern __asm __saveds BOOL MUIS_WritePubFile(
    register __a0 APTR pf,
    register __a1 struct MUI_PubScreenDesc *desc);
extern __asm __saveds void MUIS_AddInfoClient(
    register __a0 struct MUIS_InfoClient *sic,
    register __a6 struct MUIScreenBase_intern *MUIScreenBase);
extern __asm __saveds void MUIS_RemInfoClient(
    register __a0 struct MUIS_InfoClient *sic);
#endif

#endif
