/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
    
*/

#include <string.h>

#include <intuition/classes.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/utility.h>

#include "mui.h"
#include "support.h"
#include "muimaster_intern.h"
#include "area_macros.h"

extern struct Library *MUIMasterBase;
extern struct Library *KeymapBase;

void ZuneTrace(CONST_STRPTR fmt, ...)
{
    BPTR fh;
    static BPTR logfh;

    fh = Output();
    /*
     * Workbench-started programs (MUI prefs, demos) have Output()==0, so
     * traces would vanish.  Append to T:zune.log in that case.
     */
    if (fh == 0)
    {
        if (logfh == 0)
        {
            logfh = Open("T:zune.log", MODE_READWRITE);
            if (logfh == 0)
                logfh = Open("T:zune.log", MODE_NEWFILE);
        }
        fh = logfh;
        if (fh != 0)
            Seek(fh, 0, OFFSET_END);
    }
    if (fh == 0)
        return;
    VFPrintf(fh, fmt, (APTR) (&fmt + 1));
    Flush(fh);
}

/**************************************************************************
 check if region is entirely within given bounds
**************************************************************************/
int isRegionWithinBounds(struct Region *r, int left, int top, int width,
    int height)
{
    if ((left <= r->bounds.MinX) && (left + width - 1 >= r->bounds.MaxX)
        && (top <= r->bounds.MinY) && (top + height - 1 >= r->bounds.MaxY))
        return 1;

    return 0;
}


/**************************************************************************
 Converts a Rawkey to a vanillakey
**************************************************************************/
ULONG ConvertKey(struct IntuiMessage * imsg)
{
    struct InputEvent event;
    UBYTE code;
    WORD actual;

    code = 0;
    event.ie_NextEvent = NULL;
    event.ie_Class = IECLASS_RAWKEY;
    event.ie_SubClass = 0;
    event.ie_Code = imsg->Code;
    event.ie_Qualifier = imsg->Qualifier;
    /*
     * keymap.library/MapRawKey + RKM maprawkey.c:
     *   eventptr = imsg->IAddress;
     *   ie_EventAddress = *eventptr;
     * Intuition stores prev dead-key ULONG in the message and points
     * IAddress at it (always non-NULL for RAWKEY on OS3/AROS).  Still
     * guard NULL — a bad IAddress lockups the machine.
     */
    if (imsg->IAddress != NULL)
        event.ie_EventAddress = (APTR)(*(ULONG *)imsg->IAddress);
    else
        event.ie_EventAddress = NULL;

    if (KeymapBase == NULL)
        return 0;

    actual = MapRawKey(&event, (STRPTR)&code, 1, NULL);
    if (actual <= 0)
        return 0;
    return (ULONG)code;
}

/**************************************************************************
 Convenient way to get an attribute of an object easily. If the object
 doesn't support the attribute this call returns an undefined value. So use
 this call only if the attribute is known to be known by the object.
 Implemented as a macro when compiling with GCC.
**************************************************************************/
#ifndef __GNUC__
IPTR XGET(Object * obj, Tag attr)
{
    IPTR storage = 0;
    GetAttr(attr, obj, &storage);
    return storage;
}
#endif /* __GNUC__ */

/**************************************************************************
 Call the Setup Method of an given object, but before set the renderinfo
**************************************************************************/
IPTR DoSetupMethod(Object * obj, struct MUI_RenderInfo * info)
{
    struct MUIP_Setup smsg;

    /* MUI set the correct render info *before* it calls MUIM_Setup so please
     * only use this function instead of DoMethodA() */
    muiRenderInfo(obj) = info;
    smsg.MethodID = MUIM_Setup;
    smsg.RenderInfo = info;
    return DoMethodA(obj, (Msg)&smsg);
}

IPTR DoShowMethod(Object * obj)
{
    struct MUIP_Show smsg;
    IPTR ret;

    smsg.MethodID = MUIM_Show;
    ret = DoMethodA(obj, (Msg)&smsg);
    if (ret)
        ((struct __dummyAreaData__ *)(obj))->mad.mad_Flags |= MADF_CANDRAW;
    return ret;
}

IPTR DoHideMethod(Object * obj)
{
    struct MUIP_Hide hmsg;

    ((struct __dummyAreaData__ *)(obj))->mad.mad_Flags &= ~MADF_CANDRAW;
    hmsg.MethodID = MUIM_Hide;
    return DoMethodA(obj, (Msg)&hmsg);
}


Object *ZuneNextObject(Object **state)
{
    struct _Object *node;
    struct _Object *succ;

    if (state == NULL)
        return NULL;

    node = (struct _Object *)(*state);
    if (node == NULL)
        return NULL;

    succ = (struct _Object *)node->o_Node.mln_Succ;
    if (succ == NULL)
        return NULL;

    *state = (Object *)succ;
    return (Object *)BASEOBJECT(node);
}

void *Node_Next(APTR node)
{
    if (node == NULL)
        return NULL;
    if (((struct MinNode *)node)->mln_Succ == NULL)
        return NULL;
    if (((struct MinNode *)node)->mln_Succ->mln_Succ == NULL)
        return NULL;
    return ((struct MinNode *)node)->mln_Succ;
}

