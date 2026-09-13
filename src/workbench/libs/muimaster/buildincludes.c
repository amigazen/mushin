/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**************************************************************************
 This file builds the libraries/mui.h file. It outputs it to stdout
 so you can redirect them

 Currently it merges some files but later version might do more
 things
**************************************************************************/

static char linebuf[2048];

/* array of already included files */
char **included;
int included_num;

static int need_to_be_included(char *filename)
{
    int i;
    for (i = 0; i < included_num; i++)
    {
        if (!strcmp(included[i], filename))
            return 0;
    }
    return 1;
}

static void readfile(FILE *in)
{
    while (fgets(linebuf, 2048, in))
    {
        if (!strstr(linebuf, "PRIV"))
        {
            if (strchr(linebuf, '#') && strstr(linebuf, "include"))
            {
                char *start = strchr(linebuf, '"');
                if (start)
                {
                    char *end;
                    start++;
                    end = strchr(start, '"');
                    if (end)
                    {
                        *end = 0;
                        if (need_to_be_included(start))
                        {
                            FILE *in2;
                            if (!(included =
                                    realloc(included,
                                        sizeof(char *) * (included_num +
                                            1))))
                                return;
                            included[included_num++] = strdup(start);
                            if ((in2 = fopen(start, "r")))
                            {
                                readfile(in2);
                                fclose(in2);
                            }
                        }
                    }
                }
                else
                    printf("%s", linebuf);
            }
            else
                printf("%s", linebuf);
        }
    }
}

int main(void)
{
    FILE *in;

    /*
     * Class headers include "mui.h"; skip re-expansion while merging.  This
     * has to succeed: without the seed entry, the first class header that
     * names "mui.h" would send readfile() back to the top of the file it is
     * already expanding, and it would recurse until the stack ran out.
     */
    included = malloc(sizeof(char *));
    if (included == NULL)
    {
        fprintf(stderr, "buildincludes: out of memory\n");
        return 20;
    }
    included[included_num++] = strdup("mui.h");
    if (included[0] == NULL)
    {
        fprintf(stderr, "buildincludes: out of memory\n");
        return 20;
    }

    /*
     * The caller redirects stdout over include/libraries/mui.h, so failing
     * quietly here would replace the public header with an empty file and the
     * breakage would not show up until something tried to compile against it.
     */
    in = fopen("mui.h", "r");
    if (in == NULL)
    {
        fprintf(stderr, "buildincludes: cannot open mui.h\n");
        return 20;
    }
    readfile(in);
    fclose(in);

    return 0;
}
