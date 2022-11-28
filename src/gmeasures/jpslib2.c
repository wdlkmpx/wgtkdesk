/***************************************************************************
    file        : jpslib2.c
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

#include "common.h"

#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <utime.h>

#include "support.h"
#include "jpslib2.h"

unsigned char B64List[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Miscellaneous Utility Functions ///////////////////////////////////////////

char *B64Encode (char *result, unsigned long val)
{
	char res[7];
	int i;
	res[0] = B64List[(val & 0xc0000000) >> 30];
	res[1] = B64List[(val & 0x3f000000) >> 24];
	res[2] = B64List[(val & 0x00fc0000) >> 18];
	res[3] = B64List[(val & 0x0003f000) >> 12];
	res[4] = B64List[(val & 0x00000fc0) >> 6];
	res[5] = B64List[(val & 0x0000003f)];
	res[6] = 0;
	for(i = 0; i < 5; i++) {
		if(res[i] != B64List[0]) {
			strcpy(result, res+i);
			return(result);
		}
	}
	strcpy(result, res+5);
	return(result);
}

void ClearArray(GPtrArray *a)
{
	int i;
	for(i = a->len - 1; i >= 0; i--)  g_ptr_array_remove_index(a, i);
}


int FileExists (char *path)
{
	return(access(path, 0) == 0);
}

int FileSize (char *name)
{
	int eof_ftell;
	FILE *file;
	file = fopen(name, "r");
	if(file == NULL)  return(0L);
	fseek(file, 0, SEEK_END);
	eof_ftell = ftell(file);
	fclose(file);
	return(eof_ftell);
}

void FnExpand (char *rpath, char *defext)
{
	char path[MAXPATH], dir[MAXDIR], file[MAXFILE], ext[MAXEXT], *temp;
	int flags;
	strcpy(path, rpath);
	flags = FnSplit(path, dir, file, ext);
	if(!(flags & FN_DIR)) {
		temp = g_malloc0(MAXPATH);
		getcwd(temp, MAXDIR - 1);
		strcpy(dir, temp);
		g_free(temp);
	}
	if(!(flags & FN_EXT) && defext)  strcpy(ext, defext);
	FnMerge(rpath, dir, file, ext);
}

void FnMerge (char *rpath, char *dir, char *file, char *ext)
{
	int len = 0;
	strcpy(rpath, dir);
	len = strlen(dir);
	if(len && rpath[len-1] != '/')  strcat(rpath, "/");
	strcat(rpath, file);
	if(ext != NULL && strlen(ext)) {
		if(ext[0] != '.')  strcat(rpath, ".");
		strcat(rpath, ext);
	}
}

int FnSplit (char *rpath, char *dir, char *file, char *ext)
{
	int retval = 0, ndx = 0, len;
	strcpy(dir, g_path_get_dirname(rpath));
	if((len = strlen(dir))) {
		if(len == 1 && dir[0] == '.')  dir[0] = 0;
		else  retval += FN_DIR;
	}
	strcpy(file, /*fix me - possibly need to free returned string*/ g_path_get_basename(rpath));
	if(strlen(file))  retval += FN_FILE;
	ndx = StrChrIndex('.', file);
	if(ndx != -1) {
		strcpy(ext, file+ndx+1);
		*(file+ndx) = 0;
	}
	else  ext[0] = 0;
	if(strlen(ext))  retval += FN_EXT;
	return(retval);
}

void GetConfigBool (FILE *fn, char *key, gboolean *bval, gboolean bdef)
{
	char line[1024] = "", txt[1024] = "";
	int len = strlen(key);
	if(fn != NULL) {
		fseek(fn, 0, SEEK_SET);
		while(ReadLine(fn, line, 1023) != -1) {
			if(!strncmp(key, line, len)) {
				strcpy(txt, line+len+1);
				if(!strcmp(txt, "TRUE") || !strcmp(txt, "true") || !strcmp(txt, "True")) *bval = TRUE;
				else  *bval = FALSE;
				break;
			}
		}
	}
	else  *bval = bdef;
}

void GetConfigInt (FILE *fn, char *key, int *nval, int ndef)
{
	char line[1024] = "", txt[1024] = "";
	int len, i, keylen = strlen(key);
	if(fn != NULL) {
		fseek(fn, 0, SEEK_SET);
		while(ReadLine(fn, line, 1023) != -1) {
			if(!strncmp(key, line, keylen)) {
				strcpy(txt, line+keylen+1);
				len = strlen(txt);
				for(i = 0; i < len; i++) {
					switch(txt[i]) {
					  case '0': case '1': case '2': case '3': case '4': case '5': 
					  case '6': case '7': case '8': case '9':  break;
					  default:  len = 0;  break;
					}
				}
				if(len)  *nval = atoi(txt);
				else  *nval = -1;
				break;
			}
			else  *nval = ndef;
		}
	}
	else  *nval = ndef;
}

