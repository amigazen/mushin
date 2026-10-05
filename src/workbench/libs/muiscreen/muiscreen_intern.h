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
extern __ASM__ __SAVE_DS__ struct MUI_PubScreenDesc *MUIS_AllocPubScreenDesc(
    __REG__(a0, struct MUI_PubScreenDesc *src));
extern __ASM__ __SAVE_DS__ BOOL MUIS_FreePubScreenDesc(
    __REG__(a0, struct MUI_PubScreenDesc *psd));
extern __ASM__ __SAVE_DS__ char *MUIS_OpenPubScreen(
    __REG__(a0, struct MUI_PubScreenDesc *desc),
    __REG__(a6, struct MUIScreenBase_intern *MUIScreenBase));
extern __ASM__ __SAVE_DS__ BOOL MUIS_ClosePubScreen(
    __REG__(a0, char *name),
    __REG__(a6, struct MUIScreenBase_intern *MUIScreenBase));
extern __ASM__ __SAVE_DS__ APTR MUIS_OpenPubFile(
    __REG__(a0, char *name),
    __REG__(d0, ULONG mode));
extern __ASM__ __SAVE_DS__ void MUIS_ClosePubFile(
    __REG__(a0, APTR pf));
extern __ASM__ __SAVE_DS__ struct MUI_PubScreenDesc *MUIS_ReadPubFile(
    __REG__(a0, APTR pf));
extern __ASM__ __SAVE_DS__ BOOL MUIS_WritePubFile(
    __REG__(a0, APTR pf),
    __REG__(a1, struct MUI_PubScreenDesc *desc));
extern __ASM__ __SAVE_DS__ void MUIS_AddInfoClient(
    __REG__(a0, struct MUIS_InfoClient *sic),
    __REG__(a6, struct MUIScreenBase_intern *MUIScreenBase));
extern __ASM__ __SAVE_DS__ void MUIS_RemInfoClient(
    __REG__(a0, struct MUIS_InfoClient *sic));

#endif
