/****************************************************************************

  module      : RTEWrapper_KernelMemory.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Kernel Memory Function Wrapper
  description : Wrapper calling RTEMem_RteAllocator

                These wrappers are used for old pascal code

  last changed: 2000-07-18  10:36
  see also    : example.html ...
  first created:2000-06-16  16:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h" /* for tsp00_BufAddr, tsp00_PageAddr */
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static SAPDB_Bool1 RTEWrapper_Allocate(SAPDB_Int4 lengthInBytes,
                                       void ** objPointer,
                                       SAPDBMem_IRawAllocator &allocatorToUse);
static SAPDB_Bool1 RTEWrapper_BlockAllocate(SAPDB_Int4 countInPages, tsp00_BufAddr & bufptr);
static SAPDB_Bool1 RTEWrapper_BlockDeallocate(tsp00_BufAddr pagePointer, SAPDB_Int4 countInPages);

/*===========================================================================*
 *  EXPORTED FUNCTIONS                                                       *
 *===========================================================================*/

extern "C" void vallocat ( 
SAPDB_Int4    sizeInBytes ,
SAPDB_Byte ** objPointer ,
SAPDB_Bool1  * ok )
{
  SAPDBERR_ASSERT_ARGUMENT(sizeInBytes > 0);
  SAPDBERR_ASSERT_ARGUMENT(objPointer != 0);
  SAPDBERR_ASSERT_ARGUMENT(ok         != 0);

  void **voidobjPointer = (void **)objPointer;
  * ok = RTEWrapper_Allocate(sizeInBytes, voidobjPointer, RTEMem_Allocator::Instance() );
}

/*---------------------------------------------------------------------------*/
//#define REMOVE_OLD_IMPLEMENTATION
#ifndef REMOVE_OLD_IMPLEMENTATION

extern "C" void valloc_convcache ( 
SAPDB_Int4     countInPages ,
SAPDB_Int4    *available ,
tsp00_BufAddr *bufptr ,
SAPDB_Bool1  * ok )
{
  SAPDBERR_ASSERT_ARGUMENT(countInPages > 0);
  SAPDBERR_ASSERT_ARGUMENT(available != 0);
  SAPDBERR_ASSERT_ARGUMENT(bufptr != 0);
  SAPDBERR_ASSERT_ARGUMENT(ok         != 0);

  * ok = RTEWrapper_BlockAllocate(countInPages, *bufptr);
}

/*---------------------------------------------------------------------------*/

extern "C" void valloc_datacache ( 
SAPDB_Int4     countInPages ,
SAPDB_Int4    *available ,
tsp00_BufAddr *bufptr ,
SAPDB_Bool1  * ok )
{
  SAPDBERR_ASSERT_ARGUMENT(countInPages > 0);
  SAPDBERR_ASSERT_ARGUMENT(available != 0);
  SAPDBERR_ASSERT_ARGUMENT(bufptr != 0);
  SAPDBERR_ASSERT_ARGUMENT(ok         != 0);

  * ok = RTEWrapper_BlockAllocate(countInPages, *bufptr);
}

/*---------------------------------------------------------------------------*/

extern "C" void vnewbuf ( 
SAPDB_Int4      countInPages ,
SAPDB_Int4     *available ,
tsp00_PageAddr *pageptr ,
SAPDB_Bool1     *ok )
{
  SAPDBERR_ASSERT_ARGUMENT(countInPages > 0);
  SAPDBERR_ASSERT_ARGUMENT(available != 0);
  SAPDBERR_ASSERT_ARGUMENT(pageptr != 0);
  SAPDBERR_ASSERT_ARGUMENT(ok != 0);

  // tsp00_PageAddr and tsp00_BufAddr are identical in 7.2
  tsp00_BufAddr & bufptr = REINTERPRET_CAST(tsp00_BufAddr &, *pageptr);
  * ok = RTEWrapper_BlockAllocate(countInPages, bufptr);
}

/*---------------------------------------------------------------------------*/

extern "C" void vfreebuf ( 
SAPDB_Int4      countInPages ,
tsp00_PageAddr  pageptr ,
SAPDB_Bool1     *ok )
{
  SAPDBERR_ASSERT_ARGUMENT(ok != 0);

  // tsp00_PageAddr and tsp00_BufAddr are identical in 7.2
  tsp00_BufAddr bufptr = REINTERPRET_CAST(tsp00_BufAddr, pageptr);
  * ok = RTEWrapper_BlockDeallocate(bufptr, countInPages);
}
#endif /* REMOVE_OLD_IMPLEMENTATION */

/*---------------------------------------------------------------------------*/

extern "C" void vmalloc ( 
SAPDB_Int4      sizeInBytes ,
SAPDB_Byte    **objPointer ,
SAPDB_Bool1     *ok )
{
  SAPDBERR_ASSERT_ARGUMENT(sizeInBytes > 0);
  SAPDBERR_ASSERT_ARGUMENT(objPointer != 0);
  SAPDBERR_ASSERT_ARGUMENT(ok         != 0);

  void **voidobjPointer = (void **)objPointer;
  * ok = RTEWrapper_Allocate(sizeInBytes, voidobjPointer, RTEMem_Allocator::Instance() );
}

/*---------------------------------------------------------------------------*/

extern "C" void vmfree ( 
SAPDB_Byte     *objPointer )
{
  SAPDBERR_ASSERT_ARGUMENT(objPointer != 0);

  RTEMem_Allocator::Instance().Deallocate(objPointer);
}

