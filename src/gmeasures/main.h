/***************************************************************************
    file        : main.h
    project     : gMeasures
    date        : unknown
    revised     : 2002.03.25 (v0.5)
                  2006.11.15 (v0.6)
                  2007.02.01 (v0.7)
    copyright   : (C)2007 by Paul Schuurmans
    email       : paul.schuurmans@home.nl
 ***************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
*/
 
#define APPNAME		"gMeasures v0.7"

#define OF_READ		0
#define OF_WRITE	1

#define MAXPATH		260

typedef struct {
	char Unit1[32];
	gdouble Equals;
	char Unit2[32];
} TMeasuresInfo;

enum {
	LIST_UNIT1,
	N_COLUMNS
};

void AppCalculate (void);
TMeasuresInfo *AppGetMeasure (void);
void AppNumPadClicked (char *key);
void AppReadWriteConfig (int opmode);
void AppSetUnit1 (void);
void AppUpdateUnit2 (void);

