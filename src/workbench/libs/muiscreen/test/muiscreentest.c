/*
 * muiscreentest.c - Dedicated API harness for muiscreen.library
 *
 * Opens LIBS:muiscreen.library (or argv[1]) and exercises every public LVO:
 *   Alloc/FreePubScreenDesc, Open/ClosePubFile, Read/WritePubFile,
 *   Open/ClosePubScreen, Add/RemInfoClient.
 *
 * Screen tests open a BEHIND public screen named "MUISTest" and always
 * close it again.  IFF round-trips go through T: so they stay off ENV:.
 *
 * Build:  smake muiscreentest
 * Run:    muiscreentest
 *         muiscreentest muiscreen.library
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <dos/dos.h>
#include <intuition/screens.h>
#include <libraries/muiscreen.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muiscreen.h>
#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>

struct Library *MUIScreenBase;
struct IntuitionBase *IntuitionBase;

static char *libname = "muiscreen.library";
static int tests_passed;
static int tests_failed;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("  FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while (0)

#define IFF_TESTFILE "T:muiscreen_apitest.iff"
#define TEST_SCRNAME "MUISTest"

/****************************************************************************************/

static void print_desc(CONST_STRPTR label, struct MUI_PubScreenDesc *d)
{
    if (d == NULL)
    {
        printf("  %s: (null)\n", label);
        return;
    }
    printf("  %s: name=\"%s\" title=\"%s\" %ldx%ldx%ld id=%08lx "
           "auto=%ld behind=%ld sysdef=%ld\n",
           label,
           d->Name,
           d->Title,
           (long)d->DisplayWidth,
           (long)d->DisplayHeight,
           (long)d->DisplayDepth,
           (unsigned long)d->DisplayID,
           (long)d->AutoScroll,
           (long)d->Behind,
           (long)d->SysDefault);
}

static int descs_equal_core(struct MUI_PubScreenDesc *a, struct MUI_PubScreenDesc *b)
{
    if (a == NULL || b == NULL)
        return 0;
    if (strcmp(a->Name, b->Name) != 0)
        return 0;
    if (strcmp(a->Title, b->Title) != 0)
        return 0;
    if (a->DisplayID != b->DisplayID)
        return 0;
    if (a->DisplayWidth != b->DisplayWidth)
        return 0;
    if (a->DisplayHeight != b->DisplayHeight)
        return 0;
    if (a->DisplayDepth != b->DisplayDepth)
        return 0;
    if (a->OverscanType != b->OverscanType)
        return 0;
    if (a->AutoScroll != b->AutoScroll)
        return 0;
    if (a->NoDrag != b->NoDrag)
        return 0;
    if (a->Exclusive != b->Exclusive)
        return 0;
    if (a->Interleaved != b->Interleaved)
        return 0;
    if (a->Behind != b->Behind)
        return 0;
    if (a->AutoClose != b->AutoClose)
        return 0;
    if (memcmp(a->SystemPens, b->SystemPens, PSD_NUMSYSPENS) != 0)
        return 0;
    if (memcmp(a->Palette, b->Palette, sizeof(a->Palette)) != 0)
        return 0;
    return 1;
}

/****************************************************************************************/

static void test_open_library(void)
{
    printf("\n== OpenLibrary ==\n");

    MUIScreenBase = OpenLibrary(libname, 0);
    TEST_ASSERT(MUIScreenBase != NULL, "OpenLibrary succeeded");
    if (MUIScreenBase == NULL)
        return;

    printf("  opened %s V%ld.%ld @ %08lx openCnt=%ld\n",
           MUIScreenBase->lib_Node.ln_Name,
           (long)MUIScreenBase->lib_Version,
           (long)MUIScreenBase->lib_Revision,
           (unsigned long)MUIScreenBase,
           (long)MUIScreenBase->lib_OpenCnt);

    TEST_ASSERT(MUIScreenBase->lib_Node.ln_Type == NT_LIBRARY, "ln_Type is NT_LIBRARY");
    TEST_ASSERT(MUIScreenBase->lib_Version >= 1, "lib_Version >= 1");
    TEST_ASSERT(MUIScreenBase->lib_OpenCnt >= 1, "lib_OpenCnt >= 1");
}

