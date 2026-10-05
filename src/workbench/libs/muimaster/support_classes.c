/*
    Copyright (C) 2002-2020, The AROS Development Team. All rights reserved.
*/

#include <string.h>
#include <stdio.h>

#include <intuition/classes.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>


#include "mui.h"
#include "support.h"
#include "support_classes.h"
#include "muimaster_intern.h"

/*#define MYDEBUG*/
#include "debug.h"

static const struct __MUIBuiltinClass *const builtins[] = {
    &_MUI_Notify_desc,
    &_MUI_Family_desc,
    &_MUI_Application_desc,
    &_MUI_Window_desc,
    &_MUI_Area_desc,
    &_MUI_Rectangle_desc,
    &_MUI_Group_desc,
    &_MUI_Image_desc,
    &_MUI_Configdata_desc,
    &_MUI_Text_desc,
    &_MUI_Numeric_desc,
    &_MUI_Slider_desc,
    &_MUI_String_desc,
    ZUNE_BOOPSI_DESC & _MUI_Prop_desc,
    &_MUI_Scrollbar_desc,
    &_MUI_Register_desc,
    &_MUI_Menuitem_desc,
    &_MUI_Menu_desc,
    &_MUI_Menustrip_desc,
    ZUNE_VIRTGROUP_DESC ZUNE_SCROLLGROUP_DESC & _MUI_Scrollbutton_desc,
    &_MUI_Semaphore_desc,
    &_MUI_Dataspace_desc,
    &_MUI_Bitmap_desc,
    &_MUI_Bodychunk_desc,
    &_MUI_ChunkyImage_desc,
    &_MUI_Cycle_desc,
    &_MUI_Popstring_desc,
    &_MUI_Listview_desc,
    &_MUI_List_desc,
    ZUNE_POPASL_DESC & _MUI_Popobject_desc,
    ZUNE_GAUGE_DESC
        ZUNE_FLOATTEXT_DESC
        ZUNE_ABOUTMUI_DESC
        ZUNE_SETTINGSGROUP_DESC
        ZUNE_IMAGEADJUST_DESC
        ZUNE_POPIMAGE_DESC
        ZUNE_SCALE_DESC
        ZUNE_RADIO_DESC
        ZUNE_ICONLISTVIEW_DESC
        ZUNE_BALANCE_DESC
        ZUNE_COLORFIELD_DESC
        ZUNE_COLORADJUST_DESC
        ZUNE_IMAGEDISPLAY_DESC
        ZUNE_PENDISPLAY_DESC
        ZUNE_PENADJUST_DESC ZUNE_POPPEN_DESC & _MUI_Mccprefs_desc,
    ZUNE_FRAMEDISPLAY_DESC
        ZUNE_POPFRAME_DESC
        ZUNE_FRAMEADJUST_DESC
        ZUNE_VOLUMELIST_DESC
        ZUNE_DIRLIST_DESC
        ZUNE_NUMERICBUTTON_DESC
        ZUNE_POPLIST_DESC
        ZUNE_CRAWLING_DESC
        ZUNE_POPSCREEN_DESC
        ZUNE_LEVELMETER_DESC
        ZUNE_KNOB_DESC
        ZUNE_DTPIC_DESC
        ZUNE_PALETTE_DESC
        /*
         * Settings is a documented MUI class and classes/settings.o has always
         * been compiled and linked, but its descriptor was never listed here,
         * so MUI_NewObject("Settings.mui", ...) could not find it.  AROS
         * upstream has the same omission.
         *
         * ZUNE_PANEL_DESC, ZUNE_PANELGROUP_DESC, ZUNE_DRAGHANDLE_DESC and
         * ZUNE_PANELTITLE_DESC used to follow.  Those four classes exist only
         * on AROS and one of them took the class name of an unrelated real MUI
         * class; see the note in mui.h.
         */
        ZUNE_SETTINGS_DESC
};

Class *ZUNE_GetExternalClass(ClassID classname,
    struct Library *MUIMasterBase)
{
    struct Library *mcclib;
    struct MUI_CustomClass *mcc;
    CONST_STRPTR const *pathptr;
    TEXT s[255];

