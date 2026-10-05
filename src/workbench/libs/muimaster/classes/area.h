/*
    Copyright (C) 1999, David Le Corfec.
    Copyright (C) 2002 - 2014, The AROS Development Team.
    All rights reserved.

*/

#ifndef _MUI_CLASSES_AREA_H
#define _MUI_CLASSES_AREA_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#ifndef GRAPHICS_GRAPHICS_H
#include <graphics/gfx.h>
#endif

#ifndef _MUI_CLASSES_WINDOW_H
#include "classes/window.h"     /* for MUI_EventHandlerNode, will be gone if this MUI_AreaData is moved to area.c */
#endif

/*** Name *******************************************************************/
#define MUIC_Area                   "Area.mui"

/*** Identifier base (for Zune extensions) **********************************/
#define MUIB_Area                   (MUIB_ZUNE | 0x00000200)

/*** Methods ****************************************************************/
#define MUIM_AskMinMax \
    (MUIB_MUI | 0x00423874) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_Cleanup \
    (MUIB_MUI | 0x0042d985) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_ContextMenuBuild \
    (MUIB_MUI | 0x00429d2e)       /* MUI: V11 */
#define MUIM_ContextMenuChoice \
    (MUIB_MUI | 0x00420f0e)       /* MUI: V11 */
#define MUIM_CreateBubble \
    (MUIB_MUI | 0x00421c41)       /* MUI: V18 */
#define MUIM_CreateDragImage \
    (MUIB_MUI | 0x0042eb6f) /* MUI: V18 */ /* For Custom Classes only, undoc */
#define MUIM_CreateShortHelp \
    (MUIB_MUI | 0x00428e93)       /* MUI: V11 */
#define MUIM_CustomBackfill \
    (MUIB_MUI | 0x00428d73)       /* Undoc */
#define MUIM_DeleteBubble \
    (MUIB_MUI | 0x004211af)       /* MUI: V18 */
#define MUIM_DeleteDragImage \
    (MUIB_MUI | 0x00423037) /* MUI: V18 */ /* For Custom Classes only, undoc */
#define MUIM_DeleteShortHelp \
    (MUIB_MUI | 0x0042d35a)       /* MUI: V11 */
#define MUIM_DoDrag \
    (MUIB_MUI | 0x004216bb) /* MUI: V18 */ /* For Custom Classes only, undoc */
#define MUIM_DragBegin \
    (MUIB_MUI | 0x0042c03a)       /* MUI: V11 */
#define MUIM_DragDrop \
    (MUIB_MUI | 0x0042c555)       /* MUI: V11 */
#define MUIM_UnknownDropDestination \
    (MUIB_MUI | 0x00425550)       /* ZUNE */
#define MUIM_DragFinish \
    (MUIB_MUI | 0x004251f0)       /* MUI: V11 */
#define MUIM_DragQuery \
    (MUIB_MUI | 0x00420261)       /* MUI: V11 */
#define MUIM_DragReport \
    (MUIB_MUI | 0x0042edad)       /* MUI: V11 */
#define MUIM_Draw \
    (MUIB_MUI | 0x00426f3f) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_DrawBackground \
    (MUIB_MUI | 0x004238ca)       /* MUI: V11 */
#define MUIM_GoActive \
    (MUIB_MUI | 0x0042491a)       /* Undoc */
#define MUIM_GoInactive \
    (MUIB_MUI | 0x00422c0c)       /* Undoc */
#define MUIM_HandleEvent \
    (MUIB_MUI | 0x00426d66) /* MUI: V16 */        /* For Custom Classes only */
#define MUIM_HandleInput \
    (MUIB_MUI | 0x00422a1a) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_Hide \
    (MUIB_MUI | 0x0042f20f) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_Setup \
    (MUIB_MUI | 0x00428354) /* MUI: V4  */        /* For Custom Classes only */
#define MUIM_Show \
    (MUIB_MUI | 0x0042cc84) /* MUI: V4  */        /* For Custom Classes only */

struct MUIP_AskMinMax
{
    STACKED ULONG MethodID;
    STACKED struct MUI_MinMax *MinMaxInfo;
};

struct MUIP_Cleanup
{
    STACKED ULONG MethodID;
};

struct MUIP_ContextMenuBuild
{
    STACKED ULONG MethodID;
    STACKED LONG mx;
    STACKED LONG my;
};

struct MUIP_ContextMenuChoice
{
    STACKED ULONG MethodID;
    STACKED Object *item;
};

