/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <libraries/iffparse.h>
#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include <proto/exec.h>

#define DEBUG 0
#include <aros/debug.h>

#include "fileformat.h"
#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds BOOL MUIS_WritePubFile(
            register __a0 APTR pf,
            register __a1 struct MUI_PubScreenDesc *desc)

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
    struct IFFHandle *iff;
    BOOL retval;
    struct MUI_PubScreenDescArray desc_tmp;

    D(bug("MUIS_WritePubFile(%lx, %lx)\n", (ULONG)pf, (ULONG)desc));

    iff = (struct IFFHandle *)pf;
    retval = FALSE;

    if (iff == NULL || desc == NULL)
        return FALSE;

    LONG_TO_ARRAY(desc->Version, desc_tmp.Version);
    CopyMem(desc->Name, desc_tmp.Name, sizeof(desc_tmp.Name));
    CopyMem(desc->Title, desc_tmp.Title, sizeof(desc_tmp.Title));
    CopyMem(desc->Font, desc_tmp.Font, sizeof(desc_tmp.Font));
    CopyMem(desc->Background, desc_tmp.Background, sizeof(desc_tmp.Background));
    LONG_TO_ARRAY(desc->DisplayID, desc_tmp.DisplayID);
    WORD_TO_ARRAY(desc->DisplayWidth, desc_tmp.DisplayWidth);
    WORD_TO_ARRAY(desc->DisplayHeight, desc_tmp.DisplayHeight);
    desc_tmp.DisplayDepth = desc->DisplayDepth;
    desc_tmp.OverscanType = desc->OverscanType;
    desc_tmp.AutoScroll = desc->AutoScroll;
    desc_tmp.NoDrag = desc->NoDrag;
    desc_tmp.Exclusive = desc->Exclusive;
    desc_tmp.Interleaved = desc->Interleaved;
    desc_tmp.SysDefault = desc->SysDefault;
    desc_tmp.Behind = desc->Behind;
    desc_tmp.AutoClose = desc->AutoClose;
    desc_tmp.CloseGadget = desc->CloseGadget;
    desc_tmp.DummyWasForeign = desc->DummyWasForeign;
    CopyMem(desc->SystemPens, desc_tmp.SystemPens, sizeof(desc_tmp.SystemPens));
    CopyMem(desc->Reserved, desc_tmp.Reserved, sizeof(desc_tmp.Reserved));
    COLS_TO_ARRAY(desc->Palette, desc_tmp.Palette);
    COLS_TO_ARRAY(desc->rsvd, desc_tmp.rsvd);
    CopyMem(desc->rsvd2, desc_tmp.rsvd2, sizeof(desc_tmp.rsvd2));
    LONG_TO_ARRAY(desc->Changed, desc_tmp.Changed);

    if (!PushChunk(iff, ID_PREF, ID_MPUB, sizeof(struct MUI_PubScreenDescArray)))
    {
        if (WriteChunkBytes(iff, &desc_tmp, sizeof(struct MUI_PubScreenDescArray))
            == sizeof(struct MUI_PubScreenDescArray))
        {
            retval = TRUE;
        }
        PopChunk(iff);
    }

    return retval;
}
