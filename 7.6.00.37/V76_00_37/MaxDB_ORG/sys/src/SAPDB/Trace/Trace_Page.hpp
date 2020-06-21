/*!**************************************************************************

  module      : Trace_Page.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Handling of trace pages and trace entries
  description : This class is used to handle trace pages and materialize 
                trace entries on trace pages.

                Furthermore this class is used to iterate over all trace 
                entries stored on a trace page.

  last changed: 2001-01-04  10:38
  see also    :

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


#ifndef TRACE_PAGE_HPP
#define TRACE_PAGE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"          // PASCAL: SP_basic_constants_and_types
#include "ggg11.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Trace/Trace_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp" 



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TRACE_PAGE_BODY_BEGIN   5
#define TRACE_PAGE_TYPE_INFO    4
#define TRACE_FILL_BYTE         0x00


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// Note that the header of an trace page has only a size of five bytes instead
// of the default size of eight bytes. Side effect: The filler one and two are 
// overlaid  with the position of the length byte of the first trace entry.
// A zero length byte indicates that no valid trace entry exist. Therefore no
// valid trace entry is available on a new trace page.

struct Trace_PageHeader
{
    Trace_PageNo    Pno;
    tgg00_PageType  Type;
    SAPDB_UInt1     Filler1; // First trace entry length byte
    SAPDB_UInt1     Filler2; // Second trace entry length byte
    SAPDB_UInt1     Filler3;
};

/*!*****************************************************************************

   class: Trace_Page

   description: This class is used to handle trace pages and materialize 
                trace entries on trace pages.
              
*******************************************************************************/

class Trace_Page
{
    friend class Trace_PageEntryIterator;
public:

    /*!-------------------------------------------------------------------------
    function:     Trace_Page
    description:  Default constructor
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    Trace_Page();

    /*!-------------------------------------------------------------------------
    function:     AssignPage()
    description:  This method links a trace page handler with an io-buffer 
                  containing a trace page with valid trace entries. An internal 
                  maker is set to the first trace entry on the trace page.
    arguments:    pPage    [in] points to io-buffer 
                  PageSize [in] size of the io-buffer in bytes. Default is 8192.
    return value: none
    --------------------------------------------------------------------------*/

    void AssignPage(
        tsp00_PageAddr  pPage, 
        SAPDB_UInt4     PageSize = 8192);

    /*!-------------------------------------------------------------------------
    function:     AssignNewPage()
    description:  This method links a trace page handler with an io-buffer and
                  initializes the header of the buffer with an page number and
                  a page type. The body of the buffer is not touched. 
    arguments:    pPage    [in] points to io-buffer 
                  PageNo   [in] current page number
                  PageSize [in] size of the io-buffer in bytes. Default is 8192.
    return value: none
    --------------------------------------------------------------------------*/

    void AssignNewPage(
        tsp00_PageAddr  pPage, 
        Trace_PageNo    PageNo,
        SAPDB_UInt4     PageSize = 8192 ); 

    /*!-------------------------------------------------------------------------
    function:     AssignNewPageAndFormat()
    description:  This method links a trace page handler with an io-buffer and
                  initializes the header of the buffer with an page number and
                  a page type. The body of the io-buffer is filled with hex-null 
                  (format).
    arguments:    pPage    [in] points to io-buffer 
                  PageNo   [in] current page number
                  PageSize [in] size of the io-buffer in bytes. Default is 8192.
    return value: none
    --------------------------------------------------------------------------*/

    void AssignNewPageAndFormat(
        tsp00_PageAddr  pPage, 
        Trace_PageNo    PageNo,
        SAPDB_UInt4     PageSize = 8192 );
    
        /*!-------------------------------------------------------------------------
    function:     BodySize()
    description:  This method returns the usable body size of a trace page in bytes.
    arguments:    none
    return value: size in bytes
    --------------------------------------------------------------------------*/
    
    SAPDB_Int4 BodySize() const
    { return( m_PageSize - sizeof( Trace_PageNo ) - sizeof( tgg00_PageType )); };
    
    /*!-------------------------------------------------------------------------
    function:     GetPageNo()
    description:  This method provides the page no of the current trace page.
                  If no trace page is available the TRACE_NIL_PNO is provided.
    arguments:    none
    return value: Trace_PageNo
    --------------------------------------------------------------------------*/
    
