/*!**************************************************************************

    module      : Log_ActionRefreshSurrogSeq.cpp
    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle log entries for files

    last changed: 2001-04-19

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

#include "Logging/Log_ActionRefreshSurrogSeq.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "gsp00.h"
#include "ggg00.h"
#include "hkb57.h"

//---------------------------------------------------------------------------

Log_ActionRefreshSurrogate::Log_ActionRefreshSurrogate (const tgg00_Surrogate &Surrogate,
                                                        const tgg00_Surrogate &SysKey):
Log_ReadWriteActionImage (Log_RefreshSurrogate)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSurrogate::Log_ActionRefreshSurrogate", LogAction_Trace, 5);
    m_Entry.Surrogate = Surrogate;
    m_Entry.SysKey    = SysKey;
}

//---------------------------------------------------------------------------

void Log_ActionRefreshSurrogate::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSurrogate::Execute", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    kb57RefreshSurrogate (TransContext.trTaskId_gg00, m_Entry.Surrogate, m_Entry.SysKey);
}

//---------------------------------------------------------------------------

bool Log_ActionRefreshSurrogate::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                      SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSurrogate::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}


/*===========================================================================*
*  METHODS of Log_ActionRefreshSequence                                      *
*============================================================================*/

Log_ActionRefreshSequence::~Log_ActionRefreshSequence ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSequence::~Log_ActionRefreshSequence", LogAction_Trace, 5);
    if (NULL != m_pSequenceListAllocator)
    {
        m_pSequenceListAllocator->Deallocate (m_pSequenceListForRead);
        m_pSequenceListForRead = NULL;
    }
}

//---------------------------------------------------------------------------

void Log_ActionRefreshSequence::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSequence::Execute", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    for (SAPDB_Int Curr = 0; Curr < m_SequenceCount; ++Curr)
    {
        kb57RefreshSequence 
            (TransContext, (m_pSequenceList+Curr)->Surrogate, (m_pSequenceList+Curr)->Number);
        
        if (e_ok != TransContext.trError_gg00) return;
    }
}

//---------------------------------------------------------------------------

bool Log_ActionRefreshSequence::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                     SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSequence::ReadImagePersistent", LogAction_Trace, 5);
    if ( ! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_SequenceCount)),sizeof(m_SequenceCount),errlist))
        return false;
    
    if ( ! ImageReader.PersistentRead (REINTERPRET_CAST (SAPDB_Byte*&, m_pSequenceListForRead),
                                       m_SequenceCount * sizeof(SequenceItem), m_pSequenceListAllocator,
                                       errlist) )
        return false;

    m_pSequenceList = m_pSequenceListForRead;
    return true;
}

//---------------------------------------------------------------------------

bool Log_ActionRefreshSequence::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                      SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRefreshSequence::WriteImagePersistent", LogAction_Trace, 5);
    if ( ! ImageWriter.PersistentWrite (&(m_SequenceCount), sizeof (m_SequenceCount), errlist) )
        return false;
    if ( ! ImageWriter.PersistentWrite (m_pSequenceList, m_SequenceCount * sizeof(SequenceItem), errlist) )
        return false;
	return true;
}
