/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/graphics.h>
#include <proto/layers.h>

#define MUIMASTER_DEFINING_CLIPPING
#include "support.h"

#include "mui.h"
#include "muimaster_intern.h"

APTR ZuneAddClipping(struct MUI_RenderInfo *mri, LONG left, LONG top,
    LONG width, LONG height)
{
    struct Region *r;
    struct Rectangle rect;
    APTR handle;

    if ((width >= MUI_MAXMAX) || (height >= MUI_MAXMAX))
        return (APTR)-1;

    if (mri->mri_rCount > 0)
    {
        if (isRegionWithinBounds(mri->mri_rArray[mri->mri_rCount-1],
            (WORD)left, (WORD)top, (WORD)width, (WORD)height))
            return (APTR)-1;
    }

    if ((r = NewRegion()) == NULL)
        return (APTR)-1;

    rect.MinX = (WORD)left;
    rect.MinY = (WORD)top;
    rect.MaxX = (WORD)(left + width  - 1);
    rect.MaxY = (WORD)(top  + height - 1);
    OrRectRegion(r, &rect);

    /* Always the C body — never the asm LVO via a stack call. */
    handle = ZuneAddClipRegion(mri, r);

    return handle;
}

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
    return ZuneAddClipping(mri, (LONG)left, (LONG)top, (LONG)width,
        (LONG)height);
} /* MUI_AddClipping */