void GetConfigString (FILE *fn, char *key, char *sval, char *sdef)
{
	char line[1024] = "";
	int len = strlen(key);
	if(fn != NULL) {
		fseek(fn, 0, SEEK_SET);
		while(ReadLine(fn, line, 1023) != -1) {
			if(!strncmp(key, line, len)) {
				strcpy(sval, line+len+1);
				break;
			}
		}
	}
	else {
		if(strlen(sdef))  strcpy(sval, sdef);
		else  strcpy(sval, "");
	}
}

gboolean GetWidgetValBool (GtkWidget *parent, char *name, gboolean defval)
{
	GtkWidget *w;
	if((w = lookup_widget(parent, name)) == 0) {
		g_print("Widget Not Found: %s", name);
		return(defval);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))  return(TRUE);
	return(FALSE);
}

int GetWidgetValInt (GtkWidget *parent, char *name, int defval)
{
	GtkWidget *w;
	gchar *txt = NULL;
	int i, len = 0, nval = 0;
	if((w = lookup_widget(parent, name)) == 0) {
		g_print("Widget Not Found: %s", name);
		return(defval);
	}
	txt = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
	len = strlen(txt);
	for(i = 0; i < len; i++) {
		switch(txt[i]) {
		  case '0': case '1': case '2': case '3': case '4': case '5': 
		  case '6': case '7': case '8': case '9':  break;
		  default:  len = 0;  break;
		}
	}
	if(len)  nval = atoi(txt);
	else  nval = defval;
	g_free(txt);
	return(nval);
}

char *GetWidgetValString (GtkWidget *parent, char *name, char *sval, char *defval)
{
	GtkWidget *w;
	gchar *txt = NULL;
	if((w = lookup_widget(parent, name)) == 0) {
		g_print("Widget Not Found: %s", name);
		strcpy(sval, defval);
		return(sval);
	}
	txt = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
	strcpy(sval, txt);
	g_free(txt);
	return(sval);
}

gboolean IsDirectory (char *filename)
{
	struct stat st;
	if(stat(filename, &st) != 0)  return(FALSE);
	if(S_ISDIR(st.st_mode))  return(TRUE);
	return(FALSE);
}

int ReadLine (FILE *rfile, char *line, int maxlinesize)
{
	int ch, c = 0;
	do {
		ch = getc(rfile);
		if((ch == EOF) || (ch == -1)) return(-1);
		if(ch == 13)  continue;
		if(ch == 10) break;
		line[c] = ch;
		c++;
	} while(c < maxlinesize && ch != 10);
	line[c] = 0;
	return(c);
}

gboolean SetFileAttribs (char *filename, time_t *tstamp, int mode, uid_t owner, gid_t group)
{
	struct utimbuf timebuf;
	if(!FileExists(filename))  return(FALSE);
	timebuf.modtime = *tstamp;
	utime(filename, &timebuf);
	if(!mode)  mode = S_IREAD|S_IWRITE;
	chmod(filename, mode);
	chown(filename, owner, group);
	return(TRUE);
}

int StrChrIndex (char check, char *psearch)
{
	char *p;
	if((p = strchr(psearch, check)) == NULL)  return (-1);
	return((int) (p - psearch));
}

