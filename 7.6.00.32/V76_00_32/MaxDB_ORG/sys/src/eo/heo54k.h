/*!
  @file           heo54k.h
  @author         JoergM
  @special area   Kernel I/O
  @brief          Functions used inside SAPDB Kernel for I/O
  @first created  000-10-13  17:18

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



#ifndef HEO54K_H
#define HEO54K_H

#include "gsp00.h"
#include "vsp002.h"

/* --- Disk IO ---------------------------------------------------------------- */

typedef unsigned char           V2_RETURN;
typedef unsigned char           V2_DEVNO;

typedef V2_RETURN               tsp_v2_return;
typedef V2_DEVNO                tsp_v2_devno;

#define DEV_READ                0
#define DEV_WRITE               1

#define V2_OK                   0
#define V2_NOTOK                1
#define V2_NOSUPPORT            2

#define V2_DEV1                 0
#define V2_DEV2                 1
#define V2_DEV1_2               2


/* --- VOS54KC ---------------------------------------------------------------- */

externC void           vdevsize          ( tsp00_Int4                  type_spec_devno,
                                           tsp2_dev_type_Param         devtype,
                                           tsp00_Int4    VAR_VALUE_REF devcapacity,
                                           tsp00_ErrText VAR_ARRAY_REF errtext,
                                           tsp00_Bool    VAR_VALUE_REF pOk );

externC void           vdevsizeWithImage ( tsp00_Int4                  type_spec_devno,
                                           tsp2_dev_type_Param         devtype,
                                           tsp00_Int4    VAR_VALUE_REF devcapacity,
                                           tsp00_TaskId                pidResponsibleForCall,
                                           char                       *formattingPageImage,
                                           tsp00_ErrText VAR_ARRAY_REF errtext,
                                           tsp00_Bool    VAR_VALUE_REF pOk  );

externC void           v2info            ( tsp00_Int4                  devno1,
                                           tsp00_Int4                  devno2,
                                           tsp00_ErrText VAR_ARRAY_REF errtext,
                                           V2_RETURN     VAR_VALUE_REF rtc );

/*! @brief check if volume can be opened 
    @param type_spec_devno [in] type specific device number
    @param devtype [in] volume type
    @pOk [out] true if open check succeeded, false if open check failed
 */
externC void           vcheckopenvolume  ( tsp00_Int4                  type_spec_devno,
                                           tsp2_dev_type_Param         devtype,
                                           tsp00_Bool    VAR_VALUE_REF pOk );

externC void           vattach           ( tsp00_Int4                  type_spec_devno,
                                           tsp2_dev_type_Param         devtype,
                                           tsp00_Int4    VAR_VALUE_REF devno,
                                           tsp00_TaskId                taskId,
                                           tsp00_ErrText VAR_ARRAY_REF errtext,
                                           tsp00_Bool    VAR_VALUE_REF pOk );

externC void           vdetach           ( tsp00_Int4                  devno,
                                           tsp00_TaskId                taskId );

externC void           v2blckio          ( tsp00_Int4                  devno1,
                                           tsp00_Int4                  devno2,
                                           tsp00_PageNo                 devpno,
                                           tsp2_io_op_code_Param       op_code,
                                           tsp00_PageAddr              bufadr,
                                           tsp00_Int2                  buf_count,
                                           tsp00_TaskId                taskId,
                                           V2_DEVNO      VAR_VALUE_REF read_dev,
                                           V2_DEVNO      VAR_VALUE_REF error_dev,
                                           tsp00_ErrText VAR_ARRAY_REF errtext1,
                                           tsp00_ErrText VAR_ARRAY_REF errtext2,
                                           tsp00_Bool    VAR_VALUE_REF ok );