    /*
     * Classic MUI loads externals with OpenLibrary("mui/%s", 0) into
     * LIBS:mui/Name.mcc (MUImaster.doc).  Open-source MCCs (Textinput,
     * BetterString, NList, TextEditor, TheBar, HTMLview) install there and
     * open muimaster themselves in LibInit to MUI_CreateCustomClass before
     * exporting via MCC_Query LVO -30.
     *
     * AROS paths stay for Classes/Zune installs.  "MUI/%s" kept after "mui/%s"
     * for odd case-sensitive assigns.
     */
    static CONST_STRPTR const searchpaths[] = {
        "mui/%s",
        "MUI/%s",
        "Zune/%s",
        "Classes/Zune/%s",
        NULL,
    };

    mcclib = NULL;
    mcc = NULL;

    if (classname == NULL || classname[0] == '\0')
        return NULL;

    ZuneTrace("zune: GetExternalClass \"%s\"\n", classname);

    for (pathptr = searchpaths; *pathptr; pathptr++)
    {
        snprintf(s, 255, *pathptr, classname);

        ZuneTrace("zune: OpenLibrary(\"%s\")\n", s);
        mcclib = OpenLibrary(s, 0);
        if (!mcclib)
            continue;

        ZuneTrace("zune: MCC open ok base=%lx, MCC_Query(0)\n",
            (ULONG) mcclib);

        mcc = MCC_Query(0);
        if (!mcc)
        {
            ZuneTrace("zune: MCC_Query(0) null, try Query(1)\n");
            mcc = MCC_Query(1);     /* MCP? */
        }

        if (mcc && mcc->mcc_Class)
        {
            /*
             * Track GetClass refs in cl_UserData (same field builtins use).
             * FreeClass still CloseLibrarys every time to balance OpenLibrary;
             * OpenCnt is what keeps the MCC segment alive while
             * CreateCustomClass holds the class as mcc_Super.
             */
            mcc->mcc_Module = mcclib;
            mcc->mcc_Class->cl_UserData++;
            ZuneTrace("zune: external class ok \"%s\" cl=%lx mcc=%lx ud=%ld\n",
                classname, (ULONG) mcc->mcc_Class, (ULONG) mcc,
                mcc->mcc_Class->cl_UserData);
            return mcc->mcc_Class;
        }

        ZuneTrace("zune: MCC_Query failed for \"%s\" (CreateCustomClass in MCC LibInit?)\n",
            classname);
        CloseLibrary(mcclib);
        mcclib = NULL;
        mcc = NULL;
    }

    ZuneTrace("zune: GetExternalClass failed \"%s\"\n", classname);
    return NULL;
}

/**************************************************************************/
static Class *ZUNE_FindBuiltinClass(ClassID classid, struct Library *MUIMasterBase)
{
    struct MUIMasterBase_intern *intZuneBase = (struct MUIMasterBase_intern *)MUIMasterBase;
    Class *cl = NULL, *cl2;

    ForeachNode(&intZuneBase->BuiltinClasses, cl2)
    {
        if (!strcmp(cl2->cl_ID, classid))
        {
            cl = cl2;
            break;
        }
    }

    return cl;
}

static Class *ZUNE_MakeBuiltinClass(ClassID classid,
    struct Library *MUIMasterBase)
{
    int i;
    Class *cl;

    cl = NULL;

    D(bug("Makeing Builtinclass %s\n", classid));

