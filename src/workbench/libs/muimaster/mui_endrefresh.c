/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_EndRefresh(register __a0 struct MUI_RenderInfo *mri, register __d0 ULONG flags)
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
    struct Window *w = mri->mri_Window;

    if (w == NULL)
        return;

    EndRefresh(w, TRUE);
    UnlockLayerInfo(&w->WScreen->LayerInfo);
    mri->mri_Flags &= ~MUIMRI_REFRESHMODE;
    return;
} /* MUI_EndRefresh */
