/****************************************************************************

  module      : Trace_MessBlockEntry.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2000-12-14  18:15
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "Trace/Trace_MessBlockEntry.hpp"
#include "Trace/Trace_Manager.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Trace_MessBlockEntry::Trace_MessBlockEntry(
    tgg00_MessBlock      &MessBlock,
    tgg00_Debug          Layer,
    tgg00_VtraceType     Type )
        :
        Trace_ILargeEntry( Layer, Type ),
        m_CurrentPartNo( 1 ),
        m_NumberOfParts( 0 ),
        m_EntrySize( 0 ),
        m_MessBlockIter( MessBlock, Layer, Type ),
        m_MessBlockPart( Layer, Type, (*m_MessBlockIter).GetPart(), (*m_MessBlockIter).GetPartSize()),
        m_MessBlockPartIter( m_MessBlockPart )
{
    for( m_MessBlockIter.Begin(); !m_MessBlockIter.End(); ++m_MessBlockIter )
    {
        m_MessBlockPart.ReInit( *m_MessBlockIter );

        m_NumberOfParts += m_MessBlockPart.GetNumberOfParts();
        m_EntrySize     += m_MessBlockPart.GetSize();
    }
};

/*---------------------------------------------------------------------------*/

Trace_MessBlockEntry::~Trace_MessBlockEntry()
{
    Trace_Manager::GetInstance().InsertLarge(  *this );
};

/*---------------------------------------------------------------------------*/

void
Trace_MessBlockEntry::SetToNextPart()
{

    // Count the total number of parts in which the complete mess block
    // trace entry is splited.
    ++m_CurrentPartNo;

    // Set to the next part inside a mess block, e.g. from the first part of
    // a mess block header to the second part of the same mess block header.
    // Note that each part has a size limitation of MaximumSize (=> 132 bytes).

    ++m_MessBlockPartIter;
    if (! m_MessBlockPartIter.End() ) return;

    // Set to the next part of the mess block, i.e. from header to trailer or
    // from the qual part to the data part.
    ++m_MessBlockIter;
    if( m_MessBlockIter.End() ) return;

    // In case of a valid mess block part the internal mess block part
    // will be set to the new current mess block part.
    m_MessBlockPart.ReInit( *m_MessBlockIter );
    m_MessBlockPartIter.Begin();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
Trace_MessBlockIterator::operator ++()
{
    switch( m_PartKind )
    {
    case E_MessHeader:
        {
            m_PartKind  = E_MessTrailer;
            return;
        };
    case E_MessTrailer:
        {
            // PTS 1129354 M.Ki.
            m_PartKind = E_MessQual;
            if ( (NULL == m_MessBlock.mb_qual())
                    || (0 >= m_MessBlock.mb_qual_len()) )
                this->operator++();
            return;
        };
    case E_MessQual:
        {
            m_PartKind  = E_MessData;
            if ( (NULL == m_MessBlock.mb_data()) ||
                    (0 >= m_MessBlock.mb_data_len()) )
                this->operator++();
            return;
        };
    case E_MessData:
        {
            m_PartKind = E_MessStack;
            if ( (ak_send != m_TraceType)       ||
                    (NULL == m_MessBlock.mb_st())   ||
                    (NULL == m_MessBlock.mb_qual()) ||
                    (0 >= m_MessBlock.mb_qual_len()) ||
                    (1 >= m_MessBlock.mb_qual()->mstack_desc().mfirst_free()) )
                this->operator++();
            return;
        };
    case E_MessStack:
        {
            m_PartKind = E_MessStrategy;
            if( 0 >= m_MessBlock.mb_strat_len() ) // PTS 1112689
                this->operator++();
            return;
        };
    case E_MessStrategy: // PTS 1112689
        ;
    default:
        {
            m_PartKind  = E_MessEnd;
            return;
        }
    }
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