    Trace_PageNo GetPageNo() const;

    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  This method materializes a trace entry on the current insert
                  position of the trace page identified by the internal maker.
                  In cause of insufficient space the trace entry won't be
                  inserted. After successful insertion the new insert position 
                  is set to the end of the inserted trace entry. Each new trace 
                  entry is terminated with two hex null bytes to indicate that 
                  this is the last valid trace entry on the current trace page.
    arguments:    Entry [in] 
    return value: true means insert was successful; else false
    --------------------------------------------------------------------------*/

    bool Insert( Trace_IFullEntry &Entry );
    
    /*!-------------------------------------------------------------------------
    function:     IsSpaceAvailable()
    description:  This method examines whether the given space for the new trace
                  entry is still available on the trace page.
    arguments:    WantedSpace [in] size in bytes of the requested space.
    return value: true means space is available; else false
    --------------------------------------------------------------------------*/

    bool IsSpaceAvailable( SAPDB_UInt4 WantedSpace ) const;
    
private:
    
    SAPDB_UInt4 RemainingSpace() const;
   
private:

    SAPDB_UInt4     m_Offset;
    SAPDB_UInt4     m_PageSize;
    tsp00_PageAddr  m_pPage;
};  

typedef Trace_Page* Trace_PagePtr;

/*!*****************************************************************************

   endclass: Trace_Page

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_PageEntry

   description: This class is used to handle valid persistent trace entries 
                stored on a trace page. This class offers the possibility to
                read trace entries, get the trace entry sequence number and to
                store the entry to another location.
              
*******************************************************************************/

class Trace_PageEntry : public Trace_IFullEntry
{
public:

    /*!-------------------------------------------------------------------------
    function:     Trace_PageEntry
    description:  Default constructor
                  No trace entry is assigned.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    Trace_PageEntry();

    /*!-------------------------------------------------------------------------
    function:     Trace_PageEntry
    description:  Constructor
    arguments:    pEntry [in] pEntry points to an entry stored on a trace page.
    return value: none
    --------------------------------------------------------------------------*/

    Trace_PageEntry( tsp00_BytePtr pEntry );

     /*!-------------------------------------------------------------------------
    function:     Assign()
    description:  This method links the entry handler with a trace entry.
    arguments:    pEntry [in] pEntry points to an entry stored on a trace page
    return value: none
    --------------------------------------------------------------------------*/

    void Assign( tsp00_BytePtr pEntry  );

    /*!-------------------------------------------------------------------------
    function:     GetSequenceNo()
    description:  This method provides the sequence number of the current trace 
                  entry.
    arguments:    none
    return value: Trace_SeqNo
    --------------------------------------------------------------------------*/

    virtual Trace_SeqNo GetSequenceNo() const;
    
    /*!-------------------------------------------------------------------------
    function:     GetSize()
    description:  This method provides the persistent size of the current trace 
                  entry.
    arguments:    none
    return value: SAPDB_Int4
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetSize() const;
    
    /*!-------------------------------------------------------------------------
    function:     IsValid()
    description:  This method indicates whether the trace entry handler contains
                  a valid trace entry.
    arguments:    none
    return value: bool: true means is valid; else false
    --------------------------------------------------------------------------*/

    virtual bool IsValid() const;
    
    /*!-------------------------------------------------------------------------
    function:     Materialize()
    description:  This method is used to materialize the current trace entry 
                  to another location. If the trace entry is greater than the
                  available space no materialization is done.
    arguments:    pDestination    [in] pointer to the new position.
                  DestinationSize [in] size in bytes of the destination.
    return value: true means trace entry was materialized; else false
    --------------------------------------------------------------------------*/

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const;

private:

    tsp00_BytePtr       m_pEntry;
    tgg11_VtraceHeader  m_Header;
};

/*!*****************************************************************************

   endclass: Trace_PageEntry

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_PageEntryIterator

   description: This class is used to iterate over all trace entries stored on 
                a trace page.
                              
*******************************************************************************/

class Trace_PageEntryIterator
{
public:
    
    /*!-------------------------------------------------------------------------
    function:     Trace_PageEntryIterator
    description:  Default constructor. 
                  No trace page is assigned.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    Trace_PageEntryIterator();

    /*!-------------------------------------------------------------------------
    function:     Trace_PageEntryIterator
    description:  Constructor.
                  Sets the iterator to the first entry on the trace page.
    arguments:    Page [in] The iterator works on this trace page.
    return value: none
    --------------------------------------------------------------------------*/

    Trace_PageEntryIterator( Trace_Page &Page );

