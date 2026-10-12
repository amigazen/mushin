/*
    Copyright (C) 2002, The AROS Development Team.
    Copyright (C) 2026, amigazen project.
    All rights reserved.
    
*/

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

#undef ZuneTrace
void ZuneTrace(CONST_STRPTR fmt, ...)
{
#if ZUNE_TRACE
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
#else
    (void) fmt;
#endif
}

/*
 * Off until the first Menuitem dispose.  Every builtin dispatcher calls
 * ZuneLogMethod, so leaving this clear keeps splash construction quiet.
 */
int Zune_LogAfterMenu = 0;

void ZuneLogMethod(Object *obj, Msg msg)
{
    Class *cl;
    CONST_STRPTR id;

    if (!Zune_LogAfterMenu || obj == NULL || msg == NULL)
        return;

    cl = OCLASS(obj);
    id = (CONST_STRPTR) "?";
    if (cl != NULL && cl->cl_ID != NULL)
        id = cl->cl_ID;
    ZuneTrace("zune: m %08lx obj=%lx id=%s\n",
        (ULONG) msg->MethodID, (ULONG) obj, id);
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
    Class *cl;
    STRPTR id;

    cl = NULL;
    id = (STRPTR) "?";
    if (obj != NULL)
        cl = OCLASS(obj);
    if (cl != NULL && cl->cl_ID != NULL)
        id = cl->cl_ID;

    ZuneTrace("zune: Show enter obj=%lx cl=%s\n", (ULONG) obj, id);
    smsg.MethodID = MUIM_Show;
    smsg.clip = NULL;
    ret = DoMethodA(obj, (Msg)&smsg);
    ZuneTrace("zune: Show leave obj=%lx cl=%s ret=%ld\n",
        (ULONG) obj, id, (ULONG) ret);
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


void ZuneUnlinkObject(Object *obj)
{
    struct Node *node;
    struct Node *pred;
    struct Node *succ;

    if (obj == NULL)
        return;

    /*
     * Remove() only when both neighbours still name this node.  A node
     * that was already REMHEAD'd still holds stale succ/pred; splicing
     * those into a live neighbour plants a disposed child on the list
     * Voyager walks after a tab change.  A neighbour that does still
     * name us has to drop that link, or the next OM_GET follows it into
     * freed memory.  The node's own pointers are cleared either way so
     * a later AddTail does not see it as already linked.
     */
    node = (struct Node *)_OBJECT(obj);
    succ = node->ln_Succ;
    pred = node->ln_Pred;
    if (pred != NULL && succ != NULL
        && pred->ln_Succ == node && succ->ln_Pred == node)
    {
        Remove(node);
        ZuneTrace("zune: UnlinkObject %lx\n", (ULONG) obj);
    }
    else
    {
        if (pred != NULL && pred->ln_Succ == node)
            pred->ln_Succ = NULL;
        if (succ != NULL && succ->ln_Pred == node)
            succ->ln_Pred = NULL;
    }
    node->ln_Succ = NULL;
    node->ln_Pred = NULL;
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
    struct MUI_RenderInfo *mri;
    Object *wnd;
    Object *obj;
    ULONG hops;
    LONG child_right;
    LONG child_left;
    LONG child_bottom;
    LONG child_top;
    LONG obj_mleft;
    LONG obj_mright;
    LONG obj_mtop;
    LONG obj_mbottom;

    (void)MUIMasterBase;

    /*
     * First MUIM_Show of a child can run before mad_RenderInfo is filled
     * in.  Dereferencing it is an 8000000B.  Treat that child as visible;
     * the caller still requires MADF_SHOWME.  A parent cycle must stop.
     */
    if (child == NULL)
        return FALSE;

    mri = ((struct __dummyAreaData__ *)(child))->mad.mad_RenderInfo;
    if (mri == NULL)
        return TRUE;

    wnd = mri->mri_WindowObject;
    obj = child;
    hops = 0;
    child_right = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Left
        + ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Width - 1;
    child_left = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Left;
    child_bottom = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Top
        + ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Height - 1;
    child_top = ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Top;

    while (hops < 64 && get(obj, MUIA_Parent, &obj))
    {
        hops++;
        if (!obj)
            break;
        if (obj == wnd)
            break;

        obj_mleft = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_addleft;
        obj_mright = obj_mleft
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_subwidth - 1;
        obj_mtop = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_addtop;
        obj_mbottom = obj_mtop
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height
            + ((struct __dummyAreaData__ *)(obj))->mad.mad_subheight - 1;

        if (child_right < obj_mleft || child_left > obj_mright
            || child_bottom < obj_mtop || child_top > obj_mbottom)
            return FALSE;
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
