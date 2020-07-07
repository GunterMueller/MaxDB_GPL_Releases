/*!
  @file           hos06.h
  @author         RaymondR
  @brief          virtual file
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



#ifndef HOS06_H
#define HOS06_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

typedef tsp00_VFileOpCodes     OPCODES;
typedef tsp00_VfFormat   VF_FORMAT;
typedef tsp00_VfReturn   VF_RETURN;
typedef tsp00_VfResource VF_RESOURCE             ;

typedef CHAR          * VF_BUFFERADDRESS;
typedef CHAR            VF_BLOCK [ MAXBUFFERLENGTH ];

// --- Virtual file --- OPCODES ------------------------------------------------

#define VREAD                   0
#define VWRITE                  1
#define VAPPEND                 2
#define VOVERWRITE              3
#define VREAD_ORA               4

// --- Virtual file --- VF_FORMAT ---------------------------------------------

#define VF_PLAINTEXT            0
#define VF_RECORD               1
#define VF_STREAM               2
#define VF_UNKNOWN              3
#define VF_4K_BLOCK             4

// --- Virtual file --- VF_RETURN ---------------------------------------------

#define VF_OK                   0
#define VF_NOTOK                1
#define VF_EOF                  2
#define VF_NOSEEK               3

// --- Virtual file --- VF_RESOURCE -------------------------------------------

#define VF_STACK                0
#define VF_BUFFERPOOL           1

#define odd(i)          ( (i) & 1 )

//
// --- virtual file control record
//
typedef struct hostfile_ctrl_record
  {
  HANDLE          hfFileHandle;       // - file handle

  VF_FILENAMEC    szFileName;         // - null-terminated
  OPCODES         Direction;          // - opcode from vfopen()
  bool            fReadOracleData;    // - reading oracle data?
  VF_RESOURCE     Resource;           // - stack, buffer pool
  VF_FORMAT       FileFormat;         // - stream, record, plaintext, unknown
  LONG            lRecordLength;      // - fixed record length,
                                      //   or 0 == variable
  LONG            lHeaderLength;      // - page 0 / magic record size
  ULONG           ulState;            // - SEE ABOVE
  LONG            lMaxFilePos;        // - highest byte number in file
  LONG            lReadCount;         // - seek: count of bytes to read
  ULONG           ulFileType;         // - 0-Disk file, 1-Character device,
                                      //   2-Pipe.
  bool            fBlocked;           // - only for ASCII files: blocked?
  VF_FORMAT       ulEffFileFormat;    // - only for reading, stream overlays
                                      //
  PUCHAR          pBuffer;            // - pointer to this file's buffer
  LONG            lEndOfBuffer;       // - current end of this file's buffer
  LONG            lPos;               // - last pos read or to write, 0 - x
  LONG            lMaxPos;            // - highest usable position in pBuffer
  LONG            lFilePos;           // - abs. file pos of data in pBuffer
                                      //
  PUCHAR          pFirstBlock;        // - start pointer a block area for
                                      //   file i/o
  LONG            lSizeOfBlocks;      // - size of the block area

  LONG            lDevType;           // - device type

   ULONG          ulTapeFeatures;     // - features of the tape drive
   ULONG          ulMaxTapeBlkSize;   // - Max. tape block size
   bool           fECC;
   bool           fCompression;
   bool           fDataPadding;
   bool           fNxtInAutoLoader;   // - only for autoloader ( tape close )
   LONG           lNxtInAutoLoaderError; // - return value , for autoloader only
  }  HOSTFILE_CTRL_REC;

typedef HOSTFILE_CTRL_REC*      PHOSTFILE_CTRL_REC;
typedef PHOSTFILE_CTRL_REC*     PPHOSTFILE_CTRL_REC;


typedef struct stat             VF_FILEDATA;


// --- VOS06C - ( Virtual file ) ---------------------------------------------

externC void            sql06c_finit            (  INT2                buf_pool_size,
                                                   INT4*               poolptr,
                                                   bool               *pbFirstCall,
                                                   PHOSTFILE_CTRL_REC  arcFileCtrlTab,
                                                   void              **ppBufPool,
                                                   bool               *ok );

externC LONG            sql06c_alloc_buffer_pool ( ULONG               ulNumOfBlocks,
                                                   PPVOID              ppBufPool);

externC LONG            sql06c_reserve_buffer    ( PHOSTFILE_CTRL_REC  pHF,
                                                   void               *pBufPool,
                                                   VF_RESOURCE         Resource,
                                                   INT2                BufCount,
                                                   VF_BUFFERADDRESS   *Block,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_free_buffer       ( PHOSTFILE_CTRL_REC  pHF,
                                                   void               *pBufPool,
                                                   INT2                BufCount,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_search            ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  arcFileCtrlTab,
                                                   INT4                * pHostFileNo,
                                                   PHOSTFILE_CTRL_REC  *ppHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_tape_open         ( PSZ                 pszTapeName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText,
                                                   bool                fTapeRead );

externC LONG            sql06c_write_open        ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_con_open          ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_overwrite_open    ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_read_open         ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_append_open       ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_pipe_write_open   ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_pipe_read_open    ( PSZ                 pszHostFileName,
                                                   PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_close             ( PHOSTFILE_CTRL_REC  pHF,
                                                   bool                fErase,
                                                   PCHAR               pcErrText);

externC LONG            sql06c_status            ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_read_asc_var      ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                * length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_read_bin_var      ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                * length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_read_bin_fix      ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                * length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_write_asc_var     ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_write_bin_var     ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_write_bin_fix     ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_get_desc          ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_put_desc          ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_putbuffer         ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_getbuffer         ( PHOSTFILE_CTRL_REC  pHF,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_get_xk_buffer     ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                * length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_put_xk_buffer     ( PHOSTFILE_CTRL_REC  pHF,
                                                   VF_BUFFERADDRESS    block,
                                                   INT4                length,
                                                   PCHAR               pcErrText );

externC LONG            sql06c_posbuffer         ( PHOSTFILE_CTRL_REC  pHF,
                                                   LONG                lDistance,
                                                   ULONG               ulMoveType,
                                                   PULONG              pNewPointer,
                                                   PCHAR               pcErrText );

externC bool            sql06c_valid_fileno      ( INT4                HostFileNo,
                                                   PHOSTFILE_CTRL_REC  arcFileCtrlTab );

externC void            sql06c_init_file_ctrl    ( PHOSTFILE_CTRL_REC  pHF );

externC bool            sql06c_blocked_file      ( VF_FILENAMEC        TempFilename,
                                                   ULONG               FileFormat,
                                                   INT4                RecordLength );


// --- VOS06UC ----------------------------------------------------------------

void    sqlfinit    (   INT2                buf_pool_size,
                        tsp00_Int4            *poolptr,
                        bool                *ok              );

void    sqlfopen    (   tsp00_VFilename     vf_filename,
                        OPCODES             direction,
                        VF_RESOURCE         resource,
                        tsp00_Int4            *hostfileno,
                        VF_FORMAT*          format,
                        tsp00_Int4            *rec_len,
                        tsp00_Int4            poolptr,
                        INT2                buf_count,
                        VF_BUFFERADDRESS*   block,
                        VF_RETURN*          error,
                        tsp00_ErrText         errtext         );

void    sqlfclose   (   tsp00_Int4            *hostfileno,
                        bool                erase,
                        tsp00_Int4            poolptr,
                        INT2                buf_count,
                        VF_BUFFERADDRESS    block,
                        VF_RETURN*          error,
                        tsp00_ErrText         errtext         );

void    sqlfread    (   tsp00_Int4            *hostfileno,
                        VF_BUFFERADDRESS    block,
                        tsp00_Int4            *length,
                        VF_RETURN*          error,
                        tsp00_ErrText         errtext         );

void    sqlfwrite   (   tsp00_Int4            *hostfileno,
                        VF_BUFFERADDRESS    block,
                        tsp00_Int4            length,
                        VF_RETURN*          error,
                        tsp00_ErrText         errtext         );

void    sqlfseek    (   PINT4               plHostFileNo,
                        tsp00_Int4            lFilePos,
                        VF_RETURN*          peError,
                        tsp00_ErrText         acErrorText     );

void    sqlferase   (   tsp00_VFilename     acFileName,
                        VF_RETURN*          peError,
                        tsp00_ErrText         acErrorText     );

#endif
