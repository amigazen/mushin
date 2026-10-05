/*
 * voyagertest.c - Stage Voyager's MUI startup path against muimaster/zune.
 *
 * Commercial Voyager and MUI prefs fail where the tutorial demos succeed.
 * This harness walks the same MUI steps Voyager uses (open, muigfx, MCC
 * probe, CreateCustomClass, ApplicationObject + Menustrip + window) and
 * prints where each stage dies.  It is not a browser; it only exercises
 * the MUI surface Voyager needs before HTML classes exist.
 *
 * Usage (same library naming rules as opentest):
 *   voyagertest
 *   voyagertest muimaster.library
 *   voyagertest zunemaster.library
 *
 * Optional second argument "nowin" skips the interactive NewInput loop.
 *
 * Build without gst=all.gst, against include/, like opentest.
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <dos/dos.h>
#include <intuition/classes.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <stdio.h>
#include <string.h>

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) \
    ((ULONG)(a)<<24 | (ULONG)(b)<<16 | (ULONG)(c)<<8 | (ULONG)(d))
#endif

struct Library *MUIMasterBase = NULL;
struct Library *MUIGfxBase = NULL;

static char *libname = MUIMASTER_NAME;
static int do_window_loop = 1;
static int stages_ok = 0;
static int stages_fail = 0;

/* Voyager AMIGAOS mcccheck list (init.c), min ver/rev. */
static struct {
    CONST_STRPTR name;
    LONG minver;
    LONG minrev;
} voyager_mccs[] = {
    { "Textinput.mcc", 29, 0 },
    { "Textinputscroll.mcc", 29, 0 },
    { "Listtree.mcc", 15, 0 },
    { "Busy.mcc", 16, 0 },
    { "SpeedBar.mcc", 11, 5 },
    { "SpeedButton.mcc", 11, 0 },
    { NULL, 0, 0 }
};

/* Classes Voyager puts in MUIA_Application_UsedClasses. */
static STRPTR used_classes[] = {
    "Listtree.mcc",
    "Textinput.mcc",
    "Textinputscroll.mcc",
    "Busy.mcc",
    "SpeedBar.mcc",
    "Pophotkey.mcc",
    "NListviews.mcc",
    NULL
};

enum {
    VT_MENU_FILE = 1,
    VT_MENU_QUIT
};

static struct NewMenu vt_menus[] = {
    { NM_TITLE, "VoyagerTest", 0, 0, 0, (APTR)VT_MENU_FILE },
    { NM_ITEM,  "Quit", "Q", 0, 0, (APTR)VT_MENU_QUIT },
    { NM_END,   NULL, 0, 0, 0, (APTR)0 }
};

/* Minimal custom-class instance data (Voyager HtmlWin / HtmlView style). */
struct VtData {
    LONG dummy;
};

#define VT_LOG(fmt) \
    do { \
        printf(fmt); \
        fflush(stdout); \
    } while (0)

#define VT_LOG1(fmt, a) \
    do { \
        printf(fmt, a); \
        fflush(stdout); \
    } while (0)

#define VT_LOG2(fmt, a, b) \
    do { \
        printf(fmt, a, b); \
        fflush(stdout); \
    } while (0)

#define VT_LOG3(fmt, a, b, c) \
    do { \
        printf(fmt, a, b, c); \
        fflush(stdout); \
    } while (0)

static void stage_result(CONST_STRPTR name, int ok)
{
    if (ok)
    {
        printf("[PASS] %s\n", name);
        stages_ok++;
    }
    else
    {
        printf("[FAIL] %s\n", name);
        stages_fail++;
    }
    fflush(stdout);
}

static LONG vt_xget(Object *obj, ULONG attr)
{
    LONG v;

    v = 0;
    get(obj, attr, &v);
    return v;
}

/*
 * SAS/C BOOPSI dispatcher entry for CreateCustomClass.  Voyager uses the
 * same register convention via its DISPATCHER macros.
 */
static ULONG __saveds __asm
VtDispatcher(register __a0 struct IClass *cl,
    register __a2 Object *obj,
    register __a1 Msg msg)
{
    return DoSuperMethodA(cl, obj, msg);
}