    for (i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
    {
        if (!strcmp(builtins[i]->name, classid))
        {
            Class *supercl;
            ClassID superclid;

            /*
             * Do not OpenLibrary() ourselves here.  Each successful MakeClass
             * used to self-open muimaster and never CloseLibrary until
             * Expunge, so lib_OpenCnt stuck at ~20+ and LibExpunge never
             * ran - a rebuilt muimaster.library stayed resident until reboot.
             * Real clients already hold OpenCnt via their OpenLibrary; when
             * the last client closes, Expunge tears down BuiltinClasses.
             */

            if (strcmp(builtins[i]->supername, ROOTCLASS) == 0)
            {
                superclid = ROOTCLASS;
                supercl = NULL;
            }
            else
            {
                superclid = NULL;
                /* Stack-safe; do not call MUI_GetClass LVO from here. */
                supercl = ZUNE_GetBuiltinClass(builtins[i]->supername,
                    MUIMasterBase);

                if (!supercl)
                    break;
            }

            cl = MakeClass(builtins[i]->name, superclid, supercl,
                builtins[i]->datasize, 0);
            if (cl)
            {
#if defined(__MAXON__) || defined(__amigaos4__)
                cl->cl_Dispatcher.h_Entry = builtins[i]->dispatcher;
#else
                cl->cl_Dispatcher.h_Entry = (HOOKFUNC) metaDispatcher;
                cl->cl_Dispatcher.h_SubEntry = builtins[i]->dispatcher;
#endif
                /*
                 * h_Data is what metaDispatcher loads into A6 before entering
                 * the real dispatcher, which is how MUI tells a class where
                 * its owning library base is - MUI_CreateCustomClass() stores
                 * the MCC's own base here for exactly that reason.  For a
                 * builtin class the owning library is this one.
                 *
                 * A reference count used to be kept here instead.  That left
                 * A6 holding 0 (or a small integer) on entry to every builtin
                 * dispatcher, and with SCOPTIONS selecting LIBRARYCODE a
                 * __saveds function derives A4 from A6, so every dispatcher
                 * ran with a bogus near-data base.  LIBRARYCODE has since been
                 * removed as well, but A6 still has to be right for any
                 * third-party class reached through this same path.
                 */
                cl->cl_Dispatcher.h_Data = MUIMasterBase;

                /* MakeClass() leaves cl_UserData ("application specific")
                   alone and nothing else in the library uses it, so the
                   reference count lives there now. */
                cl->cl_UserData = 0;
            }

            break;
        }
    }

    return cl;
}

Class *ZUNE_GetBuiltinClass(ClassID classid, struct Library * mb)
{
    Class *cl;

    ObtainSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

    cl = ZUNE_FindBuiltinClass(classid, mb);

    if (!cl)
    {
        cl = ZUNE_MakeBuiltinClass(classid, mb);

        if (cl)
            ZUNE_AddBuiltinClass(cl, mb);
    }

    /*
     * Counted on every successful lookup, not only when the class is created.
     * MUI_FreeClass() decrements once per MUI_GetClass(), so counting only
     * creations let the count reach zero - and the class be freed - while
     * other callers still held it.  The second and later MUI_GetClass() calls
     * for the same builtin class are the common case, since every class here
     * is shared by every application that uses it.
     */
    if (cl)
        cl->cl_UserData++;

    ReleaseSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

    return cl;
}

/*
 * metaDispatcher - puts h_Data in A6 and calls real dispatcher
 */

#ifdef __AROS__
AROS_UFH3(IPTR, metaDispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
    AROS_USERFUNC_INIT

    return AROS_UFC4(IPTR, cl->cl_Dispatcher.h_SubEntry,
        AROS_UFPA(Class *, cl, A0),
        AROS_UFPA(Object *, obj, A2),
        AROS_UFPA(Msg, msg, A1),
        AROS_UFPA(APTR, cl->cl_Dispatcher.h_Data, A6));

    AROS_USERFUNC_EXIT
}

#else
#ifdef __SASC
/*
 * A6 is declared as a fourth register parameter of the dispatcher rather than
 * being poked in with putreg() beforehand.  putreg() only guarantees A6 at
 * that instant; the compiler is free to use A6 while evaluating the call, and
 * nothing in the function's signature stopped it.  Declaring it makes the
 * compiler responsible for loading A6 immediately before the jsr, which is
 * also exactly what the AROS branch above expresses with AROS_UFPA(..., A6).
 *
 * Dispatchers declared with BOOPSI_DISPATCHER only name three parameters and
 * simply ignore the fourth, which is harmless.
 */
__asm ULONG metaDispatcher(register __a0 struct IClass * cl,
    register __a2 Object * obj, register __a1 Msg msg)
{
    __asm ULONG(*entry) (register __a0 struct IClass * cl,
        register __a2 Object * obj, register __a1 Msg msg,
        register __a6 APTR base) =
        (__asm ULONG(*)(register __a0 struct IClass *,
            register __a2 Object *,
            register __a1 Msg,
            register __a6 APTR))cl->cl_Dispatcher.h_SubEntry;

    return entry(cl, obj, msg, cl->cl_Dispatcher.h_Data);
}
#endif
#endif
