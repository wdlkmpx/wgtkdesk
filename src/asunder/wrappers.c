/*
Asunder

Copyright(C) 2005 Eric Lathrop <eric@ericlathrop.com>
Copyright(C) 2007 Andrew Smith <http://littlesvr.ca/contact.php>

Any code in this file may be redistributed or modified under the terms of
the GNU General Public Licence as published by the Free Software 
Foundation; version 2 of the licence.

*/

#define _GNU_SOURCE

#include "common.h"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "main.h"
#include "wrappers.h"
#include "threads.h"
#include "prefs.h"
#include "util.h"

pid_t cdparanoia_pid = 0;
pid_t lame_pid = 0;
pid_t oggenc_pid = 0;
pid_t opusenc_pid = 0;
pid_t flac_pid = 0;
pid_t fdkaac_pid = 0;

int numCdparanoiaFailed;
int numLameFailed;
int numOggFailed;
int numOpusFailed;
int numFlacFailed;
int numAacFailed;

int numCdparanoiaOk;
int numLameOk;
int numOggOk;
int numOpusOk;
int numFlacOk;
int numAacOk;

int numchildren = 0;
static bool waitBeforeSigchld;

void blockSigChld(void)
{
    sigset_t block_chld;
    
    sigemptyset(&block_chld);
    sigaddset(&block_chld, SIGCHLD);
    
    sigprocmask(SIG_BLOCK, &block_chld, NULL);
    
    /*!! for some reason the blocking above doesn't work, so do this for now */
    waitBeforeSigchld = true;
}

void unblockSigChld(void)
{
    sigset_t block_chld;
    
    sigemptyset(&block_chld);
    sigaddset(&block_chld, SIGCHLD);
    
    sigprocmask(SIG_UNBLOCK, &block_chld, NULL);
    
    waitBeforeSigchld = false;
}

// Signal handler to find out when our child has exited.
// Do not put any printf or syslog into here, it causes a deadlock.
//
void sigchld(int signum)
{
    int status;
    pid_t pid;
    
    pid = wait(&status);
    
    /* this is because i can't seem to be able to block sigchld: */
    while(waitBeforeSigchld)
        usleep(100);
    
    if (status != 0)
    {
        if (pid == cdparanoia_pid)
        {
            cdparanoia_pid = 0;
            if(global_prefs->rip_wav)
                numCdparanoiaFailed++;
        }
        else if (pid == lame_pid)
        {
            lame_pid = 0;
            numLameFailed++;
        }
        else if (pid == oggenc_pid)
        {
            oggenc_pid = 0;
            numOggFailed++;
        }
        else if (pid == opusenc_pid)
        {
            opusenc_pid = 0;
            numOpusFailed++;
        }
        else if (pid == flac_pid)
        {
            flac_pid = 0;
            numFlacFailed++;
        }
        else if (pid == fdkaac_pid)
        {
            fdkaac_pid = 0;
            numAacFailed++;
        }
    }
    else
    {
        if (pid == cdparanoia_pid)
        {
            cdparanoia_pid = 0;
            if(global_prefs->rip_wav)
                numCdparanoiaOk++;
        }
        else if (pid == lame_pid)
        {
            lame_pid = 0;
            numLameOk++;
        }
        else if (pid == oggenc_pid)
        {
            oggenc_pid = 0;
            numOggOk++;
        }
        else if (pid == opusenc_pid)
        {
            opusenc_pid = 0;
            numOpusOk++;
        }
        else if (pid == flac_pid)
        {
            flac_pid = 0;
            numFlacOk++;
        }
        else if (pid == fdkaac_pid)
        {
            fdkaac_pid = 0;
            numAacOk++;
        }
    }
}