/*!
   @Description    Synchronous block IO of block of pages


  This routine does I/O operation on devspaces using a single block of pages.
  It waits until the I/O completed.
  The pages are written in volume identified by 'devno' in continues space
  on disk beginning at specifed page number. Offset in bytes is calculated using
    ByteOffset = (devpno + 1) * 8192
  The pages in memory must start on an I/O page aligned address. This
  is necessary for the type of I/O-Routines used, it must be fast...
  The number of pages is specified by 'blockCount'. If the
  underlying OS has a limitation several I/O calls could be issued, but normally
  only a single system call is needed.
  On error, a descriptive message is put into 'errtext' in 'pascal style', that is
  padded with spaces up to size of tsp00_ErrText. Do not expect a C_String!!!!
   @param          devno [in] Volume identifer returned by vattach()
   @param          devpno [in] Pagenumber of first volume page to write
   @param          op_code [in] I/O operation opcode
   @param          pageAddr [in] The address of the first page in memory
   @param          pageCount [in] Number of elements in vector
   @param          taskId [in] Id of calling task
   @param          errtext [out] Errortext if I/O failed
   @param          pOk [out] flag to indicate success of call

 */

externC void           vblockio          ( tsp00_Int4                  devno,
                                           tsp00_PageNo                devpno,
                                           tsp2_io_op_code_Param       op_code,
                                           tsp00_PageAddr              pageAddr,
                                           tsp00_Int2                  pageCount,
                                           tsp00_TaskId                taskId,
                                           tsp00_ErrText VAR_ARRAY_REF errtext,
                                           tsp00_Bool    VAR_VALUE_REF pOk );

/*!
   @Description    Synchronous Scatter IO of vector of pages


  This function is specified for write only. It replaces multiple vblockio calls 
  for pages separated in memory space.
  It waits until the I/O an all pages completed.
  The pages are written in volume identified by 'devno' in continues space
  on disk beginning at specifed page number. Offset in bytes is calculated using
    ByteOffset = (devpno + 1) * pagesize (currently 8192 bytes)
  The pages in memory can be on discontinuos space, but each address must be I/O
  page aligned (i.e. as they are when returned by vallocdatacache()...). Alignment
  is necessary for the type of I/O-Routines used, it must be fast...
  The number of elements in the vector given is specified by 'blockCount'. If the
  underlying OS has a limitation several I/O calls could be issued, but normally
  only a single system call is needed.
  On error, a descriptive message is put into 'errtext' in 'pascal style', that is
  padded with spaces up to size of tsp00_ErrText. Do not expect a C_String!!!!
   @param          devno [in] Volume identifer returned by vattach()
   @param          devpno [in] Pagenumber of first volume page to write
   @param          pageVector [in] Vector of page addresses in memory
   @param          pageCount [in] Number of page addresses in the vector
   @param          taskId [in] Id of calling task
   @param          errtext [out] Errortext if I/O failed
   @param          pOk [out] flag to indicate success of call

 */

externC void           vvectorio               ( tsp00_Int4                  devno,
                                                 tsp00_PageNo                devpno,
                                                 tsp00_PageAddr              pageVector[],
                                                 tsp00_Int2                  pageCount,
                                                 tsp00_TaskId                taskId,
                                                 tsp00_ErrText VAR_ARRAY_REF errtext,
                                                 tsp00_Bool    VAR_VALUE_REF pOk );

/*!
   @Description    Synchronous Scatter IO of vector of pages on two volumes in parallel


  This function is specified for write only. It works as vvectorio() but on two different
  volumes in parallel two support software mirroring.
  On error, a descriptive message is put into 'errtext1' and 'errtext2' in 'pascal style', that is
  padded with spaces up to size of tsp00_ErrText. Do not expect a C_String!!!!
  Check both returned ok flags to make sure no error occured!
   @param          devno1 [in] Volume1 identifer returned by vattach()
   @param          devno2 [in] Volume2 identifer returned by vattach()
   @param          devpno [in] Pagenumber of first volume page to write
   @param          pageVector [in] Vector of page addresses in memory
   @param          pageCount [in] Number of page addresses in the vector
   @param          taskId [in] Id of calling task
   @param          errtext1 [out] Errortext if I/O on volume1 failed
   @param          errtext2 [out] Errortext if I/O on volume2 failed
   @param          pOk1 [out] flag to indicate success of call for volume1
   @param          pOk2 [out] flag to indicate success of call for volume2

 */

