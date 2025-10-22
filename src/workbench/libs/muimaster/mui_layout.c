/*
    Copyright (C) 2002-2020, The AROS Development Team. All rights reserved.
*/

#include <intuition/classusr.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "classes/area.h"
#include "area_macros.h"

/* Ensure MADF_ISVIRTUALGROUP is defined */
#ifndef MADF_ISVIRTUALGROUP
#define MADF_ISVIRTUALGROUP (1<<30)
#endif

//#define MYDEBUG 1
#include "debug.h"

/*****************************************************************************

    NAME */
        __asm __saveds BOOL MUI_Layout(register __a0 Object *obj, register __d0 LONG left, register __d1 LONG top, register __d2 LONG width, register __d3 LONG height, register __d4 ULONG flags)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES
Z
    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    static const struct MUIP_Layout method = { MUIM_Layout };
    Object *parent = ((struct __dummyAreaData__ *)(obj))->mnd.mnd_ParentObject;

/*
 * Called only by groups, never by windows
 */
//    ASSERT(parent != NULL);

    if (((struct __dummyAreaData__ *)(parent))->mad.mad_Flags & MADF_ISVIRTUALGROUP)
    {
        /* I'm not yet sure what to do by virtual groups in virtual groups,
         * eighter add their offsets too or not, will be tested soon */
        IPTR val = 0;
        get(parent,MUIA_Virtgroup_Left,&val);
        left -= val;
        get(parent,MUIA_Virtgroup_Top,&val);
        top -= val;
    }

    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left = left + (((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(parent))->mad.mad_addleft);
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top = top + (((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(parent))->mad.mad_addtop);
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width = width;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height = height;

    D(bug("muimaster.library/mui_layout.c: 0x%p %ldx%ldx%ldx%ld\n",obj,((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left,((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top,((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width - 1,((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height - 1));

    DoMethodA(obj, (Msg)&method);
    return TRUE;
} /* MUI_Layout */
