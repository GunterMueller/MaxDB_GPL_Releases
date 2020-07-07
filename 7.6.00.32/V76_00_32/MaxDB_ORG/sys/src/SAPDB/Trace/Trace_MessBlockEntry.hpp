/*!**************************************************************************

  module      : Trace_MessBlockEntry.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Handling of mess block trace entries
  description : 

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


#ifndef TRACE_MESSBLOCKENTRY_HPP
#define TRACE_MESSBLOCKENTRY_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Trace/Trace_Types.hpp"
#include "Trace/Trace_Entry.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

enum Trace_MessBlockKind
{
    E_MessHeader,
    E_MessTrailer,
    E_MessQual,
    E_MessData,
    E_MessStack,
    E_MessStrategy, // PTS 1112689 TS 2001-11-19
    E_MessEnd
};

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

class Trace_MessBlockPart : public Trace_LargeEntry
{
public:

    Trace_MessBlockPart(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type,
        tsp00_BytePtr       pEntry,
        SAPDB_Int4          EntrySize )
    :Trace_LargeEntry( Layer, Type, pEntry, EntrySize ){}

};

/*---------------------------------------------------------------------------*/

class Trace_MessBlockIterator
{
public:

    Trace_MessBlockIterator(
        tgg00_MessBlock     &MessBlock,
        tgg00_Debug         &Layer,
        tgg00_VtraceType    &Type )
    :
    m_PartKind( E_MessHeader ),
    m_TraceLayer( Layer),
    m_TraceType( Type ),
    m_MessBlock( MessBlock )
    {}

    void operator ++();

    Trace_MessBlockPart operator *() const
    {
        return( Trace_MessBlockPart( m_TraceLayer, m_TraceType,
                                     GetPart(), GetPartSize() ));
    }

    void Begin(){
        m_PartKind = E_MessHeader;
    }


    bool End() const{
        return( E_MessEnd <= m_PartKind );
    }

private:

    SAPDB_Int4 GetPartSize() const;

    tsp00_BytePtr GetPart() const;

private:

    Trace_MessBlockKind     m_PartKind;
    tgg00_Debug             m_TraceLayer; // PTS 1109936 TS 2001-04-06
    tgg00_VtraceType        m_TraceType;  // PTS 1109936 TS 2001-04-06
    tgg00_MessBlock         &m_MessBlock;
};

/*---------------------------------------------------------------------------*/

class Trace_MessBlockPartIterator : public Trace_LargeEntryIterator
{
public:

    Trace_MessBlockPartIterator( Trace_MessBlockPart &MessBlockPart )
    :Trace_LargeEntryIterator( MessBlockPart ){}
};

/*---------------------------------------------------------------------------*/

class Trace_MessBlockEntry : public Trace_ILargeEntry
{
public:

    Trace_MessBlockEntry(
        tgg00_MessBlock     &MessBlock,
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type );

    ~Trace_MessBlockEntry();


    virtual tgg00_Debug GetLayer() const{
        return( m_Layer );
    }

    virtual SAPDB_Int4 GetNumberOfParts() const{
        return( m_NumberOfParts );
    }

    virtual tsp00_BytePtr GetPart() const{
        return( m_MessBlockPartIter.GetPart() );
    }

    virtual SAPDB_Int4 GetPartNo() const{
        return( m_CurrentPartNo );
    }

    virtual SAPDB_Int4 GetPartSize() const{
        return( m_MessBlockPartIter.GetPartSize() );
    }

    virtual SAPDB_Int4 GetSize() const
    {
        // This is the size of the complete entry. This includes all mess block
        // parts (header, trailer, ...) and the internal header size.
        return( m_EntrySize );
    }


    virtual tgg00_VtraceType GetType() const{
        return( m_Type );
    }

    virtual bool PartEndReached() const{
        return( m_CurrentPartNo > m_NumberOfParts );
    }

    virtual void SetToFirstPart()
    {
        m_CurrentPartNo = 1;
        m_MessBlockIter.Begin();
        m_MessBlockPart.ReInit( *m_MessBlockIter );
        m_MessBlockPartIter.Begin();
    }

    virtual void SetToNextPart();

private:

    SAPDB_Int4                  m_CurrentPartNo;
    SAPDB_Int4                  m_NumberOfParts; // fixed
    SAPDB_Int4                  m_EntrySize;     // fixed
    Trace_MessBlockIterator     m_MessBlockIter;
    Trace_MessBlockPart         m_MessBlockPart;
    Trace_MessBlockPartIterator m_MessBlockPartIter;
};

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

inline tsp00_BytePtr
Trace_MessBlockIterator::GetPart() const
{
    switch( m_PartKind )
    {
    case E_MessHeader:
        return( REINTERPRET_CAST( tsp00_BytePtr, &m_MessBlock.mb_header()) );

    case E_MessTrailer:
        return( REINTERPRET_CAST( tsp00_BytePtr, &m_MessBlock.mb_trailer()) );

    case E_MessQual:
        return( REINTERPRET_CAST( tsp00_BytePtr, m_MessBlock.mb_qual()) );

    case E_MessData:
        return( REINTERPRET_CAST( tsp00_BytePtr, m_MessBlock.mb_data()) );

    case E_MessStack:
        return( REINTERPRET_CAST( tsp00_BytePtr, m_MessBlock.mb_st()));

    case E_MessStrategy: // PTS 1112689 TS 2001-11-19
        return( REINTERPRET_CAST( tsp00_BytePtr, m_MessBlock.mb_strat()) );

    default:
        return( 0 );
    }
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4
Trace_MessBlockIterator::GetPartSize() const
{
    switch( m_PartKind )
    {
    case E_MessHeader:
        return( sizeof( m_MessBlock.mb_header()) );

    case E_MessTrailer:
        return( sizeof( m_MessBlock.mb_trailer()) );

    case E_MessQual:
        return( m_MessBlock.mb_qual_len() );

    case E_MessData:
        return( m_MessBlock.mb_data_len() );

    case E_MessStack:
        {
            SAPDB_Int4 stackLength = 0;

            if (( ak_send == m_TraceType ) && ( NULL != m_MessBlock.mb_qual() ))
            {
                stackLength = ( sizeof( tgg00_StackEntry ) *
                                (m_MessBlock.mb_qual()->mfirst_free() - 1 ));
            }
            return( stackLength );
        };
    case E_MessStrategy: // PTS 1112689 TS 2001-11-19
        return( m_MessBlock.mb_strat_len() );

    default:
        return( 0 );
    }
};

#endif  /* TRACE_MESSBLOCKENTRY_HPP */
