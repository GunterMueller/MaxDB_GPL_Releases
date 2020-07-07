/*!
  @file           gen54.h
  @author         JoergM
  @special area   disk_io
  @brief          Disk I/O
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef GEN54_H
#define GEN54_H

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "gsp00.h"

#include "vsp002.h"

/*===========================================================================*
*  FUNCTION PROTOTYPES                                                      *
*===========================================================================*/

externC void *en54_Mdevio (void *);

externC int  e54_init_self_io (
                ten50_UKT_Control *         this_ukt,
                tsp00_Int4                  devno,
				tsp00_PageNo                devpno,
				tsp2_io_op_code_Param       op_code,
				int           VAR_VALUE_REF fd, 
				tsp00_ErrText VAR_ARRAY_REF errtext );

externC void e54_int_to_text ( int                         io_err,
                               tsp00_ErrText VAR_ARRAY_REF errtext );

externC void en54DumpNewRteDumpOnly( /* void */ );

#ifdef OSF1
externC int e54a_install_aio_signal_handler( /* void */ );
#endif

/* ven54asnyc */
externC void  e54a_aio_catch_signal ( int sig,
                                      siginfo_t *info ,
                                      void      *ignore ); 
externC int   e54a_get_aio_struct_size ();
externC void  e54a_init_sys_aio_struc () ;
externC int   e54a_install_aio_signal_handler (/* void */);
externC int   e54_asyn_vblockio ( struct DOUBLY_LINKED  * request,
                                  tsp00_ErrText VAR_ARRAY_REF errtext);
externC int   e54_asyn_vasynio ( struct DOUBLY_LINKED  * request,
                                 tsp00_ErrText           errtext);
externC int   e54_asyn_vasynio_result ( struct DOUBLY_LINKED  * request,
                                        tsp00_ErrText VAR_ARRAY_REF errtext);

/* PTS 1115397 */
/*!
   @Description    Test different algorithms for checking on pipe input


  HPUX does not support poll on pipe (always input pending...), while Linux beginning with 2.4.x does NO LONGER support 
  fstat() result on open pipes (always no input). Both are checked and whatever works is used later on (fstat is preferred).
   @Argument       dbname [in] database name is used to create a unique testpipe name

 */

externC void  en54FindPipeCheckAlgorithm(char *dbname);

/*!
  @brief create semaphores and threads of IO worker threads
  The parameter PREALLOCATE_IOWORKER controls, if only DEV0 thread is started or all other possible DEVi threads too.
  In case of a fatal resource allocation problem, the routine writes diagnostics into knldiag
  @return 0 if start succeeded, else kernel should terminate...
 */
externC int   en54CreateIOWorkerThreads();

#endif  /* GEN54_H */