static int stage_open_muimaster(void)
{
    VT_LOG1("[1] OpenLibrary(\"%s\", 18)  /* Voyager open_muimaster */\n",
        libname);

    MUIMasterBase = OpenLibrary(libname, 18);
    if (MUIMasterBase == NULL)
    {
        VT_LOG("    OpenLibrary failed\n");
        stage_result("open muimaster V18", 0);
        return 0;
    }

    VT_LOG2("    base=%08lx  name=%s\n",
        (unsigned long)MUIMasterBase,
        MUIMasterBase->lib_Node.ln_Name ?
            MUIMasterBase->lib_Node.ln_Name : "(null)");
    VT_LOG2("    version=%ld  revision=%ld\n",
        (long)MUIMasterBase->lib_Version,
        (long)MUIMasterBase->lib_Revision);

    if (MUIMasterBase->lib_Version < 18)
    {
        VT_LOG("    version below Voyager minimum 18\n");
        stage_result("open muimaster V18", 0);
        return 0;
    }

    stage_result("open muimaster V18", 1);

    /* MUI 5 prefs asks for V20; report whether that open would succeed. */
    VT_LOG("[1b] Would OpenLibrary(..., 20) succeed for MUI5 prefs?\n");
    if (MUIMasterBase->lib_Version >= 20)
        VT_LOG("    yes (lib_Version >= 20)\n");
    else
        VT_LOG1("    NO - prefs would print Failed to open muimaster.library V20; have %ld\n",
            (long)MUIMasterBase->lib_Version);

    return 1;
}

static int stage_open_muigfx(void)
{
    VT_LOG("[2] OpenLibrary(\"muigfx.library\", 1)  /* Voyager companion */\n");

    MUIGfxBase = OpenLibrary("muigfx.library", 1);
    if (MUIGfxBase == NULL)
    {
        VT_LOG("    FAILED - Voyager open_muimaster() aborts here with EasyRequest\n");
        stage_result("open muigfx.library V1", 0);
        return 0;
    }

    VT_LOG3("    base=%08lx  version=%ld.%ld\n",
        (unsigned long)MUIGfxBase,
        (long)MUIGfxBase->lib_Version,
        (long)MUIGfxBase->lib_Revision);
    stage_result("open muigfx.library V1", 1);
    return 1;
}

static int try_new_mcc(CONST_STRPTR name, Object **out)
{
    Object *o;
    struct Library *raw;
    char path[160];

    *out = NULL;

    /*
     * Diagnose OpenLibrary separately from GetClass.  Classic MUI and the
     * open-source MCC trees install to LIBS:mui/Name.mcc and open as mui/%s.
     */
    strcpy(path, "mui/");
    strcat(path, name);
    raw = OpenLibrary(path, 0);
    VT_LOG2("    OpenLibrary(\"%s\") = %08lx\n", path, (unsigned long)raw);
    if (raw)
        CloseLibrary(raw);
    else
    {
        strcpy(path, "LIBS:mui/");
        strcat(path, name);
        raw = OpenLibrary(path, 0);
        VT_LOG2("    OpenLibrary(\"%s\") = %08lx\n", path, (unsigned long)raw);
        if (raw)
            CloseLibrary(raw);
    }

    o = MUI_NewObject(name, TAG_DONE);
    *out = o;
    return o != NULL;
}

static int stage_mcccheck(void)
{
    int i;
    int errors;
    Object *o;
    LONG ver;
    LONG rev;

    errors = 0;
    VT_LOG("[3] MCC check (Voyager mcccheck)\n");

    for (i = 0; voyager_mccs[i].name != NULL; i++)
    {
        VT_LOG3("    probe %s (need >= %ld.%ld)\n",
            voyager_mccs[i].name,
            (long)voyager_mccs[i].minver,
            (long)voyager_mccs[i].minrev);

        if (!try_new_mcc(voyager_mccs[i].name, &o))
        {
            VT_LOG("      MISSING - MUI_NewObject returned NULL\n");
            errors++;
            continue;
        }

        ver = vt_xget(o, MUIA_Version);
        rev = vt_xget(o, MUIA_Revision);
        VT_LOG2("      created ok, MUIA_Version/Revision = %ld.%ld\n",
            (long)ver, (long)rev);

        if (ver < voyager_mccs[i].minver
            || (ver == voyager_mccs[i].minver && rev < voyager_mccs[i].minrev))
        {
            VT_LOG("      TOO OLD\n");
            errors++;
        }
        else
            VT_LOG("      version OK\n");

        MUI_DisposeObject(o);
    }

    if (errors)
    {
        VT_LOG1("    Voyager would stop here (%ld MCC problem(s))\n",
            (long)errors);
        stage_result("mcccheck", 0);
        return 0;
    }

    stage_result("mcccheck", 1);
    return 1;
}