struct MUIP_CreateBubble
{
    STACKED ULONG MethodID;
    STACKED LONG x;
    STACKED LONG y;
    STACKED char *txt;
    STACKED ULONG flags;
};

struct MUIP_CreateDragImage
{
    STACKED ULONG MethodID;
    STACKED LONG touchx;
    STACKED LONG touchy;
    STACKED ULONG flags;
};

struct MUIP_CreateShortHelp
{
    STACKED ULONG MethodID;
    STACKED LONG mx;
    STACKED LONG my;
};

struct MUIP_CustomBackfill
{
    STACKED ULONG MethodID;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG right;
    STACKED LONG bottom;
    STACKED LONG xoffset;
    STACKED LONG yoffset;
};

struct MUIP_DeleteBubble
{
    STACKED ULONG MethodID;
    STACKED APTR bubble;
};

struct MUIP_DeleteDragImage
{
    STACKED ULONG MethodID;
    STACKED struct MUI_DragImage *di;
};

struct MUIP_DeleteShortHelp
{
    STACKED ULONG MethodID;
    STACKED STRPTR help;
};

struct MUIP_DoDrag
{
    STACKED ULONG MethodID;
    STACKED LONG touchx;
    STACKED LONG touchy;
    STACKED ULONG flags;
};

struct MUIP_UnknownDropDestination
{
    STACKED ULONG MethodID;
    STACKED struct IntuiMessage *imsg;
};

struct MUIP_DragBegin
{
    STACKED ULONG MethodID;
    STACKED Object *obj;
};

struct MUIP_DragDrop
{
    STACKED ULONG MethodID;
    STACKED Object *obj;
    STACKED LONG x;
    STACKED LONG y;
};

struct MUIP_DragFinish
{
    STACKED ULONG MethodID;
    STACKED Object *obj;
};

struct MUIP_DragQuery
{
    STACKED ULONG MethodID;
    STACKED Object *obj;
};

struct MUIP_DragReport
{
    STACKED ULONG MethodID;
    STACKED Object *obj;
    STACKED LONG x;
    STACKED LONG y;
    STACKED LONG update;
};

struct MUIP_Draw
{
    STACKED ULONG MethodID;
    STACKED ULONG flags;
};

struct MUIP_DrawBackground
{
    STACKED ULONG MethodID;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG width;
    STACKED LONG height;
    STACKED LONG xoffset;
    STACKED LONG yoffset;
    STACKED LONG flags;
};

struct MUIP_DrawBackgroundBuffered
{
    STACKED ULONG MethodID;
    STACKED struct RastPort *rp;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG width;
    STACKED LONG height;
    STACKED LONG xoffset;
    STACKED LONG yoffset;
    STACKED LONG flags;
};

struct MUIP_GoActive
{
    STACKED ULONG MethodID;
};

struct MUIP_GoInactive
{
    STACKED ULONG MethodID;
};

struct MUIP_HandleEvent
{
    STACKED ULONG MethodID;
    STACKED struct IntuiMessage *imsg;
    STACKED LONG muikey;
};

struct MUIP_HandleInput
{
    STACKED ULONG MethodID;
    STACKED struct IntuiMessage *imsg;
    STACKED LONG muikey;
};

struct MUIP_Hide
{
    STACKED ULONG MethodID;
};

struct MUIP_Setup
{
    STACKED ULONG MethodID;
    STACKED struct MUI_RenderInfo *RenderInfo;
};

struct MUIP_Show
{
    STACKED ULONG MethodID;
    /* Commercial MUI 3.8 / Voyager: clip rect for virtgroup ShowClipped. */
    STACKED APTR clip;
};

#define MUIM_Layout \
    (MUIB_Area | 0x00000000)
#define MUIM_DrawParentBackground \
    (MUIB_Area | 0x00000001)
#define MUIM_DragQueryExtended  /* PRIV */ \
    (MUIB_Area | 0x00000002)    /* PRIV - returns a object or NULL */
#define MUIM_Timer              /* PRIV */ \
    (MUIB_Area | 0x00000003)    /* PRIV */
#define MUIM_UpdateInnerSizes   /* PRIV */ \
    (MUIB_Area | 0x00000004)    /* PRIV for now */
#define MUIM_FindAreaObject     /* PRIV */ \
    (MUIB_Area | 0x00000005)    /* PRIV */
#define MUIM_DrawBackgroundBuffered /* PRIV */ \
    (MUIB_Area | 0x00000006)    /* PRIV */
#define MUIM_CreateFrameClippingRegion \
    (MUIB_Area | 0x00000007)