char *StrCvt (char *psource, int conv)
{
	char *pfrom = psource;	       //Next character to get fron source
	char *pto = psource;  // Next position to fill in target
	char c;
	char quote_char = '\0';
	int rlwhite = conv & RLWHITE;
	int rwhite = conv & RWHITE;
	int reduce = (!rwhite) && (conv & REDUCE);
	int ckquotes = conv & NOQUOTE;
	int to_up = conv & TOUP;
	int to_low = conv & TOLOW;
	int rcontrol = conv & RCONTROL;
	int to_case = conv & TOCASE;
	int in_white = FALSE;   	// Not in a white field yet.
	int hit_nonwhite = FALSE;   	// No nonwhite chars encountered.
	int quote_on = FALSE;   	// Not in a quote yet.
	while((c = *pfrom++) != '\0') {
		if(quote_on) {
			*pto++ = c;
			if(c == quote_char)  quote_on = FALSE;
		}
		else if (ckquotes && ((c == '"') || (c == '\''))) {
			*pto++ = c;
			in_white = FALSE;
			hit_nonwhite = TRUE;
			quote_on = TRUE;
			quote_char = c;
		}
		else if(isspace(c) && isascii(c)) {
			if(rwhite)  ;
			else if(rlwhite && !hit_nonwhite)  ;
			else if(reduce) {
				if(in_white)  ;
				else {
					*pto++ = BLANK;
					in_white = TRUE;
				}
			}
			else if(rcontrol && iscntrl(c))  ;
			else  *pto++ = c;
		}
		else if(iscntrl(c) && isascii(c)) {
			in_white = FALSE;
			hit_nonwhite = TRUE;
			if (rcontrol)  ;
			else  *pto++ = c;
		}
		else {
			in_white = FALSE;
			hit_nonwhite = TRUE;
			if(isascii(c)) {
				if(to_up)  c = toupper(c);
				if(to_low)  c = tolower(c);
				if(to_case)  c = islower(c) ? toupper(c) : tolower(c);
			}
			*pto++ = c;
		}
	}
	*pto = '\0';
	if(conv & RTWHITE)
		for (c = *--pto; isspace(c) && isascii(c) && (pto >= psource); c = *--pto)
			*pto = '\0';
	return (psource);
}

char *StrExpand (char *ptarget, char *psource, int incr, int tarsize)
{
	char c;
	int numspaces;
	int len = 0;
	tarsize--;
	while(((c = *psource) != NUL) && (len < tarsize)) {
		switch(c) {
		  case TAB:
			if((incr > 0) && ((numspaces = (incr - (len % incr))) != 0)) {
				if((len += numspaces) < tarsize) {
					while(numspaces--) *ptarget++ = BLANK;
					break;
				}
				else   continue;	// There isn't enough room, so quit
			}
		  default:
			*ptarget++ = c;
			len++;
			break;
		}
		psource++;
	}
	*ptarget = '\0';
	return(c ? psource : NULL);
}

char *StrJustify (char *ptarget, char *psource, char fill, int fldsize, int code)
{
	int diff,i;
	int numleft;
	char *savetarget = ptarget;
	if(fldsize < 0)  fldsize = 0;
	if((diff = ((int) strlen(psource)) - fldsize) >= 0) {
		switch (code) {
		  case JUST_RIGHT:  psource += diff;  break;
		  case JUST_CENTER:  psource += diff >> 1;  break;
		  case JUST_LEFT:
		  default:  break;
		}
		while (fldsize--)  *ptarget++ = *psource++;
	}
	else {
		diff = -diff;
		switch(code) {
		  case JUST_RIGHT:  numleft = diff;  break;
		  case JUST_CENTER:  numleft = diff / 2;  break;
		  case JUST_LEFT:
		  default:  numleft = 0;  break;
		}
		for(i = numleft; i; i--)  *ptarget++ = fill;
		while(*psource)  *ptarget++ = *psource++;
		for (i = diff - numleft; i; i--)  *ptarget++ = fill;
	}
	*ptarget = '\0';
	return(savetarget);
}

int StrPos (char *searchstr, char *targstr, gboolean caseflag)
{
	int c = 0, c1 = 0;
	if(searchstr[0] == 0 || targstr[0] == 0)  return(-1);
	else {
		while(targstr[c] && searchstr[c1]) {
			if(caseflag) {
				if(targstr[c] == searchstr[c1])  c1++;
				else  c1 = 0;
				c++;
			}
			else {
				if(toupper(targstr[c]) == toupper(searchstr[c1]))  c1++;
				else  c1 = 0;
				c++;
			}
		}
		if(searchstr[c1] == 0)	return(c - c1);
		else  return(-1);
	}
}

char *StrTabfy (char *psource, int incr)
{
	char c;
	int col = 0, numblanks = 0;
	char *pfrom = psource;
	char *pto = psource;
	do {
		switch(c = *pfrom++) {
		  case BLANK:
			numblanks++;
			col++;
			if((incr <= 0) || (col % incr == 0)) {
				*pto++ = (char) ((numblanks > 1) ? TAB : BLANK);
				numblanks = 0;
			}
			break;
		  case TAB:
			col = numblanks = 0;
			*pto++ = TAB;
			break;
		  default:
			col++;
			for(; numblanks; numblanks--)  *pto++ = BLANK;
			*pto++ = c;
			break;
		}
	} while(c);
	return(psource);
}


