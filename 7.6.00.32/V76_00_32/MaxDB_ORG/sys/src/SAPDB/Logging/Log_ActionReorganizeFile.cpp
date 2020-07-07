/*!**************************************************************************

    module      : Log_ActionReorganizeFile.cpp
    -------------------------------------------------------------------------

    responsible : UweH

    special area: Logging
    description : defines a class to handle load begin

    last changed: 2002-08-09

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
*  INCLUDES                                                                  *
*============================================================================*/

#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Logging/Log_ActionReorganizeFile.hpp"
#include "Logging/Log_ActionFileHandling.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd09.h"

/*===========================================================================*
*  FUNCTIONS                                                                 *
*============================================================================*/

//---------------------------------------------------------------------------

void Log_ActionReorganizeFile::ExecuteAction
            ( tgg00_TransContext &TransContext,
              tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionReorganizeFile::ExecuteAction", LogAction_Trace, 5);
    
    if ( TransContext.trError_gg00 != e_ok )
        return; 
    
    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId; 

    pExecuteFileId = Log_GetFileId (m_head.filename, RedoFileId, pCommandFileId);

    tgg00_Lkey lastKey;
    lastKey.keyLen_gg00() = m_head.lastKeyLength;

    if ( lastKey.keyLen_gg00() > 0 )
        SAPDB_RangeMove(__FILE__, 1,
                        m_head.lastKeyLength, sizeof(lastKey.keyVal_gg00()),
                        m_pLastKey, 1,
                        &(lastKey.keyVal_gg00()), 1,
                        lastKey.keyLen_gg00(),
                        TransContext.trError_gg00);

    if ( TransContext.trError_gg00 != e_ok )
        return;

    bd09ReorgTreeIndex (TransContext, *pExecuteFileId, lastKey);

    if ( e_file_not_found == TransContext.trError_gg00 )
        TransContext.trError_gg00 = e_ok;
}

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
