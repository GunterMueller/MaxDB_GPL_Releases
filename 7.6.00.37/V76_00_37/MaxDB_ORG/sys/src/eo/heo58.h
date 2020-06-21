/*!
  @file           heo58.h
  @author         JoergM
  @special area   v*dev*
  @brief          devspace description ( after xparamfiles has been read )
  @see            

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


#ifndef HEO58_H
#define HEO58_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp200.h"
#include "vsp002.h"
#include "geo002.h"
#include "vsp0058.h"
#ifdef __cplusplus
class Msg_List;
#endif
/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC DEV_DESCR   * sql58k_get_devsp_descr    ( tsp2_dev_type_Param dev_type,
                                                 tsp00_Int4       type_spec_devno );

externC void	      vget_devsize      ( tsp2_dev_type_Param    dev_type ,
                                        tsp00_Int4                 type_spec_devno ,
                                        tsp00_Int4   VAR_VALUE_REF dev_size ,
                                        tsp00_Bool VAR_VALUE_REF ok ) ;

externC void	      vget_devname      ( tsp2_dev_type_Param         dev_type ,
                                        tsp00_Int4                      type_spec_devno ,
                                        tsp00_VFilename VAR_ARRAY_REF dev_name ,
                                        tsp00_Bool      VAR_VALUE_REF ok ) ;
#ifdef __cplusplus
externC void	      vnew_devspace     ( tsp2_dev_type_Param         dev_type ,
                                        tsp00_Int4                      type_spec_devno ,
                                        tsp00_VFilename VAR_ARRAY_REF dev_name ,
                                        tsp00_Int4                      dev_size ,
                                        tsp00_Bool      VAR_VALUE_REF ok,
                                        Msg_List                       &errList) ;
#endif
externC void	      vget_uniqe_id     ( tsp00_Line      VAR_ARRAY_REF uniqe_id );

externC void	      vrevoke_devspace  ( tsp2_dev_type_Param  dev_type ,
                                        tsp00_Int4               type_spec_devno );

externC void	      vinit_akdump_ptr  ( void  *acv_pointer ,      /* CR 1000209 */
                                        void (*ak_dump_proc)() ); /* CR 1000209 */

externC void	      *vGetAcvPtrFromCurrentTask() ;                /* CR 1104345 */

externC void	      vgetrteinfo ( tsp0058_RTEInfo VAR_VALUE_REF RTEInfo );

externC void          vdiaginit       ( tsp00_Bool      VAR_VALUE_REF traceflag,
					                    tsp00_Bool      VAR_VALUE_REF dumpflag,
						                tsp00_VFilename VAR_ARRAY_REF vtracename,
						                tsp00_Int4        VAR_VALUE_REF vtracesize,
						                tsp00_VFilename VAR_ARRAY_REF vdumpname
            					                );

/* PTS 1110561 */
/*!
   @brief          Called to set task in alive state, to prevent from aborting a
              running task during shutdown (i.e. Tracewriter...).
   @param          pid [in] task to check
   @return         none

 */

externC void        vpid_alive          ( tsp00_TaskId pid );

/* PTS 1110561 */
/*!
   @brief          Called to keep tracewriter in alive state, to prevent from aborting it
              during shutdown (same as vpid_alive(TraceWriterPid)) .
   @return         none

 */

externC void        vtracewriter_alive  ( );

/* PTS 1108768 */
/*!
   @Description    Initialization Function to retrieve Trace and Dump Information


  This function also formats the knltrace devspace, thereby reserving space
  on disk for it.
   @param          vtraceName [out] Name of Tracefile (knltrace)
   @param          vdumpName [out] Name of Dumpfile (knldump)

 */

externC void        vInitTraceAndDump ( tsp00_VFilename VAR_ARRAY_REF vtraceName,
                                        tsp00_VFilename VAR_ARRAY_REF vdumpName
            					                );
/*!
   @Description    Request current status of dump wanted flag
   @Return value   true if dump of kernel structures is wanted, false if not

 */

externC tsp00_Bool  vIsDumpWanted  ( );

/*!
   @Description    Request current status of trace wanted flag
   @Return value   true if writing kernel trace is wanted, false if not

 */

externC tsp00_Bool  vIsTraceWanted ( );

/*!
   @Description    Check if given names has duplicate in devspacesArray
   @param          devspacesArray [in] Array of devspace description arrays
   @param          szDevspaceName [in] Devspacename to check
   @Return value   true is no duplicate, false if duplicate

 */

externC tsp00_Bool  eo58CheckDuplicateDevSpaceName( DEVSPACES_DESCR *devspacesArray,
                                                    tsp2_devnamec  szDevspaceName );


#endif  /* HEO58_H */