// fork() and exec() the file listed in "args"
//
// args - a valid array for execvp()
// toread - the file descriptor to pipe back to the parent
// p - a place to write the PID of the exec'ed process
// dir - directory to run program in
// 
// returns - a file descriptor that reads whatever the program outputs on "toread"
int exec_with_output(const char * args[], int toread, pid_t * p, const char * dir)
{
    char logStr[2048];
    int pipefd[2];
    
    blockSigChld();
    
    if (pipe(pipefd) != 0)
        fatalError("exec_with_output(): failed to create a pipe");
    
    if ((*p = fork()) == 0)
    {
        // im the child
        // i get to execute the command
        
        // close the side of the pipe we don't need
        close(pipefd[0]);
        
        /* this causes a segfault on fedora, and I don't really see why it's needed anyway */
        //~ // close all standard streams to keep output clean
        //~ close(STDOUT_FILENO);
        //~ close(STDIN_FILENO);
        //~ close(STDERR_FILENO);
        
        /* instead redirect to /dev/null */
        if (gbl_null_fd != -1)
        {
            dup2(STDOUT_FILENO, gbl_null_fd);
            close(STDOUT_FILENO);
            dup2(STDERR_FILENO, gbl_null_fd);
            close(STDERR_FILENO);
        }
        
        // setup output
        dup2(pipefd[1], toread);
        close(pipefd[1]);
        
        // Change directory if required
        if (dir && (chdir(dir) < 0))
        {
            snprintf(logStr, 2048, "Failed to change directory for %s, errno=%d", args[0], errno);
            debugLog(logStr);
            exit(0);
        }
        
        // call execvp
        execvp(args[0], (char **)args);
        
        // should never get here
        fatalError("exec_with_output(): execvp() failed");
    }
    
    int count;
    snprintf(logStr, 2048, "%d started: %s ", *p, args[0]);
    for (count = 1; args[count] != NULL; count++)
    {
        if (strlen(logStr) + 1 + strlen(args[count]) < 2048)
        {
            strcat(logStr, " ");
            strcat(logStr, args[count]);
        }
    }
    debugLog(logStr);
    
    // i'm the parent, get ready to wait for children
    numchildren++;
    
    // close the side of the pipe we don't need
    close(pipefd[1]);
    
    unblockSigChld();
    
    return pipefd[0];
}

// uses cdparanoia to rip a WAV track from a cdrom
//
// cdrom    - the path to the cdrom device
// tracknum - the track to rip
// filename - the name of the output WAV file
// progress - the percent done
void cdparanoia(const char * cdrom,
                int tracknum,
                const char * filename,
                double * progress)
{
    const char * args[8];
    int pos;
    char logStr[1024];
    int fd;
    char buf[256];
    int size;
    
    int start;
    int end;
    int code;
    char type[200];
    int sector;
    
    char trackstring[4];
    
    snprintf(trackstring, 4, "%d", tracknum);

    // Evade cdparanoia's 245 char path limit by changing to the target
    // directory (in exec_with_output()) and using a temporary file name
    // with no absolute path.
    char trackname[16];
    snprintf(trackname, sizeof(trackname), "x%02d.wav", tracknum);
    gchar * dir = g_path_get_dirname(filename);
    gchar * xfilename = g_build_filename(dir, trackname, NULL);
    
    pos = 0;
    args[pos++] = "cdparanoia";
    if (global_prefs->do_fast_rip)
        args[pos++] = "-Z";
    args[pos++] = "-e";
    args[pos++] = "-d";
    args[pos++] = cdrom;
    args[pos++] = trackstring;
    args[pos++] = trackname;
    args[pos++] = NULL;
    
    fd = exec_with_output(args, STDERR_FILENO, &cdparanoia_pid, dir);
    
    // to convert the progress number stat cdparanoia spits out
    // into sector numbers divide by 1176
    // note: only use the "[wrote]" numbers
    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;
            
            pos++;
            size = read(fd, &buf[pos], 1);
            
            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("cdparanoia() interrupted");
                interrupted = TRUE;
            }
        } while ((size > 0 && pos < 255 && buf[pos] != '\n') || interrupted);
        buf[pos] = '\0';

        if ((buf[0] == 'R') && (buf[1] == 'i'))
        {
            sscanf(buf, "Ripping from sector %d", &start);
        } else if (buf[0] == '\t') {
            sscanf(buf, "\t to sector %d", &end);
        } else if (buf[0] == '#') {
            sscanf(buf, "##: %d %s @ %d", &code, type, &sector);
            sector /= 1176;
            if (strncmp("[wrote]", type, 7) == 0)
            {
                *progress = (double)(sector-start)/(end-start);
            }
        }
    } while (size > 0);
    
    snprintf(logStr, 1024, "Ripping %s finished\n", trackstring);
    debugLog(logStr);
    
    close(fd);
    /* don't go on until the signal for the previous call is handled */
    while (cdparanoia_pid != 0)
    {
        debugLog("w3\n");
        usleep(100000);
    }

    int rc = rename(xfilename, filename);
    if (rc == -1)
    {
        snprintf(logStr, 1024, "cdparanoia(): rename() to '%s' failed with errno=%d\n", filename, errno);
        debugLog(logStr);
    }
    g_free(xfilename);
    g_free(dir);

    *progress = 1;
}

