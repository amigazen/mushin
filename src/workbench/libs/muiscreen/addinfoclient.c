/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <exec/nodes.h>
#include <proto/exec.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds void MUIS_AddInfoClient(
            register __a0 struct MUIS_InfoClient *sic,
            register __a6 struct MUIScreenBase_intern *MUIScreenBase)

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
    D(bug("MUIS_AddInfoClient(%lx)\n", (ULONG)sic));

    if (sic)
        AddHead(&MUIScreenBase->clients, (struct Node *)sic);
}