externC void           vdualvectorio           ( tsp00_Int4                  devno1,
                                                 tsp00_Int4                  devno2,
                                                 tsp00_PageNo                devpno,
                                                 tsp00_PageAddr              pageVector[],
                                                 tsp00_Int2                  pageCount,
                                                 tsp00_TaskId                taskId,
                                                 tsp00_ErrText VAR_ARRAY_REF errtext1,
                                                 tsp00_Bool    VAR_VALUE_REF pOk1,
                                                 tsp00_ErrText VAR_ARRAY_REF errtext2,
                                                 tsp00_Bool    VAR_VALUE_REF pOk2 );

/*!
   @Description    Copy action for volumes


  This function copies the content of one source volume to a target volume beginning with first page.
  The amount of pages copied is given. This call is used during mirror establish actions.
  On error, a descriptive message is put into 'errtext' in 'pascal style', that is
  padded with spaces up to size of tsp00_ErrText. Do not expect a C_String!!!!
   @param          devnoSource [in] Source volume identifer returned by vattach()
   @param          devnoDestination [in] Destination volume identifer returned by vattach()
   @param          firstPage [in] Pagenumber of first volume page to read/write
   @param          pageCount [in] Number of pages to copy
   @param          taskId [in] Id of calling task
   @param          errtext [out] Errortext if I/O failed
   @param          pOk [out] flag to indicate success of copy action

 */

externC void vcopyvolume ( tsp00_Int4                  devnoSource,
                           tsp00_Int4                  devnoDestination,
                           tsp00_PageNo                firstPage,
                           tsp00_Int4                  pageCount,
                           tsp00_TaskId                taskId,
                           tsp00_ErrText VAR_ARRAY_REF errtext,
                           tsp00_Bool    VAR_VALUE_REF ok );

externC void           vmark_bad_dev           ( tsp00_Int4                  devno,
                                                 tsp00_PageNo                devpno ) ;

externC tsp00_Int4     vget_data_io_cnt        ( void ) ;
externC void           vdump_rte               ( void );

externC void  vasynopen                        ( tsp00_VFilename VAR_ARRAY_REF device,
                                                 tsp00_Bool                    is_devspace,
                                                 tsp00_Bool                    for_writing,
                                                 tsp00_VfType_Param            filetype,
                                                 tsp00_Int4                    block_size,
                                                 tsp00_BoolAddr                cancel_pointer,
                                                 tsp00_Int4      VAR_VALUE_REF max_blockcount,
                                                 tsp00_Int4      VAR_VALUE_REF deviceno,
                                                 tsp00_VaReturn  VAR_VALUE_REF error,
                                                 tsp00_ErrText   VAR_ARRAY_REF errtext );

externC void  vasynclose                       ( tsp00_Int4                   deviceno,
                                                 tsp00_Bool                   rewind,
                                                 tsp00_Bool                   nxt_in_autoloader,
                                                 tsp00_VaReturn VAR_VALUE_REF nxt_tape_available,
                                                 tsp00_VaReturn VAR_VALUE_REF error,
                                                 tsp00_ErrText  VAR_ARRAY_REF errtext );

externC void  vasynio                          ( tsp00_Int4                   deviceno,
                                                 tsp00_Int4                   io_id,
                                                 tsp00_Int4                   file_pos,
                                                 tsp00_Int4                   io_block_cnt,
                                                 tsp00_PageAddr               io_block_ptr,
                                                 tsp00_VaReturn VAR_VALUE_REF error,
                                                 tsp00_ErrText  VAR_ARRAY_REF errtext );

externC void  vasynwait                        ( tsp00_Int4                   deviceno,
                                                 tsp00_Int4     VAR_VALUE_REF io_id,
                                                 tsp00_Int4     VAR_VALUE_REF io_block_cnt,
                                                 tsp00_VaReturn VAR_VALUE_REF error,
                                                 tsp00_ErrText  VAR_ARRAY_REF errtext );

externC void  vasyninit_tape                   ( tsp00_Int4                   deviceno,
                                                 tsp00_Bool                   destructive,
                                                 tsp00_VfType   VAR_VALUE_REF filetype,
                                                 tsp00_VaReturn VAR_VALUE_REF error,
                                                 tsp00_ErrText  VAR_ARRAY_REF errtext );
#endif