/*---------------------------------------------------------------------------*/
#ifdef NO_LONGER_SUPPORTED

    extern "C" void vmnewbuf ( 
    SAPDB_Int4      countInPages ,
    tsp00_BufAddr  *bufptr ,
    SAPDB_Bool1     *ok )
    {
      SAPDBERR_ASSERT_ARGUMENT("vmnewbuf no longer supported" == 0);
    }

/*---------------------------------------------------------------------------*/

    extern "C" void vmfreebuf ( 
    tsp00_BufAddr   bufptr )
    {
      SAPDBERR_ASSERT_ARGUMENT("vmfreebuf no longer supported" == 0);
    }

#endif /* NO LONGER SUPPORTED */

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Int4 sql57k_pmalloc  ( SAPDB_UInt4 ulLine,
                                        SAPDB_Char *pszModuleName,
                                        void **ppMemory,
                                        SAPDB_UInt4 lengthInBytes )
{
    if ( !RTEWrapper_Allocate(lengthInBytes, ppMemory, RTEMem_RteAllocator::Instance() ) )
  {
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
      return -1;
#else
      throw SAPDBMem_BadAllocException(pszModuleName, ulLine, lengthInBytes);
#endif
  }
  else
  {
    return 0;
  }
}

extern "C" SAPDB_Int4 sql57k_pfree ( SAPDB_UInt4 ulLine,
                                     SAPDB_Char *pszModuleName,
                                     void *pMemory )
{
  if ( !pMemory )
  {
      return -1;
  }
  RTEMem_RteAllocator::Instance().Deallocate(pMemory);
  return 0;
}

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

static SAPDB_Bool1 RTEWrapper_Allocate(SAPDB_Int4 lengthInBytes, 
                                       void ** objPointer, 
                                       SAPDBMem_IRawAllocator &allocatorToUse)
{
    SAPDB_Bool1 Ok = true;

#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    // This routine will not propagate SAPDBMem_BadAllocException
    try
    {
#endif /* !SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING */

        *objPointer = allocatorToUse.Allocate(lengthInBytes);

#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
        if ( 0 == *objPointer )
        {
            Ok = false;
        }
#else
    }
    catch ( SAPDBMem_BadAllocException & )
    {
        *objPointer = 0;
        Ok = false;
    }
#endif

#if defined(SAPDB_SLOW)
    /*
     * For slow mode compilation all allocated memory is prefilled with
     * 0xba pattern to detect uninitialized memory
     */
    if ( 0 != *objPointer )
    {
        memset(*objPointer, 0xba, lengthInBytes);
    }
#endif
  return Ok;
}

// XXX need access to RTE Configuration object to find configured database page size...
#define DATABASE_PAGE_SIZE sizeof(tsp00_Page)

/*---------------------------------------------------------------------------*/

static SAPDB_Bool1 RTEWrapper_BlockAllocate(SAPDB_Int4 countInPages, tsp00_BufAddr & pagePointer)
{
    SAPDB_Bool1 Ok = true;

  //
  // countInPages must be converted into count in block, since memory pages
  // may differ in size from database pages...
  //
    SAPDB_Int4 countInBlocks = (SAPDB_Int4) ( ( (DATABASE_PAGE_SIZE * countInPages)
                                + RTEMem_BlockAllocator::Instance().GetBlockSize() - 1 )
                              / RTEMem_BlockAllocator::Instance().GetBlockSize() );

#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    // This routine will not propagate SAPDBMem_BadAllocException
    try
    {
#endif /* !SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING */

        pagePointer = static_cast<tsp00_BufAddr>(RTEMem_BlockAllocator::Instance().Allocate(countInBlocks));

#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
        if ( 0 == pagePointer )
        {
            Ok = false;
        }
#else
    }
    catch ( SAPDBMem_BadAllocException & )
    {
        pagePointer = 0;
        Ok = false;
    }
#endif

#if defined(SAPDB_SLOW)
    if ( pagePointer != 0 )
    {
        /*
         * For slow mode compilation all allocated memory is prefilled with
         * 0xba pattern to detect uninitialized memory
         */
        memset(pagePointer, 0xba, countInBlocks * RTEMem_BlockAllocator::Instance().GetBlockSize());
    }
#endif

    return Ok;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool1 RTEWrapper_BlockDeallocate(tsp00_BufAddr pagePointer, SAPDB_Int4 countInPages)
{
    SAPDB_Bool1 Ok = true;

    if ( 0 == pagePointer || countInPages <= 0 )
    {
        return Ok;
    }

  //
  // countInPages must be converted into count in block, since memory pages
  // may differ in size from database pages...
  //
    SAPDB_Int4 countInBlocks = (SAPDB_Int4) ( ( (DATABASE_PAGE_SIZE * countInPages)
                                + RTEMem_BlockAllocator::Instance().GetBlockSize() - 1 )
                              / RTEMem_BlockAllocator::Instance().GetBlockSize() );

#if defined(SAPDB_SLOW)
    /*
     * For slow mode compilation all allocated memory is filled with
     * 0xcd pattern to detect reused memory
     */
    memset(pagePointer, 0xcd, countInBlocks * RTEMem_BlockAllocator::Instance().GetBlockSize());
#endif

#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    // This routine will not propagate SAPDBMem_BadAllocException
    try
    {
#endif /* !SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING */

        RTEMem_BlockAllocator::Instance().Deallocate(pagePointer, countInBlocks);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    }
    catch ( SAPDBMem_BadAllocException & )
    {
        Ok = false;
    }
#endif

    return Ok;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
