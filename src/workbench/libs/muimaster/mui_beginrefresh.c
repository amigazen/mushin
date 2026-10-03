/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        MUI_LIB_ENTRY BOOL MUI_BeginRefresh(MUI_LIB_ARG(a0, struct MUI_RenderInfo *mri), MUI_LIB_ARG(d0, ULONG flags))

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
    struct Layer  *l;

    if ((w == NULL) || !(w->Flags & WFLG_SIMPLE_REFRESH))
        return 0;

    l = w->WLayer;

    /* doesn't need refreshing */
    if (!(l->Flags & LAYERREFRESH))
        return 0;

    /* already refreshing */
    if (mri->mri_Flags & MUIMRI_REFRESHMODE)
        return 0;

    mri->mri_Flags |= MUIMRI_REFRESHMODE;
    /*
     * intuition.doc BeginRefresh: only simple gfx between Begin/End.
     * AROS Zune also LockLayerInfo here; that lock is not recursive on
     * classic Amiga and deadlocks if input.device already holds it
     * (or if we still have the IDCMP message).  BeginRefresh itself
     * locks the layers it needs.
     */
    BeginRefresh(w);
    return 1;
} /* MUI_BeginRefresh */
