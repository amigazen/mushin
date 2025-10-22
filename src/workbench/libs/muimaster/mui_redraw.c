/*
    Copyright (C) 2003-2023, The AROS Development Team. All rights reserved.
*/

#include <string.h>
#include <clib/alib_protos.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include "muimaster_intern.h"
#include "mui.h"
#include "classes/area.h"
#include "area_macros.h"

/* Ensure MADF_ISVIRTUALGROUP is defined */
#ifndef MADF_ISVIRTUALGROUP
#define MADF_ISVIRTUALGROUP (1<<30)
#endif
#include "mui.h"
#include "support.h"

#include "debug.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_Redraw(register __a0 Object *obj, register __d0 ULONG flags)

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

    APTR clip = (APTR)-1;
    IPTR disabled = 0;

    if (!(((struct __dummyAreaData__ *)(obj))->mad.mad_Flags & MADF_CANDRAW)) return;

    if (((struct __dummyAreaData__ *)(obj))->mad.mad_Flags & MADF_INVIRTUALGROUP)
    {
        Object *wnd = NULL;
        Object *parent;
        struct Region *region = NULL;

        get(obj,MUIA_WindowObject,&wnd);
        parent = obj;

        while (get(parent,MUIA_Parent,&parent))
        {
            if (!parent) break;
            if (parent == wnd) break;

            if (((struct __dummyAreaData__ *)(parent))->mad.mad_Flags & MADF_ISVIRTUALGROUP)
            {
                struct Rectangle rect;

                rect.MinX = ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(parent))->mad.mad_addleft;
                rect.MinY = ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(parent))->mad.mad_addtop;
                rect.MaxX = ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(parent))->mad.mad_addleft + ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Width + ((struct __dummyAreaData__ *)(parent))->mad.mad_subwidth - 1;
                rect.MaxY = ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(parent))->mad.mad_addtop + ((struct __dummyAreaData__ *)(parent))->mad.mad_Box.Height + ((struct __dummyAreaData__ *)(parent))->mad.mad_subheight - 1;

                if (!region)
                {
                    if ((region = NewRegion()))
                    {
                        OrRectRegion(region, &rect);
                    }
                } else
                {
                    AndRectRegion(region, &rect);
                }
            }
        }

            if (region)
        {
            clip = MUI_AddClipRegion(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo,region);
        }
        
    } /* if object is in a virtual group */

    if (1)
    {
            struct Region *region;
        struct Rectangle *clip_rect;
        struct Layer *l;
        
        clip_rect = &((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_ClipRect;

            if (((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_Window)
        {
            l = ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_Window->WLayer;
        }
        else
        {
            l = ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort->Layer;
        }
        
        if (l && (region = l->ClipRegion))
        {
            /* Maybe this should went to MUI_AddClipRegion() */
            clip_rect->MinX = MAX(((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left,region->bounds.MinX);
            clip_rect->MinY = MAX(((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top,region->bounds.MinY);
            clip_rect->MaxX = MIN(((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width - 1,region->bounds.MaxX);
            clip_rect->MaxY = MIN(((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height - 1,region->bounds.MaxY);

        } else
        {
            clip_rect->MinX = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left;
            clip_rect->MinY = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top;
            clip_rect->MaxX = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width - 1;
            clip_rect->MaxY = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height - 1;
        }
    }
    
    ((struct __dummyAreaData__ *)(obj))->mad.mad_Flags = (((struct __dummyAreaData__ *)(obj))->mad.mad_Flags & ~MADF_DRAWFLAGS) | (flags & MADF_DRAWFLAGS);

    DoMethod(obj, MUIM_Draw, 0);

    if (get(obj, MUIA_Disabled, &disabled))
    {
#if 0
        /*
          Commented out, because group children were drawn wrongly
          when they have been disabled while window is open.
        */
        if (_parent(obj))
        {
            IPTR parentDisabled;
            if (get(_parent(obj), MUIA_Disabled, &parentDisabled))
            {
                /* Let the parent draw the pattern... */
                if (parentDisabled) disabled = FALSE;
            }
        }
#endif

        if ((disabled) && (XGET(obj, MUIA_NestedDisabled) != TRUE))
        {
#ifdef __AROS__
#if 0
            /*
              This aproach might be faster *provided* that the buffer is
              allocated and filled *once* at startup of muimaster.library.
                
              In reality, the WritePixelArray() call has quite a big
              overhead, so you should only use this buffer if the gadget
              completely fits inside, and fall back to allocating a new
              buffer if the gadget is too big.
                
              Perhaps a future optimization...
            */
            LONG  width  = 200;
            LONG  height = 100;
            LONG *buffer = AllocVec(width * height * sizeof(LONG), MEMF_ANY);
            LONG  x, y;
            
            memset(buffer, 0xAA, width * height * sizeof(LONG));
            
            for (y = 0; y < ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height; y += height)
            {
                for (x = 0; x < ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width; x += width)
                {
                    WritePixelArrayAlpha
                    (
                        buffer, 0, 0, width * sizeof(LONG),
                        ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + x, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + y,
                        x + width  > ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width  ? ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width  - x : width,
                        y + height > ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height ? ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height - y : height,
                        0xffffffff
                    );
                }
            }
#else
            LONG  width  = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width;
            LONG  height = ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height;
            LONG *buffer = NULL;
            
            if (GetBitMapAttr(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort->BitMap, BMA_DEPTH) >= 15)
            {
                buffer = AllocVec(width * sizeof(LONG), MEMF_ANY);
            }

            if (buffer != NULL)
            {
                memset(buffer, 0xAA, width * sizeof(LONG));

                WritePixelArrayAlpha
                (
                    buffer, 0, 0, 0,
                    ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top, width, height,
                    0xffffffff
                );
                FreeVec(buffer);
            }   else
#endif
#endif
            {
                /* fallback */
                const static UWORD pattern[] = { 0x8888, 0x2222, };
                LONG fg = ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_Pens[MPEN_SHADOW];
                
                SetDrMd(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, JAM1);
                SetAPen(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, fg);
                SetAfPt(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, pattern, 1);
                RectFill(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width - 1,
                    ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top + ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height - 1);
                SetAfPt(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_RastPort, NULL, 0);
            }
        }
    } /* if (object is disabled) */

    /* copy buffer to window */
    if (((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_BufferBM)
    {
        ClipBlit(&((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_BufferRP, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top,
                 ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_Window->RPort, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Left, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Top,
                 ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Width, ((struct __dummyAreaData__ *)(obj))->mad.mad_Box.Height, 0xc0);
    }

    if (clip != (APTR)-1)
    {
        /* This call actually also frees the region */
        MUI_RemoveClipRegion(muiRenderInfo(obj), clip);
    }

} /* MUI_Redraw */
