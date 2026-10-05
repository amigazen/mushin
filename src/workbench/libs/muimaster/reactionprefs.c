/*
 * Read classic Amiga ReAction prefs (ENV:sys/reaction.prefs) and map the
 * styles that have a clear MUI/Zune counterpart.
 *
 * On-disk RACT layout matches NDK <prefs/reaction.h> field order, but the
 * chunk is written without C structure padding: after the nine UWORD/BOOL
 * fields, the two TextAttr records are packed immediately (unaligned), then
 * the two FONTNAMESIZE name arrays and the pattern.  Do not memcpy the chunk
 * into a native struct ReactionPrefs.
 */

#include <string.h>
#include <stdio.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <prefs/prefhdr.h>
#include <prefs/reaction.h>
#include <intuition/screens.h>
#include <graphics/text.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>

#include "mui.h"
#include "prefs.h"
#include "frame.h"
#include "reactionprefs.h"

/*  #define MYDEBUG 1 */
#include "debug.h"

/* Bevel/glyph constants from <reaction/reaction_prefs.h> */
#ifndef BVT_GT
#define BVT_GT      0
#define BVT_THIN    1
#define BVT_THICK   2
#define BVT_XEN     3
#define BVT_XENTHIN 4
#endif

/* Packed on-disk offsets inside the RACT chunk body */
#define RACT_OFF_BEVEL        0
#define RACT_OFF_GLYPH        2
#define RACT_OFF_SPACING      4
#define RACT_OFF_3DPROP       6
#define RACT_OFF_LABELPEN     8
#define RACT_OFF_LABELPLACE  10
#define RACT_OFF_3DLABEL     12
#define RACT_OFF_SIMPLEREF   14
#define RACT_OFF_3DLOOK      16
#define RACT_OFF_FALLBACK_TA 18   /* TextAttr, 8 bytes, may be unaligned */
#define RACT_OFF_LABEL_TA    26
#define RACT_OFF_FALLBACK_NM 34
#define RACT_OFF_LABEL_NM    (34 + FONTNAMESIZE)
#define RACT_OFF_PATTERN     (34 + FONTNAMESIZE + FONTNAMESIZE)
#define RACT_MIN_SIZE        (RACT_OFF_LABEL_NM + FONTNAMESIZE)

struct ReactionPrefsDisk
{
    UWORD bevel;
    UWORD glyph;
    UWORD spacing;
    BOOL  prop3d;
    UWORD labelpen;
    UWORD labelplace;
    BOOL  label3d;
    BOOL  simplerefresh;
    BOOL  look3d;
    UWORD fallback_ysize;
    UWORD label_ysize;
    UBYTE fallback_name[FONTNAMESIZE];
    UBYTE label_name[FONTNAMESIZE];
};

extern struct Library *MUIMasterBase;
extern struct Library *IFFParseBase;

static UWORD ract_be16(UBYTE *p)
{
    return (UWORD) ((((UWORD) p[0]) << 8) | (UWORD) p[1]);
}

static BOOL ract_bool(UBYTE *p)
{
    return (BOOL) (ract_be16(p) != 0);
}

static void set_frame(struct ZunePrefsNew *prefs, LONG id, CONST_STRPTR spec)
{
    if (id < 0 || id >= MUIV_Frame_Count)
        return;
    zune_frame_spec_to_intern(spec, &prefs->frames[id]);
}

