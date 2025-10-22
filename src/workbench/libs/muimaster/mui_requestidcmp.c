/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <clib/alib_protos.h>

#include <proto/intuition.h>
#include <intuition/classusr.h>

#include "muimaster_intern.h"
#include "mui.h"
#include "area_macros.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_RequestIDCMP(register __a0 Object *obj, register __d0 ULONG flags)

/*  FUNCTION
        For custom class implementors. Valid between MUIM_Setup/MUIM_Cleanup.
        Indicates that this object wants to receive some events. MUIM_HandleInput
        will then be called on this object whenever there's a matching event.

    INPUTS
        obj   - an object whose class is derived from Area
        flags - IDCMP flags

    RESULT
        None.

    NOTES
        Deprecated in favor of MUIM_Window_AddEventHandler and MUIM_HandleEvent
        event handling model (introduced in MUI V16).

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
        This is a wrapper around MUIM_Window_RemEventHandler and
        MUIM_Window_AddEventHandler.

*****************************************************************************/
{
    /* Use the Eventhandler to simulate a MUIM_HandleInput */
    if (((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo && (((struct __dummyAreaData__ *)(obj))->mad.mad_Flags & MADF_SETUP) && ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject)
    {
        if (((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events)
        {
            DoMethod(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject, MUIM_Window_RemEventHandler,
                (IPTR)&((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn);
        }
        ((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events |= flags;
        if (((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events)
        {
            DoMethod(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject, MUIM_Window_AddEventHandler,
                (IPTR)&((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn);
        }
    }
    else
    {
        ((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events |= flags;
    }
} /* MUI_RequestIDCMP */
