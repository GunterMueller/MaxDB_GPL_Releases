/*!**************************************************************************

  module       : Trace_Info.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Handling of trace entries

  description : This class is used to handle the a various number of trace entries.
                
                The class Trace_DiagnoseFullEntry is used to sort persistent 
                trace entries stored on different trace page. This class offers 
                the possibility to  read trace entries, get the trace entry 
                sequence number and to store the entry to another location.


  last changed: 2000-12-18  20:43
  see also    : example.html ...
  first created:2000-12-14  10:44

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


#ifndef TRACE_INFO_HPP
#define TRACE_INFO_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TRACE_INFOPAGE_MAGIC ((tsp00_Int4)0x00dafeed)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  Class: Trace_InfoPage
  Description: Wrapper class for accessing first page of new kernel trace file

  The new kernel trace file consists of this page, containing the total number 
  and the individual size of each UKT slot following.

  ++++++Header++++++

    Trace_InfoPage

  +++++UKT Slot 0+++

  N(0) Trace_Page's

  +++++UKT Slot 1+++

  ...

  ++UKT Slot (N-1)++

  N(uktCount-1) Trace_Page's

  ++++++++++++++++++

 */
class Trace_InfoPage
{
public:

  /*!--------------------------------------------------------------------------
     Function: GetInfoPage
     Description: Return address of trace info page if already setup.
     Return value: true if valid page found, false else
   */
  static bool GetInfoPage(tsp00_PageAddr &infoPage);
  
  /*!--------------------------------------------------------------------------
     Function: GetUktTraceBuffer
     Description: Return address of first trace info page for given UKT
     
     Arguments: uktId [in] Ukt Identifier (1 == first)
                firstPage [out] First page for that Ukt
                sizeInPages [out] Size in pages for that Ukt
     Return value: true if valid pages found, false else
   */
  static bool GetUktTraceBuffer(SAPDB_Int4 uktId,
                                tsp00_PageAddr &firstPage,
                                SAPDB_Int4 &sizeInPages);
  
  /*!--------------------------------------------------------------------------
  declaration: Trace_InfoPage::Layout
  description: Describes the layout of the trace file

  The following members are defined

  tsp00_Int4 magic is a well known magic number (TRACE_INFOPAGE_MAGIC)

  tsp00_Int4 pageSize is the size of all trace pages in bytes

  tsp00_Int4 uktCount is the number of ukt slots defined

  tsp00_Int4[] uktTraceSizeInPages is an array containing the number of pages in each ukt slot
  --------------------------------------------------------------------------*/

  struct Layout 
  {
    tsp00_Int4 magic;
    tsp00_Int4 pageSize;
    tsp00_Int4 uktCount;
    tsp00_Int4 uktTraceSizeInPages[2000];
    // followed by UktCount - 1 size informations
  };
 
  /*!-------------------------------------------------------------------------
  function:     Trace_InfoPage
  description:  ctor allows to setup a class.
  
  If argument is 0 or defaults to 0, a AssignPage() is needed before work on this
  page is meaningfull.
  
  arguments:    pPage    [in] points to io-buffer 
  --------------------------------------------------------------------------*/
  Trace_InfoPage(tsp00_PageAddr pPage=0)
  {
    AssignPage(pPage);
  }

  /*!-------------------------------------------------------------------------
  function:     AssignPage()
  description:  Links a trace info page handler with an io-buffer.
  
  Used after an trace info page was read in from disk.

  arguments:    pPage    [in] points to io-buffer 
  --------------------------------------------------------------------------*/
  void AssignPage( tsp00_PageAddr  pPage )
  {
    m_Layout = REINTERPRET_CAST(Layout *, pPage);
  }

  /*!-------------------------------------------------------------------------
  function:     AssignNewPageAndFormat()
  description:  Links a trace info page handler with an io-buffer and formats it
                
  Format means to initializes the header of the buffer with the layout information
  given. The rest of the page is cleared to zero.

  NOTE: Correct pageSize must be given!!!

  arguments:    pPage    [in] points to io-buffer 
                pageLayout [in] Given Page Information
  --------------------------------------------------------------------------*/
  void AssignNewPageAndFormat(
      tsp00_PageAddr  pPage,
      Layout &pageLayout )
  {
    AssignPage(pPage);

    tsp00_Int4 infoSize = (3+pageLayout.uktCount)*sizeof(tsp00_Int4);
    if ( infoSize < pageLayout.pageSize )
    {
      memcpy( m_Layout, &pageLayout, infoSize );
      // always override this information
      memset( ((tsp00_BytePtr)m_Layout) + infoSize, 0, pageLayout.pageSize - infoSize );
    }
    m_Layout->magic = TRACE_INFOPAGE_MAGIC;
  }

  /*!-------------------------------------------------------------------------
  function:     GetPageSize()
  description:  Get the page size information from info page
  
  This page size is the size of all trace pages.

  return value: Page size found on info page
  --------------------------------------------------------------------------*/
  const tsp00_Int4 GetPageSize() const 
  {
    return m_Layout->pageSize; 
  }

  /*!-------------------------------------------------------------------------
  function:     GetUktCount()
  description:  Get the number of ukt slots following the info page
  
  return value: Number of UKT slots stored
  --------------------------------------------------------------------------*/
  const tsp00_Int4 GetUktCount() const 
  {
    return m_Layout->uktCount;
  }

  /*!-------------------------------------------------------------------------
  function:     GetUktPNO()
  description:  Get the PNO used to write first page of ukt slots into knltrace devspace
  
  arguments:    uktIndex [in] the slot index (0 = first)
  return value: PNO of UKT Slot in kernel trace devspace
  --------------------------------------------------------------------------*/
  const tsp00_Int4 GetUktPNO(tsp00_Int4 uktIndex) const 
  {
      // PTS 1108976 JoergM
      tsp00_Int4 UKTPno = 1;

      for ( tsp00_Int4 i = 0; i < uktIndex; i++ )
      {
          UKTPno += m_Layout->uktTraceSizeInPages[i];
      }

      return UKTPno;
  }

  /*!-------------------------------------------------------------------------
  function:     GetUktTraceSize()
  description:  Get the size of a specific ukt slot in pages
  arguments:    uktIndex [in] the slot index (0 = first) 
  return value: Number of pages in given UKT slot
  --------------------------------------------------------------------------*/
  tsp00_Int4 GetUktTraceSize(tsp00_Int4 uktIndex) const 
  {
    return m_Layout->uktTraceSizeInPages[uktIndex]; 
  }

  /*!-------------------------------------------------------------------------
  function:     CheckMagic()
  description:  Check the magic number 
  return value: true if magic number correct, false if corrupted
  --------------------------------------------------------------------------*/
  bool CheckMagic()
  {
    return (m_Layout == 0 ? false : (m_Layout->magic == TRACE_INFOPAGE_MAGIC));
  }

  /*!-------------------------------------------------------------------------
  function:     CheckPageSize()
  description:  Check the page size

  Valid pagesize means larger or equal 4K, smaller than 64K and power of 2.

  return value: true if valid page size, false if not
  --------------------------------------------------------------------------*/
  bool CheckPageSize()
  {
    if ( !m_Layout )
      return false;

    if ( m_Layout->pageSize < (4*1024) )
      return false; // too small

    if ( m_Layout->pageSize > (64*1024) )
      return false; // too big

    if ( ((m_Layout->pageSize - 1) & m_Layout->pageSize) != 0 )
      return false; // not power of 2

    return true;
  }

private:
  Layout *m_Layout;
}; /*! EndClass: Trace_InfoPage */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* TRACE_INFO_HPP */