static void apply_bevel_frames(struct ZunePrefsNew *prefs, UWORD bevel,
    BOOL look3d)
{
    CONST_STRPTR button;
    CONST_STRPTR stringf;
    CONST_STRPTR groupf;
    CONST_STRPTR textf;
    CONST_STRPTR propf;

    /*
     * framespec: type, recessed, L, R, U, D (see DefFramespecValues).
     * Map ReAction bevel families onto the closest builtin FST_*.
     */
    if (!look3d)
        bevel = BVT_THIN;

    switch (bevel)
    {
    case BVT_THIN:
        button = "302111";
        stringf = "302111";
        groupf = "302222";
        textf = "302111";
        propf = "302111";
        break;
    case BVT_THICK:
        button = "402211";
        stringf = "402211";
        groupf = "404444";
        textf = "402211";
        propf = "402211";
        break;
    case BVT_XEN:
        button = "502211";
        stringf = "502211";
        groupf = "504444";
        textf = "502211";
        propf = "502211";
        break;
    case BVT_XENTHIN:
        button = "802111";
        stringf = "802111";
        groupf = "802222";
        textf = "802111";
        propf = "802111";
        break;
    case BVT_GT:
    default:
        button = "202211";
        stringf = "302211";
        groupf = "314444";
        textf = "212211";
        propf = "202211";
        break;
    }

    set_frame(prefs, MUIV_Frame_Button, button);
    set_frame(prefs, MUIV_Frame_ImageButton, button);
    set_frame(prefs, MUIV_Frame_String, stringf);
    set_frame(prefs, MUIV_Frame_Text, textf);
    set_frame(prefs, MUIV_Frame_ReadList, textf);
    set_frame(prefs, MUIV_Frame_InputList, button);
    set_frame(prefs, MUIV_Frame_Prop, propf);
    set_frame(prefs, MUIV_Frame_Group, groupf);
    set_frame(prefs, MUIV_Frame_Virtual, textf);
    set_frame(prefs, MUIV_Frame_PopUp, "112211");
    set_frame(prefs, MUIV_Frame_Knob, button);
    set_frame(prefs, MUIV_Frame_Slider, "400000");
}

static STRPTR dup_font_name(UBYTE *name)
{
    STRPTR copy;
    ULONG len;

    if (name == NULL || name[0] == '\0')
        return NULL;
    len = strlen((char *) name) + 1;
    copy = AllocVec(len, MEMF_ANY);
    if (copy)
        strcpy(copy, (char *) name);
    return copy;
}

static BOOL parse_ract_chunk(UBYTE *buf, LONG size,
    struct ReactionPrefsDisk *out)
{
    if (buf == NULL || out == NULL || size < RACT_MIN_SIZE)
        return FALSE;

    memset(out, 0, sizeof(*out));
    out->bevel = ract_be16(buf + RACT_OFF_BEVEL);
    out->glyph = ract_be16(buf + RACT_OFF_GLYPH);
    out->spacing = ract_be16(buf + RACT_OFF_SPACING);
    out->prop3d = ract_bool(buf + RACT_OFF_3DPROP);
    out->labelpen = ract_be16(buf + RACT_OFF_LABELPEN);
    out->labelplace = ract_be16(buf + RACT_OFF_LABELPLACE);
    out->label3d = ract_bool(buf + RACT_OFF_3DLABEL);
    out->simplerefresh = ract_bool(buf + RACT_OFF_SIMPLEREF);
    out->look3d = ract_bool(buf + RACT_OFF_3DLOOK);
    out->fallback_ysize = ract_be16(buf + RACT_OFF_FALLBACK_TA + 4);
    out->label_ysize = ract_be16(buf + RACT_OFF_LABEL_TA + 4);

    memcpy(out->fallback_name, buf + RACT_OFF_FALLBACK_NM, FONTNAMESIZE);
    memcpy(out->label_name, buf + RACT_OFF_LABEL_NM, FONTNAMESIZE);
    out->fallback_name[FONTNAMESIZE - 1] = '\0';
    out->label_name[FONTNAMESIZE - 1] = '\0';
    return TRUE;
}

static BOOL read_reaction_file(CONST_STRPTR path, struct ReactionPrefsDisk *out)
{
    struct IFFHandle *iff;
    BOOL ok;
    UBYTE *buf;
    LONG size;

    ok = FALSE;
    buf = NULL;
    iff = AllocIFF();
    if (iff == NULL)
        return FALSE;

    iff->iff_Stream = (IPTR) Open(path, MODE_OLDFILE);
    if (iff->iff_Stream == 0)
    {
        FreeIFF(iff);
        return FALSE;
    }

    InitIFFasDOS(iff);
    if (OpenIFF(iff, IFFF_READ) == 0)
    {
        StopChunk(iff, ID_PREF, ID_RACT);
        while (ParseIFF(iff, IFFPARSE_SCAN) == 0)
        {
            struct ContextNode *cn;

            cn = CurrentChunk(iff);
            if (cn == NULL)
                continue;
            if (cn->cn_ID == ID_RACT && cn->cn_Size >= RACT_MIN_SIZE)
            {
                size = cn->cn_Size;
                buf = AllocVec(size, MEMF_ANY);
                if (buf != NULL
                    && ReadChunkBytes(iff, buf, size) == size)
                {
                    ok = parse_ract_chunk(buf, size, out);
                }
                if (buf != NULL)
                    FreeVec(buf);
                break;
            }
        }
        CloseIFF(iff);
    }

    Close((BPTR) iff->iff_Stream);
    FreeIFF(iff);
    return ok;
}