#define MUIM_QueryFrameCharacteristics \
    (MUIB_Area | 0x00000008)

struct MUIP_Layout
{
    STACKED ULONG MethodID;
};

struct MUIP_DrawParentBackground
{
    STACKED ULONG MethodID;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG width;
    STACKED LONG height;
    STACKED LONG xoffset;
    STACKED LONG yoffset;
    STACKED LONG flags;
};

struct MUIP_DragQueryExtended /* PRIV */
{                             /* PRIV */
    STACKED ULONG MethodID;   /* PRIV */
    STACKED Object *obj;      /* PRIV */
    STACKED LONG x;           /* PRIV */
    STACKED LONG y;           /* PRIV */
};                            /* PRIV */

struct MUIP_Timer             /* PRIV */
{                             /* PRIV */
    STACKED ULONG MethodID;   /* PRIV */
};                            /* PRIV */

struct MUIP_UpdateInnerSizes  /* PRIV */
{                             /* PRIV */
    STACKED ULONG MethodID;   /* PRIV */
};                            /* PRIV */

struct MUIP_FindAreaObject    /* PRIV */
{                             /* PRIV */
    STACKED ULONG MethodID;   /* PRIV */
    STACKED Object *obj;      /* PRIV */
};                            /* PRIV */

struct MUI_DragImage
{
    struct BitMap *bm;
    WORD width;                 /* exact width and height of bitmap */
    WORD height;
    WORD touchx;                /* position of pointer click relative to bitmap */
    WORD touchy;
    ULONG flags;
};

/* Message structure for querying frame clipping information */
struct MUIP_QueryFrameCharacteristics
{
    STACKED ULONG MethodID;
    STACKED struct MUI_FrameCharacteristics *characteristics; /* OUT: Frame information */
};

/* Message structure for creating frame clipping region */
struct MUIP_CreateFrameClippingRegion
{
    STACKED ULONG MethodID;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG width;
    STACKED LONG height;
    STACKED struct Region *clipinfo; /* OUT: Frame clipping region */
};

// #define MUIF_DRAGIMAGE_HASMASK       (1<<0) /* Use provided mask for drawing */
                                               /* Not supported at the moment */
#define MUIF_DRAGIMAGE_SOURCEALPHA   (1<<1)     /* Use drag image source alpha
                                                 * information for transparent
                                                 * drawing */

/*** Attributes *************************************************************/
#define MUIA_Background \
    (MUIB_MUI | 0x0042545b)   /* MUI: V4  is. LONG              */
#define MUIA_BottomEdge \
    (MUIB_MUI | 0x0042e552)   /* MUI: V4  ..g LONG              */
#define MUIA_ContextMenu \
    (MUIB_MUI | 0x0042b704)   /* MUI: V11 isg Object *          */
#define MUIA_ContextMenuTrigger \
    (MUIB_MUI | 0x0042a2c1)   /* MUI: V11 ..g Object *          */
#define MUIA_ControlChar \
    (MUIB_MUI | 0x0042120b)   /* MUI: V4  isg char              */
#define MUIA_CustomBackfill \
    (MUIB_MUI | 0x00420a63)   /* undoc    i..                   */
#define MUIA_CycleChain \
    (MUIB_MUI | 0x00421ce7)   /* MUI: V11 isg LONG              */
#define MUIA_Disabled \
    (MUIB_MUI | 0x00423661)   /* MUI: V4  isg BOOL              */
#define MUIA_DoubleBuffer \
    (MUIB_MUI | 0x0042a9c7)   /* MUI: V20 isg BOOL              */
#define MUIA_Draggable \
    (MUIB_MUI | 0x00420b6e)   /* MUI: V11 isg BOOL              */
#define MUIA_Dropable \
    (MUIB_MUI | 0x0042fbce)   /* MUI: V11 isg BOOL              */
#define MUIA_FillArea \
    (MUIB_MUI | 0x004294a3)   /* MUI: V4  is. BOOL              */
#define MUIA_FixHeight \
    (MUIB_MUI | 0x0042a92b)   /* MUI: V4  i.. LONG              */
#define MUIA_FixHeightTxt \
    (MUIB_MUI | 0x004276f2)   /* MUI: V4  i.. STRPTR            */
#define MUIA_FixWidth \
    (MUIB_MUI | 0x0042a3f1)   /* MUI: V4  i.. LONG              */
#define MUIA_FixWidthTxt \
    (MUIB_MUI | 0x0042d044)   /* MUI: V4  i.. STRPTR            */
