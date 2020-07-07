/*!**********************************************************************

  module: WAHTTP_Memory.h

  -----------------------------------------------------------------------
  
  responsible:  Markus Oezgen

  special area: SAP DB WWW Web Server

  description:  Memory Management

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#define MODULE_NAME "WAHTTP_Memory.c"

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include <stdlib.h>

#ifdef WIN32
#define _DEBUG
#include <crtdbg.h>
#endif

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

static allocCount = 0;

static Excl MallocExcl;

#define MBS 1000

struct st_mallocs
{
 void	*p;
 char	*f;
 long	l;
 long	w;
};

static struct st_mallocs *mallocs = NULL;
static long top = -1;
static long size = 0;
static long Which = 0;
static int Initialized = FALSE;

void wahttp_mallocinit(tsp00_BoolAddr	pbOk
)
{
  initexcl(&MallocExcl, 0, NULL);
 *pbOk = TRUE;
  Initialized = TRUE;
}

void wahttp_allocat( sapdbwa_Int4	lSize,
		     sapdbwa_UInt1	**ppucObjPtr,
		     tsp00_BoolAddr	pbOk,
		     char		*FILE,
		     long		LINE
)
{
 long	i;

    allocCount++;
    *ppucObjPtr = malloc( lSize );
    *pbOk = (tsp00_Bool) (ppucObjPtr != NULL);

 if (!Initialized)
	return;

 begexcl(MallocExcl);

 Which++;

 if (!*ppucObjPtr);
 
 for (i=0; top>=0&&i<=top; i++)
	if (NULL == mallocs[i].p)
	{
		mallocs[i].p = *ppucObjPtr;
		mallocs[i].f = FILE;
		mallocs[i].l = LINE;
		mallocs[i].w = Which;

		endexcl(MallocExcl);

		return;
	}

 ++top;

 if (top >= size)
 {
	size += MBS;
	mallocs = realloc(mallocs, sizeof(*mallocs)*size);
 }

 mallocs[top].p = *ppucObjPtr;
 mallocs[top].f = FILE;
 mallocs[top].l = LINE;
 mallocs[i].w	= Which;

 endexcl(MallocExcl);
}


void wahttp_reallocat( sapdbwa_Int4     lSize,
		       sapdbwa_UInt1  **ppucObjPtr,
		       tsp00_BoolAddr   pbOk,
		     char		*FILE,
		     long		LINE
)
{
 long	i;
 sapdbwa_UInt1  *was = *ppucObjPtr;

    *ppucObjPtr = realloc( *ppucObjPtr, lSize );
    *pbOk = (tsp00_Bool) (ppucObjPtr != NULL);

 if (!Initialized)
	return;

 begexcl(MallocExcl);

 if (was)
 {
	char	tmp[1000];

	 for (i=0; i<=top; i++)
		if (was == mallocs[i].p)
		{
			mallocs[i].p = *ppucObjPtr;
			mallocs[i].f = FILE;
			mallocs[i].l = LINE;

			endexcl(MallocExcl);

			return;
		}

	sprintf(tmp, "File:%.40s Line:%d: This realloc is NOT ok", FILE, LINE);
	wd20WriteLogMsg( tmp );

	endexcl(MallocExcl);

	return;
 }
 else
 {
     allocCount++;
	 for (i=0; top>=0&&i<=top; i++)
		if (NULL == mallocs[i].p)
		{
			mallocs[i].p = *ppucObjPtr;
			mallocs[i].f = FILE;
			mallocs[i].l = LINE;
			mallocs[i].w = Which;

			endexcl(MallocExcl);

			return;
		}

	 ++top;

	 if (top >= size)
	 {
		size += MBS;
		mallocs = realloc(mallocs, sizeof(*mallocs)*size);
	 }

	 mallocs[top].p = *ppucObjPtr;
	 mallocs[top].f = FILE;
	 mallocs[top].l = LINE;
	 mallocs[i].w	= Which;
 }
 endexcl(MallocExcl);
}

static void verifymallocs
(
);

void wahttp_free( sapdbwa_UInt1*  pucObjPtr,
		     char		*FILE,
		     long		LINE
)
{
 long	i;
 int	FreeOk = FALSE;
 char	tmp[1000];

    tsp00_Bool dump = (pucObjPtr == (sapdbwa_UInt1*) 17);

    if (dump) {
#ifdef sapdbwa_MEMCHECK
        _CrtDumpMemoryLeaks();
#endif        

	begexcl(MallocExcl);

	verifymallocs();

	endexcl(MallocExcl);
    };


 if (!Initialized)
	FreeOk = TRUE;
 else
 {
	begexcl(MallocExcl);

	for (i=0; i<=top; i++)
		if (pucObjPtr == mallocs[i].p)
		{
			mallocs[i].p = NULL;
			if (i == top)
				top--;
			FreeOk = TRUE;
		}

	endexcl(MallocExcl);
 }
   
 if (FreeOk)
 {
    allocCount--;
    free( pucObjPtr );
 }
 else
 {
	sprintf(tmp, "File:%.40s Line:%d: This free is NOT ok", FILE, LINE);
	wd20WriteLogMsg( tmp );
 }
}

static int ourmallocsfreed
(
 int	Lg,
 char	**FILE,
 long	*LINE,
 long	*Which,
 long	*rLg
)
{
 long	i;

 if (!Initialized)
	return(1);

 *rLg = 0;

 for (i=0; i<top; i++)
	if (mallocs[i].p)
	{
		if (*rLg < Lg)
		{
			FILE[*rLg] = mallocs[i].f;
			LINE[*rLg] = mallocs[i].l;
			Which[*rLg] = mallocs[i].w;
			*rLg += 1;
		}
		else
			return(0);
	}
 if (*rLg > 0)
	return(0);
 else
	return(1);
}

static void verifymallocs
(
)
{
#define howmanymallocs	10000
 char	*files[howmanymallocs];
 long	lines[howmanymallocs];
 long	which[howmanymallocs];
 long	lg, i;
 char	tmp[1000];


 if (!Initialized)
	return;

 if (!ourmallocsfreed(howmanymallocs, files, lines, which, &lg))
 {
	sprintf(tmp, "Mallocs were not freed (%ld)", lg);
	wd20WriteLogMsg( tmp );
	for (i=0; i<lg; i++)
	{
		sprintf(tmp, "File:%.40s Line:%d Which:%d", files[i], lines[i], which[i]);
		wd20WriteLogMsg( tmp );
	}
 }
 else
 {
	sprintf(tmp, "All Mallocs were Freed");
	wd20WriteLogMsg( tmp );
 }

}