/*
 * Public Domain
 */

#ifndef __RESUME_CACHE_H__
#define __RESUME_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RESUME_CACHE_MAX 50

struct resume_state
{
    char *file;
    double position;
};

typedef struct resume_state ResumeState;

int resume_cache_load (const char * cache_file);
void resume_cache_add (const char *file, double position);
void resume_cache_remove (const char *file);
void resume_cache_free (void);
int resume_cache_save (const char * cache_file);
ResumeState *resume_cache_find (const char *file);

#ifdef __cplusplus
}
#endif

#endif /* __RESUME_CACHE_H__ */
