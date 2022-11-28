/*
 * Public Domain

 compile test app:
    cc -DCREATE_EXE -g -Wall -o resume_cache resume_cache.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resume_cache.h"

static ResumeState *resume_cache = NULL;
static int cindex = 0;
static char * last_cache_file = NULL;

// ===============================================================

int resume_cache_load (const char * cache_file)
{
    // returns -1 if it didn't load any file
    FILE *inf;
    char buf[1024];
    char *file, *p;
    double position;
    if (!resume_cache) {
        resume_cache = malloc (sizeof(ResumeState) * RESUME_CACHE_MAX);
        memset (resume_cache, 0, sizeof(ResumeState) * RESUME_CACHE_MAX);
    }
    if (!cache_file || !*cache_file) {
        return -1;
    }
    if (last_cache_file) {
        free (last_cache_file);
    }
    last_cache_file = strdup (cache_file);
    inf = fopen (cache_file, "r");
    if (!inf) {
        return -1;
    }
    while (fgets (buf, sizeof(buf), inf))
    {
        buf[strlen(buf)-1] = '\0'; // remove new line
        // 19.100000 file_name
        p = strchr (buf, ' ');
        if (!p) continue; // invalid entry
        *p = '\0';
        p++;
        if (!p) continue; // invalid entry
        position = atof (buf);
        file = p;
        resume_cache_add (file, position);
    }
    fclose (inf);
    return 0; // success
}


void resume_cache_add (const char *file, double position)
{
    int i;
    if (!file || !*file) {
        return;
    }
    // see if file is already there, update position
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file && strcmp(file,resume_cache[i].file) == 0) {
            resume_cache[i].position = position;
            return;
        }
    }
    // find empty item and use it
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file == NULL) {
            resume_cache[i].file = strdup (file);
            resume_cache[i].position = position;
            return;
        }
    }
    // cache is full, use a random logic
    if (cindex >= RESUME_CACHE_MAX) {
        cindex = 0;
    }
    resume_cache[cindex].file = strdup (file);
    resume_cache[cindex].position = position;
    cindex++;
}


void resume_cache_remove (const char *file)
{
    int i;
    if (!file || !*file) {
        return;
    }
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file && strcmp(file,resume_cache[i].file) == 0) {
            free (resume_cache[i].file);
            resume_cache[i].file = NULL;
            resume_cache[i].position = 0.0;
        }
    }
}


void resume_cache_free (void)
{
    int i;
    if (resume_cache) {
        for (i = 0;  i < RESUME_CACHE_MAX; i++) {
            if (resume_cache[i].file) {
                free (resume_cache[i].file);
                resume_cache[i].file = NULL;
            }
        }
        free (resume_cache);
        resume_cache = NULL;
    }
    if (last_cache_file) {
        free (last_cache_file);
        last_cache_file = NULL;
    }
}


int resume_cache_save (const char * cache_file)
{
    // returns -1 if it didn't save to any file
    FILE * outf;
    int i;
    if (!cache_file || !*cache_file) {
        cache_file = last_cache_file;
    }
    if (!cache_file) {
        return -1;
    }
    outf = fopen (cache_file, "w");
    if (!outf) {
        return -1;
    }
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file) {
            fprintf (outf, "%lf %s\n", resume_cache[i].position, resume_cache[i].file);
        }
    }
    fclose (outf);
    return 0; // success
}


ResumeState *resume_cache_find (const char *file)
{
    int i;
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file && strcmp(file,resume_cache[i].file) == 0) {
            return &resume_cache[i];
        }
    }
    return NULL;
}


// ===============================================================
// test application

#ifdef CREATE_EXE

// only 20 items for this text app, so I can see more details..
#undef RESUME_CACHE_MAX
#define RESUME_CACHE_MAX 20

static void resume_cache_print(void)
{
    int i;
    for (i = 0; i < RESUME_CACHE_MAX; i++) {
        if (resume_cache[i].file) {
            printf ("%lf - %s\n", resume_cache[i].position, resume_cache[i].file);
        }
    }
}


int main (int argc, char **argv)
{
    ResumeState *res;
    char *cfile = NULL;
    if (argc > 1) {
        cfile = argv[1];
    }

    if (resume_cache_load(cfile) != -1)
    {
        printf ("** Loaded entries from file %s\n", cfile);
        resume_cache_print ();
        puts ("----");
        //return 0;
    }

    resume_cache_add ("aaa", 1.0);
    resume_cache_add ("bbb", 2.0);
    resume_cache_add ("ccc", 3.0);
    resume_cache_add ("ddd", 4.0);
    resume_cache_add ("eee", 5.0);
    resume_cache_add ("fff", 6.0);
    resume_cache_add ("ggg", 7.0);
    resume_cache_add ("aaa", 8.0);
    resume_cache_add ("hhh", 9.0);
    resume_cache_add ("iii", 10.0);
    resume_cache_add ("jjj", 11.0);
    resume_cache_add ("kkk", 12.0);
    resume_cache_add ("lll", 13.0);
    resume_cache_add ("mmm", 14.0);
    resume_cache_add ("nnn", 15.0);
    resume_cache_add ("ooo", 16.0);
    resume_cache_add ("ppp", 17.0);
    resume_cache_add ("aaa", 18.0);
    resume_cache_print ();
    
    puts ("----");
    resume_cache_remove ("ppp");
    resume_cache_remove ("ooo");
    resume_cache_remove ("aaa");
    resume_cache_remove ("mmm");
    resume_cache_remove ("lll");
    resume_cache_print ();

    puts ("----");
    resume_cache_add ("zzz", 19.0);
    resume_cache_add ("wwww", 20.0);
    resume_cache_add ("wwww", 450.0);
    resume_cache_add ("yyyyy", 21.0);
    resume_cache_add ("yyyyy", 200.0);
    resume_cache_print ();
    
    puts ("----");
    res = resume_cache_find ("wwww");
    if (res) {
        printf ("%lf - %s\n", res->position, res->file);
    }

    resume_cache_save (NULL);
    resume_cache_free ();
    return 0;
}

#endif  /* CREATE_EXE */
