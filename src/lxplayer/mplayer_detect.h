/*
 * Public Domain
 */

#ifndef __DRIVER_LIST__
#define __DRIVER_LIST__

void get_output_drivers (char * mplayerbin);
void free_output_drivers (void);

struct output_driver
{
    char *name;
    char *desc;
};

#define MAX_DRIVERS 50

extern struct output_driver audio_drivers[MAX_DRIVERS];
extern struct output_driver video_drivers[MAX_DRIVERS];

#endif