    /*!-------------------------------------------------------------------------
    function:     operator ++
    description:  This method steps to the next trace entry on the current
                  trace page. Precondition: The current position contains a 
                  valid trace entry.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void operator ++();

    /*!-------------------------------------------------------------------------
    function:     operator *
    description:  This methods provides a trace entry.
    arguments:    none
    return value: Trace_PageEntry
    --------------------------------------------------------------------------*/

    Trace_PageEntry operator *() const;
    
    /*!-------------------------------------------------------------------------
    function:     Assign
    description:  This methods assigns the iterator a trace page and sets the 
                  iterator to the first entry on the trace page.
    arguments:    Page [in] The iterator works on this trace page.
    return value: none
    --------------------------------------------------------------------------*/

    void Assign( Trace_Page &Page );

    /*!-------------------------------------------------------------------------
    function:     Begin
    description:  This methods sets the iterator to the first possible 
                  trace entry position. 
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    
    void Begin();
    
    /*!-------------------------------------------------------------------------
    function:     End
    description:  This methods indicates if the iterator has reached the last
                  trace entry on the current trace page.
    arguments:    none
    return value: true means that the iterator has not(!) reached the last entry.
    --------------------------------------------------------------------------*/
    
    bool End() const;

private:

    SAPDB_UInt4     m_Offset;
    tsp00_BytePtr   m_pEntry;
    Trace_PagePtr   m_pPage;
};

/*!*****************************************************************************

   endclass: Trace_PageEntryIterator

*******************************************************************************/

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

inline
Trace_Page::Trace_Page()
:
m_Offset( 0 ),
m_PageSize( 0 ),
m_pPage( NULL )
{
};

/*---------------------------------------------------------------------------*/

inline void
Trace_Page::AssignPage(
                       tsp00_PageAddr   pPage, 
                       SAPDB_UInt4      PageSize)
{
    m_Offset    = TRACE_PAGE_BODY_BEGIN;
    m_PageSize  = PageSize;
    m_pPage     = pPage;
};

/*---------------------------------------------------------------------------*/

inline void 
Trace_Page::AssignNewPage(
                          tsp00_PageAddr    pPage, 
                          Trace_PageNo      PageNo,
                          SAPDB_UInt4       PageSize)
{

    this->AssignPage( pPage, PageSize );

    Trace_PageHeader Header;

    Header.Pno      = PageNo;
    Header.Type     = tgg00_PageType::fromConst( ptVtrace_egg00 );
    Header.Filler1  = 0; // => Position of the first trace entry length bytes
    Header.Filler2  = 0; // => No valid trace entry on this page!
    Header.Filler3  = 0;

    // Store header onto current trace page
    SAPDB_MemCopyNoCheck( pPage, &Header, sizeof( Header ));
};

/*---------------------------------------------------------------------------*/

inline void 
Trace_Page::AssignNewPageAndFormat(
                                   tsp00_PageAddr   pPage, 
                                   Trace_PageNo     PageNo,
                                   SAPDB_UInt4      PageSize)
{
    memset( pPage, TRACE_FILL_BYTE, PageSize );
    this->AssignNewPage( pPage, PageNo, PageSize );
};

/*---------------------------------------------------------------------------*/

inline Trace_PageNo 
Trace_Page::GetPageNo() const
{
    if ( NULL == m_pPage ) 
        return( TRACE_NIL_PNO );

    return( *(REINTERPRET_CAST( Trace_PageNo*, m_pPage )));
};

/*---------------------------------------------------------------------------*/

inline bool 
Trace_Page::Insert( Trace_IFullEntry &Entry )
{
    if (! IsSpaceAvailable( Entry.GetSize() ))
        return( false );

    tsp00_BytePtr pDestination = REINTERPRET_CAST( tsp00_BytePtr, m_pPage );
    
    if (! Entry.Materialize(( pDestination + m_Offset ), RemainingSpace() ))
        return( false );
    
    m_Offset += Entry.GetSize();
    *(pDestination + m_Offset)     = TRACE_FILL_BYTE; // no valid trace entry available
    *(pDestination + m_Offset + 1) = TRACE_FILL_BYTE; // after the current trace entry

    return( true );
};

/*---------------------------------------------------------------------------*/

