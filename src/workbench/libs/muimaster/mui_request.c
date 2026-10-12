/*
    Copyright (C) 2002-2025, The AROS Development Team. All rights reserved.
    Copyright (C) 2026, amigazen project. All rights reserved.
*/

#ifdef __AROS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif
#include <proto/exec.h>
#include <proto/intuition.h>
#include <string.h>

#ifdef __AROS__
#endif

#include "mui.h"
#include "support.h"
#include "muimaster_intern.h"

extern struct Library *MUIMasterBase;

#ifdef __AROS__
AROS_UFH2S(void, cpy_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(STRPTR *, strPtrPtr, A3))
{
    AROS_USERFUNC_INIT
    
    *(*strPtrPtr)++ = chr;
    
    AROS_USERFUNC_EXIT
}

AROS_UFH2S(void, len_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(LONG *, lenPtr, A3))
{
    AROS_USERFUNC_INIT
    
    (*lenPtr)++;
    
    AROS_USERFUNC_EXIT
}

static void muiDataStreamFromFormat(CONST_STRPTR format, APTR dataStream,  ULONG *dataSize,
                             ULONG *indexStream, ULONG *indexSize, ...)
{
    va_list empty;
    va_start(empty, indexSize);
    GetDataStreamFromFormat(format, empty, dataStream, dataSize,
                        indexStream, indexSize);
    va_end(empty);
}
#endif

/*****************************************************************************

    NAME */
        __asm __saveds LONG MUI_RequestA(register __d0 APTR app, register __d1 APTR win, register __d2 LONGBITS flags, register __a0 CONST_STRPTR title, register __a1 CONST_STRPTR gadgets, register __a2 CONST_STRPTR format, register __a3 APTR params)

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

    LONG result;
    char *reqtxt;
    LONG reqtxt_len;
#ifndef __AROS__
    static const ULONG len_func = 0x52934e75; /* addq.l  #1,(A3) ; rts */
    static const ULONG cpy_func = 0x16c04e75; /* move.b d0,(a3)+ ; rts */
#endif

    Object *req_wnd;
    Object *req_group;
    Object *req_but[32]; /* shouldn't be more than 32 buttons per requester */
    /*
     * app arrives in D0 and req_wnd lives in a scratch register.  The
     * input loop's DoMethod return overwrites both, and the dispose
     * below then frees the application.  Apdf's About path did that,
     * and cleanup() freed the same object again (memory list corrupt).
     */
    volatile APTR keep_app;
    volatile Object *keep_wnd;

#ifdef __AROS__
    /* Repack IPTR[] to RAWARG */
    RAWARG _params;
    ULONG *_index;
    ULONG _indexsize = 0, _datasize = 0;
    UBYTE *ptr;
    muiDataStreamFromFormat(format, NULL, NULL, NULL, &_indexsize);
    _index = (ULONG *)alloca(_indexsize + sizeof(ULONG));
    muiDataStreamFromFormat(format, NULL, &_datasize, _index, &_indexsize);
    _index[_indexsize / sizeof(ULONG)] = _datasize;
    _params = (RAWARG)alloca(_datasize);
    ptr = (UBYTE *)_params;

    for (int i = 0; i < _indexsize / sizeof (ULONG); i++)
    {
        UBYTE dsize = _index[i + 1] - _index[i];

        switch(dsize)
        {
            case (2): *(UWORD *)ptr = (UWORD)((IPTR*)params)[i]; break;
            case (4): *(ULONG *)ptr = (ULONG)((IPTR*)params)[i]; break;
            case (8): *(UQUAD *)ptr = (UQUAD)((IPTR*)params)[i]; break;
        }
        ptr += dsize;
    }
