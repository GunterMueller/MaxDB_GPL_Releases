/*!**************************************************************************

  module      : Trace_Entry.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Handling of trace entries
  description : This class is used to handle a various number of trace entries.
                All this trace entries are derivations from the abstract trace
                entries of Trace_Types.hpp.

                The class Trace_CommonEntry is used for ordinary trace entries.
                This trace entries are shorter than 132 bytes and therefore
                each entry resides within one persistent trace entry.

                The class Trace_LargeEntry is used for trace entries larger
                than 132 bytes for example the SAPDB order packet or the 
                internal kernel mess block. The class offers the possibility
                to split an entry larger than 132 bytes into more than one
                separate persistent entries. Each entry contains an internal
                header that includes the original trace layer information and
                the current split count. This causes that the proper body of
                the pesistent trace entry is reduced in size to 129 bytes.
                Note that each trace entry derived from Trace_LargeEntry has 
                this internal header independend of the original size.

                Thes class Trace_LargeEntryIterator works on the above mentioned
                class Trace_LargeEntry. It is used by the trace manager to 
                iterate over all parts of a large trace entry.

                The class Trace_OverflowEntry is used by the trace manager to
                store large trace entries within the kernel trace file. For 
                each part of a large trace entry a Trace_OverflowEntry is created.
                This new entry resembles a Trace_CommonEntry and offers 
                mechanism to store themself within kernel trace file.
                
                The class Trace_KernelFullEntry combines the user defined trace
                entry with the trace entry header containing administrative
                informations like task id and transaction number.

  last changed: 2000-11-27  18:15
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


#ifndef TRACE_ENTRY_HPP
#define TRACE_ENTRY_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "ggg11.h"

#include "Trace/Trace_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

class Trans_Context;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!*****************************************************************************

   class: Trace_CommonEntry

   description: The class Trace_CommonEntry is used for ordinary trace entries.
                This trace entries are shorter than 132 bytes and therefore
                each entry resides within one persistent trace entry.
   
*******************************************************************************/

class Trace_CommonEntry : public Trace_IBaseEntry
{
public:

    /*!-------------------------------------------------------------------------
    function:     Trace_CommonEntry
    description:  This method is a handler for a trace entry smaller than 132 bytes.
    arguments:    Layer     [in] debugging layer
                  Type      [in] debugging type
                  pEntry    [in] points to the trace entry body
                  EntrySize [in] size of the trace entry body in bytes.
    return value: none
    --------------------------------------------------------------------------*/

    Trace_CommonEntry(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type,
        const tsp00_Byte   *pEntry,
        SAPDB_Int4          EntrySize )
    :
    Trace_IBaseEntry( Layer, Type ),
    m_EntrySize( EntrySize ),
    m_pEntry( pEntry )
    {}

    /*!-------------------------------------------------------------------------
    function:     ~Trace_CommonEntry
    description:  The destructor calls the insert method of the trace manager.
                  This is the common way to materialize trace entries within the 
                  memory trace buffers.
    arguments:    Trans     [in] current transaction context
                  Layer     [in] debugging layer
                  Type      [in] debugging type
                  pEntry    [in] points to the trace entry body
                  EntrySize [in] size of the trace entry body in bytes.
    return value: none
    --------------------------------------------------------------------------*/

    ~Trace_CommonEntry();

    /*!-------------------------------------------------------------------------
    function:     GetLayer
    description:  This method returns the debugging layer information of the
                  trace entry.
    arguments:    none
    return value: debugging layer
    --------------------------------------------------------------------------*/

    virtual tgg00_Debug GetLayer() const{
        return( m_Layer );
    }

    /*!-------------------------------------------------------------------------
    function:     GetSize
    description:  This method returns the size of trace entry body in bytes.
    arguments:    none
    return value: entry size in bytes
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetSize() const{
        return( m_EntrySize );
    }

    /*!-------------------------------------------------------------------------
    function:     GetType
    description:  This method returns the debugging type information of the
                  trace entry.
    arguments:    none
    return value: debugging type
    --------------------------------------------------------------------------*/

    virtual tgg00_VtraceType GetType() const{
        return( m_Type );
    }

    /*!-------------------------------------------------------------------------
    function:     Materialize
    description:  This method is used to store the trace entry within the memory
                  trace buffers.
    arguments:    pDestination    [in] points to the destination
                  DestinationSize [in] gives the size of the destination in bytes
    return value: true means that the materialization was successful; else false
    --------------------------------------------------------------------------*/

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const;

private:

    SAPDB_Int4          m_EntrySize;
    const tsp00_Byte   *m_pEntry;
};

