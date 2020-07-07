
/*!**************************************************************************

    module      : vkb661.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : KB_ModifyTable_interface

    last changed: 2001-03-30

    -------------------------------------------------------------------------

    copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_ActionModifyTable.hpp"
#include "Logging/Log_ActionReassignSecondaryFile.hpp"
#include "Logging/Log_ActionSetFileAttributes.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Transaction/Trans_Context.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "hkb661.h" // check of PASCAL header vkb661


/*===========================================================================*
* PASCAL INTERFACE of Log_ActionModifyTable                                  *
*============================================================================*/

externPascal void kb661ModifyTable (tgg00_TransContext       &TransContext,
                                    tgg00_FileId             &SourceFileId,
                                    tgg00_FileId             &IndexFileId,
                                    tgg00_FileId             &DestinFileId,
                                    tsp00_Int4                StackListEntries,
                                    const tgg00_StackListPtr  SourceStackList,
                                    const tgg00_StackListPtr  DestinStackList,
                                    pasbool                   IsSorted,
                                    tsp00_Int4                TruncCheckPos,
                                    tsp00_Int4                TruncCheckCnt,
                                    tgg00_SampleInfo         &ResultSampleInfo)
{
    if (e_ok != TransContext.trError_gg00) return;
    
    const bool Not_LookForUniqueEntries = false;
    
    Log_InvDescMap SourceInvDescMap;
    Log_InvDescMap DestinInvDescMap;
    
    SourceInvDescMap.Assign (SourceStackList, StackListEntries, Not_LookForUniqueEntries);
    DestinInvDescMap.Assign (DestinStackList, StackListEntries, Not_LookForUniqueEntries);
    
    {
        Log_ActionModifyTable AfterImage (
            SourceFileId.fileName_gg00(),
            DestinFileId.fileName_gg00(),
            &SourceInvDescMap,
            &DestinInvDescMap);
        
        AfterImage.ExecuteAction (
            TransContext, 
            &SourceFileId,
            &IndexFileId,
            &DestinFileId,
            (SAPDB_TRUE == IsSorted),
            TruncCheckPos,
            TruncCheckCnt,
            &ResultSampleInfo);
        
        if (e_ok != TransContext.trError_gg00) return;
        
        AfterImage.WriteAfterImage (TransContext);        
    }
}

/*===========================================================================*
* PASCAL INTERFACE of Log_ActionReassignSecondaryFile                        *
*============================================================================*/
externPascal void kb661ReassignSecondaryFile (
    tgg00_TransContext VAR_VALUE_REF trans,
    tgg00_Surrogate    VAR_ARRAY_REF secondaryFileSurrogate,
    tgg00_Surrogate    VAR_ARRAY_REF fromPrimaryFileSurrogate,
    tgg00_Surrogate    VAR_ARRAY_REF toPrimaryFileSurrogate)
{
    Log_ActionReassignSecondaryFile beforeImage ( secondaryFileSurrogate,
                                                  toPrimaryFileSurrogate,
                                                  fromPrimaryFileSurrogate );
    beforeImage.WriteBeforeImage (trans);

    Log_ActionReassignSecondaryFile afterImage ( secondaryFileSurrogate,
                                                 fromPrimaryFileSurrogate,
                                                 toPrimaryFileSurrogate );
    afterImage.ExecuteAction (trans);

    afterImage.WriteAfterImage (trans);
}

/*===========================================================================*
* PASCAL INTERFACE of Log_ActionSetFileAttributes                            *
*============================================================================*/
externPascal void kb661SetFileAttributePagesClustered (
    tgg00_TransContext    VAR_VALUE_REF trans,
    const tgg00_Surrogate VAR_ARRAY_REF fileSurrogate,
    pasbool                             bPagesClustered)
{
    Log_ActionSetFileAttributes action (fileSurrogate);
    if (! action.SetPagesClustered (Trans_Context::GetContext (trans), bPagesClustered ? true : false))
    {
        trans.trError_gg00 = e_file_not_found;
        return;
    }
    action.WriteBeforeImage (trans);
    if (trans.trError_gg00 != e_ok)
        return;

    action.Execute (trans);
    if (trans.trError_gg00 == e_ok)
        action.WriteAfterImage (trans);
}

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
