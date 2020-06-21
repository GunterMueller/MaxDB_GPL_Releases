/*


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
#ifndef __RTE_INCLUDED
#define __RTE_INCLUDED

#define RcRteOk		1
#define RcRteError	4

#ifndef rc_t_Declared
#define rc_t_Declared
typedef unsigned short rc_t;
#endif

#include <stdio.h>

typedef FILE  *file_p, **file_pp;

typedef struct
{
  unsigned short lg;                /* length of element                    */
  unsigned short ty;                /* data type code                       */
  unsigned char  *pt;               /* Pointer to element                   */
} *st_d_p, st_d;

typedef enum
{
  fil_get, fil_put, fil_new
} fil_op_t;

unsigned short RteFilOpen
(
 fil_op_t      r_fil_op,	    /* file operation code                  */        
 char         *r_filename,          /* name of file to be opened            */
 file_pp       w_channel_p          /* pointer to channel                   */
);

unsigned short RteFilClose
(
  file_pp      w_channel_p	         /* pointer to channel                 */
);

unsigned short RteFilRewind
(
 file_p        r_channel             /* channel number	                   */
);

unsigned short RteFilDelete
(
  char	   *r_filename	             /* name of file to delete	           */
);

unsigned short RteFilRename
(
  char	   *r_oldname,	             /* name of file to be renamed         */
  char	   *r_newname	             /* new filename                       */
);

unsigned short RteFilRecord
(
 fil_op_t       r_fil_op,	         /* file operation code                 */        
 file_p         r_channel,           /* channel number	                    */
 unsigned short r_bufsz,             /* buffer size                         */
 st_d_p         v_buf_dp             /* descriptor read/write buffer        */
);
         
typedef struct
{
 unsigned char  id;                  /* ID of attribute                     */
 char           nm[15];              /* nam of attribute                    */
 char           vc[256];             /* attribute value in character format */
 unsigned long  vi;                  /* attribute value in integer format   */
 unsigned long  df;                  /* default value for attribute         */
 unsigned long  rc;                  /* return code for attribute           */
} st_ParAtL, *st_ParAtLp;

typedef struct
{
 unsigned short ct;                  /* number of row in attribute list     */
 unsigned short ty;                  /* type of data element                */
 st_ParAtLp     pt;                  /* pointer to data element             */
} st_ParLdsc, *st_ParLdscp;

unsigned short RteArg
(
 st_ParLdscp ArL_dp,
 int         argc,
 char        **argv
);

void* RteStdMalloc
(
 unsigned long  size                           /* requested memory size */
);

void RteStdFree(void *ptr);

void RteShowTimer
(
  unsigned long  *r_time_p                        /* pointer to save time   */
);

void* RteStdRealloc
(
  void           *ptr,                 /* existing memory block */
  unsigned long  size                  /* requested memory size */
);

void RteInitTimer
(
  unsigned long  *w_time_p                        /* pointer to save time   */
);

#ifndef __RTEBIO
#define __RTEBIO

typedef FILE  *filebio_p, **filebio_pp;

rc_t RteBioAllocate
(
 strng         r_filename,           /* name of file to be allocated        */
 s_uint        r_filecap,            /* file capacity                       */
 s_uint        r_blksize             /* block size                          */
);

rc_t RteBioOpen
(
 strng         r_filename,          /* name of file to be opened            */
 file_p *      w_channel_p          /* pointer to channel                   */
);

rc_t RteBioClose
(
  file_p *	w_channel_p	/* pointer to channel			      */
);

rc_t RteBioDelete
(
  strng		r_filename	/* name of file to delete		      */
);

rc_t RteBioBlock
(
 file_p        r_channel,            /* channel number	                    */
 s_uint        r_blkno,              /* block number to read or write       */
 s_uint        r_blksize,            /* block size                          */
 io_op_code_t  r_op_code,	         /* file operation code                 */        
 b_uint_p      v_buffer_p            /* pointer to read/write buffer        */
);

rc_t RteBloAllocate
(
 strng         r_filename,           /* name of file to be allocated        */
 l_uint        r_filecap,            /* file capacity                       */
 l_uint        r_blksize             /* block size                          */
);

rc_t RteBloOpen
(
 strng         r_filename,          /* name of file to be opened            */
 file_p *      w_channel_p          /* pointer to channel                   */
);

rc_t RteBloClose
(
  file_p *	w_channel_p	/* pointer to channel			      */
);

rc_t RteBloDelete
(
  strng		r_filename	/* name of file to delete		      */
);

rc_t RteBloBlock
(
 file_p        r_channel,            /* channel number	                    */
 l_uint        r_blkno,              /* block number to read or write       */
 l_uint        r_blksize,            /* block size                          */
 io_op_code_t  r_op_code,	         /* file operation code                 */        
 b_uint_p      v_buffer_p            /* pointer to read/write buffer        */
);


#endif
#endif
