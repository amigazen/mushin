/*
    Copyright (C) 2002-2003, The AROS Development Team. All rights reserved.

    Virtgroup must expose commercial MUI 3.8 instance data (VirtgroupData38).
    Voyager's htmlview does INST_DATA(cl->cl_Super) and reads clipdraw/pos;
    with size 0 that aliased GroupData, so Draw ScrollRaster'd garbage and
    Update smashed layout_hook -- corrupted HTML paint then crash on relayout.
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <exec/memory.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/layers.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"
#include "support_classes.h"
#include "classes/area.h"
#include "area_macros.h"

extern struct Library *MUIMasterBase;

/* Private LVO used by Virtgroup_Update / Voyager scroll. */
extern VOID MUI_Offset(Object *obj, LONG addx, LONG addy);

#ifndef MUIM_Virtgroup_InstallClip
#define MUIM_Virtgroup_InstallClip (MUIB_MUI | 0x00425b5c)
#endif
#ifndef MUIM_Virtgroup_RemoveClip
#define MUIM_Virtgroup_RemoveClip  (MUIB_MUI | 0x0042da52)
#endif
#ifndef MUIM_Virtgroup_Update
#define MUIM_Virtgroup_Update      (MUIB_MUI | 0x0042ffbf)
#endif

struct MUIP_Virtgroup_Update
{
    ULONG MethodID;
    LONG layout;
};

/*
 * Layout must match Voyager htmlview.c VirtgroupData38 (MUI 3.8 / lib_Version
 * <= 19).  Keep field order and packing identical.
 */
struct MUI_VirtgroupData
{
    APTR clip;
    Object *scrollgroup;
    LONG pos[2];
    LONG size[2];
    LONG oldpos[2];
    BYTE free_xy[2];
    LONG mx;
    LONG my;
    unsigned clipdraw:1,
             moving1:1,
             input:1,
             reqticks:1,
             moving2:1,
             dummy:1;
};

IPTR Virtgroup__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MUI_VirtgroupData *data;

    obj = (Object *) DoSuperNewTags
        (cl, obj, NULL,
        MUIA_Group_Virtual, TRUE, TAG_MORE, (IPTR) msg->ops_AttrList);
    if (obj == NULL)
        return (IPTR) NULL;

    data = INST_DATA(cl, obj);
    data->clip = NULL;
    data->scrollgroup = NULL;
    data->pos[0] = 0;
    data->pos[1] = 0;
    data->size[0] = 0;
    data->size[1] = 0;
    data->oldpos[0] = 0;
    data->oldpos[1] = 0;
    data->free_xy[0] = 0;
    data->free_xy[1] = 0;
    data->mx = 0;
    data->my = 0;
    data->clipdraw = 0;
    data->moving1 = 0;
    data->input = 0;
    data->reqticks = 0;
    data->moving2 = 0;
    data->dummy = 0;

    return (IPTR) obj;
}

/*
 * Update commercial pos[] BEFORE Group sees Left/Top so Virtgroup_Update
 * (invoked from Group) computes the correct delta.  Voyager htmlview
 * overrides Update; this default Offsets children for plain Virtgroups.
 */
IPTR Virtgroup__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MUI_VirtgroupData *data = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((const struct TagItem **)&tags)) != NULL)
    {
        switch (tag->ti_Tag)
        {
        case MUIA_Virtgroup_Left:
            data->oldpos[0] = data->pos[0];
            data->pos[0] = (LONG) tag->ti_Data;
            break;
        case MUIA_Virtgroup_Top:
            data->oldpos[1] = data->pos[1];
            data->pos[1] = (LONG) tag->ti_Data;
            break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg) msg);
}

/*
 * Zero commercial pos[] after Group restores child boxes and clears virt_off.
 * Also drop any InstallClip sentinel left from a prior HTML draw so reload
 * does not RemoveClip against a stale rCount stack entry.
 */
IPTR Virtgroup__MUIM_InitChange(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_VirtgroupData *data = INST_DATA(cl, obj);
    IPTR rc;

    if (data->clip != NULL && muiRenderInfo(obj) != NULL)
    {
        MUI_RemoveClipRegion(muiRenderInfo(obj), data->clip);
        data->clip = NULL;
    }
    data->clipdraw = 0;

    rc = DoSuperMethodA(cl, obj, msg);
    data->pos[0] = 0;
    data->pos[1] = 0;
    data->oldpos[0] = 0;
    data->oldpos[1] = 0;
    return rc;
}

/*
 * Commercial scroll: Offset children by -(pos-oldpos), then redraw.
 * Voyager htmlview replaces this with ShowClipped + BeginRefresh.
 */
