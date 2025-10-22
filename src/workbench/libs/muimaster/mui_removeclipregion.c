/*
    Copyright (C) 2002-2012, The AROS Development Team.
    All rights reserved.
    
*/

#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include "mui.h"
#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_RemoveClipRegion(register __a0 struct MUI_RenderInfo *mri, register __a1 APTR handle)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

        sba: This function frees the region given in MUI_AddClipRegion, but this seems wrong to me.
        MUI_AddClipRegion should better duplicate the region.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{

    struct Window *w = mri->mri_Window;
    struct Layer  *l;

    if (handle == (APTR)-1)
        return;

    if (w != NULL)
        l = w->WLayer;
    else
        l = mri->mri_RastPort->Layer;

    if (l == NULL)
        return;

    mri->mri_rCount--;

    if ((w != NULL) && (mri->mri_Flags & MUIMRI_REFRESHMODE))
    {
            LockLayerInfo(&w->WScreen->LayerInfo);
        EndRefresh(w, FALSE);
    }
    
#if 1 /* stegerg: what's this good for */
    if ((w != NULL) && !(w->Flags & WFLG_SIMPLE_REFRESH))
        LockLayerInfo(&w->WScreen->LayerInfo);
#endif

    InstallClipRegion(l, (mri->mri_rCount > 0)
        ? mri->mri_rArray[mri->mri_rCount-1] : NULL);

#if 1 /* stegerg: what's this good for */
    if ((w != NULL) && !(w->Flags & WFLG_SIMPLE_REFRESH))
        UnlockLayerInfo(&w->WScreen->LayerInfo);
#endif

    if ((w != NULL) && (mri->mri_Flags & MUIMRI_REFRESHMODE))
    {
        BeginRefresh(w);
        UnlockLayerInfo(&w->WScreen->LayerInfo);
    }

    DisposeRegion(mri->mri_rArray[mri->mri_rCount]);
    mri->mri_rArray[mri->mri_rCount] = NULL;

} /* MUI_RemoveClipRegion */