#define MUIA_Font \
    (MUIB_MUI | 0x0042be50)   /* MUI: V4  i.g struct TextFont * */
#define MUIA_Frame \
    (MUIB_MUI | 0x0042ac64)   /* MUI: V4  i.. LONG              */
#define MUIA_FramePhantomHoriz \
    (MUIB_MUI | 0x0042ed76)   /* MUI: V4  i.. BOOL              */
#define MUIA_FrameTitle \
    (MUIB_MUI | 0x0042d1c7)   /* MUI: V4  i.. STRPTR            */
#define MUIA_Height \
    (MUIB_MUI | 0x00423237)   /* MUI: V4  ..g LONG              */
#define MUIA_HorizDisappear \
    (MUIB_MUI | 0x00429615)   /* MUI: V11 isg LONG              */
#define MUIA_HorizWeight \
    (MUIB_MUI | 0x00426db9)   /* MUI: V4  isg WORD              */
#define MUIA_InnerBottom \
    (MUIB_MUI | 0x0042f2c0)   /* MUI: V4  i.g LONG              */
#define MUIA_InnerLeft \
    (MUIB_MUI | 0x004228f8)   /* MUI: V4  i.g LONG              */
#define MUIA_InnerRight \
    (MUIB_MUI | 0x004297ff)   /* MUI: V4  i.g LONG              */
#define MUIA_InnerTop \
    (MUIB_MUI | 0x00421eb6)   /* MUI: V4  i.g LONG              */
#define MUIA_InputMode \
    (MUIB_MUI | 0x0042fb04)   /* MUI: V4  i.. LONG              */
#define MUIA_LeftEdge \
    (MUIB_MUI | 0x0042bec6)   /* MUI: V4  ..g LONG              */
#define MUIA_MaxHeight \
    (MUIB_MUI | 0x004293e4)   /* MUI: V11 i.. LONG              */
#define MUIA_MaxWidth \
    (MUIB_MUI | 0x0042f112)   /* MUI: V11 i.. LONG              */
#define MUIA_Pressed \
    (MUIB_MUI | 0x00423535)   /* MUI: V4  ..g BOOL              */
#define MUIA_RightEdge \
    (MUIB_MUI | 0x0042ba82)   /* MUI: V4  ..g LONG              */
#define MUIA_Selected \
    (MUIB_MUI | 0x0042654b)   /* MUI: V4  isg BOOL              */
#define MUIA_ShortHelp \
    (MUIB_MUI | 0x00428fe3)   /* MUI: V11 isg STRPTR            */
#define MUIA_ShowMe \
    (MUIB_MUI | 0x00429ba8)   /* MUI: V4  isg BOOL              */
#define MUIA_ShowSelState \
    (MUIB_MUI | 0x0042caac)   /* MUI: V4  i.. BOOL              */
#define MUIA_Timer \
    (MUIB_MUI | 0x00426435)   /* MUI: V4  ..g LONG              */
#define MUIA_TopEdge \
    (MUIB_MUI | 0x0042509b)   /* MUI: V4  ..g LONG              */
#define MUIA_VertDisappear \
    (MUIB_MUI | 0x0042d12f)   /* MUI: V11 isg LONG              */
#define MUIA_VertWeight \
    (MUIB_MUI | 0x004298d0)   /* MUI: V4  isg WORD              */
#define MUIA_Weight \
    (MUIB_MUI | 0x00421d1f)   /* MUI: V4  i.. WORD              */
#define MUIA_Width \
    (MUIB_MUI | 0x0042b59c)   /* MUI: V4  ..g LONG              */
#define MUIA_Window \
    (MUIB_MUI | 0x00421591)   /* MUI: V4  ..g struct Window *   */
#define MUIA_WindowObject \
    (MUIB_MUI | 0x0042669e)   /* MUI: V4  ..g Object *          */

#define MUIA_NestedDisabled \
    (MUIB_Area | 0x00000000)        /* Zune 20030530  isg BOOL        */

#ifdef MUI_OBSOLETE
#define MUIA_ExportID (MUIB_MUI | 0x0042d76e)     /* V4  isg ULONG */
#endif /* MUI_OBSOLETE */

struct MUI_ImageSpec_intern;

