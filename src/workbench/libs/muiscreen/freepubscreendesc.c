/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <proto/exec.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds BOOL MUIS_FreePubScreenDesc(
            register __a0 struct MUI_PubScreenDesc *psd)

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
    D(bug("MUIS_FreePubScreenDesc(%lx)\n", (ULONG)psd));

    if (psd)
        FreeMem(psd, sizeof(struct MUI_PubScreenDesc));

    return TRUE;
}
