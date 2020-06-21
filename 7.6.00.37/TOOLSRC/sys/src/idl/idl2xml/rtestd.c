/* IDENT ('V0.0-00') HOE 24.06.93 module generation 

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/
/* #module rtestd O Filename:RTE:RTESTD.C */
/* Standard Routines */

#ifndef OS_VMS
#include <time.h>
#ifndef FREEBSD         // BSD requires stdlib only for malloc
#include <malloc.h>
#endif
#else
#include <stdlib.h>
#endif

#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/rte.h"

#define M__NAME "RTESTD"

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section for Locals                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of External Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of Internal Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Routine Section                                                            */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/******************************************************************************/
/* Memory Allocation                                                          */
/******************************************************************************/

/* TEST TEST FJH TESTING THE ALLOC VERSUS FREE BALANCE */
#ifdef WRITE_TO_C_TMP
long isfileopen; /* FJH */
FILE *testfile;  /* FJH */
#endif
 
  s_uint   tstcnt;

void*  RteStdMalloc
(
  l_uint  size                           /* requested memory size */
)

{
  s_uint   ssize;
  void    *alloc_p;/* FJH */

#ifdef WRITE_TO_C_TMP
  if (isfileopen != 1010101) /* FJH */
  {/* FJH */
	  isfileopen = 1010101;
	  testfile = fopen("c:\\tmp\\testfree.tst", "w");/* FJH */
    tstcnt = 0;
  }/* FJH */
  tstcnt++;
  if ( _CrtCheckMemory()) fprintf(testfile, "heapcheck ok\n");/* FJH */
  else fprintf(testfile, "heapcheck failed\n");/* FJH */
#endif


  /* type conversion to small int 
     dies when not tested !!
	 pointer is OK but length IS NOT  KF & FJH 8-11-1996 */
  if (size > 65535) return NULL;

  if ( size == 0 ) size =1; /* FF */
  ssize = (s_uint) size;
  
  /* return (void*  ) malloc (ssize);  */
#ifdef MS_DOS
  alloc_p = (void*  ) malloc (ssize);/* FJH */
#else
  alloc_p = (void*  ) malloc (ssize+8);/* FJH */
  *(long*)alloc_p = ssize;
  alloc_p = (void *)((char*)alloc_p + 8);
#endif

#ifdef WRITE_TO_C_TMP
  if (tstcnt == 43) 
  {
    tstcnt = 43;
  }
  if ( _CrtCheckMemory()) fprintf(testfile, "heapcheck ok\n");/* FJH */
  else fprintf(testfile, "heapcheck failed\n");/* FJH */
  fprintf(testfile, "%10.10ld %s %10.10ld\n", (long)alloc_p, "ALLOC", size);/* FJH */
  fflush(testfile);/* FJH */
#endif

  return alloc_p;/* FJH */
} /* END OF RteStdMalloc */

/*b_uint_p RteStdHalloc
(  */                                               
/*  l_uint  r_blk_ct,     */         /* requested block count */
/*  s_uint  r_blk_sz      */         /* requested block size  */
/*)

{  */       
  
/*    
 return (b_uint_p) _halloc ( r_blk_ct, r_blk_sz);
 return 0;

  
} */ /* END OF RteStdMalloc */

void RteStdFree(void *ptr)
{
#ifdef WRITE_TO_C_TMP
  fprintf(testfile, "%10.10ld %s\n", (long)ptr, "FREE");/* FJH */
  fflush(testfile);/* FJH */
#endif
#ifdef MS_DOS
  free(ptr);
#else
  if ( ptr )
    free((char*)ptr - 8);
#endif
}

void* RteStdRealloc
(
  void           *ptr,                 /* existing memory block */
  unsigned long  size                  /* requested memory size */
)
{
 void           *pt;
 size_t         sz;

 if (ptr != 0)
#ifdef MS_DOS
   sz = _msize (ptr);
#else
   sz = *((long*)ptr - 8);
#endif
 else
   sz = 0;

  if ( size == 0 ) size =1; /* FF */

#ifdef MS_DOS
 pt = realloc (ptr, size); 
#else
 if ( ptr )
   pt = realloc ((char*)ptr - 8,size+8);
 else
   pt = realloc (ptr,size+8);

 *(long*)pt = size;
 pt = (void*)((char*)pt + 8);
#endif
 if (pt == 0)
   {
	/* try malloc */
   pt = 0;
#ifdef MS_DOS
   pt = realloc (pt, size);
#else
   if ( ptr )
     pt = realloc ((char*)ptr-8,size+8);
   else
     pt = realloc (ptr,size+8);
     *(long*)pt = size;
   pt = (void*)((char*)pt + 8);
#endif
   if (pt == 0) return pt; /*  no memory available */
   
#ifdef WRITE_TO_C_TMP
   if ( _CrtCheckMemory()) fprintf(testfile, "heapcheck ok\n");/* FJH */
   else fprintf(testfile, "heapcheck failed\n");/* FJH */
   fprintf(testfile, "%10.10ld %s %10.10ld\n", (long)pt, "REALLOC", size);/* FJH */
   fprintf(testfile, "%10.10ld\n", (long)ptr, "REALLOC-FREE");/* FJH */
   fflush(testfile);/* FJH */
#endif
   
   /* move contents of old memory */
   if (ptr)
     memcpy (pt, ptr, sz);

    /* free old memory */
#ifdef MS_DOS
     free (ptr);
#else
     if ( ptr )
       free ((char*)ptr-8);
#endif
	}

  return pt;
}

/******************************************************************************/
/* Show time in seconds since last call of RteInitTimer                       */
/******************************************************************************/


void RteInitTimer
(
  unsigned long *  w_time_p                        /* pointer to save time   */
)

{

  
  time ((time_t*) w_time_p);

              
} /* END OF RteInitTimer */

/******************************************************************************/
/* Show time in seconds since last call of RteInitTimer                       */
/******************************************************************************/


void RteShowTimer
(
  unsigned long  *r_time_p                        /* pointer to save time   */
)

{
  time_t   etime;
  
  time (&etime);
/*              
  printf ("Elapsed time: %d secs\n", (l_uint) etime - *r_time_p);            
*/              
} /* END OF RteShowTimer */

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* End of Module                                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


