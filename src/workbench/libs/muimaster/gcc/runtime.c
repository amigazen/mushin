/* Resources normally opened by executable startup, owned by this library. */
#include <proto/exec.h>

struct Library *MathIeeeDoubBasBase;
struct Library *MathIeeeDoubTransBase;

void ZuneGccCleanup(void)
{
    if (MathIeeeDoubTransBase)
        CloseLibrary(MathIeeeDoubTransBase);
    if (MathIeeeDoubBasBase)
        CloseLibrary(MathIeeeDoubBasBase);
    MathIeeeDoubTransBase = NULL;
    MathIeeeDoubBasBase = NULL;
}

BOOL ZuneGccInit(void)
{
    MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library", 0);
    if (MathIeeeDoubBasBase)
        MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library", 0);
    if (!MathIeeeDoubBasBase || !MathIeeeDoubTransBase)
    {
        ZuneGccCleanup();
        return FALSE;
    }
    return TRUE;
}