// uses LAME to encode a WAV file into a MP3 and tag it
//
// tracknum - the track number
// artist - the artist's name
// album - the album the song came from
// title - the name of the song
// wavfilename - the path to the WAV file to encode
// mp3filename - the path to the output MP3 file
// vbr - (boolean) wether or not to encode with Variable Bit Rate
// bitrate - the bitrate to encode at (or maximum bitrate if using VBR)
// progress - the percent done
void lame(int tracknum,
          const char * artist,
          const char * album,
          const char * title,
          const char * genre,
          const char * year,
          const char * wavfilename,
          const char * mp3filename,
          int vbr,
          int bitrate,
          double * progress)
{
    int fd;
    
    char buf[256];
    int size;
    int pos;
    
    int sector;
    int end;
    
    char * tracknum_text = NULL;
    char bitrate_text[4];
    const char * args[19];

//    fprintf( stderr, " lame()   Genre: %s Artist: %s Title: %s\n", genre, artist, title );	// lnr

    pos = 0;
    args[pos++] = "lame";
    if (vbr)
    {
        args[pos++] = "-V";
        snprintf(bitrate_text, 4, "%d", int_to_vbr_int(bitrate));
    } else {
        args[pos++] = "-b";
        snprintf(bitrate_text, 4, "%d", int_to_bitrate(bitrate, vbr));
    }
    args[pos++] = bitrate_text;
    args[pos++] = "--id3v2-only";
    if (tracknum > 0 && asprintf(&tracknum_text, "%d", tracknum) > 0)
    {
        args[pos++] = "--tn";
        args[pos++] = tracknum_text;
    }
    if ((artist != NULL) && (strlen(artist) > 0))
    {
        args[pos++] = "--ta";
        args[pos++] = artist;
    }
    if ((album != NULL) && (strlen(album) > 0))
    {
        args[pos++] = "--tl";
        args[pos++] = album;
    }
    if ((title != NULL) && (strlen(title) > 0))
    {
        args[pos++] = "--tt";
        args[pos++] = title;
    }

    // lame refuses to accept some genres that come from cddb, and users get upset
    // No longer an issue - users can now edit the genre field -lnr 

    // if (false && (genre != NULL) && (strlen(genre) > 0))
    if (( genre != NULL )								// lnr
    &&  ( strlen( genre )))
    {
        args[pos++] = "--tg";
        args[pos++] = genre;
    }

    if ((year != NULL) && (strlen(year) > 0))
    {
        args[pos++] = "--ty";
        args[pos++] = year;
    }
    args[pos++] = wavfilename;
    args[pos++] = mp3filename;
    args[pos++] = NULL;

    fd = exec_with_output(args, STDERR_FILENO, &lame_pid, NULL);
    free(tracknum_text);
    
    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;
            
            pos++;
            size = read(fd, &buf[pos], 1);
            
            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("lame() interrupted");
                interrupted = TRUE;
            }
            
        } while ((size > 0 && pos < 255 && buf[pos] != '\r' && buf[pos] != '\n') || interrupted);
        buf[pos] = '\0';
        
        if (sscanf(buf, "%d/%d", &sector, &end) == 2)
        {
            *progress = (double)sector/end;
        }
    } while (size > 0);
    
    close(fd);
    /* don't go on until the signal for the previous call is handled */
    while (lame_pid != 0)
    {
        debugLog("w4\n");
        usleep(100000);
    }
    *progress = 1;
}

