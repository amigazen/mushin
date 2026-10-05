/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <intuition/screens.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds struct MUI_PubScreenDesc *MUIS_AllocPubScreenDesc(
            register __a0 struct MUI_PubScreenDesc *src)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    struct MUI_PubScreenDesc *psd;
    struct Screen *wbscreen;
    int def_pens[12];
    struct MUI_RGBcolor col[8];
    int i;

    D(bug("MUIS_AllocPubScreenDesc(%lx)\n", (ULONG)src));

    psd = AllocMem(sizeof(struct MUI_PubScreenDesc), MEMF_ANY | MEMF_CLEAR);
    if (psd == NULL)
        return NULL;

    if (src)
    {
        CopyMem(src, psd, sizeof(struct MUI_PubScreenDesc));
    }
    else
    {
        /* Copy default values from Workbench screen */
        wbscreen = LockPubScreen(NULL);
        if (wbscreen == NULL)
        {
            FreeMem(psd, sizeof(struct MUI_PubScreenDesc));
            return NULL;
        }

        CopyMem(PSD_INITIAL_NAME, psd->Name, sizeof(PSD_INITIAL_NAME));
        CopyMem(PSD_INITIAL_TITLE, psd->Title, sizeof(PSD_INITIAL_TITLE));
        psd->DisplayID = GetVPModeID(&wbscreen->ViewPort);
        psd->DisplayWidth = wbscreen->Width;
        psd->DisplayHeight = wbscreen->Height;
        psd->DisplayDepth = GetBitMapAttr(wbscreen->RastPort.BitMap, BMA_DEPTH);
        psd->OverscanType = OSCAN_TEXT;
        psd->AutoScroll = (wbscreen->Flags & AUTOSCROLL) ? TRUE : FALSE;
        psd->NoDrag = FALSE;
        psd->Exclusive = FALSE;
        psd->Interleaved = (GetBitMapAttr(wbscreen->RastPort.BitMap, BMA_FLAGS) & BMF_INTERLEAVED) ? TRUE : FALSE;
        psd->SysDefault = FALSE;
        psd->Behind = (wbscreen->Flags & SCREENBEHIND) ? TRUE : FALSE;
        psd->AutoClose = FALSE;
        psd->CloseGadget = FALSE;

        UnlockPubScreen(NULL, wbscreen);

        def_pens[0] = 0;
        def_pens[1] = 1;
        def_pens[2] = 1;
        def_pens[3] = 2;
        def_pens[4] = 1;
        def_pens[5] = 3;
        def_pens[6] = 1;
        def_pens[7] = 0;
        def_pens[8] = 2;
        def_pens[9] = 1;
        def_pens[10] = 2;
        def_pens[11] = 1;
        for (i = 0; i < 12; i++)
            psd->SystemPens[i] = def_pens[i];

        col[0].red = 0xAAAAAAAA; col[0].green = 0xAAAAAAAA; col[0].blue = 0xAAAAAAAA;
        col[1].red = 0x00000000; col[1].green = 0x00000000; col[1].blue = 0x00000000;
        col[2].red = 0xFFFFFFFF; col[2].green = 0xFFFFFFFF; col[2].blue = 0xFFFFFFFF;
        col[3].red = 0x66666666; col[3].green = 0x88888888; col[3].blue = 0xBBBBBBBB;
        col[4].red = 0xEEEEEEEE; col[4].green = 0x44444444; col[4].blue = 0x44444444;
        col[5].red = 0x55555555; col[5].green = 0xDDDDDDDD; col[5].blue = 0x55555555;
        col[6].red = 0x00000000; col[6].green = 0x44444444; col[6].blue = 0xDDDDDDDD;
        col[7].red = 0xEEEEEEEE; col[7].green = 0x99999999; col[7].blue = 0x00000000;

        for (i = 0; i < 8; i++)
            psd->Palette[i] = col[i];
    }

    D(bug("Allocated struct MUI_PubScreenDesc %lx\n", (ULONG)psd));

    return psd;
}