BOOL Zune_ApplyReactionPrefs(struct ZunePrefsNew *prefs,
    STRPTR *font_normal, STRPTR *font_button)
{
    struct ReactionPrefsDisk rp;
    UWORD spacing;
    STRPTR fn;
    STRPTR fl;

    if (prefs == NULL)
        return FALSE;

    memset(&rp, 0, sizeof(rp));
    if (!read_reaction_file(ZUNE_REACTION_PREFS_ENV, &rp))
    {
        if (!read_reaction_file(ZUNE_REACTION_PREFS_ENVARC, &rp))
            return FALSE;
    }

    D(bug("zune: reaction.prefs bevel=%ld glyph=%ld space=%ld "
        "3dprop=%ld labelpen=%ld 3dlabel=%ld refresh=%ld 3dlook=%ld\n",
        (LONG) rp.bevel, (LONG) rp.glyph,
        (LONG) rp.spacing, (LONG) rp.prop3d,
        (LONG) rp.labelpen, (LONG) rp.label3d,
        (LONG) rp.simplerefresh, (LONG) rp.look3d));

    apply_bevel_frames(prefs, rp.bevel, rp.look3d);

    spacing = rp.spacing;
    if (spacing < 1)
        spacing = 1;
    if (spacing > 16)
        spacing = 16;
    prefs->group_hspacing = (WORD) (spacing * 2);
    prefs->group_vspacing = (WORD) spacing;
    prefs->window_inner_left = (WORD) (spacing + 2);
    prefs->window_inner_right = (WORD) (spacing + 2);
    prefs->window_inner_top = (WORD) (spacing + 1);
    prefs->window_inner_bottom = (WORD) (spacing + 1);
    prefs->radiobutton_hspacing = (WORD) (spacing + 2);
    prefs->radiobutton_vspacing = (WORD) spacing;

    if (rp.prop3d)
        prefs->scrollbar_type = SCROLLBAR_TYPE_NEWLOOK;
    else
        prefs->scrollbar_type = SCROLLBAR_TYPE_STANDARD;

    if (rp.simplerefresh)
        prefs->window_refresh = WINDOW_REFRESH_SIMPLE;
    else
        prefs->window_refresh = WINDOW_REFRESH_SMART;

    if (rp.label3d)
        prefs->group_title_color = GROUP_TITLE_COLOR_3D;
    else if (rp.labelpen == SHINEPEN || rp.labelpen == HIGHLIGHTTEXTPEN)
        prefs->group_title_color = GROUP_TITLE_COLOR_HILITE;
    else
        prefs->group_title_color = GROUP_TITLE_COLOR_STANDARD;

    /* ClassAct-style place: 2 = above gadget. */
    if (rp.labelplace == 2)
        prefs->group_title_position = GROUP_TITLE_POSITION_ABOVE;
    else
        prefs->group_title_position = GROUP_TITLE_POSITION_CENTERED;

    if (rp.look3d)
        prefs->register_look = REGISTER_LOOK_TRADITIONAL;
    else
        prefs->register_look = REGISTER_LOOK_GADTOOLS;

    fn = dup_font_name(rp.fallback_name);
    fl = dup_font_name(rp.label_name);
    if (fn)
    {
        if (font_normal && *font_normal)
            FreeVec(*font_normal);
        if (font_normal)
            *font_normal = fn;
        prefs->fonts[-MUIV_Font_Normal] = fn;
        prefs->fonts[-MUIV_Font_List] = fn;
        prefs->fonts[-MUIV_Font_Tiny] = fn;
        prefs->fonts[-MUIV_Font_Fixed] = fn;
    }
    if (fl)
    {
        if (font_button && *font_button)
            FreeVec(*font_button);
        if (font_button)
            *font_button = fl;
        prefs->fonts[-MUIV_Font_Button] = fl;
        prefs->fonts[-MUIV_Font_Title] = fl;
        prefs->fonts[-MUIV_Font_Big] = fl;
        prefs->fonts[-MUIV_Font_Knob] = fl;
    }

    (void) rp.glyph;
    (void) rp.fallback_ysize;
    (void) rp.label_ysize;

    return TRUE;
}