// uses oggenc to encode a WAV file into a OGG and tag it
//
// tracknum - the track number
// artist - the artist's name
// album - the album the song came from
// title - the name of the song
// wavfilename - the path to the WAV file to encode
// oggfilename - the path to the output OGG file
// quality_level - how hard to compress the file (0-10)
// progress - the percent done
void oggenc(int tracknum,
            const char * artist,
            const char * album,
            const char * title,
            const char * year,
            const char * genre,
            const char * wavfilename,
            const char * oggfilename,
            int quality_level,
            double * progress)
{
    int fd;

    char buf[256];
    int size;
    int pos;
    
    int sector;
    int end;

    char * tracknum_text = NULL;
    char quality_level_text[3];
    const char * args[19];

    snprintf(quality_level_text, 3, "%d", quality_level);
    
    pos = 0;
    args[pos++] = "oggenc";
    args[pos++] = "-q";
    args[pos++] = quality_level_text;
    
    if (tracknum > 0 && asprintf(&tracknum_text, "%d", tracknum) > 0)
    {
        args[pos++] = "-N";
        args[pos++] = tracknum_text;
    }
    if ((artist != NULL) && (strlen(artist) > 0))
    {
        args[pos++] = "-a";
        args[pos++] = artist;
    }
    if ((album != NULL) && (strlen(album) > 0))
    {
        args[pos++] = "-l";
        args[pos++] = album;
    }
    if ((title != NULL) && (strlen(title) > 0))
    {
        args[pos++] = "-t";
        args[pos++] = title;
    } 
    if ((year != NULL) && (strlen(year) > 0))
    {
        args[pos++] = "-d";
        args[pos++] = year;
    }
    if ((genre != NULL) && (strlen(genre) > 0))
    {
        args[pos++] = "-G";
        args[pos++] = genre;
    }
    args[pos++] = wavfilename;
    args[pos++] = "-o";
    args[pos++] = oggfilename;
    args[pos++] = NULL;
    
    fd = exec_with_output(args, STDERR_FILENO, &oggenc_pid, NULL);
    free(tracknum_text);
    
    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;
            
            pos++;
            size = read(fd, &buf[pos], 1);
            
            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("oggenc() interrupted");
                interrupted = TRUE;
            }
            
        } while ((size > 0 && pos < 255 && buf[pos] != '\r' && buf[pos] != '\n') || interrupted);
        buf[pos] = '\0';

        if (sscanf(buf, "\t[\t%d.%d%%]", &sector, &end) == 2)
        {
            *progress = (double)(sector + (end*0.1))/100;
        }
        else if (sscanf(buf, "\t[\t%d,%d%%]", &sector, &end) == 2)
        {
            *progress = (double)(sector + (end*0.1))/100;
        }
    } while (size > 0);
    
    close(fd);
    /* don't go on until the signal for the previous call is handled */
    while (oggenc_pid != 0)
    {
        debugLog("w6\n");
        usleep(100000);
    }
    *progress = 1;
}

// uses opusenc to encode a WAV file into a OGG and tag it
//
// tracknum - the track number
// artist - the artist's name
// album - the album the song came from
// title - the name of the song
// wavfilename - the path to the WAV file to encode
// opusfilename - the path to the output OPUS file
// bitrate - the bitrate to encode at
// progress - the percent done
void opusenc(int tracknum,
            const char * artist,
            const char * album,
            const char * title,
            const char * year,
            const char * genre,
            const char * wavfilename,
            const char * opusfilename,
            int bitrate,
            double * progress)
{
    int fd;

    int size;
    char buf[256];
    int pos;
    int percent;
    char bitrate_text[4];
    char * tracknum_text = NULL;
    const char * args[19];

    snprintf(bitrate_text, 4, "%d", int_to_bitrate(bitrate,FALSE));

    pos = 0;
    args[pos++] = "opusenc";
    args[pos++] = "--bitrate";
    args[pos++] = bitrate_text;

    if (tracknum > 0 && asprintf(&tracknum_text, "TRACKNUMBER=%d", tracknum) > 0)
    {
        args[pos++] = "--comment";
        args[pos++] = tracknum_text;
    }
    if ((artist != NULL) && (strlen(artist) > 0))
    {
        args[pos++] = "--artist";
        args[pos++] = artist;
    }
    if ((album != NULL) && (strlen(album) > 0))
    {
        args[pos++] = "--album";
        args[pos++] = album;
    }
    if ((title != NULL) && (strlen(title) > 0))
    {
        args[pos++] = "--title";
        args[pos++] = title;
    }
    if ((year != NULL) && (strlen(year) > 0))
    {
        args[pos++] = "--date";
        args[pos++] = year;
    }
    if ((genre != NULL) && (strlen(genre) > 0))
    {
        args[pos++] = "--genre";
        args[pos++] = genre;
    }
    args[pos++] = wavfilename;
    args[pos++] = opusfilename;
    args[pos++] = NULL;

    fd = exec_with_output(args, STDERR_FILENO, &opusenc_pid, NULL);
    free(tracknum_text);

    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;
            
            pos++;
            size = read(fd, &buf[pos], 1);
            
            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("opusenc() interrupted");
                interrupted = TRUE;
            }
            
        } while ((size > 0 && pos < 255 && buf[pos] != '\r' && buf[pos] != '\n') || interrupted);
        buf[pos] = '\0';

        if (sscanf(buf, "[%*c] %d%%", &percent) == 1)
        {
            *progress = (double)(percent)/100;
        }
    } while (size > 0);
    
    close(fd);
    /* don't go on until the signal for the previous call is handled */
    while (opusenc_pid != 0)
    {
        debugLog("w11\n");
        usleep(100000);
    }
    *progress = 1.0;
}