static void test_alloc_free(void)
{
    struct MUI_PubScreenDesc *d1;
    struct MUI_PubScreenDesc *d2;
    struct MUI_PubScreenDesc *d3;
    BOOL freed;

    printf("\n== Alloc / FreePubScreenDesc ==\n");

    d1 = MUIS_AllocPubScreenDesc(NULL);
    TEST_ASSERT(d1 != NULL, "AllocPubScreenDesc(NULL) returns a desc");
    if (d1 == NULL)
        return;

    print_desc("default", d1);
    TEST_ASSERT(d1->Name[0] != '\0', "default Name is non-empty");
    TEST_ASSERT(d1->Title[0] != '\0', "default Title is non-empty");
    TEST_ASSERT(d1->DisplayWidth > 0, "default DisplayWidth > 0");
    TEST_ASSERT(d1->DisplayHeight > 0, "default DisplayHeight > 0");
    TEST_ASSERT(d1->DisplayDepth > 0, "default DisplayDepth > 0");

    strcpy(d1->Name, "CopySrc");
    strcpy(d1->Title, "Copy Source Title");
    d1->Behind = TRUE;
    d1->AutoClose = FALSE;
    d1->SysDefault = FALSE;
    d1->Changed = 42;

    d2 = MUIS_AllocPubScreenDesc(d1);
    TEST_ASSERT(d2 != NULL, "AllocPubScreenDesc(src) returns a copy");
    if (d2)
    {
        TEST_ASSERT(d2 != d1, "copy is a distinct allocation");
        TEST_ASSERT(descs_equal_core(d1, d2), "copy matches source core fields");
        TEST_ASSERT(d2->Changed == 42, "copy preserves Changed");
        print_desc("copy", d2);
    }

    /* Clone-of-NULL path already covered; free both. */
    freed = MUIS_FreePubScreenDesc(d1);
    TEST_ASSERT(freed == TRUE, "FreePubScreenDesc(d1) returns TRUE");
    if (d2)
    {
        freed = MUIS_FreePubScreenDesc(d2);
        TEST_ASSERT(freed == TRUE, "FreePubScreenDesc(d2) returns TRUE");
    }

    /* Freeing NULL should not crash; return value is unspecified but TRUE here. */
    freed = MUIS_FreePubScreenDesc(NULL);
    TEST_ASSERT(freed == TRUE, "FreePubScreenDesc(NULL) returns TRUE");

    d3 = MUIS_AllocPubScreenDesc(NULL);
    TEST_ASSERT(d3 != NULL, "Alloc after Free still works");
    if (d3)
        MUIS_FreePubScreenDesc(d3);
}

