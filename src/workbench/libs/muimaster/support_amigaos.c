#include <stdarg.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/mathieeedoubbas.h>
#include <proto/mathieeedoubtrans.h>

#include "support_amigaos.h"

/*
 * These replace the SAS/C sc.lib copies.  CopyMem does not accept overlap,
 * so memmove copies backwards in that case.  memset is a CPU fill: BltClear
 * only applies to chip memory, and this library's buffers are ordinary RAM.
 * utility.library Strncpy is V47 and does not pad the way strncpy does, so
 * the copy routines stay in C and call CopyMem for the byte move.
 */

void *memcpy(void *dest, const void *src, size_t n)
{
    if (n != 0 && dest != src)
        CopyMem((APTR)src, dest, (ULONG)n);
    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    const unsigned char *s;
    unsigned char *d;
    size_t i;

    if (n == 0 || dest == src)
        return dest;
    s = (const unsigned char *)src;
    d = (unsigned char *)dest;
    if (d < s || d >= s + n)
        CopyMem((APTR)src, dest, (ULONG)n);
    else
    {
        i = n;
        while (i > 0)
        {
            i--;
            d[i] = s[i];
        }
    }
    return dest;
}

void *memset(void *dest, int c, size_t n)
{
    unsigned char *d;
    unsigned char b;
    ULONG fill;
    ULONG *dw;
    size_t nlong;

    d = (unsigned char *)dest;
    b = (unsigned char)c;
    while (n != 0 && (((ULONG)d) & 3) != 0)
    {
        *d++ = b;
        n--;
    }
    if (n >= 4)
    {
        fill = (ULONG)b | ((ULONG)b << 8) | ((ULONG)b << 16) | ((ULONG)b << 24);
        dw = (ULONG *)d;
        nlong = n >> 2;
        while (nlong != 0)
        {
            *dw++ = fill;
            nlong--;
        }
        d = (unsigned char *)dw;
        n = n & 3;
    }
    while (n != 0)
    {
        *d++ = b;
        n--;
    }
    return dest;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const unsigned char *pa;
    const unsigned char *pb;

    pa = (const unsigned char *)a;
    pb = (const unsigned char *)b;
    while (n != 0)
    {
        if (*pa != *pb)
            return (int)*pa - (int)*pb;
        pa++;
        pb++;
        n--;
    }
    return 0;
}

size_t strlen(const char *s)
{
    const char *p;

    p = s;
    if (p == NULL)
        return 0;
    while (*p != '\0')
        p++;
    return (size_t)(p - s);
}

char *strcpy(char *dest, const char *src)
{
    char *d;

    d = dest;
    if (dest == NULL || src == NULL)
        return dest;
    while ((*d++ = *src++) != '\0')
        ;
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    char *d;
    size_t i;

    d = dest;
    i = 0;
    if (dest == NULL)
        return dest;
    while (i < n && src != NULL && src[i] != '\0')
    {
        d[i] = src[i];
        i++;
    }
    while (i < n)
    {
        d[i] = '\0';
        i++;
    }
    return dest;
}

char *strcat(char *dest, const char *src)
{
    char *d;

    d = dest;
    if (dest == NULL)
        return dest;
    while (*d != '\0')
        d++;
    if (src != NULL)
    {
        while ((*d++ = *src++) != '\0')
            ;
    }
    return dest;
}

int strcmp(const char *a, const char *b)
{
    if (a == NULL || b == NULL)
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    while (*a != '\0' && *a == *b)
    {
        a++;
        b++;
    }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n)
{
    if (a == NULL || b == NULL)
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    while (n != 0 && *a != '\0' && *a == *b)
    {
        a++;
        b++;
        n--;
    }
    if (n == 0)
        return 0;
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

char *strchr(const char *s, int c)
{
    unsigned char ch;

    ch = (unsigned char)c;
    if (s == NULL)
        return NULL;
    while (*s != '\0')
    {
        if ((unsigned char)*s == ch)
            return (char *)s;
        s++;
    }
    if (ch == 0)
        return (char *)s;
    return NULL;
}

char *strrchr(const char *s, int c)
{
    const char *last;
    unsigned char ch;

    ch = (unsigned char)c;
    last = NULL;
    if (s == NULL)
        return NULL;
    while (*s != '\0')
    {
        if ((unsigned char)*s == ch)
            last = s;
        s++;
    }
    if (ch == 0)
        return (char *)s;
    return (char *)last;
}

char *strstr(const char *hay, const char *needle)
{
    size_t nlen;
    size_t i;

    if (hay == NULL || needle == NULL)
        return NULL;
    nlen = strlen(needle);
    if (nlen == 0)
        return (char *)hay;
    while (*hay != '\0')
    {
        i = 0;
        while (i < nlen && hay[i] == needle[i])
            i++;
        if (i == nlen)
            return (char *)hay;
        hay++;
    }
    return NULL;
}

int stricmp(const char *a, const char *b)
{
    if (a == NULL || b == NULL)
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    return (int)Stricmp((STRPTR)a, (STRPTR)b);
}

int toupper(int c)
{
    return (int)ToUpper((ULONG)(unsigned char)c);
}

int isdigit(int c)
{
    return ((unsigned char)c >= '0' && (unsigned char)c <= '9');
}

unsigned long strtoul(const char *nptr, char **endptr, int base)
{
    const char *s;
    unsigned long val;
    int digit;
    int neg;

    s = nptr;
    val = 0;
    neg = 0;
    digit = 0;
    if (s == NULL)
    {
        if (endptr != NULL)
            *endptr = NULL;
        return 0;
    }
    while (*s == ' ' || *s == '\t')
        s++;
    if (*s == '+')
        s++;
    else if (*s == '-')
    {
        neg = 1;
        s++;
    }
    if ((base == 0 || base == 16) && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    {
        base = 16;
        s += 2;
    }
    else if (base == 0)
        base = 10;
    while (*s != '\0')
    {
        if (*s >= '0' && *s <= '9')
            digit = *s - '0';
        else if (*s >= 'a' && *s <= 'f')
            digit = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F')
            digit = *s - 'A' + 10;
        else
            break;
        if (digit >= base)
            break;
        val = val * (unsigned long)base + (unsigned long)digit;
        s++;
    }
    if (endptr != NULL)
        *endptr = (char *)s;
    if (neg)
        val = (unsigned long)(-(long)val);
    return val;
}

double ZuneSin(double x)
{
    if (MathIeeeDoubTransBase == NULL)
        return 0.0;
    return IEEEDPSin(x);
}

double ZuneCos(double x)
{
    if (MathIeeeDoubTransBase == NULL)
        return 0.0;
    return IEEEDPCos(x);
}

double ZuneAtan2(double y, double x)
{
    double pi;
    double a;

    pi = 3.14159265358979323846;
    if (MathIeeeDoubTransBase == NULL || MathIeeeDoubBasBase == NULL)
        return 0.0;
    if (x > 0.0)
        return IEEEDPAtan(IEEEDPDiv(y, x));
    if (x < 0.0)
    {
        a = IEEEDPAtan(IEEEDPDiv(y, x));
        if (y >= 0.0)
            return a + pi;
        return a - pi;
    }
    if (y > 0.0)
        return pi / 2.0;
    if (y < 0.0)
        return -(pi / 2.0);
    return 0.0;
}

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
