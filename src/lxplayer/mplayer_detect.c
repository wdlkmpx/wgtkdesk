/*
 * Public Domain

 compile test app:
    cc -DCREATE_EXE -g -Wall -o mplayer_detect mplayer_detect.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mplayer_detect.h"

struct output_driver audio_drivers[MAX_DRIVERS];
struct output_driver video_drivers[MAX_DRIVERS];

static int video_driver_is_unwanted (const char *driver)
{
    int i;
    static const char *unwanted_video_drivers[] =
    {
        "aa", "caca", "png", "jpeg",
        "gif89a", "tga", "yuv4mpeg",
        "pnm", "md5sum", "dga", "matrixview",
        "directfb", "fbdev", "fbdev2", "dfbmga", NULL
    };
    for (i = 0; unwanted_video_drivers[i]; i++) {
        //puts (unwanted_video_drivers[i]);
        if (strcmp (driver, unwanted_video_drivers[i]) == 0) {
            return 1; // yes
        }
    }
    return 0; // no
}


void get_output_drivers (char * mplayerbin)
{
    FILE *fp = NULL;
    char line[1024];
    int audio_list = 0;
    int video_list = 0;
    int audio_index = 0;
    int video_index = 0;
    char *p = NULL;
    char *driver = NULL;
    char *driver_desc = NULL;
    char mplayer_cmd[256];

    audio_drivers[0].name = NULL;
    video_drivers[0].name = NULL;

    if (!mplayerbin) {
        fp = popen ("mplayer -ao help -vo help", "r");
    } else {
        snprintf (mplayer_cmd, sizeof(mplayer_cmd), "%s -ao help -vo help", mplayerbin);
        fp = popen (mplayer_cmd, "r");
    }

    if (!fp) {
        puts ("ERROR RUNNING APP");
        return;
    }

    // parse lines and add items to driver lists
    while (fgets (line, sizeof(line), fp))
    {
        line[strlen(line)-1] = '\0'; // fgets doesn't remove \n
        if (!*line) {
            continue; // no empty lines
        }
        if (strstr (line, "Available audio output drivers")) {
            video_list = 0;
            audio_list = 1;
            continue;
        }
        if (strstr (line, "Available video output drivers")) {
            video_list = 1;
            audio_list = 0;
            continue;
        }
        if (!audio_list && !video_list) {
            continue;
        }
        //puts (line);
        // --
        //	alsa	ALSA-0.9.x-1.x audio output
        p = line;
        while (*p <= 32) p++;
        driver = p; //alsa	ALSA-0.9.x-1.x audio output
        while (*p > 32) p++;
        *p = 0;
        p++;
        while (*p <= 32) p++;
        driver_desc = p; //ALSA-0.9.x-1.x audio output
        ///puts (driver_desc);
        ///puts (driver);
        if (strcmp (driver, "null") == 0) {
            continue;
        }

        if (audio_list && audio_index < MAX_DRIVERS-1) {
#ifdef __linux__
            if (strcmp (driver, "oss") == 0) continue;
#endif
            audio_drivers[audio_index].name = strdup (driver);
            audio_drivers[audio_index].desc = strdup (driver_desc);
            audio_index++;
            
        }
        if (video_list && video_index < MAX_DRIVERS-1) {
            if (video_driver_is_unwanted (driver)) {
                continue;
            }
            video_drivers[video_index].name = strdup (driver);
            video_drivers[video_index].desc = strdup (driver_desc);
            video_index++;
        }
    }
    pclose (fp);
}


void free_output_drivers (void)
{
    int i;
    for (i = 0; audio_drivers[i].name; i++) {
        free (audio_drivers[i].name);
        free (audio_drivers[i].desc);
        audio_drivers[i].name = NULL;
        audio_drivers[i].desc = NULL;
    }
    for (i = 0; video_drivers[i].name; i++) {
        free (video_drivers[i].name);
        free (video_drivers[i].desc);
        video_drivers[i].name = NULL;
        video_drivers[i].desc = NULL;
    }
}


// ================================================================
// test app

#ifdef CREATE_EXE

static void print_output_drivers (void)
{
    int i;
    puts ("Audio drivers:");
    for (i = 0; audio_drivers[i].name; i++) {
        printf ("   %s   %s\n", audio_drivers[i].name, audio_drivers[i].desc);
    }
    puts ("Video drivers:");
    for (i = 0; video_drivers[i].name; i++) {
        printf ("   %s   %s\n", video_drivers[i].name, video_drivers[i].desc);
    }
}


int main (int argc, char **argv)
{
    char *mpb = NULL;
    memset (audio_drivers, 0, sizeof(audio_drivers));
    memset (video_drivers, 0, sizeof(video_drivers));
    if (argc > 1) {
        mpb = argv[1];
    }
    get_output_drivers (mpb);
    print_output_drivers ();
    free_output_drivers ();
    return 0;
}

#endif  /* CREATE_EXE */