static void test_iff_roundtrip(void)
{
    struct MUI_PubScreenDesc *src;
    struct MUI_PubScreenDesc *got;
    struct MUI_PubScreenDesc *extra;
    APTR pf;
    BOOL ok;
    BPTR lock;

    printf("\n== Open/Write/Read/ClosePubFile ==\n");

    DeleteFile(IFF_TESTFILE);

    src = MUIS_AllocPubScreenDesc(NULL);
    TEST_ASSERT(src != NULL, "alloc src for IFF test");
    if (src == NULL)
        return;

    strcpy(src->Name, "IffRound");
    strcpy(src->Title, "IFF Roundtrip Screen");
    src->Behind = TRUE;
    src->AutoClose = FALSE;
    src->SysDefault = FALSE;
    src->NoDrag = TRUE;
    src->Changed = 7;

    pf = MUIS_OpenPubFile(IFF_TESTFILE, MODE_NEWFILE);
    TEST_ASSERT(pf != NULL, "OpenPubFile(MODE_NEWFILE)");
    if (pf == NULL)
    {
        MUIS_FreePubScreenDesc(src);
        return;
    }

    ok = MUIS_WritePubFile(pf, src);
    TEST_ASSERT(ok == TRUE, "WritePubFile(src)");

    /* Second write into the same FORM should also succeed. */
    strcpy(src->Name, "IffRound2");
    ok = MUIS_WritePubFile(pf, src);
    TEST_ASSERT(ok == TRUE, "WritePubFile(second desc)");

    MUIS_ClosePubFile(pf);

    lock = Lock(IFF_TESTFILE, ACCESS_READ);
    TEST_ASSERT(lock != 0, "IFF file exists on disk after ClosePubFile");
    if (lock)
        UnLock(lock);

    pf = MUIS_OpenPubFile(IFF_TESTFILE, MODE_OLDFILE);
    TEST_ASSERT(pf != NULL, "OpenPubFile(MODE_OLDFILE)");
    if (pf == NULL)
    {
        MUIS_FreePubScreenDesc(src);
        return;
    }

    got = MUIS_ReadPubFile(pf);
    TEST_ASSERT(got != NULL, "ReadPubFile returns first desc");
    if (got)
    {
        print_desc("read#1", got);
        TEST_ASSERT(strcmp(got->Name, "IffRound") == 0, "first Name is IffRound");
        TEST_ASSERT(strcmp(got->Title, "IFF Roundtrip Screen") == 0, "first Title matches");
        TEST_ASSERT(got->NoDrag == TRUE, "first NoDrag preserved");
        TEST_ASSERT(got->Changed == 7, "first Changed preserved");
        TEST_ASSERT(got->DisplayWidth == src->DisplayWidth, "DisplayWidth preserved");
        TEST_ASSERT(got->DisplayHeight == src->DisplayHeight, "DisplayHeight preserved");
        TEST_ASSERT(got->DisplayDepth == src->DisplayDepth, "DisplayDepth preserved");
        TEST_ASSERT(got->DisplayID == src->DisplayID, "DisplayID preserved");
        MUIS_FreePubScreenDesc(got);
    }

    extra = MUIS_ReadPubFile(pf);
    TEST_ASSERT(extra != NULL, "ReadPubFile returns second desc");
    if (extra)
    {
        print_desc("read#2", extra);
        TEST_ASSERT(strcmp(extra->Name, "IffRound2") == 0, "second Name is IffRound2");
        MUIS_FreePubScreenDesc(extra);
    }

    /* Third read should fail (no more MPUB chunks). */
    got = MUIS_ReadPubFile(pf);
    TEST_ASSERT(got == NULL, "ReadPubFile at EOF returns NULL");

    MUIS_ClosePubFile(pf);

    /* Negative: open missing file. */
    pf = MUIS_OpenPubFile("T:muiscreen_apitest_missing.iff", MODE_OLDFILE);
    TEST_ASSERT(pf == NULL, "OpenPubFile(missing MODE_OLDFILE) returns NULL");

    pf = MUIS_OpenPubFile(NULL, MODE_NEWFILE);
    TEST_ASSERT(pf == NULL, "OpenPubFile(NULL) returns NULL");

    ok = MUIS_WritePubFile(NULL, src);
    TEST_ASSERT(ok == FALSE, "WritePubFile(NULL,src) returns FALSE");

    MUIS_ClosePubFile(NULL); /* must not crash */
    TEST_ASSERT(1, "ClosePubFile(NULL) did not crash");

    MUIS_FreePubScreenDesc(src);
    DeleteFile(IFF_TESTFILE);
}