/*!*****************************************************************************

   endclass: Trace_CommonEntry

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_OverflowEntry

   description: The class Trace_OverflowEntry is used by the trace manager to
                store large trace entries within the kernel trace file. For 
                each part of a large trace entry a Trace_OverflowEntry is created.
                This new entry resembles a Trace_CommonEntry and offers 
                mechanism to store themself within kernel trace file.
   
*******************************************************************************/

class Trace_OverflowEntry : public Trace_IBaseEntry
{
public:

    Trace_OverflowEntry(
        tgg00_Debug         Layer,
        tgg00_Debug         OriginalLayer,
        tgg00_VtraceType    Type,
        const tsp00_Byte   *pEntry,
        SAPDB_Int4          EntrySize,
        SAPDB_Int2          PartCount )
    :
    Trace_IBaseEntry( Layer, Type ),
    m_OriginalLayer( OriginalLayer ),
    m_PartCount( PartCount ),
    m_EntrySize( EntrySize ),
    m_pEntry( pEntry )
    {}

    /*!-------------------------------------------------------------------------
    function:     GetLayer
    description:  This method returns the debugging layer information of the
                  trace entry.
    arguments:    none
    return value: debugging layer
    --------------------------------------------------------------------------*/

    virtual tgg00_Debug GetLayer() const{
        return( m_Layer );
    }

    /*!-------------------------------------------------------------------------
    function:     GetSize
    description:  This method returns the size of the trace entry body inclusive
                  the size of the internal header!
    arguments:    none
    return value: complete entry size in bytes
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetSize() const{
        return( m_EntrySize + TRACE_INTERNAL_HEADER_SIZE );
    }

    /*!-------------------------------------------------------------------------
    function:     GetType
    description:  This method returns the debugging type information of the
                  trace entry.
    arguments:    none
    return value: debugging type
    --------------------------------------------------------------------------*/

    virtual tgg00_VtraceType GetType() const{
        return( m_Type );
    }

    /*!-------------------------------------------------------------------------
    function:     Materialize
    description:  This method is used to store the trace entry within the memory
                  trace buffers. The trace entry consists of the proper trace
                  entry body and the internal header.
    arguments:    pDestination    [in] points to the destination
                  DestinationSize [in] gives the size of the destination in bytes
    return value: true means that the materialization was successful; else false
    --------------------------------------------------------------------------*/

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const;

private:

    tgg00_Debug         m_OriginalLayer;
    SAPDB_Int2          m_PartCount;
    SAPDB_Int4          m_EntrySize;
    const tsp00_Byte    *m_pEntry;
};

/*!*****************************************************************************

   endclass: Trace_OverflowEntry

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_LargeEntry

   description: The class Trace_LargeEntry is used for trace entries larger
                than 132 bytes for example the SAPDB order packet or the 
                internal kernel mess block. The class offers the possibility
                to split an entry larger than 132 bytes into more than one
                separate persistent entries. Each entry contains an internal
                header that includes the original trace layer information and
                the current split count. This causes that the proper body of
                the pesistent trace entry is reduced in size to 129 bytes.
                Note that each trace entry derived from Trace_LargeEntry has 
                this internal header independend of the original size.
   
*******************************************************************************/

class Trace_LargeEntry : public Trace_ILargeEntry
{
public:

    /*!-------------------------------------------------------------------------
    function:     ReInit
    description:  This method is used to reinitialized the members of the
                  class. Note that the members of the base class TRace_ILarge
                  are not touched!
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void ReInit( const Trace_LargeEntry &LargeEntry )
    {
        // Note that the members of Trace_ILargeEntry are no reinitialized
        // i.e. Trans, Layer, Type are unchanged.

        m_NumberOfParts   = LargeEntry.m_NumberOfParts;
        m_CurrentPartNo   = 1;
        m_CurrentPartSize = LargeEntry.m_CurrentPartSize;
        m_EntrySize       = LargeEntry.m_EntrySize;
        m_pCurrentPart    = LargeEntry.m_pCurrentPart;
        m_pEntry          = LargeEntry.m_pEntry;
    };


    /*!-------------------------------------------------------------------------
    function:     GetLayer
    description:  This method returns the original debugging layer information 
                  of the trace entry.
    arguments:    none
    return value: debugging layer
    --------------------------------------------------------------------------*/

    virtual tgg00_Debug GetLayer() const{
        return( m_Layer );
    }

    /*!-------------------------------------------------------------------------
    function:     GetNumberOfParts
    description:  This method returns the number of separate parts needed to
                  materialize the whole trace entry within the memory trace 
                  buffers. This number considers the reduced size of a trace body
                  caused by the internal header.
    arguments:    none
    return value: part number
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetNumberOfParts() const{
        return( m_NumberOfParts );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPart
    description:  This method returns a pointer onto the current part of the
                  complete trace entry.
    arguments:    none
    return value: pointer onto the current trace body part
    --------------------------------------------------------------------------*/

