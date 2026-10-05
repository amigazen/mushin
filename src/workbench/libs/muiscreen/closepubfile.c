/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <libraries/iffparse.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds void MUIS_ClosePubFile(
            register __a0 APTR pf)

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

    D(bug("MUIS_ClosePubFile(%lx)\n", (ULONG)pf));

    iff = (struct IFFHandle *)pf;
    if (iff)
    {
        if (iff->iff_Flags & IFFF_WRITE)
            PopChunk(iff);

        CloseIFF(iff);
        Close((BPTR)iff->iff_Stream);
        FreeIFF(iff);
    }
}
