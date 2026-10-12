/*
    Copyright (C) 2002-2013, The AROS Development Team. All rights reserved.
    Copyright (C) 2026, amigazen project. All rights reserved.
*/

#ifndef _MUIMASTER_SUPPORT_H
#define _MUIMASTER_SUPPORT_H

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef INTUITION_CLASSUSR_H
#   include <intuition/classusr.h>
#endif
#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif
#ifndef CLIB_MACROS_H
#   include <clib/macros.h>
#endif

#ifdef __AROS__
#   ifndef AROS_ASMCALL_H
#       include <aros/asmcall.h>
#   endif
#   include <aros/macros.h>
#   define IMSPEC_EXTERNAL_PREFIX "IMAGES:Zune/"
#   include "support_aros.h"
#else
#   include "support_amigaos.h"
#endif

struct MUI_RenderInfo;
struct IntuiMessage;
struct Region;
struct Rectangle;
struct Object;
struct Library;

#define mui_alloc(x) AllocVec(x,MEMF_CLEAR)
#define mui_alloc_struct(x) ((x *)AllocVec(sizeof(x),MEMF_CLEAR))
#define mui_free(x) FreeVec(x)


int isRegionWithinBounds(struct Region *r, int left, int top, int width,
    int height);
ULONG ConvertKey(struct IntuiMessage *imsg);

#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(obj, x, y) (_between(_mleft(obj), (x), _mright(obj)) \
                          && _between(_mtop(obj), (y), _mbottom(obj)))

/* add mask in flags if tag is true, else sub mask */
#define _handle_bool_tag(flags, tag, mask) \
((tag != 0) ? ((flags) |= (mask)) : ((flags) &= ~(mask)))

#define CLAMP(x, low, high) \
    (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifndef __GNUC__
IPTR XGET(Object * obj, Tag attr);
#endif

IPTR DoSetupMethod(Object * obj, struct MUI_RenderInfo *info);
IPTR DoShowMethod(Object * obj);
IPTR DoHideMethod(Object * obj);

/*
 * Live traces to Output() or T:zune.log.  Off unless a build explicitly
 * passes -DZUNE_TRACE=1.  The GNU form drops the calls; other compilers
 * keep a call that the function turns into an immediate return.
 */
#ifndef ZUNE_TRACE
#define ZUNE_TRACE 0
#endif

void ZuneTrace(CONST_STRPTR fmt, ...);
#if !ZUNE_TRACE
#if defined(__GNUC__)
#define ZuneTrace(fmt...) ((void)0)
#else
#define ZuneTrace if (0) ZuneTrace
#endif
#endif

/*
 * Set when the first Menuitem is disposed. Builtin dispatchers and
 * MUI_NewObjectA then log, so the call IBrowse makes after that
 * dispose is visible. Stays off during splash construction.
 */
extern int Zune_LogAfterMenu;
void ZuneLogMethod(Object *obj, Msg msg);

/*
 * SAS/C DoMethod() varargs is unsafe for 32-bit MUI method IDs (and for
 * mixed pointer/integer args).  Draw used that path, so MUIM_DrawBackground
 * jumped into data.  Always use DoMethodA for these.
 */
IPTR ZuneDrawBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags);
IPTR ZuneDrawParentBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags);
IPTR ZuneWindowDrawBackground(Object *obj, LONG left, LONG top, LONG width,
    LONG height, LONG xoffset, LONG yoffset, LONG flags);
IPTR ZuneLayout(Object *obj);
/* C body of the MUI_Offset LVO (register args; never call that from C). */
VOID ZuneOffset(Object *obj, LONG addx, LONG addy);

/* True if obj's ancestor Group is in MUIM_Group_InitChange (commercial:
 * ShowMe must not RecalcDisplay until ExitChange). */
BOOL Zune_GroupExchangeActive(Object *obj);

/* returns next node of this node */
void *Node_Next(APTR node);
/* returns first node of this list */
void *List_First(APTR list);

/*
 * Walk a list of BOOPSI objects whose nodes are struct _Object headers
 * (the same nodes AddHead/AddTail(_OBJECT(obj)) install).  *state starts as
 * lh_Head.  Kickstart NextObject is an intuition register LVO.  A plain C
 * call (missing/stale pragma, or IGNORE=63 hiding no-prototype) uses the
 * wrong convention and returns garbage; DisposeObject then jumps into
 * chip/fast data.  Empty lists return NULL and survive, which is why
 * Test 4/7 passed.
 */
Object *ZuneNextObject(Object **state);

/*
 * Splice obj out of a Family/Group child list.  The list node is the
 * BOOPSI _Object header.  Remove() only when both neighbours still point
 * at that node; a REMHEAD'd node keeps stale ln_Succ/ln_Pred and Remove()
 * would splice some other list.  Always clear the pointers afterwards so
 * a pool block reused for the next MenuitemObject is not still named by
 * the parent's lh_Head/ln_Succ (IBrowse then MUI_DisposeObject's the
 * replacement).
 */
void ZuneUnlinkObject(Object *obj);

#ifndef MUIMASTER_DEFINING_REDRAW
void ZuneRedraw(ULONG obj, ULONG flags);
#define MUI_Redraw(obj, flags) ZuneRedraw((ULONG)(obj), (ULONG)(flags))
#endif

/*
 * Same trap as MUI_Redraw: the clipping LVOs are __REG__ functions.
 * A plain C stack call does not load A0/A1/D0.. correctly.  Text.mui hits
 * this on the first button paint (MUI_AddClipping -> MUI_AddClipRegion).
 * Route internal callers through C wrappers.
 */
APTR ZuneAddClipping(struct MUI_RenderInfo *mri, LONG left, LONG top,
    LONG width, LONG height);
VOID ZuneRemoveClipping(struct MUI_RenderInfo *mri, APTR handle);
APTR ZuneAddClipRegion(struct MUI_RenderInfo *mri, struct Region *r);
VOID ZuneRemoveClipRegion(struct MUI_RenderInfo *mri, APTR handle);
#ifndef MUIMASTER_DEFINING_CLIPPING
#define MUI_AddClipping(mri, l, t, w, h) \
    ZuneAddClipping((mri), (LONG)(l), (LONG)(t), (LONG)(w), (LONG)(h))
#define MUI_RemoveClipping(mri, handle) \
    ZuneRemoveClipping((mri), (handle))
#define MUI_AddClipRegion(mri, r) \
    ZuneAddClipRegion((mri), (r))
#define MUI_RemoveClipRegion(mri, handle) \
    ZuneRemoveClipRegion((mri), (handle))
#endif

#ifdef NextObject
#undef NextObject
#endif
#define NextObject ZuneNextObject

WORD SubtractRectFromRect(struct Rectangle *a, struct Rectangle *b,
    struct Rectangle *destrectarray);
ULONG IsObjectVisible(Object * child, struct Library *MUIMasterBase);

/* After a layout-time Virtgroup_Top/Left that did not run Update, make
 * oldpos[] equal pos[] so the next HtmlView Update has no leftover delta. */
void ZuneVirtgroupCommitScroll(Object *obj);

#endif /* _MUIMASTER_SUPPORT_H */
