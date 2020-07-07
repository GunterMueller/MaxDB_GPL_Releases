/****************************************************************************

  module      : OMS_Sequence.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: OMS 
  description : OMS sequence

  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

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

#include "Oms/OMS_Sequence.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Session.hpp"
#include "ggg00.h"
#include <new.h>

struct OmsSequenceBody
{
    tgg00_Surrogate m_sequenceId;
    tgg00_SeqInfo   m_sequenceInfo;
};

/*----------------------------------------------------------------------*/

OmsSequence::OmsSequence (const OmsHandle& handle, const char* sequenceName)
: m_pBody(0)
{
    const char* msg = "OmsSequence::OmsSequence";
    short error;
    m_pBody = new (OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sizeof(OmsSequenceBody))) OmsSequenceBody;
    tsp00_KnlIdentifier sequenceIdentifier;
    OMS_Globals::MakeKnlIdentifier(sequenceName, sequenceIdentifier);
    (*handle.m_session)->AttachSequence(sequenceIdentifier, m_pBody->m_sequenceId, 
        m_pBody->m_sequenceInfo, error);
    if (0 != error)
    {
        OMS_Globals::Throw(error, msg, __MY_FILE__, __LINE__);
    }
}

/*----------------------------------------------------------------------*/

OmsSequence::OmsSequence(const OmsSequence& source)
: m_pBody(0)
{
    *this = source;
}

/*----------------------------------------------------------------------*/

void OmsSequence::operator= (const OmsSequence& source)
{

    if (&source != this)
    {
        OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(m_pBody);
        m_pBody  = new (OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sizeof(OmsSequenceBody))) OmsSequenceBody;
        *m_pBody = *source.m_pBody;
    }
}

/*----------------------------------------------------------------------*/

OmsSequence::~OmsSequence ()
{
    OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(m_pBody);
}

/*----------------------------------------------------------------------*/

OmsTypeUInt8 OmsSequence::omsNextVal(const OmsHandle& handle)
{
    const char* msg = "omsNextVal";
    OmsTypeUInt4 nextVal;
    short        sqlError;
    (*handle.m_session)->NextVal(m_pBody->m_sequenceId, m_pBody->m_sequenceInfo, nextVal, sqlError);
    if (0 != sqlError)
    {
        OMS_Globals::Throw(sqlError, msg, __MY_FILE__, __LINE__);
    }
    return nextVal;
}

/*----------------------------------------------------------------------*/