#endif

    keep_app = app;
    keep_wnd = NULL;

    if (!keep_app)
    {
        struct EasyStruct es;
        es.es_StructSize   = sizeof(struct EasyStruct);
        es.es_Flags        = 0;
        es.es_Title        = title;
        es.es_TextFormat   = format;
        es.es_GadgetFormat = gadgets;
        return EasyRequestArgs(NULL,&es,NULL,params);
    }

    reqtxt_len = 0;
#ifdef __AROS__
    RawDoFmt(format,_params,(VOID_FUNC)AROS_ASMSYMNAME(len_func),&reqtxt_len);
#else
    RawDoFmt(format,params,(void(*)())&len_func,&reqtxt_len);
#endif

    /* Return cancel if something failed */
    if (!(reqtxt = AllocVec(reqtxt_len+1,0)))
        return 0;

#ifdef __AROS__
    {
        char *reqtxtptr = reqtxt;

        RawDoFmt(format, _params, (VOID_FUNC)AROS_ASMSYMNAME(cpy_func),
            &reqtxtptr);
    }
#else
    RawDoFmt(format, params, (void(*)())&cpy_func, reqtxt);
#endif
    
    if (title == NULL && keep_app != NULL)
    {
        title = (CONST_STRPTR) XGET((Object *)keep_app, MUIA_Application_Title);
    }

    req_wnd = WindowObject,
        MUIA_Window_Title,        title,
        MUIA_Window_RefWindow,    win,
        MUIA_Window_LeftEdge,     MUIV_Window_LeftEdge_Centered,
        MUIA_Window_TopEdge,      MUIV_Window_TopEdge_Centered,
        MUIA_Window_CloseGadget,  FALSE,
        MUIA_Window_SizeGadget,   FALSE,
        WindowContents, VGroup,
            MUIA_Background,       MUII_RequesterBack,
            Child, HGroup,
                TextFrame,
                MUIA_Background,    MUII_TextBack,
                Child, HSpace(0),
                Child, TextObject,
                    MUIA_InnerBottom,   8,
                    MUIA_InnerLeft,     8,
                    MUIA_InnerRight,    8,
                    MUIA_InnerTop,      8,
                    MUIA_Text_SetMax,   TRUE,
                    MUIA_Text_Contents, reqtxt,
                    End,
                Child, HSpace(0),
                End,
            Child, VSpace(2),
            Child, req_group = HGroup, End,
            End,
        End;

    FreeVec(reqtxt);

    result = 0;

    keep_wnd = req_wnd;

    if (keep_wnd != NULL)
    {
        char *gadgs = StrDup(gadgets);
        if (gadgs)
        {
            char *current = gadgs;
            int active = -1;
            int num_gads = 0;
            IPTR isopen = FALSE;

//            set(app, MUIA_Application_Sleep, TRUE);
            DoMethod((Object *)keep_app, OM_ADDMEMBER, (IPTR)keep_wnd);

            while(current)
            {
                char *next = strchr(current,'|');
                if (next) *next++ = 0;

                if (current[0] == '*')
                {
                    current++;
                    active = num_gads;
                }

                if (!(req_but[num_gads] = SimpleButton(current)))
                    break;
                num_gads++;
                current = next;
            }

            FreeVec(gadgs);
            DoMethod(req_group, MUIM_Group_InitChange);

            /* if this is only one button lets add it separatly */
            /*
             * Button ids sit above any application ReturnID.  Menus.c
             * uses 2 for About, and the old single-button id was also 2,
             * so OK fell through to the application's loop.  Closing
             * the requester then disposed the application.
             * 0x4D520000 + n is the same n the old code queued.
             */
            if (num_gads == 1)
            {
                DoMethod(req_group, OM_ADDMEMBER, (IPTR)HSpace(0));
                DoMethod(req_group, OM_ADDMEMBER, (IPTR)req_but[0]);
                DoMethod(req_group, OM_ADDMEMBER, (IPTR)HSpace(0));
                DoMethod(req_but[0], MUIM_Notify, MUIA_Pressed, FALSE,
                         (IPTR)keep_app, 2, MUIM_Application_ReturnID,
                         0x4D520000UL + 2);
            }
            else
            {
                int j;

                for(j = 0; j < num_gads; j++)
                {
                    ULONG bid;

                    if (j > 0)
                    {
                        DoMethod(req_group, OM_ADDMEMBER, (IPTR)HSpace(0));
                    }
                    if (j + 2 <= num_gads)
                        bid = (ULONG)(j + 2);
                    else
                        bid = 1;
                    DoMethod(req_group, OM_ADDMEMBER, (IPTR)req_but[j]);
                    DoMethod(req_but[j], MUIM_Notify, MUIA_Pressed, FALSE,
                             (IPTR)keep_app, 2, MUIM_Application_ReturnID,
                             0x4D520000UL + bid);
                    set(req_but[j], MUIA_CycleChain, 1);
                }
            }
            DoMethod((Object *)keep_wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                     (IPTR)keep_app, 2, MUIM_Application_ReturnID, 0x4D520000UL + 1);
            DoMethod(req_group, MUIM_Group_ExitChange);

            /* now activate that button with a starting "*" */
            if(active != -1) set((Object *)keep_wnd, MUIA_Window_ActiveObject, (IPTR) req_but[active]);

            /* lets collect the waiting returnIDs now */
            // COLLECT_RETURNIDS;

            set((Object *)keep_wnd, MUIA_Window_Open, TRUE);
            get((Object *)keep_wnd, MUIA_Window_Open, &isopen);

            if (isopen)
            {
                ULONG sigs = 0;
                result = -1;

                while (result == -1)
                {
                    ULONG ret;
                    ULONG button_id;

                    ret = DoMethod((Object *)keep_app, MUIM_Application_NewInput, (IPTR)&sigs);

                    /*
                     * Main-window close while the requester is up.
                     * Hand it back to the application after we return.
                     * One requeue, then leave: NewInput would otherwise
                     * return the same id forever.
                     */
                    if (ret == (ULONG)MUIV_Application_ReturnID_Quit)
                    {
                        DoMethod((Object *)keep_app, MUIM_Application_ReturnID, ret);
                        result = 0;
                        break;
                    }

                    /* Requester button or its own close gadget. */
                    if (ret >= 0x4D520000UL && ret < 0x4D520040UL)
                    {
                        button_id = ret - 0x4D520000UL;
                        if (button_id > 0 && button_id <= (ULONG)num_gads + 1)
                        {
                            result = (LONG)button_id - 1;
                            break;
                        }
                    }

                    if (sigs) sigs = Wait(sigs);
                }
            }

            /* now lets reissue the collected returnIDs again */
            // REISSUE_RETURNIDS;

//            set(app, MUIA_Application_Sleep, FALSE);
        }

        {
            /*
             * keep_wnd was stored before the input loop.  Reading req_wnd
             * here used to see the application pointer, and disposing
             * that freed the program while Apdf was still inside About.
             */
            Object *dispose_wnd;
            Class *wcl;
            int is_app;

            dispose_wnd = (Object *)keep_wnd;
            is_app = (dispose_wnd == (Object *)keep_app);
            wcl = NULL;
            if (!is_app && dispose_wnd != NULL)
            {
                wcl = OCLASS(dispose_wnd);
                if (wcl != NULL && wcl->cl_ID != NULL
                    && strcmp(wcl->cl_ID, "Application.mui") == 0)
                    is_app = 1;
            }
            if (dispose_wnd != NULL && !is_app)
            {
                set(dispose_wnd, MUIA_Window_Open, FALSE);
                DoMethod((Object *)keep_app, OM_REMMEMBER, (IPTR)dispose_wnd);
                MUI_DisposeObject(dispose_wnd);
            }
            keep_wnd = NULL;
            req_wnd = NULL;
        }
    }
    return result;

} /* MUI_RequestA */
