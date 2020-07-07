/*!**************************************************************************

  module      : Log_ActionObjectFile.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : implements class Log_ActionObjectFile



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

#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionObjectFile.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "hbd04.h"
#include "hbd95.h"
#include "hbd91.h"

/* --------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------*/
bool Log_ActionObjectFile::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                 SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionObjectFile::WriteImagePersistent", LogAction_Trace, 5);
    
    if ( ! IsValid() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Log_ActionObjectFile::WriteImagePersistent() Action must be valid.");
        return false;
    }

    if ( ! ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist) )
        return false;
    
    if ( m_Head.secListLength > 0 )
        if (! ImageWriter.PersistentWrite ( m_pPersistentSecList,
                                            m_Head.secListLength,
                                            errlist ) )
            return false;
    return true;
}

/* --------------------------------------------------------------------------*/
bool Log_ActionObjectFile::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionObjectFile::ReadImagePersistent", LogAction_Trace, 5);

    if ( ! ImageReader.CopyImage(reinterpret_cast<SAPDB_Byte*>(&m_Head), sizeof(m_Head), errlist) )
        return false;

    if ( m_Head.secListLength > 0 )
    {
        if ( ! ImageReader.PersistentRead ( m_pAllocatedPersistentSecFileIdList,
                                           m_Head.secListLength,
                                           m_pAllocator,
                                           errlist ) )
            return false;    
        m_pPersistentSecList = m_pAllocatedPersistentSecFileIdList;
    }
    return true;
}

/* --------------------------------------------------------------------------*/
void Log_ActionObjectFile::Redo (tgg00_TransContext   &TransContext,
                                 Log_IImage           &Image,
                                 SAPDBErr_MessageList &Errlist) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionObjectFile::Redo", LogAction_Trace, 5);
    if (e_ok != TransContext.trError_gg00)
        return;

    switch ( Image.GetActionType() )
    {
    case Log_CreateObjectFile:
        {
            LVC_FileNoArray fileNos;
            switch ( m_Head.filetype )
            {
            case Data_OmsKeyFile:
                fileNos.Fill (m_pPersistentSecList,m_Head.secListLength);
                break;
            default:
                break;            
            }
            bd95Rollforward_CreateObjFile (TransContext,
                                           m_Head.fileno,
                                           m_Head.filetype,
                                           m_Head.bodyLength,
                                           m_Head.keyLength,
                                           m_Head.rootPage,
                                           fileNos);

        }
		break;
    case Log_DropObjectFile:
        bd95Rollforward_DropObjFile (TransContext, m_Head.fileno);
        break;
    default:
        TransContext.trError_gg00 = e_not_implemented;
        return;
    }
}

/* --------------------------------------------------------------------------*/
void Log_ActionObjectFile::Undo (tgg00_TransContext &Trans,
                                 Log_IImage         &Image) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionObjectFile::Undo", LogAction_Trace, 5);
    if (e_ok != Trans.trError_gg00)
        return;

    switch ( Image.GetActionType() )
    {
    case Log_CreateObjectFile:
        bd04RollbackCreateObjContainer( Trans, m_Head.fileno );
        if ( e_ok != Trans.trError_gg00 )
            WriteToTrace ( "bd04RollbackDropObjContainer: ERROR" );
        else
        {
            if ( LogAction_Trace.TracesLevel(6) )
            {
                if ( LogAction_Trace.TracesLevel(6) )
                {
                    Image.WriteToTrace ( "bd04RollbackCreateObjContainer: OK" );
                    WriteToTrace (NULL);
                }
            }
        }
        break;
    case Log_DropObjectFile:
        bd04RollbackDropObjContainer( Trans, m_Head.fileno );
        if ( e_ok != Trans.trError_gg00 )
            WriteToTrace ( "bd04RollbackDropObjContainer: ERROR" );
        else
        {
            if ( LogAction_Trace.TracesLevel(6) )
            {
                if ( LogAction_Trace.TracesLevel(6) )
                {
                    Image.WriteToTrace ( "bd04RollbackDropObjContainer: OK" );
                    WriteToTrace (NULL);
                }
            }
        }
        break;
    default:
        Trans.trError_gg00 = e_not_implemented;
    }
}
    

/* --------------------------------------------------------------------------*/
void Log_ActionObjectFile::WriteToTrace (const char * Title) const
{
    if ( Title != NULL )
        Kernel_VTrace() << Title;
//    Kernel_VTrace() << "ObjFileNo: " << m_Head.fileno;
    Kernel_VTrace() << "BodySize: "  << m_Head.bodyLength
                    << ", KeySize: " << m_Head.keyLength;
}
    
/* --------------------------------------------------------------------------*/
bool Log_ActionObjectFile::RemoveGarbage (tgg00_TransContext   &trans,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionObjectFile::RemoveGarbage", LogAction_Trace, 5);

    bd91DropObjFile ( trans, m_Head.fileno );
    if (trans.trError_gg00 != e_ok)
    {
        WriteToTrace("Log_ActionObjectFile::RemoveGarbage");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"bd91DropObjFile");
        Kernel_AppendBasisError(trans.trError_gg00,errlist,"bd91DropObjFile");
        return false;
    }
    return true;
}
