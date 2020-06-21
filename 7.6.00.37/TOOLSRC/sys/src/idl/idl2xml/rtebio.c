/* IDENT ('1.0-00')  HOE 06.03.91  new module 

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
/* #module rtebio O Filename:RTE:RTEBIO.C */
/* Block Input and Output Routines */

#include <string.h>

#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/rte.h"
#include "idl/idl2xml/rtebio.h"

#define M__NAME "RTEBIO"

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
/* Allocation of a File                                                     */
/****************************************************************************/

rc_t RteBioAllocate
(
 strng         r_filename,           /* name of file to be allocated        */
 s_uint        r_filecap,            /* file capacity                       */
 s_uint        r_blksize             /* block size                          */
)
{
  int	 fs_rtc;
  file_p all_fil;
  char	 *ptr;
  /* add */ l_int offset;

  if (0 != (ptr = strstr (r_filename, " "))) *ptr = '\0';
  if (0 == (all_fil = fopen (r_filename, "r+b")))
  {
    if (0 == (all_fil = fopen (r_filename, "wb"))) return (RcRteError);
  }
  if (r_filecap)
  {
    
    offset  = (l_uint) r_blksize;
    offset *= (l_uint) r_filecap;
    offset--;
    fs_rtc  = fseek (all_fil, offset, SEEK_SET);

    if (0 != fs_rtc)
    {
      fclose (all_fil);
      remove (r_filename);
      return (RcRteError);
    }
    else
    {
      fs_rtc = fwrite (" ", 1, 1, all_fil);
    }
  }
  fclose (all_fil);
  return (RcRteOk);

} /* END OF RteBioAllocate */

/****************************************************************************/
/* Open of a file                                                           */
/****************************************************************************/

rc_t RteBioOpen
(
  strng		r_filename,	/* name of file to be opened		      */
  file_p *	w_channel_p	/* pointer to channel			      */
)
{
  char	*ptr;

  if (0 != (ptr = strstr (r_filename, " "))) *ptr = '\0';
  if (0 == (*w_channel_p = fopen (r_filename, "r+b")))
    return (RcRteError);
  return (RcRteOk);

} /* END OF RteBioOpen */

/****************************************************************************/
/* Close of a File                                                          */
/****************************************************************************/

rc_t RteBioClose
(
  file_p *	w_channel_p	/* pointer to channel			      */
)
{
  rc_t  rc;
  rc = RcRteOk;
  if (0 != fclose (*w_channel_p)) rc = RcRteError;
  *w_channel_p = 0;
  return (rc);

} /* END OF RteClose */

/****************************************************************************/
/* Deletion of a File                                                       */
/****************************************************************************/

rc_t RteBioDelete
(
  strng		r_filename	/* name of file to delete		      */
)
{
  char	*ptr;

  if (0 != (ptr = strstr (r_filename, " "))) *ptr = '\0';
  if (0 == remove (r_filename)) return (RcRteOk);
  return (RcRteError);

} /* END OF RteBioDelete */

/****************************************************************************/
/* Block Read/Write                                                         */
/****************************************************************************/

rc_t RteBioBlock
(
 file_p        r_channel,            /* channel number	                    */
 s_uint        r_blkno,              /* block number to read or write       */
 s_uint        r_blksize,            /* block size                          */
 io_op_code_t  r_op_code,	         /* file operation code                 */        
 b_uint_p      v_buffer_p            /* pointer to read/write buffer        */
)
{
  size_t	rc;
  int		fs_rtc;
  l_int 	offset;


  offset  =  r_blkno - 1;
  offset  *= r_blksize;
  
  fs_rtc = fseek (r_channel, offset, SEEK_SET);
  if (fs_rtc) return (RcRteError);
  switch (r_op_code)
  {
    case bio_read:
    {
      rc = fread (v_buffer_p, r_blksize, 1, r_channel);
      if (rc != 1)
      {
       return (RcRteError);
      }
      break;
    }

    case bio_write:
    {
      rc = fwrite (v_buffer_p, r_blksize, 1, r_channel);
      if (rc != 1)
      {
	return (RcRteError);
      }
      break;
    }
  }
  return (RcRteOk);

} /* END OF RteBioBlock */




/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* End of Module                                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

