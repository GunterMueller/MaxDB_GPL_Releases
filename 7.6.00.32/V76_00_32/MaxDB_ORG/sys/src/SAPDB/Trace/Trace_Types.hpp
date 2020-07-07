/****************************************************************************

  module      : Trace_Types.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
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


#ifndef TRACE_TYPES_HPP
#define TRACE_TYPES_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg11.h"
#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TRACE_NIL_PNO	            0
#define TRACE_FIRST_PNO             1
#define TRACE_FIRST_SEQNO           1
#define TRACE_INTERNAL_HEADER_SIZE  3


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Trace );

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef SAPDB_UInt4 Trace_PageNo;
typedef SAPDB_UInt8	Trace_SeqNo;

/*---------------------------------------------------------------------------*/

class Trace_IBaseEntry
{
public:

    static SAPDB_Int4 MaximumSize(){
        return( sizeof( tgg11_VtraceBody ));
    }

    virtual tgg00_Debug GetLayer() const = 0;

    virtual SAPDB_Int4 GetSize() const = 0;

    virtual tgg00_VtraceType GetType() const = 0;

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const = 0;

protected:

    Trace_IBaseEntry(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type )
    :
    m_Layer( Layer ),
    m_Type( Type )
    {}

    virtual ~Trace_IBaseEntry()
    {}

protected:

    tgg00_Debug         m_Layer;
    tgg00_VtraceType    m_Type;
};

/*---------------------------------------------------------------------------*/

class Trace_ILargeEntry
{
public:

    static SAPDB_Int4 UsablePartSize(){
        return( Trace_IBaseEntry::MaximumSize() - TRACE_INTERNAL_HEADER_SIZE );
    }

    virtual tgg00_Debug GetLayer() const = 0;

    virtual SAPDB_Int4 GetNumberOfParts() const = 0;

    virtual tsp00_BytePtr GetPart() const = 0;

    virtual SAPDB_Int4 GetPartNo() const = 0;

    virtual SAPDB_Int4 GetPartSize() const = 0;

    virtual SAPDB_Int4 GetSize() const = 0;

    virtual tgg00_VtraceType GetType() const = 0;

    virtual bool PartEndReached() const = 0;

    virtual void SetToFirstPart() = 0;

    virtual void SetToNextPart() = 0;

protected:

    Trace_ILargeEntry(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type )
    :
    m_Layer( Layer ),
    m_Type( Type )
    {}

    virtual ~Trace_ILargeEntry()
    {}

protected:

    tgg00_Debug         m_Layer;
    tgg00_VtraceType    m_Type;
};

/*---------------------------------------------------------------------------*/

class Trace_IFullEntry
{
public:

    virtual Trace_SeqNo GetSequenceNo() const = 0;

    virtual SAPDB_Int4 GetSize() const = 0;

    virtual bool IsValid() const = 0;

    virtual bool Materialize(
        tsp00_BytePtr   pDestination,
        SAPDB_Int4      DestinationSize ) const = 0;
};

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/


#endif  /* TRACE_TYPES_HPP */
