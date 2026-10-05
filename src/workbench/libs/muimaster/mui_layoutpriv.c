/*
 * Private MUI LVOs past MUI_EndRefresh that Voyager (and other MUI3 apps)
 * call from layout hooks / virtgroup scrolling.  Without these vectors the
 * jump table ends at -0xC6 and jsr -0xE4 (MUI_LayoutObj) hangs in random
 * memory - seen as lockup right after the first HTML lo_group layout.
 */

#include <intuition/classusr.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "classes/area.h"
#include "area_macros.h"
#include "support.h"

struct LongRect
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

/*****************************************************************************
 * LVO -0xCC MUIP_ObtainImage / -0xD2 MUIP_ReleaseImage
 * Stubs until imspec image API is wired; Voyager has its own image path.
 *****************************************************************************/
__ASM__ __SAVE_DS__ APTR MUIP_ObtainImage(
    __REG__(a0, struct MUI_RenderInfo *mri),
    __REG__(a1, APTR spec),
    __REG__(d0, ULONG flags))
{
    (void)mri;
    (void)spec;
    (void)flags;
    ZuneTrace("zune: MUIP_ObtainImage stub\n");
    return NULL;
}

__ASM__ __SAVE_DS__ VOID MUIP_ReleaseImage(
    __REG__(a0, struct MUI_RenderInfo *mri),
    __REG__(a1, APTR image))
{
    (void)mri;
    (void)image;
}

/*****************************************************************************
 * LVO -0xD8 MUI_Show / -0xDE MUI_Hide
 *****************************************************************************/
__ASM__ __SAVE_DS__ ULONG MUI_Show(__REG__(a0, Object *obj))
{
    if (obj == NULL)
        return 0;
    ZuneTrace("zune: MUI_Show obj=%lx\n", (ULONG) obj);
    return (ULONG) DoShowMethod(obj);
}

__ASM__ __SAVE_DS__ ULONG MUI_Hide(__REG__(a0, Object *obj))
{
    if (obj == NULL)
        return 0;
    ZuneTrace("zune: MUI_Hide obj=%lx\n", (ULONG) obj);
    return (ULONG) DoHideMethod(obj);
}

/*****************************************************************************
 * LVO -0xE4 MUI_LayoutObj -- absolute window coords (unlike MUI_Layout).
 *****************************************************************************/
__ASM__ __SAVE_DS__ BOOL MUI_LayoutObj(
    __REG__(a0, Object *obj),
    __REG__(d0, LONG left),
    __REG__(d1, LONG top),
    __REG__(d2, LONG width),
    __REG__(d3, LONG height),
    __REG__(d4, ULONG flags))
{
    static const struct MUIP_Layout method = { MUIM_Layout };

    (void)flags;

    if (obj == NULL)
        return FALSE;

    /* Hot path: Voyager HTML layout calls this thousands of times per page. */
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left = (WORD) left;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top = (WORD) top;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width = (WORD) width;
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height = (WORD) height;
    /*
     * Voyager HTML layout/hit-test uses _vtop (mad_VirtualTop).  Keep it
     * in sync with the absolute box Y LayoutObj installs.
     */
    ((struct __dummyAreaData__ *)(obj))->mad.mad_VirtualTop = top;

    DoMethodA(obj, (Msg)&method);
    return TRUE;
}

/*****************************************************************************
 * LVO -0xEA MUI_Offset -- virtgroup scroll shift.
 * Voyager lo_image caches mleft/mtop from _left/_top in Show; nested Bitmap
 * and layout gadgets must move with their parent.  Walk only the subtree of
 * the object Voyager passes (its ChildList walk already scopes the call).
 * Box and VirtualTop both move (lo_group paints with offs_y = _vtop).
 *****************************************************************************/
__ASM__ __SAVE_DS__ VOID MUI_Offset(
    __REG__(a0, Object *obj),
    __REG__(d0, LONG addx),
    __REG__(d1, LONG addy))
{
    /* Static: UI is single-threaded; keeps the queue off the task stack. */
    static Object *queue[512];
    ULONG qh, qt;
    Object *cur;
    Object *child;
    Object *cstate;
    struct List *childlist;

    if (obj == NULL)
        return;

    qh = 0;
    qt = 0;
    queue[qt++] = obj;

    while (qh < qt)
    {
        cur = queue[qh++];

        ((struct __dummyAreaData__ *)(cur))->mad.mad_Box.Left += (WORD) addx;
        ((struct __dummyAreaData__ *)(cur))->mad.mad_Box.Top += (WORD) addy;
        ((struct __dummyAreaData__ *)(cur))->mad.mad_VirtualTop += addy;

        childlist = NULL;
        if (!get(cur, MUIA_Group_ChildList, (IPTR *) & childlist)
            || childlist == NULL)
            continue;

        cstate = (Object *) ((struct MinList *)childlist)->mlh_Head;
        while ((child = NextObject(&cstate)) != NULL)
        {
            if (qt < 512)
                queue[qt++] = child;
            else
            {
                /* Queue full: still shift this node; deeper kids may stick. */
                ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Left +=
                    (WORD) addx;
                ((struct __dummyAreaData__ *)(child))->mad.mad_Box.Top +=
                    (WORD) addy;
                ((struct __dummyAreaData__ *)(child))->mad.mad_VirtualTop +=
                    addy;
            }
        }
    }
}

/*****************************************************************************
 * LVO -0xF0 MUIP_GetVirtualRect -- window X, VirtualTop Y (commercial).
 *****************************************************************************/
__ASM__ __SAVE_DS__ VOID MUIP_GetVirtualRect(
    __REG__(a0, Object *obj),
    __REG__(a1, struct LongRect *r))
{
    WORD l, w, h;
    LONG t;

    if (obj == NULL || r == NULL)
        return;

    l = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left;
    t = ((struct __dummyAreaData__ *)(obj))->mad.mad_VirtualTop;
    w = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width;
    h = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height;

    r->left = l;
    r->top = t;
    r->right = l + w - 1;
    r->bottom = t + h - 1;
}
