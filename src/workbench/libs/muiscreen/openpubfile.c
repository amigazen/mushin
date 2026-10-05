/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include <libraries/iffparse.h>

#define DEBUG 0
#include <aros/debug.h>

#include "fileformat.h"
#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __ASM__ __SAVE_DS__ APTR MUIS_OpenPubFile(
            __REG__(a0, char *name),
            __REG__(d0, ULONG mode))

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
    struct IFFHandle *iff;
    struct FilePrefHeader head;
    BPTR fh;

    D(bug("MUIS_OpenPubFile(%s, %ld)\n", name ? name : "(null)", mode));

    if (name == NULL)
        return NULL;

    iff = AllocIFF();
    if (iff == NULL)
        return NULL;

    fh = Open(name, mode);
    if (fh == 0)
    {
        FreeIFF(iff);
        return NULL;
    }

    iff->iff_Stream = (ULONG)fh;
    InitIFFasDOS(iff);

    if (OpenIFF(iff, (mode == MODE_OLDFILE ? IFFF_READ : IFFF_WRITE)))
    {
        Close(fh);
        FreeIFF(iff);
        return NULL;
    }

    if (mode == MODE_NEWFILE)
    {
        if (PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN))
            goto fail_write;

        if (PushChunk(iff, ID_PREF, ID_PRHD, sizeof(struct FilePrefHeader)))
            goto fail_write;

        head.ph_Version = 0; /* FIXME: should be PHV_CURRENT */
        head.ph_Type = 0;
        head.ph_Flags[0] = 0;
        head.ph_Flags[1] = 0;
        head.ph_Flags[2] = 0;
        head.ph_Flags[3] = 0;

        if (WriteChunkBytes(iff, &head, sizeof(head)) != sizeof(head))
            goto fail_write;

        PopChunk(iff);
        return (APTR)iff;

fail_write:
        CloseIFF(iff);
        Close(fh);
        FreeIFF(iff);
        return NULL;
    }

    return (APTR)iff;
}
