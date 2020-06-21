/* typedef FILE  *filebio_p, **filebio_pp; 

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

