/*!**********************************************************************

  module: WAHTTP_Misc.h

  -----------------------------------------------------------------------

  responsible:	Tony Guepin
				Markus Oezgen

  special area: SAP DB WWW

  description:  

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end





************************************************************************/

#ifndef WAHTTP_MISC_H
#define WAHTTP_MISC_H

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "hsp77.h"

/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

#ifdef WIN32
#define DIRPATH_SEP	'\\'
#else
#define DIRPATH_SEP	'/'
#endif

#define WORKER_STACK_WAHTTP	1024*256

#define MAX_FILENAME_LEN	1024 /* max. absolute path length */
#define MAX_ERROR_TEXT_LEN	255

#define MAXURISIZE			2000
#define MAX_ERRORTEXT_LG	100

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef WAHTTP_MIN
#define WAHTTP_MIN(a,b)		(((a)<(b))?(a):(b))
#endif

#ifndef strcasecmp
#ifdef WIN32
#define strcasecmp(s1, s2)	stricmp(s1, s2)
#endif
#endif

#ifndef strncasecmp
#ifdef _WIN32
#define strncasecmp(s1, s2, n)	strnicmp(s1, s2, n)
#endif
#endif


typedef struct st_excl	*Excl;
typedef struct st_sem	*Sem;

int initexcl
(
 Excl		*excl,
 unsigned short ErrorTextSize,
 char		ErrorText[]
);

void begexcl
(
 Excl	excl
);

void endexcl
(
 Excl
 excl
);

void exitexcl
(
 Excl excl
);

int initsem
(
 Sem		*sem,
 unsigned short	ErrorTextSize,
 char		ErrorText[]
);

void begsem
(
 Sem		sem
);

void endsem
(
 Sem		sem
);

void exitsem
(
 Sem		sem
);

char *strcpymax
(
 size_t  m,
 char	*dst,
 char	*src
);

char *strcatmax
(
 size_t  m,
 char	*dst,
 char	*src
);

char *strnstrcase
(
 char   *string,
 size_t  stringlg,
 char   *search
);

char *trim
(
 char Str[]
);

typedef struct st_dynstring {
	char   *str;
	size_t  sz;
	size_t  lg;
} *DynStr;

DynStr DSinit
(
 void
);

DynStr DSextent
(
 DynStr	*ds,
 size_t	sz
);

void DSdelete
(
 DynStr ds
);

void Basename
(
 char	Basename[],
 char	Path[],
 char	PathSeparator
);

struct st_data {
	void   *H;
	size_t  (*read)(struct st_data *This, void *Buf, size_t Lg);
	int     (*rewind)(struct st_data *This);
	int     (*destroy)(struct st_data *This);
};

struct st_data *CreateData
(
 void
);

void DeleteData
(
 struct st_data *Data
);


#if defined(__cplusplus)
extern "C" {
#endif

    int GetOpt( int          argc,
                char* const *argv,
                const char  *optstr,
                char        **optarg
    );

#if defined(__cplusplus)
}
#endif


#endif
