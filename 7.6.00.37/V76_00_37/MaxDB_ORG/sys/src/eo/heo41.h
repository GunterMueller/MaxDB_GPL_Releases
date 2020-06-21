/*!
  @file           heo41.h
  @author         JoergM
  @special area   Shared Memory Access Functions
  @brief          Allows to create shared memory chunks
  @first created  1999-11-29  11:00


                Used i.e. to switch precompiler trace ON/OFF during runtime.

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



#ifndef HEO41_H
#define HEO41_H

#ifdef __cplusplus
#define externC extern "C"
#else
#define externC
#endif 

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @Description    Allocate a shared memory chunk identified by path


               If path points to an existing Shared Memory Object in the file system,
               it is opened, otherwise it is created (Read/Writable). The path does 
               not only uniquely identifies the shared memory chunk, but allows to 
               access the information even if the creator process already terminated.
   @Parameter      Path [in] Path to Shared memory chunk
        SizeInBytes [in] Size of Chunk in Bytes
   @Return value   Address of Shared Memory Segment or NULL if call failed or Path is not Read/Writable

 */


externC void *sqlAllocSharedMem(const char *Path, const unsigned int SizeInBytes);

/*!
   @Description    Free a shared memory chunk


               This releases a mapped shared memory chunk from process.
               Since the UNIX Interface needs the size of the previous allocated chunk,
               the size information is needed when returning the chunk too.
   @Parameter      ShmAddress  [in] Address returned by a previous call to sqlAllocSharedMem
        SizeInBytes [in] Number of bytes requested by a previous call to sqlAllocSharedMem
   @Return value   none

 */

externC void sqlFreeSharedMem(void *ShmAddress, const unsigned int SizeInBytes);

#endif  /* HEO41_H */
