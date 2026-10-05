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
        __ASM__ __SAVE_DS__ BOOL MUIS_FreePubScreenDesc(
            __REG__(a0, struct MUI_PubScreenDesc *psd))

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