    virtual tsp00_BytePtr GetPart() const{
        return( m_pCurrentPart );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPartNo
    description:  This method returns the number of the current part. This number
                  has to be greater than zero and less equal GetNumberOfParts.
    arguments:    none
    return value: current part number
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetPartNo() const{
        return( m_CurrentPartNo );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPartSize
    description:  This method returns the size of the current part in bytes.
                  Note that this size doesn't contains the size of the internal 
                  header! Therefore the maximum returned size is 129 bytes.
    arguments:    none
    return value: current part size in bytes
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetPartSize() const{
        return( m_CurrentPartSize );
    }

    /*!-------------------------------------------------------------------------
    function:     GetSize
    description:  This method returns the size in bytes that the trace entry 
                  needs if it is materialized. This includes the proper trace
                  entry body and the internal header for all parts. This doesn't 
                  includes the common trace entry header.
    arguments:    none
    return value: complete size in bytes
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetSize() const
    {
        // This is the size of the complete entry if it is materialized
        // in one or more than one trace entries.
        return(
                  m_EntrySize +
                  ( GetNumberOfParts() * TRACE_INTERNAL_HEADER_SIZE ));
    }

    /*!-------------------------------------------------------------------------
    function:     GetType
    description:  This method returns the original debugging type information 
                  of the trace entry.
    arguments:    none
    return value: debugging type
    --------------------------------------------------------------------------*/

    virtual tgg00_VtraceType GetType() const{
        return( m_Type );
    }

    /*!-------------------------------------------------------------------------
    function:     PartEndReached
    description:  This method is used to report whether the current part is valid.
    arguments:    none
    return value: true means no following part exist; else false
    --------------------------------------------------------------------------*/

    virtual bool PartEndReached() const{
        return( m_CurrentPartNo > m_NumberOfParts );
    }


    /*!-------------------------------------------------------------------------
    function:     SetToFirstPart
    description:  This method is used to set the internal state to the first part
                  of the current trace entry. Note that there is no check whether 
                  the part is valid.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    virtual void SetToFirstPart()
    {
        m_CurrentPartNo   = 1;
        m_pCurrentPart    = m_pEntry;
        m_CurrentPartSize =
            ( UsablePartSize() < m_EntrySize ) ? UsablePartSize() : m_EntrySize;
    }

    /*!-------------------------------------------------------------------------
    function:     SetToNextPart
    description:  This method is used to set the internal state to the next part
                  of the current trace entry. Note that there is no check whether 
                  the part is valid.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    virtual void SetToNextPart()
    {
        ++m_CurrentPartNo;
        m_pCurrentPart += m_CurrentPartSize;

        if ( m_CurrentPartNo < m_NumberOfParts ) return;

        // Last part of the complete trace entry is reached
        m_CurrentPartSize = ( m_EntrySize % this->UsablePartSize() );
    }

protected:

    Trace_LargeEntry(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type,
        tsp00_BytePtr       pEntry,
        SAPDB_Int4          EntrySize )
    :
    Trace_ILargeEntry( Layer, Type ),
    m_NumberOfParts( 1 ),
    m_CurrentPartNo( 1 ),
    m_CurrentPartSize( EntrySize ), // only current part; No internal header!
    m_EntrySize( EntrySize ),       // whole trace entry; No internal header!
    m_pCurrentPart( pEntry ),       // points to current part; No internal header!
    m_pEntry( pEntry )              // points to whole trace entry; No internal header!
    {
        // If the trace entry is longer than MaximumSize, the trace
        // entry will be splited into separate parts. Note that each
        // part has an internal header and therefore it is not possible
        // to fill the whole MaximumSize with plain trace info!

        if ( UsablePartSize() < EntrySize )
        {
            m_CurrentPartSize = UsablePartSize();
            m_NumberOfParts   = ( m_EntrySize / m_CurrentPartSize ) + 1;
        }
    }

protected:

    SAPDB_Int4          m_NumberOfParts;
    SAPDB_Int4          m_CurrentPartNo;
    SAPDB_Int4          m_CurrentPartSize;
    SAPDB_Int4          m_EntrySize;
    tsp00_BytePtr       m_pCurrentPart;
    tsp00_BytePtr       m_pEntry;
};

/*!*****************************************************************************

   endclass: Trace_LargeEntry

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_LargeEntryIterator

   description: Thes class Trace_LargeEntryIterator works on the above mentioned
                class Trace_LargeEntry. It is used by the trace manager to 
                iterate over all parts of a large trace entry.
   
*******************************************************************************/

class Trace_LargeEntryIterator
{
public:

    /*!-------------------------------------------------------------------------
    function:     constructor
    description:  This method is used to construct an iterator working on a
                  large trace entry.
    arguments:    Entry [in] the iterator works on this trace entry
    return value: none
    --------------------------------------------------------------------------*/

    Trace_LargeEntryIterator( Trace_ILargeEntry &Entry )
    :
    m_LargeEntry( Entry )
    {}

    /*!-------------------------------------------------------------------------
    function:     operator ++
    description:  This method is used to set the iterator to the next part of 
                  given large trace entry. Note that there is no check whether 
                  the part is valid.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void operator ++(){
        m_LargeEntry.SetToNextPart();
    }

    /*!-------------------------------------------------------------------------
    function:     Begin
    description:  This method is used to set the iterator to the first part of the
                  current trace entry. Note that there is no check whether the part 
    is valid.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void Begin(){
        m_LargeEntry.SetToFirstPart();
    }

    /*!-------------------------------------------------------------------------
    function:     End
    description:  This method is used to report whether the current position of
                  the iterator points to a valid part.
    arguments:    none
    return value: true means that the current part isn't valid.
    --------------------------------------------------------------------------*/

    bool End() const{
        return( m_LargeEntry.PartEndReached() );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPart
    description:  This method returns a pointer onto the current part of the
                  complete trace entry.
    arguments:    none
    return value: pointer onto the current trace body part
    --------------------------------------------------------------------------*/

    tsp00_BytePtr GetPart() const{
        return( m_LargeEntry.GetPart() );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPartNo
    description:  This method returns the number of the current part.
    arguments:    none
    return value: current part number
    --------------------------------------------------------------------------*/

    SAPDB_Int4 GetPartNo() const{
        return( m_LargeEntry.GetPartNo() );
    }

    /*!-------------------------------------------------------------------------
    function:     GetPartSize
    description:  This method returns the size of the current part in bytes.
                  Note that this size doesn't contains the size of the internal 
                  header! Therefore the maximum returned size is 129 bytes.
    arguments:    none
    return value: current part size in bytes
    --------------------------------------------------------------------------*/

    SAPDB_Int4 GetPartSize() const{
        return( m_LargeEntry.GetPartSize() );
    }

private:

    Trace_ILargeEntry   &m_LargeEntry;
};

/*!*****************************************************************************

   endclass: Trace_LargeEntryIterator

*******************************************************************************/

/*!*****************************************************************************

   class: Trace_KernelFullEntry

   description: The class Trace_KernelFullEntry combines the user defined trace
                entry with the trace entry header containing administrative
                informations like task id and transaction number.
   
*******************************************************************************/

class Trace_KernelFullEntry : public Trace_IFullEntry
{
public:

    Trace_KernelFullEntry(
        Trans_Context       &trans,
        Trace_IBaseEntry    &Entry,
        const Trace_SeqNo   SequenceNo);


    /*!-------------------------------------------------------------------------
    function:     GetSequenceNo
    description:  This method is used to get the unique number of the current
                  trace entry.
    arguments:    none
    return value: unique number of the current trace entry
    --------------------------------------------------------------------------*/

    virtual Trace_SeqNo GetSequenceNo() const{
        return( m_Header.vhSequenceNo_gg11 );
    }

    /*!-------------------------------------------------------------------------
    function:     GetSize
    description:  This method returns the persistent size of the current trace
                  entry. This includes the common trace entry header.
    return value: complete persistent size in bytes of the trace entry
    --------------------------------------------------------------------------*/

    virtual SAPDB_Int4 GetSize() const{
        return( m_Header.vhLen_gg11 );
    }

    /*!-------------------------------------------------------------------------
    function:     IsValid
    description:  This method reports wheather a trace entry is valid.
    return value: true means that the trace entry handler points to a vald trace
                  entry; else false
    --------------------------------------------------------------------------*/

    virtual bool IsValid() const
    {
        return
            (
                ( 0 < m_Header.vhLen_gg11 ) &&
                ( Trace_IBaseEntry::MaximumSize() + sizeof( m_Header )
                  > SAPDB_UInt4 (m_Header.vhLen_gg11) ) // cast to avoid warning C4018 on NT
            );
    }

    /*!-------------------------------------------------------------------------
    function:     Materialize
    description:  This method is used to store the trace entry within the memory
                  trace buffers. The trace entry consists of the proper trace
                  entry body, the internal header and the common trace entry header
                  containg information about transaction, task session and so on.
    arguments:    pDestination    [in] points to the destination
                  DestinationSize [in] gives the size of the destination in bytes
    return value: true means that the materialization was successful; else false
    --------------------------------------------------------------------------*/

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const;

private:

    Trace_IBaseEntry    &m_Info;
    tgg11_VtraceHeader  m_Header;
};

/*!*****************************************************************************

   endclass: Trace_KernelFullEntry

*******************************************************************************/

#endif  /* TRACE_ENTRY_HPP */
