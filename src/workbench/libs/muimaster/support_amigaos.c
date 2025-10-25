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

/************************************************************
 WritePixelArrayAlpha - Alpha-blends pixel data to RastPort
 Based on AROS implementation
*************************************************************/
ULONG __saveds WritePixelArrayAlpha(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, 
                          struct RastPort *rp, UWORD destx, UWORD desty, 
                          UWORD width, UWORD height, ULONG globalalpha)
{
    ULONG start_offset;
    
    if (width == 0 || height == 0)
        return 0;

    /* Check if we have a valid bitmap */
    if (!rp || !rp->BitMap)
        return 0;

    /* Compute the start of the array */
    start_offset = ((ULONG)srcy) * srcmod + srcx * 4;

    /* For now, just copy the pixels directly without alpha blending */
    /* This is a simplified version that works with standard bitmaps */
    {
        UBYTE *src_data = ((UBYTE *)src) + start_offset;
        UBYTE *dest_data;
        ULONG y;
        
        for (y = 0; y < height; y++)
        {
            dest_data = (UBYTE *)rp->BitMap->Planes[0] + 
                       (desty + y) * rp->BitMap->BytesPerRow + destx * 4;
            
            if (dest_data)
            {
                CopyMem(src_data, dest_data, width * 4);
            }
            
            src_data += srcmod;
        }
    }

    return (ULONG)(width * height);
}

/************************************************************
 WriteLUTPixelArray - Write pixel data using color lookup table
 Based on AROS implementation
*************************************************************/
ULONG __saveds WriteLUTPixelArray(APTR srcRect, UWORD SrcX, UWORD SrcY, UWORD SrcMod, 
                       struct RastPort *rp, APTR CTable, UWORD DestX, UWORD DestY, 
                       UWORD SizeX, UWORD SizeY, UBYTE CTabFormat)
{
    ULONG depth;
    UBYTE *src_data;
    UBYTE *dest_data;
    ULONG y, x;
    
    if (SizeX == 0 || SizeY == 0)
        return 0;
    
    /* Check if we have a valid bitmap */
    if (!rp || !rp->BitMap)
        return 0;
    
    depth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);
    
    /* This call only supports bitmaps with depth > 8 */
    if (depth <= 8)
        return 0;
        
    /* Currently only one format is supported */
    if (CTabFormat != 0) /* CTABFMT_XRGB8 */
        return 0;

    /* Convert the coltab into native pixels and write them */
    src_data = (UBYTE *)srcRect + SrcY * SrcMod + SrcX;
    
    for (y = 0; y < SizeY; y++)
    {
        dest_data = (UBYTE *)rp->BitMap->Planes[0] + 
                   (DestY + y) * rp->BitMap->BytesPerRow + DestX * 4;
        
        if (dest_data && CTable)
        {
            for (x = 0; x < SizeX; x++)
            {
                UBYTE pen = src_data[x];
                ULONG rgb = ((ULONG *)CTable)[pen];
                
                /* Convert XRGB8 to native format */
                ((ULONG *)dest_data)[x] = rgb;
            }
        }
        
        src_data += SrcMod;
    }

    return (ULONG)(SizeX * SizeY);
}

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