struct MUI_AreaData
{
    struct MUI_RenderInfo *mad_RenderInfo;      /* RenderInfo for this object */
    struct MUI_ImageSpec_intern *mad_Background;        /* bg setting - *private* ! */
    struct TextFont *mad_Font;  /* Font which is used to draw */
    struct MUI_MinMax mad_MinMax;       /* min/max/default dimensions */
    struct IBox mad_Box;        /* coordinates and dim of this object after layout */
    BYTE mad_addleft;           /* left offset (frame & innerspacing) */
    BYTE mad_addtop;            /* top offset (frame & innerspacing) */
    BYTE mad_subwidth;          /* additional width (frame & innerspacing) */
    BYTE mad_subheight;         /* additional height (frame & innerspacing) */
    ULONG mad_Flags;            /* some flags; see below */
    /*
     * Commercial MUI 3.x layout through mad_Flags2 (Voyager _vtop etc.).
     * Zune-only fields follow.
     */
    /* START PRIV */
    WORD mad_HorizWeight;
    WORD mad_VertWeight;
    WORD mad_HorizDisappear;
    WORD mad_VertDisappear;
    ULONG mad_IDCMP;
    CONST_STRPTR mad_BackgroundSpec;
    IPTR mad_FontPreset;
    CONST_STRPTR mad_ShortHelp;
    LONG mad_FixWidth;
    LONG mad_FixHeight;
    LONG mad_VirtualTop;
    ULONG mad_Flags2;

    CONST_STRPTR mad_FrameTitle;
    BYTE mad_InnerLeft;
    BYTE mad_InnerTop;
    BYTE mad_InnerRight;
    BYTE mad_InnerBottom;
    BYTE mad_FrameOBSOLETE;
    BYTE mad_InputMode;
    TEXT mad_ControlChar;
    BYTE mad_TitleHeightAdd;
    BYTE mad_TitleHeightBelow;
    BYTE mad_TitleHeightAbove;
    IPTR mad_Frame;
    WORD mad_HardHeight;
    WORD mad_HardWidth;
    CONST_STRPTR mad_HardWidthTxt;
    CONST_STRPTR mad_HardHeightTxt;
    struct MUI_ImageSpec_intern *mad_SelBack;
    struct MUI_EventHandlerNode mad_ehn;
    struct MUI_InputHandlerNode mad_Timer;
    ULONG mad_Timeval;
    struct MUI_EventHandlerNode mad_ccn;
    Object *mad_ContextMenu;
    LONG mad_ClickX;
    LONG mad_ClickY;
    struct ZMenu *mad_ContextZMenu;
    struct MUI_EventHandlerNode mad_hiehn;
    LONG mad_DisableCount;
    /* END PRIV */
};

/*
 * NOTE: do not define MUI_AREADATA_DEFINED here.  That macro means
 * "struct __dummyAreaData__ has been declared", and it is owned by macros.h /
 * area_macros.h.  This header only declares struct MUI_AreaData itself.
 *
 * Defining it here used to break the build in a way the compiler could not
 * report: macros.h includes this file *before* it tests the guard, so the test
 * always failed and muiAreaData/muiGlobalInfo/_left/_rp/_pens/_flags and the
 * rest were never defined at all in any file that got its macros through
 * macros.h.  That is why so many sources ended up with hand-expanded
 * ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left instead of _left(obj).
 */

/*
 * mad_Flags bit layout MUST match commercial MUI 3.8 (Voyager's mui.h).
 * Voyager sets MADF_PARTIAL / MADF_DRAWALL / MADF_KNOWSACTIVE / MADF_VISIBLE
 * on AreaData directly; Zune-only bits live in mad_Flags2.
 */
#define MADF_DRAWOBJECT        (1<< 0)
#define MADF_DRAWUPDATE        (1<< 1)
#define MADF_DRAWACTIVE        (1<< 2)
#define MADF_DRAGGABLE         (1<< 3)
#define MADF_CUSTOMBACKFILL    (1<< 4)
#define MADF_CYCLECHAIN        (1<< 5)
#define MADF_FIXMEANSMAX       (1<< 6)
#define MADF_KNOWSACTIVE       (1<< 7)
#define MADF_GROUP             (1<< 8)
#define MADF_SHOWME            (1<< 9)
#define MADF_PARTIAL           (1<<10)
#define MADF_DRAWOUTER         (1<<11)
#define MADF_DRAWCHILD         (1<<12)
#define MADF_DROPABLE          (1<<13)
#define MADF_VISIBLE           (1<<14)
#define MADF_CANDRAW           MADF_VISIBLE
#define MADF_DISABLED          (1<<15)
#define MADF_SHOWSELSTATE      (1<<16)
#define MADF_PRESSED           (1<<17)
#define MADF_SELECTED          (1<<18)
#define MADF_PAGETITLES        (1<<19)
#define MADF_FILLAREA          (1<<20)
#define MADF_FIXWIDTHTXT       (1<<21)
#define MADF_FIXHEIGHTTXT      (1<<22)
#define MADF_INNERLEFT         (1<<23)
#define MADF_INNERTOP          (1<<24)
#define MADF_INNERRIGHT        (1<<25)
#define MADF_INNERBOTTOM       (1<<26)
#define MADF_FRAMEPHANTOM      (1<<27)
#define MADF_DRAWDROPBOX       (1<<28)
#define MADF_INVIRTUALGROUP    (1<<29)
#define MADF_INVIRTUAL         MADF_INVIRTUALGROUP
#define MADF_ISVIRTUALGROUP    (1<<30)
#define MADF_VIRTUAL           MADF_ISVIRTUALGROUP
#define MADF_FRAMEOFFSET       (1<<31)

