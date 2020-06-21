/* IDENT ('1.0-00')  HOE 25.09.93  new module 

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
/* #module rtefil O Filename:RTE:RTEFIL.C */
/* Line Input and Output Routines */

#include <string.h>
#include <stdio.h>
#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/rte.h"

#define M__NAME "RTEFIL"

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of External Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of Internal Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/



/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Routine Section                                                          */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


/****************************************************************************/
/* Open of a file                                                           */
/****************************************************************************/

unsigned short RteFilOpen
(
 fil_op_t      r_fil_op,	    /* file operation code                  */        
 char         *r_filename,          /* name of file to be opened            */
 file_pp       w_channel_p          /* pointer to channel                   */
)
{
  char	*ptr;
  char  op[4];
  /* added fil_new to empty old file
     and put everything in a switch statement 
	 FJH 02-10-1996 */
  memset (op, 0, sizeof (op));
  switch (r_fil_op)
  {	  
    case fil_new: strcpy (op, "wb"); break;
    case fil_get: strcpy (op, "rb"); break;
    case fil_put: 
#ifdef OS_VMS
    default:      strcpy (op, "w");
#else  
    /* this is stupid, it will break down on every Ctrl Z 
	   in binary files, so I changed it to r+b
	   FJH 07-10-1996 
     Problems with existing programs so set to w+b (with clear)
     FJH 21-01-1997
    default:      strcpy (op, "w+t");*/
/*    default:      strcpy (op, "r+b"); */
    default:      strcpy (op, "w+b");
#endif                                                  
  }   
  
  if (0 != (ptr = strstr (r_filename, " "))) *ptr = '\0';
  if (NULL == (*w_channel_p = fopen (r_filename, op)))
  {
    /* if not readplus binary try to create binary
    	   FJH 07-10-1996 
    */
    if (!strcmp(op,"r+b")) 
    {
      if (NULL == (*w_channel_p = fopen (r_filename, "wb")))
        return (RcRteError);
    }
    else return (RcRteError);
  }
  return (RcRteOk);

} /* END OF RteFilOpen */

/****************************************************************************/
/* Close of a File                                                          */
/****************************************************************************/

unsigned short RteFilClose
(
  file_pp      w_channel_p	     /* pointer to channel                  */
)
{
  rc_t  rc;
  rc = RcRteOk;
  if (EOF == fclose (*w_channel_p)) rc = RcRteError;
  *w_channel_p = 0;
  return rc;

} /* END OF RteClose */

/****************************************************************************/
/* Deletion of a File                                                       */
/****************************************************************************/

unsigned short RteFilDelete
(
  char	   *r_filename	/* name of file to delete		      */
)
{
  char	*ptr;

  if (0 != (ptr = strstr (r_filename, " "))) *ptr = '\0';
  if (0 == remove (r_filename)) return (RcRteOk);
  return (RcRteError);

} /* END OF RteFilDelete */

/****************************************************************************/
/* Rewind a File                                                            */
/****************************************************************************/

unsigned short RteFilRewind
(
 file_p        r_channel             /* channel number	                    */
)
{
 
 rewind (r_channel);
 return RcRteOk;
 
} /* END OF RteFilRewind */


/****************************************************************************/
/* Block Read/Write                                                         */
/****************************************************************************/

unsigned short RteFilRecord
(
 fil_op_t       r_fil_op,	     /* file operation code                 */        
 file_p         r_channel,           /* channel number	                    */
 unsigned short r_bufsz,             /* buffer size                         */
 st_d_p         v_buf_dp             /* descriptor read/write buffer        */
)
{

 char     *c_rc;
 s_uint   lg;                     
 b_uint_p pt;
 int      rc;
 b_uint   sav[3];
      
   switch (r_fil_op)
  {
    case fil_get:
    {
      /* 11-01-1999 FJH clear buffer to make it posible to read in null characters */
      memset(v_buf_dp->pt, 00, r_bufsz); 
      c_rc = fgets ( (char_p)v_buf_dp->pt, r_bufsz, r_channel);
      v_buf_dp->lg = 0;
      if (NULL ==  c_rc) return RcRteError;
      /* 11-01-1999 FJH detect real length by walking backwards  */
      lg = r_bufsz - 1;
      while((*(((char_p)v_buf_dp->pt)+lg) == 0) && lg) lg--; 
/*      lg = strlen ((char_p)v_buf_dp->pt);  */
      if ('\n' == *(v_buf_dp->pt + lg - 1)) lg--;
      if ('\r' == *(v_buf_dp->pt + lg - 1)) lg--;
      v_buf_dp->lg = lg;
      break;
    }

    case fil_put:
    {
      if (r_bufsz <= (v_buf_dp->lg + 3)) return RcRteError;
      pt = v_buf_dp->pt + v_buf_dp->lg;

      sav[0] = *pt;
      sav[1] = *(pt + 1);
      sav[2] = *(pt + 2);
      
      /* strcpy ((char_p)pt, "\n");*/
      /* changed to \r\n while the stream type is untranslated 
      BUT WATCH OUT: if the file is really binary this \r is 
      added to EVERY \n on EVERY load and save !!!!!
      CHANGED ONLY FOR DOS 
      FJH 22-01-1997 */
#ifdef MS_DOS
      strcpy ((char_p)pt, "\r\n");
#else  
      strcpy ((char_p)pt, "\n");
#endif
      
      rc = fputs ( (char_p)v_buf_dp->pt, r_channel);
      if (EOF == rc) return RcRteError;

      *pt       = sav[0];
      *(pt + 1) = sav[1];
      *(pt + 2) = sav[2];

      break;
    }
  }
  return (RcRteOk);

} /* END OF RteFilRecord */


unsigned short RteFilRename
(
  char	   *r_oldname,	/* name of file to be renamed        */
  char	   *r_newname	/* new filename                      */
)
{

  if (0 == rename (r_oldname, r_newname)) return (RcRteOk);
  return (RcRteError);

} /* END OF RteFilRename */

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* End of Module                                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
