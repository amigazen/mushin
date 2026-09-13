#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include "support_amigaos.h"
 
/***************************************************************************/

#ifndef __amigaos4__   

/************************************************************
 Like AllocVec() but for pools
*************************************************************/
APTR AllocVecPooled(APTR pool, ULONG size)
{
    IPTR *memory;
    
    if (pool == NULL) return NULL;
    
    size   += sizeof(IPTR);
    memory  = AllocPooled(pool, size);
    
    if (memory != NULL)
    {
        *memory++ = size;
    }

    return memory;
}

/************************************************************
 Like FreeVec() but for pools
*************************************************************/
VOID FreeVecPooled(APTR pool, APTR memory)
{   
    if (memory != NULL)
    {
        IPTR *real = (IPTR *) memory;
        IPTR size  = *--real;

        FreePooled(pool, real, size);
    }
}

/*
 * snprintf() and sprintf() below are deliberately built on RawDoFmt() rather
 * than taken from sc.lib.  A shared library is linked without startup code,
 * so pulling in the SAS/C stdio formatting engine would drag in globals that
 * nothing ever initialises; RawDoFmt() is in exec and needs no setup.
 *
 * The consequence is that THE WHOLE LIBRARY USES THE RawDoFmt FORMAT DIALECT,
 * in which the length modifier is not optional:
 *
 *     %ld %lu %lx %lc   read 32 bits   <- always use these
 *     %d  %u  %x  %c    read 16 bits   <- silently wrong for an int/LONG
 *
 * A 32-bit argument formatted with %d makes RawDoFmt() consume only the high
 * half of it and then take the low half as the *next* argument, so one stray
 * %d corrupts every conversion after it too.  Twelve call sites still had
 * non-'l' specifiers - producing broken image specs, frame specs, pen specs,
 * volume sizes, scale labels and application port names - and were fixed;
 * keep any new format string in the 'l' form.
 */

struct snprintf_msg
{
	int size;
	char *buf;
};

/************************************************************
 Snprintf function for RawDoFmt()
*************************************************************/
__asm void snprintf_func(register __d0 UBYTE chr, register __a3 struct snprintf_msg *msg)
{
    if (msg->size)
    {
		  *msg->buf++ = chr;
    	msg->size--;
    }
}

/************************************************************
 Snprintf via RawDoFmt()
*************************************************************/
int snprintf(char *buf, int size, const char *fmt, ...)
{
    struct snprintf_msg msg;
		if (!size) return 0;

    msg.size = size;
    msg.buf = buf;

    RawDoFmt(fmt, (((ULONG *)&fmt)+1), snprintf_func, &msg);

    buf[size-1] = 0;

    return (int)strlen(buf);
}

/************************************************************
 sprintf via RawDoFmt()
*************************************************************/
int sprintf(char *buf, const char *fmt, ...)
{
		static const ULONG cpy_func = 0x16c04e75; /* move.b d0,(a3)+ ; rts */
		RawDoFmt(fmt, (((ULONG *)&fmt)+1), (void(*)())&cpy_func, buf);
		return (int)strlen(buf);
}

Object *VARARGS68K DoSuperNewTags(struct IClass *cl, Object *obj, void *dummy, ...)
{
    va_list argptr;
    va_start(argptr,dummy);
    obj = DoSuperNewTagList(cl,obj,dummy,(struct TagItem*)argptr);
    va_end(argptr);
    return obj;
}

#else


ASM ULONG HookEntry(REG(a0, struct Hook *hook),REG(a2, APTR obj), REG(a1, APTR msg))
{
	return hook->h_SubEntry(hook,obj,msg);
}

Object *VARARGS68K DoSuperNewTags(struct IClass *cl, Object *obj, void *dummy, ...)
{
    va_list argptr;
    struct TagItem *tagList;

    va_startlinear(argptr, dummy);
    tagList = va_getlinearva(argptr, struct TagItem *);
    obj = DoSuperNewTagList(cl,obj,dummy,tagList);
    va_end(argptr);
    return obj;
}

int VARARGS68K SPrintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int result;
    
    va_start(args, fmt);
    result = VSNPrintf(buf, 1024, (STRPTR)fmt, args);
    va_end(args);
    
    return result;
}

/***************************************************
 Like StrToLong() but for hex numbers
 that represent addresses
***************************************************/
#endif

LONG __saveds HexToIPTR(CONST_STRPTR s, IPTR *val)
{
    return HexToLong((STRPTR)s, val);
}

LONG __saveds HexToLong(CONST_STRPTR s, ULONG *val)
{
    char *end;
    *val = (ULONG)strtoul(s,&end,16);
    if (end == (char*)s) return -1;
    return end - (char*)s;
}

/************************************************************
 __XCEXIT - SAS/C library function stub
 No-op implementation for compatibility
*************************************************************/
//void __saveds _XCEXIT(void)
//{
    /* No-op function for SAS/C compatibility */
//    return;
//}

/*
 * WritePixelArrayAlpha() and WriteLUTPixelArray() used to be reimplemented
 * here.  Both have been removed, for two reasons.
 *
 * They were dead code.  Every one of the nine files that calls them includes
 * <proto/cybergraphics.h>, which supplies
 * "#pragma libcall CyberGfxBase WritePixelArrayAlpha 0d8 ...", so those calls
 * were always compiled as indirect jsrs into the real cybergraphics.library
 * and never reached the definitions here.
 *
 * They were also unsafe.  Both wrote straight into rp->BitMap->Planes[0] as
 * though it were a chunky 32-bit framebuffer, using
 * "desty * BytesPerRow + destx * 4".  On a planar AGA/ECS BitMap Planes[0] is
 * a single bitplane, so that scribbles across the neighbouring planes and off
 * the end of the allocation; on a real RTG BitMap the memory may not be CPU
 * addressable at all without LockBitMapTags().  Neither checked the depth,
 * and WritePixelArrayAlpha() ignored the alpha channel it is named for.
 *
 * The right fix for a missing cybergraphics.library is to not make the call,
 * which is what the CyberGfxBase tests in datatypescache.c, dragndrop.c and
 * textengine.c now do.
 */

/***************************************************************************/

char *StrDup(const char *x)
{
    char *dup;
    if (!x) return NULL;
    dup = AllocVec(strlen(x) + 1, MEMF_PUBLIC);
    if (dup) CopyMem((char*)x, dup, strlen(x) + 1);
    return dup;
}

Object *DoSuperNewTagList(struct IClass *cl, Object *obj,void *dummy, struct TagItem *tags)
{
	  return (Object*)DoSuperMethod(cl,obj,OM_NEW,tags,NULL);
}

size_t strlcat(char *buf, const char *src, size_t len)
{
    int l = strlen(buf);
    buf += l;
    len -= l;

    if (len>0)
    {
	int i;
	for (i=0; i < len - 1 && *src; i++)
	    *buf++ = *src++;
	*buf = 0;
    }
    return 0; /* Actually don't know right rt here */
}