#define MADF_DRAWALL    (MADF_DRAWACTIVE | MADF_DRAWOUTER | MADF_DRAWOBJECT)
#define MADF_DRAWMASK   (MADF_DRAWOUTER | MADF_DRAWOBJECT | MADF_DRAWCHILD \
    | MADF_DRAWUPDATE | MADF_DRAWACTIVE)
#define MADF_DRAWFLAGS  MADF_DRAWMASK

/* Old Zune names kept as aliases where the commercial bit matches. */
#define MADF_DRAW_XXX          MADF_DRAWACTIVE
#define MADF_DRAWFRAME         MADF_DRAWOUTER
#define MADF_DRAW_XXX_2        MADF_DRAWCHILD

/* mad_Flags2 -- Zune-only state (must not use commercial mad_Flags bits) */
#define MADF2_SETUP                (1<< 0)
#define MADF2_MAXHEIGHT            (1<< 1)
#define MADF2_MAXWIDTH             (1<< 2)
#define MADF2_BORDERGADGET         (1<< 3)
#define MADF2_OWNBG                (1<< 4)
#define MADF2_DRAGGING             (1<< 5)
#define MADF2_FIXHEIGHT            (1<< 6)
#define MADF2_FIXWIDTH             (1<< 7)


// offset 94 (byte) (frame << 1) (lsb is SETUP_DONE flag)
enum
{
    MUIV_Frame_None = 0,
    MUIV_Frame_Button,
    MUIV_Frame_ImageButton,
    MUIV_Frame_Text,
    MUIV_Frame_String,
    MUIV_Frame_ReadList,
    MUIV_Frame_InputList,
    MUIV_Frame_Prop,
    MUIV_Frame_Gauge,
    MUIV_Frame_Group,
    MUIV_Frame_PopUp,
    MUIV_Frame_Virtual,
    MUIV_Frame_Slider,
    MUIV_Frame_Knob,
    MUIV_Frame_Drag,
    /* Values reserved for existing MUI4/MUI5 types*/
    MUIV_Frame_Register = 21,
    /* Values reserved for existing MUI4/MUI5 types*/
    MUIV_Frame_Count = 24
};

// offset 95
enum
{
    MUIV_InputMode_None = 0,    // 0x00
    MUIV_InputMode_RelVerify,   // 0x40 (1<<6)
    MUIV_InputMode_Immediate,   // 0x80 (1<<7)
    MUIV_InputMode_Toggle,      // 0xc0 (1<<7 | 1<<6)
};



enum
{
    MUIV_DragQuery_Refuse = 0,
    MUIV_DragQuery_Accept,
};

enum
{
    MUIV_DragReport_Abort = 0,
    MUIV_DragReport_Continue,
    MUIV_DragReport_Lock,
    MUIV_DragReport_Refresh,
};

#define MUIV_CreateBubble_DontHidePointer (1<<0)

/* A private functions and macros */
void __area_finish_minmax(Object *obj, struct MUI_MinMax *MinMaxInfo); /* PRIV */

/*#define DRAW_BG_RECURSIVE (1<<1)*/
#define _vweight(obj)                                              /* PRIV */ \
    (muiAreaData(obj)->mad_VertWeight)    /* accesses private members PRIV */
#define _hweight(obj)                                              /* PRIV */ \
    (muiAreaData(obj)->mad_HorizWeight)   /* accesses private members PRIV */


/**************************************************************************
  Frame clipping
 **************************************************************************/

extern const struct __MUIBuiltinClass _MUI_Area_desc;   /* PRIV */

#endif /* _MUI_CLASSES_AREA_H */