static int stage_createcustom(void)
{
    struct MUI_CustomClass *mcc_group;
    struct MUI_CustomClass *mcc_virt;
    struct MUI_CustomClass *mcc_area;
    struct MUI_CustomClass *mcc_win;
    struct MUI_CustomClass *mcc_pen;
    int ok;

    ok = 1;
    VT_LOG("[4] MUI_CreateCustomClass (Voyager HtmlWin/HtmlView/prefs-style)\n");

    mcc_group = MUI_CreateCustomClass(NULL, MUIC_Group, NULL,
        sizeof(struct VtData), (APTR)VtDispatcher);
    VT_LOG1("    Group subclass: %08lx\n", (unsigned long)mcc_group);
    if (mcc_group == NULL)
        ok = 0;

    mcc_virt = MUI_CreateCustomClass(NULL, MUIC_Virtgroup, NULL,
        sizeof(struct VtData), (APTR)VtDispatcher);
    VT_LOG1("    Virtgroup subclass: %08lx\n", (unsigned long)mcc_virt);
    if (mcc_virt == NULL)
        ok = 0;

    mcc_area = MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
        sizeof(struct VtData), (APTR)VtDispatcher);
    VT_LOG1("    Area subclass: %08lx\n", (unsigned long)mcc_area);
    if (mcc_area == NULL)
        ok = 0;

    mcc_win = MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
        sizeof(struct VtData), (APTR)VtDispatcher);
    VT_LOG1("    Window subclass: %08lx\n", (unsigned long)mcc_win);
    if (mcc_win == NULL)
        ok = 0;

    /* PSI / MUI prefs InitClasses first entry is Pendisplay. */
    mcc_pen = MUI_CreateCustomClass(NULL, MUIC_Pendisplay, NULL,
        sizeof(struct VtData), (APTR)VtDispatcher);
    VT_LOG1("    Pendisplay subclass (prefs InitClasses): %08lx\n",
        (unsigned long)mcc_pen);
    if (mcc_pen == NULL)
    {
        VT_LOG("    prefs would report \"Out of memory (20).\" here\n");
        ok = 0;
    }

    if (mcc_group)
        MUI_DeleteCustomClass(mcc_group);
    if (mcc_virt)
        MUI_DeleteCustomClass(mcc_virt);
    if (mcc_area)
        MUI_DeleteCustomClass(mcc_area);
    if (mcc_win)
        MUI_DeleteCustomClass(mcc_win);
    if (mcc_pen)
        MUI_DeleteCustomClass(mcc_pen);

    stage_result("CreateCustomClass supers", ok);
    return ok;
}

static int stage_makeobject_menustrip(Object **menu_out)
{
    Object *menu;

    *menu_out = NULL;
    VT_LOG("[5] MUI_MakeObject(MUIO_MenustripNM)  /* Voyager menus */\n");

    menu = MUI_MakeObject(MUIO_MenustripNM, (ULONG)vt_menus,
        MUIO_MenustripNM_CommandKeyCheck);
    VT_LOG1("    menustrip=%08lx\n", (unsigned long)menu);
    if (menu == NULL)
    {
        stage_result("MenustripNM", 0);
        return 0;
    }

    *menu_out = menu;
    stage_result("MenustripNM", 1);
    return 1;
}

static int stage_application(Object *menu, Object **app_out, Object **win_out)
{
    Object *app;
    Object *win;
    Object *root;
    Object *btn;
    Object *ti;
    LONG mui_err;

    *app_out = NULL;
    *win_out = NULL;

    VT_LOG("[6] ApplicationObject (Voyager buildapp tags)\n");
    VT_LOG("    SingleTask=TRUE Base=VOYAGER UsedClasses=...\n");

    /*
     * Build window contents first so a NULL child is obvious in the log
     * before it is swallowed by Application OM_NEW.
     */
    btn = SimpleButton("_Quit");
    VT_LOG1("    SimpleButton: %08lx\n", (unsigned long)btn);

    ti = MUI_NewObject("Textinput.mcc",
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Contents, (IPTR)"http://example/",
        TAG_DONE);
    if (ti == NULL)
        ti = StringObject,
            MUIA_Frame, MUIV_Frame_String,
            MUIA_String_Contents, (IPTR)"http://example/ (String fallback)",
            End;
    VT_LOG1("    URL field (Textinput or String): %08lx\n", (unsigned long)ti);

    root = VGroup,
        Child, TextObject,
            MUIA_Text_Contents, (IPTR)"\033cVoyagerTest - MUI stage harness",
            End,
        Child, ti,
        Child, btn,
        End;
    VT_LOG1("    VGroup root: %08lx\n", (unsigned long)root);

    win = WindowObject,
        MUIA_Window_Title, (IPTR)"VoyagerTest",
        MUIA_Window_ID, MAKE_ID('V','T','S','T'),
        WindowContents, root,
        End;
    VT_LOG1("    WindowObject: %08lx\n", (unsigned long)win);

    app = ApplicationObject,
        MUIA_Application_Title, (IPTR)"VoyagerTest",
        MUIA_Application_Version, (IPTR)"$VER: VoyagerTest 1.0 (13.09.26)",
        MUIA_Application_Copyright, (IPTR)"Kitsune / Zune harness",
        MUIA_Application_Author, (IPTR)"voyagertest",
        MUIA_Application_Description, (IPTR)"Voyager MUI startup probe",
        MUIA_Application_Base, (IPTR)"VOYAGER",
        MUIA_Application_SingleTask, TRUE,
        MUIA_Application_UsedClasses, (IPTR)used_classes,
        MUIA_Application_Menustrip, (IPTR)menu,
        MUIA_Application_Window, (IPTR)win,
        End;

    VT_LOG1("    ApplicationObject: %08lx\n", (unsigned long)app);
    mui_err = MUI_Error();
    VT_LOG1("    MUI_Error()=%ld  /* Zune stub is always 0; Voyager silent-exits */\n",
        (long)mui_err);

    if (app == NULL)
    {
        VT_LOG("    Voyager buildapp() returns FALSE here (often with no requester)\n");
        /* menu was consumed only on success; dispose leftovers if any */
        if (win)
            MUI_DisposeObject(win);
        else if (root)
            MUI_DisposeObject(root);
        stage_result("ApplicationObject", 0);
        return 0;
    }

    *app_out = app;
    *win_out = win;
    stage_result("ApplicationObject", 1);
    return 1;
}

