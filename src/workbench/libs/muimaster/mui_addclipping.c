/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>
#include <proto/layers.h>

#include "support.h"

#include "mui.h"
#include "muimaster_intern.h"

/* Forward declarations for internal functions */
APTR MUI_AddClipRegion(struct MUI_RenderInfo *mri, struct Region *r);

/*****************************************************************************

    NAME */
        __asm __saveds APTR MUI_AddClipping(register __a0 struct MUI_RenderInfo *mri, register __d0 WORD left, register __d1 WORD top, register __d2 WORD width, register __d3 WORD height)

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
    struct Region *r;
    struct Rectangle rect;
    APTR handle;

    if ((width >= MUI_MAXMAX) || (height >= MUI_MAXMAX))
        return (APTR)-1;

    if (mri->mri_rCount > 0)
    {
        if (isRegionWithinBounds(mri->mri_rArray[mri->mri_rCount-1],
            left, top, width, height))
            return (APTR)-1;
    }

    if ((r = NewRegion()) == NULL)
        return (APTR)-1;

    rect.MinX = left;
    rect.MinY = top;
    rect.MaxX = left + width  - 1;
    rect.MaxY = top  + height - 1;
    OrRectRegion(r, &rect);

    handle = MUI_AddClipRegion(mri, r);

#if 0 /* MUI_AddClipRegion frees region itself upon failure */
    if (handle == (APTR)-1)
    {
        DisposeRegion(r);
    }
#endif
    return handle;
} /* MUI_AddClipping */
