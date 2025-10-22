/*
    Copyright (C) 2002-2012, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>

#include "mui.h"
#include "muimaster_intern.h"

#define MUIPEN_HIMASK 0xFFFF0000   /* ??? */

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_ReleasePen(register __a0 struct MUI_RenderInfo *mri, register __d0 LONG pen)

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
    if (pen == -1)
        return;

    if (mri->mri_Colormap && ((pen & MUIPEN_HIMASK) == 0x10000))
    {
        ReleasePen(mri->mri_Colormap, MUIPEN(pen));
    }
} /* MUI_ReleasePen */