// uses the FLAC reference encoder to encode a WAV file into a FLAC and tag it
//
// tracknum - the track number
// artist - the artist's name
// albumartist - we want to tag this if it is a compilation
// album - the album the song came from
// title - the name of the song
// wavfilename - the path to the WAV file to encode
// flacfilename - the path to the output FLAC file
// compression_level - how hard to compress the file (0-8) see flac man page
// progress - the percent done
void flac(int tracknum,
          const char * artist,
          const char * albumartist, //mw
          gboolean single_artist, //mw
          const char * album,
          const char * title,
          const char * genre,
          const char * year,
          const char * wavfilename,
          const char * flacfilename,
          int compression_level,
          double * progress)
{
    int fd;

    char buf[256];
    int size;
    int pos;
    
    int sector;
    
    char * tracknum_text = NULL;
    char * artist_text = NULL;
    char * albumartist_text = NULL; //mw
    char * album_text = NULL;
    char * title_text = NULL;
    char * genre_text = NULL;
    char * year_text = NULL;
    char compression_level_text[3];
    const char * args[21];
    
    if(artist != NULL)
    {
        artist_text = malloc(strlen(artist) + 8);
        if (artist_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(artist)+8)) failed. Out of memory.");
        snprintf(artist_text, strlen(artist) + 8, "ARTIST=%s", artist);
    }
    
    //mw
    if((albumartist != NULL) && (!single_artist))
    {
        albumartist_text = malloc(strlen(albumartist) + 13);
        if (albumartist_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(albumartist)+13)) failed. Out of memory.");
        snprintf(albumartist_text, strlen(albumartist) + 13, "ALBUMARTIST=%s", albumartist);
    }
    //mw end
    
    if(album != NULL)
    {
        album_text = malloc(strlen(album) + 7);
        if (album_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(album)+7)) failed. Out of memory.");
        snprintf(album_text, strlen(album) + 7, "ALBUM=%s", album);
    }
    
    if(title != NULL)
    {
        title_text = malloc(strlen(title) + 7);
        if (title_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(title)+7) failed. Out of memory.");
        snprintf(title_text, strlen(title) + 7, "TITLE=%s", title);
    }
    
    if(genre != NULL)
    {
        genre_text = malloc(strlen(genre) + 7);
        if (genre_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(genre)+7) failed. Out of memory.");
        snprintf(genre_text, strlen(genre) + 7, "GENRE=%s", genre);
    }
    
    if(year != NULL)
    {
        year_text = malloc(strlen(year) + 6);
        if (year_text == NULL)
            fatalError("malloc(sizeof(char) * (strlen(year)+6) failed. Out of memory.");
        snprintf(year_text, strlen(year) + 6, "DATE=%s", year);
    }
    
    snprintf(compression_level_text, 3, "-%d", compression_level);
    
    pos = 0;
    args[pos++] = "flac";
    args[pos++] = "-f";
    args[pos++] = compression_level_text;
    if (tracknum > 0 && asprintf(&tracknum_text, "TRACKNUMBER=%d", tracknum) > 0)
    {
        args[pos++] = "-T";
        args[pos++] = tracknum_text;
    }
    if ((artist != NULL) && (strlen(artist) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = artist_text;
    }

    //mw
    if ((albumartist != NULL) && (!single_artist) && (strlen(albumartist) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = albumartist_text;
    }
    //mw end

    if ((album != NULL) && (strlen(album) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = album_text;
    }
    if ((title != NULL) && (strlen(title) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = title_text;
    }
    if ((genre != NULL) && (strlen(genre) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = genre_text;
    }
    if ((year != NULL) && (strlen(year) > 0))
    {
        args[pos++] = "-T";
        args[pos++] = year_text;
    }
    args[pos++] = wavfilename;
    args[pos++] = "-o";
    args[pos++] = flacfilename;
    args[pos++] = NULL;
    
    fd = exec_with_output(args, STDERR_FILENO, &flac_pid, NULL);
    
    free(tracknum_text);
    free(artist_text);
    free(album_text);
    free(albumartist_text);
    free(title_text);
    free(genre_text);
    free(year_text);
    
    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;
            
            pos++;
            size = read(fd, &buf[pos], 1);
            
            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("flac() interrupted");
                interrupted = TRUE;
            }
            
        } while ((size > 0 && pos < 255 && buf[pos] != '\r' && buf[pos] != '\n' && buf[pos] != '\b') || interrupted);
        buf[pos] = '\0';

        if (sscanf(buf, "%d%% ", &sector) == 1)
        {
            *progress = (double)sector/100;
        }
    } while (size > 0);
    
    close(fd);
    
    /* don't go on until the signal for the previous call is handled */
    while (flac_pid != 0)
    {
        debugLog("w7\n");
        usleep(100000);
    }
    *progress = 1;
}


void fdkaac(int tracknum,
         const char * artist,
         const char * album,
         const char * title,
         const char * genre,
         const char * year,
         const char* wavfilename,
         const char* aacfilename,
         int bitrate,
         double* progress)
{
    const char* args[21];
    char bitrate_text[4];
    int fd;
    int pos;

    pos = 0;
    args[pos++] = "fdkaac";

    /* fdkaac has a VBR option but it's unsupported */
    args[pos++] = "-m";
    args[pos++] = "0";
    args[pos++] = "-b";
    snprintf(bitrate_text, 4, "%d", int_to_bitrate(bitrate, 0));
    args[pos++] = bitrate_text;


    char * track = NULL;
    if (tracknum > 0 && asprintf(&track, "%d", tracknum) > 0)
    {
        args[pos++] = "--track";
        args[pos++] = track;
    }

    if ((title != NULL) && (strlen(title) > 0))
    {
        args[pos++] = "--title";
        args[pos++] = title;
    }

    if ((artist != NULL) && (strlen(artist) > 0))
    {
        args[pos++] = "--artist";
        args[pos++] = artist;
    }

    if ((album != NULL) && (strlen(album) > 0))
    {
        args[pos++] = "--album";
        args[pos++] = album;
    }

    if ((genre != NULL) && (strlen(genre) > 0))
    {
        args[pos++] = "--genre";
        args[pos++] = genre;
    }

    if ((year != NULL) && (strlen(year) > 0))
    {
        args[pos++] = "--date";
        args[pos++] = year;
    }

    args[pos++] = "-o";
    args[pos++] = aacfilename;

    args[pos++] = wavfilename;

    args[pos++] = NULL;

    fd = exec_with_output(args, STDERR_FILENO, &fdkaac_pid, NULL);
    free(track);

    int size;
    char buf[256];

    do
    {
        pos = -1;
        bool interrupted;
        do
        {
            interrupted = FALSE;

            pos++;
            size = read(fd, &buf[pos], 1);

            if (size == -1 && errno == EINTR)
            /* signal interrupted read(), try again */
            {
                pos--;
                debugLog("fdkaac() interrupted");
                interrupted = TRUE;
            }

        } while ((size > 0 && pos < 255 && buf[pos] != '\r' && buf[pos] != '\n') || interrupted);

        buf[pos] = '\0';

        /* We get lines like this:
            [28%] 00:14.037/00:49.771 (22x), ETA 00:01.599
        */
        if ((pos >= 4) && (buf[0] == '[')) {
            int percent = 0;
            if (sscanf(buf, "[%d%%]", &percent) == 1)
            {
                *progress = (double) percent / 100.0;
            }
        }
    } while (size > 0);

    close(fd);

    /* don't go on until the signal for the previous call is handled */
    while (fdkaac_pid != 0)
    {
        debugLog("w12\n");
        usleep(100000);
    }

    *progress = 1;
}