void *List_First(APTR list)
{
    if (!((struct MinList *)list)->mlh_Head)
        return NULL;
    if (((struct MinList *)list)->mlh_Head->mln_Succ == NULL)
        return NULL;
    return ((struct MinList *)list)->mlh_Head;
}

/* subtract rectangle b from rectangle b. resulting rectangles will be put into
   destrectarray which must have place for at least 4 rectangles. Returns number
   of resulting rectangles */

WORD SubtractRectFromRect(struct Rectangle *a, struct Rectangle *b,
    struct Rectangle *destrectarray)
{
    struct Rectangle intersect;
    BOOL intersecting = FALSE;
    WORD numrects = 0;

    /* calc. intersection between a and b */

    if (a->MinX <= b->MaxX)
    {
        if (a->MinY <= b->MaxY)
        {
            if (a->MaxX >= b->MinX)
            {
                if (a->MaxY >= b->MinY)
                {
                    intersect.MinX = MAX(a->MinX, b->MinX);
                    intersect.MinY = MAX(a->MinY, b->MinY);
                    intersect.MaxX = MIN(a->MaxX, b->MaxX);
                    intersect.MaxY = MIN(a->MaxY, b->MaxY);

                    intersecting = TRUE;
                }
            }
        }
    }

    if (!intersecting)
    {
        destrectarray[numrects++] = *a;

    }                           /* not intersecting */
    else
    {
        if (intersect.MinY > a->MinY)   /* upper */
        {
            destrectarray->MinX = a->MinX;
            destrectarray->MinY = a->MinY;
            destrectarray->MaxX = a->MaxX;
            destrectarray->MaxY = intersect.MinY - 1;

            numrects++;
            destrectarray++;
        }

        if (intersect.MaxY < a->MaxY)   /* lower */
        {
            destrectarray->MinX = a->MinX;
            destrectarray->MinY = intersect.MaxY + 1;
            destrectarray->MaxX = a->MaxX;
            destrectarray->MaxY = a->MaxY;

            numrects++;
            destrectarray++;
        }

        if (intersect.MinX > a->MinX)   /* left */
        {
            destrectarray->MinX = a->MinX;
            destrectarray->MinY = intersect.MinY;
            destrectarray->MaxX = intersect.MinX - 1;
            destrectarray->MaxY = intersect.MaxY;

            numrects++;
            destrectarray++;
        }

        if (intersect.MaxX < a->MaxX)   /* right */
        {
            destrectarray->MinX = intersect.MaxX + 1;
            destrectarray->MinY = intersect.MinY;
            destrectarray->MaxX = a->MaxX;
            destrectarray->MaxY = intersect.MaxY;

            numrects++;
            destrectarray++;
        }

    }                           /* intersecting */

    return numrects;

}

ULONG IsObjectVisible(Object * child, struct Library * MUIMasterBase)
{
    Object *wnd;
    Object *obj;

    wnd = ((struct __dummyAreaData__ *)(child))->mad.mad_RenderInfo->mri_WindowObject;
    obj = child;

    while (get(obj, MUIA_Parent, &obj))
    {
        if (!obj)
            break;
        if (obj == wnd)
            break;

        {
            LONG child_right = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Width - 1;
            LONG child_left = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Left;
            LONG child_bottom = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Height - 1;
            LONG child_top = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Top;
            
            LONG obj_mleft = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_addleft;
            LONG obj_mright = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_addleft + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width + ((struct __dummyAreaData__ *)(obj))->mad.mad_subwidth - 1;
            LONG obj_mtop = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_addtop;
            LONG obj_mbottom = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_addtop + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height + ((struct __dummyAreaData__ *)(obj))->mad.mad_subheight - 1;
            
            if (child_right < obj_mleft || child_left > obj_mright || child_bottom < obj_mtop || child_top > obj_mbottom)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

IPTR ZuneDrawBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags)
{
    struct MUIP_DrawBackground msg;

    if (obj == NULL)
        return 0;
    msg.MethodID = MUIM_DrawBackground;
    msg.left = left;
    msg.top = top;
    msg.width = width;
    msg.height = height;
    msg.xoffset = xoffset;
    msg.yoffset = yoffset;
    msg.flags = flags;
    return DoMethodA(obj, (Msg) &msg);
}

IPTR ZuneDrawParentBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags)
{
    struct MUIP_DrawParentBackground msg;

    if (obj == NULL)
        return 0;
    msg.MethodID = MUIM_DrawParentBackground;
    msg.left = left;
    msg.top = top;
    msg.width = width;
    msg.height = height;
    msg.xoffset = xoffset;
    msg.yoffset = yoffset;
    msg.flags = flags;
    return DoMethodA(obj, (Msg) &msg);
}

IPTR ZuneWindowDrawBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags)
{
    struct MUIP_Window_DrawBackground msg;

    if (obj == NULL)
        return 0;
    msg.MethodID = MUIM_Window_DrawBackground;
    msg.left = left;
    msg.top = top;
    msg.width = width;
    msg.height = height;
    msg.xoffset = xoffset;
    msg.yoffset = yoffset;
    msg.flags = flags;
    return DoMethodA(obj, (Msg) &msg);
}

IPTR ZuneLayout(Object *obj)
{
    struct MUIP_Layout msg;

    if (obj == NULL)
        return 0;
    msg.MethodID = MUIM_Layout;
    return DoMethodA(obj, (Msg) &msg);
}