IPTR Virtgroup__MUIM_Update(struct IClass *cl, Object *obj,
    struct MUIP_Virtgroup_Update *msg)
{
    struct MUI_VirtgroupData *data = INST_DATA(cl, obj);
    struct List *childlist;
    Object *child;
    Object *cstate;
    LONG addx, addy;

    addx = data->pos[0];
    addy = data->pos[1];
    if (msg == NULL || !msg->layout)
    {
        addx -= data->oldpos[0];
        addy -= data->oldpos[1];
    }

    if (addx != 0 || addy != 0)
    {
        childlist = NULL;
        get(obj, MUIA_Group_ChildList, (IPTR *) & childlist);
        if (childlist != NULL)
        {
            cstate = (Object *) ((struct MinList *)childlist)->mlh_Head;
            while ((child = NextObject(&cstate)) != NULL)
                MUI_Offset(child, -addx, -addy);
        }

        if (_flags(obj) & MADF_CANDRAW)
            MUI_Redraw(obj, MADF_DRAWOBJECT);
    }

    data->oldpos[0] = data->pos[0];
    data->oldpos[1] = data->pos[1];
    return 0;
}

/*
 * Voyager V_GroupDraw aborts if InstallClip fails.  Clip the virtgroup's
 * visible inner box so HTML draw does not spill into chrome.  On clip
 * failure still return TRUE so the page paints unclipped rather than blank.
 *
 * MUI_AddClipRegion returns InstallClipRegion's *previous* clip (often NULL
 * on the first install), or (APTR)-1 if no clip was installed.  Storing that
 * return in data->clip and testing != NULL meant a successful first install
 * never got RemoveClip — the HTML rect stayed on the window layer and every
 * later Busy/toolbar/status redraw was clipped away (invisible footer Busy,
 * no depressed fastlink chrome).  Use a sentinel once Add succeeds.
 */
IPTR Virtgroup__MUIM_InstallClip(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_VirtgroupData *data = INST_DATA(cl, obj);
    struct Region *region;
    struct Rectangle r;
    APTR handle;

    (void)msg;

    if (data->clip != NULL && muiRenderInfo(obj) != NULL)
    {
        MUI_RemoveClipRegion(muiRenderInfo(obj), data->clip);
        data->clip = NULL;
    }

    if (_rp(obj) == NULL || muiRenderInfo(obj) == NULL)
        return TRUE;

    region = NewRegion();
    if (region == NULL)
        return TRUE;

    r.MinX = _mleft(obj);
    r.MinY = _mtop(obj);
    r.MaxX = _mright(obj);
    r.MaxY = _mbottom(obj);

    if (r.MinX > r.MaxX || r.MinY > r.MaxY || !OrRectRegion(region, &r))
    {
        DisposeRegion(region);
        return TRUE;
    }

    handle = MUI_AddClipRegion(muiRenderInfo(obj), region);
    if (handle == (APTR) -1)
        return TRUE;

    /* Non-NULL sentinel: RemoveClipRegion only treats (APTR)-1 as a no-op. */
    data->clip = (APTR) 1;
    return TRUE;
}

IPTR Virtgroup__MUIM_RemoveClip(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_VirtgroupData *data = INST_DATA(cl, obj);

    (void)msg;

    if (data->clip != NULL && muiRenderInfo(obj) != NULL)
    {
        MUI_RemoveClipRegion(muiRenderInfo(obj), data->clip);
        data->clip = NULL;
    }
    return TRUE;
}

#if ZUNE_BUILTIN_VIRTGROUP
BOOPSI_DISPATCHER(IPTR, Virtgroup_Dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
    case OM_NEW:
        return Virtgroup__OM_NEW(cl, obj, (struct opSet *)msg);
    case OM_SET:
        return Virtgroup__OM_SET(cl, obj, (struct opSet *)msg);
    case MUIM_Group_InitChange:
        return Virtgroup__MUIM_InitChange(cl, obj, msg);
    case MUIM_Virtgroup_InstallClip:
        return Virtgroup__MUIM_InstallClip(cl, obj, msg);
    case MUIM_Virtgroup_RemoveClip:
        return Virtgroup__MUIM_RemoveClip(cl, obj, msg);
    case MUIM_Virtgroup_Update:
        return Virtgroup__MUIM_Update(cl, obj,
            (struct MUIP_Virtgroup_Update *)msg);
    default:
        return DoSuperMethodA(cl, obj, msg);
    }
}
BOOPSI_DISPATCHER_END

const struct __MUIBuiltinClass _MUI_Virtgroup_desc =
{
    MUIC_Virtgroup,
    MUIC_Group,
    sizeof(struct MUI_VirtgroupData),
    (void *) Virtgroup_Dispatcher
};
#endif /* ZUNE_BUILTIN_VIRTGROUP */
