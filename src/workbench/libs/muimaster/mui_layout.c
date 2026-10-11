/*
    Copyright (C) 2002-2020, The AROS Development Team. All rights reserved.
    Copyright (C) 2026, amigazen project. All rights reserved.
*/

#include <intuition/classusr.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "classes/area.h"
#include "area_macros.h"
#include "support.h"

/* Ensure MADF_ISVIRTUALGROUP is defined */
#ifndef MADF_ISVIRTUALGROUP
#define MADF_ISVIRTUALGROUP (1<<30)
#endif

//#define MYDEBUG 1
#include "debug.h"

/*
 * LVO -126, MUI 3.8 FD registers (a0,d0,d1,d2,d3,d4).
 * left/top are relative to the parent group's inner box.  MUI_Layout()
 * adds that origin.  A virtual parent is scrolled, so its Virtgroup
 * Left/Top are removed first.  Same placement as Original MUI.
 */
__asm __saveds BOOL MUI_Layout(register __a0 Object *obj,
    register __d0 LONG left, register __d1 LONG top, register __d2 LONG width,
    register __d3 LONG height, register __d4 ULONG flags)
{
    Object *parent;
    IPTR val;
    struct MUIP_Layout method;

    (void)flags;

    parent = ((struct __dummyAreaData__ *)(obj))->mnd.mnd_ParentObject;
    if (parent == NULL)
    {
        ZuneTrace("zune: MUI_Layout abort NULL parent obj=%lx\n", (ULONG) obj);
        return FALSE;
    }

    if (((struct __dummyAreaData__ *)(parent))->mad.mad_Flags & MADF_ISVIRTUALGROUP)
    {
        /* I'm not yet sure what to do by virtual groups in virtual groups,
         * eighter add their offsets too or not, will be tested soon */
        val = 0;
        get(parent, MUIA_Virtgroup_Left, &val);
        left -= (LONG) val;
        val = 0;
        get(parent, MUIA_Virtgroup_Top, &val);
        top -= (LONG) val;
    }

    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left = (WORD)(left
        + ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Left
        + ((struct __dummyAreaData__ *)(parent))->mad.mad_addleft);
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top = (WORD)(top
        + ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Top
        + ((struct __dummyAreaData__ *)(parent))->mad.mad_addtop);
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width = (WORD) width;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height = (WORD) height;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_VirtualTop =
        ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top;

    if ((left > 32767) || (left < -32768) || (top > 32767) || (top < -32768)
        || (width > 32767) || (width < 0) || (height > 32767) || (height < 0))
    {
        ZuneTrace("zune: MUI_Layout obj=%lx %ldx%ld+%ldx%ld box=%ldx%ld\n",
            (ULONG) obj, left, top, width, height,
            (LONG) ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width,
            (LONG) ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height);
    }

    method.MethodID = MUIM_Layout;
    DoMethodA(obj, (Msg) &method);
    return TRUE;
} /* MUI_Layout */