static int stage_open_window(Object *app, Object *win)
{
    ULONG sigs;
    ULONG mid;
    LONG open;

    VT_LOG("[7] Open window + NewInput (optional)\n");

    if (win == NULL || app == NULL)
    {
        stage_result("window open", 0);
        return 0;
    }

    DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        (IPTR)app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    set(win, MUIA_Window_Open, TRUE);
    open = vt_xget(win, MUIA_Window_Open);
    VT_LOG1("    MUIA_Window_Open=%ld\n", (long)open);
    if (!open)
    {
        stage_result("window open", 0);
        return 0;
    }

    stage_result("window open", 1);

    if (!do_window_loop)
    {
        VT_LOG("    nowin: skipping NewInput loop\n");
        set(win, MUIA_Window_Open, FALSE);
        return 1;
    }

    VT_LOG("    NewInput until close gadget or Ctrl-C...\n");
    sigs = 0;
    while ((mid = DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs))
        != MUIV_Application_ReturnID_Quit)
    {
        if (sigs)
        {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if (sigs & SIGBREAKF_CTRL_C)
                break;
        }
    }

    set(win, MUIA_Window_Open, FALSE);
    return 1;
}

static void stage_builtin_spotcheck(void)
{
    Object *o;
    CONST_STRPTR names[] = {
        MUIC_Application,
        MUIC_Window,
        MUIC_Group,
        MUIC_Virtgroup,
        MUIC_Area,
        MUIC_List,
        /* Listview/Scrollgroup need Child tags; bare NewObject is expected to fail */
        MUIC_String,
        MUIC_Text,
        MUIC_Notify,
        MUIC_Menustrip,
        MUIC_Pendisplay,
        MUIC_Dataspace,
        MUIC_Scrollbar,
        NULL
    };
    int i;
    int bad;

    bad = 0;
    VT_LOG("[0] Builtin MUI_NewObject spot-check\n");
    for (i = 0; names[i] != NULL; i++)
    {
        o = MUI_NewObject(names[i], TAG_DONE);
        VT_LOG2("    %-18s %s\n", names[i], o ? "ok" : "FAIL");
        if (o)
            MUI_DisposeObject(o);
        else
            bad++;
    }
    stage_result("builtin spot-check", bad == 0);
}

int main(int argc, char **argv)
{
    Object *menu;
    Object *app;
    Object *win;
    int i;

    menu = NULL;
    app = NULL;
    win = NULL;

    printf("=== voyagertest: Voyager MUI startup harness ===\n");
    fflush(stdout);

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "nowin") == 0)
            do_window_loop = 0;
        else
            libname = argv[i];
    }

    VT_LOG1("Library under test: %s\n", libname);
    VT_LOG1("Compiled-in MUIMASTER_NAME: %s\n", MUIMASTER_NAME);

    if (!stage_open_muimaster())
        goto done;

    /* Continue after muigfx failure so later stages still run for diagnosis. */
    stage_open_muigfx();

    stage_builtin_spotcheck();
    stage_mcccheck();
    stage_createcustom();

    if (!stage_makeobject_menustrip(&menu))
        menu = NULL;

    if (stage_application(menu, &app, &win))
        stage_open_window(app, win);
    else if (menu)
    {
        /* Application failed; Menustrip was not adopted. */
        MUI_DisposeObject(menu);
        menu = NULL;
    }

    if (app)
    {
        MUI_DisposeObject(app);
        app = NULL;
        menu = NULL;
        win = NULL;
    }

done:
    printf("=== summary: %ld passed, %ld failed ===\n",
        (long)stages_ok, (long)stages_fail);
    fflush(stdout);

    if (MUIGfxBase)
    {
        CloseLibrary(MUIGfxBase);
        MUIGfxBase = NULL;
    }
    if (MUIMasterBase)
    {
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
    }

    return stages_fail ? RETURN_FAIL : RETURN_OK;
}