inline bool 
Trace_Page::IsSpaceAvailable( SAPDB_UInt4 WantedSpace ) const
{
    // The given space needs two more bytes for hex null termination mark

    return( RemainingSpace() >= ( WantedSpace + 2 ));
};

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4 
Trace_Page::RemainingSpace() const
{
    return( m_PageSize - m_Offset );
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


inline
Trace_PageEntry::Trace_PageEntry()
{
    m_pEntry                   = NULL;
    m_Header.vhSequenceNo_gg11 = 0;
    m_Header.vhLen_gg11        = 0;
};

/*---------------------------------------------------------------------------*/

inline
Trace_PageEntry::Trace_PageEntry( tsp00_BytePtr pEntry )
:
m_pEntry( pEntry )
{
    SAPDB_MemCopyNoCheck( &m_Header, pEntry, sizeof( m_Header ));
};
    
/*---------------------------------------------------------------------------*/

inline void 
Trace_PageEntry::Assign( tsp00_BytePtr pEntry  )
{
    m_pEntry = pEntry;
    SAPDB_MemCopyNoCheck( &m_Header, pEntry, sizeof( m_Header ));
};
/*---------------------------------------------------------------------------*/

inline Trace_SeqNo 
Trace_PageEntry::GetSequenceNo() const
{
    return( m_Header.vhSequenceNo_gg11 );
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4  
Trace_PageEntry::GetSize() const
{
    return( m_Header.vhLen_gg11 );
};

/*---------------------------------------------------------------------------*/

inline bool 
Trace_PageEntry::IsValid() const
{
    return
        ( 
        ( 0 < m_Header.vhLen_gg11 ) && 
        ( Trace_IBaseEntry::MaximumSize() + sizeof( m_Header ) 
        >= SAPDB_UInt4 (m_Header.vhLen_gg11) ) // cast to avoid warning C4018 on NT
        );
};

/*---------------------------------------------------------------------------*/

inline bool 
Trace_PageEntry::Materialize( 
                             tsp00_BytePtr  pDestination,
                             SAPDB_Int4     DestinationSize ) const
{
    if ( NULL == m_pEntry )
        return( false );
    
    if ( m_Header.vhLen_gg11 > DestinationSize ) 
        return( false );
    
    SAPDB_MemCopyNoCheck( pDestination, m_pEntry, m_Header.vhLen_gg11 );
    return( true );
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline
Trace_PageEntryIterator::Trace_PageEntryIterator()
:
m_Offset( 0 ),
m_pEntry( NULL ),
m_pPage( NULL )
{
};

/*---------------------------------------------------------------------------*/

inline
Trace_PageEntryIterator::Trace_PageEntryIterator( Trace_Page &Page )
:
m_Offset( TRACE_PAGE_BODY_BEGIN ),
m_pPage( &Page )
{
    m_pEntry = REINTERPRET_CAST( tsp00_BytePtr, Page.m_pPage );
};

/*---------------------------------------------------------------------------*/

inline void 
Trace_PageEntryIterator::operator ++()
{
    if ( NULL == m_pPage ) return;

    // Create an trace entry handler for the current internal position.
    // Precondition: This position contains a valid trace entry.

    Trace_PageEntry TraceEntry(( m_pEntry + m_Offset ));

    m_Offset += TraceEntry.GetSize();
};

/*---------------------------------------------------------------------------*/

inline Trace_PageEntry
Trace_PageEntryIterator::operator *() const
{
    if ( NULL == m_pPage )
    {
        Trace_PageEntry Entry; // Workaround for LinuxCompiler TS 19.12.2000
        return( Entry );
    }
    else
        return( Trace_PageEntry(( m_pEntry + m_Offset )));
};

/*---------------------------------------------------------------------------*/
 
inline void
Trace_PageEntryIterator::Assign( Trace_Page &Page )
{
    m_Offset = TRACE_PAGE_BODY_BEGIN;
    m_pEntry = REINTERPRET_CAST( tsp00_BytePtr, Page.m_pPage );
    m_pPage  = &Page;
};

/*---------------------------------------------------------------------------*/

inline void
Trace_PageEntryIterator::Begin()
{
    m_Offset = TRACE_PAGE_BODY_BEGIN;
};

/*---------------------------------------------------------------------------*/

inline bool
Trace_PageEntryIterator::End() const
{
    if ( NULL == m_pPage ) 
        return( true );

    // The internal offset has reached the end of the trace page.
    // At least there is not enough space for the two length bytes
    // of the next possible trace entry.

    if ( (m_Offset+2) > m_pPage->m_PageSize ) 
        return( true );

    Trace_PageEntry TraceEntry(( m_pEntry + m_Offset ));

    // The trace entry handler contains no valid entry.
    if (! TraceEntry.IsValid() )
        return( true );

    // The current internal position conatins a valid trace entry.
    return( false );
};


#endif  /* TRACE_PAGE_HPP */