static void test_infoclient_and_screen(void)
{
    struct MUI_PubScreenDesc *desc;
    struct MUIS_InfoClient client;
    char *opened;
    BOOL closed;
    BYTE sigbit;
    ULONG sigmask;
    ULONG got;
    struct List *publist;
    struct PubScreenNode *node;
    int found;

    printf("\n== InfoClient + Open/ClosePubScreen ==\n");

    desc = MUIS_AllocPubScreenDesc(NULL);
    TEST_ASSERT(desc != NULL, "alloc desc for screen test");
    if (desc == NULL)
        return;

    strcpy(desc->Name, TEST_SCRNAME);
    strcpy(desc->Title, "muiscreen.library API test");
    desc->Behind = TRUE;
    desc->AutoClose = FALSE;
    desc->SysDefault = FALSE;
    desc->CloseGadget = FALSE;

    sigbit = AllocSignal(-1);
    TEST_ASSERT(sigbit != -1, "AllocSignal for InfoClient");
    if (sigbit == -1)
    {
        MUIS_FreePubScreenDesc(desc);
        return;
    }

    sigmask = 1UL << sigbit;
    client.task = FindTask(NULL);
    client.sigbit = sigmask;

    MUIS_AddInfoClient(&client);
    TEST_ASSERT(1, "AddInfoClient accepted client");

    SetSignal(0, sigmask);

    opened = MUIS_OpenPubScreen(desc);
    TEST_ASSERT(opened != NULL, "OpenPubScreen returned name");
    if (opened)
        TEST_ASSERT(strcmp(opened, TEST_SCRNAME) == 0, "OpenPubScreen name is MUISTest");

    got = SetSignal(0, sigmask);
    TEST_ASSERT((got & sigmask) != 0, "InfoClient signaled on OpenPubScreen");

    /* Confirm it is on the public screen list. */
    found = 0;
    publist = LockPubScreenList();
    for (node = (struct PubScreenNode *)publist->lh_Head;
         node->psn_Node.ln_Succ;
         node = (struct PubScreenNode *)node->psn_Node.ln_Succ)
    {
        if (strcmp(node->psn_Node.ln_Name, TEST_SCRNAME) == 0)
        {
            found = 1;
            break;
        }
    }
    UnlockPubScreenList();
    TEST_ASSERT(found, "MUISTest is on the public screen list");

    SetSignal(0, sigmask);
    closed = MUIS_ClosePubScreen(TEST_SCRNAME);
    TEST_ASSERT(closed == TRUE, "ClosePubScreen(MUISTest) returns TRUE");

    got = SetSignal(0, sigmask);
    TEST_ASSERT((got & sigmask) != 0, "InfoClient signaled on ClosePubScreen");

    found = 0;
    publist = LockPubScreenList();
    for (node = (struct PubScreenNode *)publist->lh_Head;
         node->psn_Node.ln_Succ;
         node = (struct PubScreenNode *)node->psn_Node.ln_Succ)
    {
        if (strcmp(node->psn_Node.ln_Name, TEST_SCRNAME) == 0)
        {
            found = 1;
            break;
        }
    }
    UnlockPubScreenList();
    TEST_ASSERT(!found, "MUISTest removed from public screen list");

    closed = MUIS_ClosePubScreen(TEST_SCRNAME);
    TEST_ASSERT(closed == FALSE, "ClosePubScreen again returns FALSE");

    closed = MUIS_ClosePubScreen(NULL);
    TEST_ASSERT(closed == FALSE, "ClosePubScreen(NULL) returns FALSE");

    opened = MUIS_OpenPubScreen(NULL);
    TEST_ASSERT(opened == NULL, "OpenPubScreen(NULL) returns NULL");

    MUIS_RemInfoClient(&client);
    TEST_ASSERT(1, "RemInfoClient did not crash");

    FreeSignal(sigbit);
    MUIS_FreePubScreenDesc(desc);
}

static void test_multiple_opens(void)
{
    struct Library *second;
    struct Library *third;
    ULONG cnt_after_first;

    printf("\n== Multiple OpenLibrary ==\n");

    if (MUIScreenBase == NULL)
        return;

    cnt_after_first = MUIScreenBase->lib_OpenCnt;

    second = OpenLibrary(libname, 0);
    TEST_ASSERT(second != NULL, "second OpenLibrary succeeded");
    TEST_ASSERT(second == MUIScreenBase, "second open returns same base");
    if (second)
    {
        TEST_ASSERT(second->lib_OpenCnt == cnt_after_first + 1,
                    "OpenCnt incremented on second open");
        CloseLibrary(second);
        TEST_ASSERT(MUIScreenBase->lib_OpenCnt == cnt_after_first,
                    "OpenCnt restored after second CloseLibrary");
    }

    third = OpenLibrary(libname, 99);
    TEST_ASSERT(third == NULL, "OpenLibrary(version 99) fails on V1 library");
}

/****************************************************************************************/

int main(int argc, char **argv)
{
    tests_passed = 0;
    tests_failed = 0;

    if (argc > 1 && argv[1] && argv[1][0])
        libname = argv[1];

    printf("muiscreentest\n");
    printf("library: %s\n", libname);

    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
    if (IntuitionBase == NULL)
    {
        printf("FAIL: cannot open intuition.library V39\n");
        return 20;
    }

    test_open_library();
    if (MUIScreenBase == NULL)
    {
        printf("\nCannot continue without the library.\n");
        printf("Install muiscreen.library on LIBS: and re-run.\n");
        CloseLibrary((struct Library *)IntuitionBase);
        return 20;
    }

    test_alloc_free();
    test_iff_roundtrip();
    test_infoclient_and_screen();
    test_multiple_opens();

    CloseLibrary(MUIScreenBase);
    MUIScreenBase = NULL;
    CloseLibrary((struct Library *)IntuitionBase);
    IntuitionBase = NULL;

    printf("\n== Summary ==\n");
    printf("  passed: %d\n", tests_passed);
    printf("  failed: %d\n", tests_failed);

    if (tests_failed)
        return 10;
    return 0;
}
