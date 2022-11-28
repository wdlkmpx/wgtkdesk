/***************************************************************************
    file        : jpslib2.h
    project     : JPS Library for GTK2
    date        : 2007.01.22
    copyright   : (C)2007 Paul Schuurmans
    email       : paul.schuurmans@home.nl
***************************************************************************/

/***************************************************************************
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Library General Public License for more details.
***************************************************************************/

// Format codes for StrCvt
#define RWHITE		1  	// Remove white space.
#define RLWHITE    	2	// Remove leading white space.
#define RTWHITE    	4	// Remove trailing white space.
#define REDUCE	   	8	// Reduce white space to 1 blank.
#define NOQUOTE   	16	// Quoted substrings not altered.
#define TOUP	  	32	// Convert to upper case.
#define TOLOW	  	64	// Convert to lower case.
#define RCONTROL 	128	// Remove all control characters.
#define TOCASE	 	256	// Toggle case

// Dir codes for StrJustify
#define JUST_LEFT   	(-1)
#define JUST_CENTER   	0
#define JUST_RIGHT    	1

// Miscellaneous
#define BLANK	' '
#define NEWLINE '\n'
#define TAB	 	'\t'
#define NUL	 	'\0'
#define MAXSRCHLEN	64
#define BUFFERSIZE	32767

#define FA_NORMAL		0x00
#define FA_RDONLY		0x01
#define FA_HIDDEN		0x02
#define FA_SYSTEM		0x04
#define FA_LABEL		0x08
#define FA_DIREC		0x10
#define FA_ARCH			0x20

#define FN_DIR			0x01
#define FN_FILE			0x02
#define FN_EXT			0x04

#define MAXPATH			260
#define MAXDIR			260
#define MAXFILE			260
#define MAXEXT			260

// Miscellaneous Utility Functions ///////////////////////////////////////////

char *B64Encode (char *result, unsigned long val);
void ClearArray (GPtrArray *a);
int FileExists (char *path);
int FileSize (char *name);
void FnExpand (char *rpath, char *defext);
void FnMerge (char *rpath, char *dir, char *file, char *ext);
int FnSplit (char *rpath, char *dir, char *file, char *ext);
void GetConfigBool (FILE *fn, char *key, gboolean *bval, gboolean bdef);
void GetConfigInt (FILE *fn, char *key, int *nval, int ndef);
void GetConfigString (FILE *fn, char *key, char *sval, char *sdef);
gboolean GetWidgetValBool (GtkWidget *parent, char *name, gboolean defval);
int GetWidgetValInt (GtkWidget *parent, char *name, int defval);
char *GetWidgetValString (GtkWidget *parent, char *name, char *sval, char *defval);
gboolean IsDirectory (char *filename);
int ReadLine (FILE *rfile, char *line, int maxlinesize);
gboolean SetFileAttribs (char *filename, time_t *tstamp, int mode, uid_t owner, gid_t group);
int StrChrIndex (char check, char *psearch);
char *StrCvt (char *psource, int conv);
char *StrExpand (char *ptarget, char *psource, int incr, int tarsize);
char *StrJustify (char *ptarget, char *psource, char fill, int fldsize, int code);
int StrPos (char *searchstr, char *targstr, gboolean caseflag);
char *StrTabfy (char *psource, int incr);

