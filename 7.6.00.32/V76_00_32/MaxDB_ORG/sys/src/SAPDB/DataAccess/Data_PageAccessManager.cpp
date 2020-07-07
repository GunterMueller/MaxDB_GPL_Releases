/*!***************************************************************************

  module      : Data_PageAccessManager.hpp
  special area: DataAccess
  last changed: 2000-11-20
  last changed: 2000-11-20  12:00
  Copyright (c) 2000-2005 SAP AG
  responsible : UweH
  description : defines an interface to the Datacache



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


#include "ggg00.h"
#include "hbd01_1.h"    // b01nilcurrent
#include "hbd06.h"      // b06file_opmsg
#include "hbd13.h"
#include "hgg04.h"      // g04build_temp_tree_id
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"

/* --------------------------------------------------------------------------- */
inline static tbd_node_request_Param
ConvertToRequestMode (Data_AccessMode Mode)
{
    switch (Mode)
    {
    case Data_ForRead:
        return nr_for_read;
    case Data_ForStructureChange:
    case Data_ForUpdate:
    case Data_ForIntend:
        return nr_for_update;
    default:
        return nr_for_read;
    }
}

/* --------------------------------------------------------------------------- */

inline tbd_lru_info_Param
ConvertToChainInfo( const Data_LRUChainInfo chainInfo )
{
    switch( chainInfo )
    {
    case Data_LruMiddle:
        return lru_mid;
    case Data_LruLast:
        return lru_last;
    default:
        return lru_normal;
    }
}

/* --------------------------------------------------------------------------- */
Data_PageAccessManager::Data_PageAccessManager
    ( tgg00_TransContext&           oldcontext,
      const Data_FileType           FileType,
      const Data_PageRecoveryMode&  RecoveryMode,
      const Data_PageNo             RootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::Data_PageAccessManager", DataPam_Trace, 5);

    SAPDBTRACE_WRITELN (DataPam_Trace, 6, "FileType: " << FileType);

    m_Current            = b01nilcurrent;
    m_Current.curr_trans = &oldcontext;

    tgg00_FileId &FileId = m_Current.curr_tree_id;

    if ( RootPageNo.IsValid()
         &&
         ! Kernel_IAdminConfig::Instance().MigrationHandler().
         SearchForChangeInfo("HISTORY ROOT ID CHECK").IsEnabled() ) // PTS 1114877 UH 2002-03-18
        FileId.fileRoot_gg00() = RootPageNo; // PTS 1114728 UH 2002-03-11

 
    switch (FileType)
    {
    case Data_UnknownFile:
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_UndoFile:
        FileId.fileTfn_gg00().becomes(tfnUndoLog_egg00);
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_RedoFile:
        FileId.fileTfn_gg00().becomes(tfnRedoLog_egg00);
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_HistoryFile:
        FileId.fileTfn_gg00().becomes(tfnHistory_egg00);
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_HistoryDirectory:
        FileId.fileTfn_gg00().becomes(tfnHistoryDirectory_egg00);
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_OpenTransFile:
        FileId.fileTfn_gg00().becomes(tfnOpenTrans_egg00);
        FileId.fileType_gg00().clear();
        FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        FileId.fileType_gg00().addElement (ftsPerm_egg00);
        break;
    case Data_TableFile:
        {
            if( RecoveryMode.PageIsRecoverable())
            {
                FileId.fileTfn_gg00().becomes(tfnTable_egg00);
                FileId.fileType_gg00().clear();
                FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
                FileId.fileType_gg00().addElement (ftsPerm_egg00);
            }
            else
            {
                FileId.fileTfn_gg00().becomes(tfnTemp_egg00);
                FileId.fileType_gg00().clear();
                FileId.fileType_gg00().addElement (ftsConcurrent_egg00);
                FileId.fileType_gg00().addElement (ftsTemp_egg00);
            }
            break;
        }
    case Data_IndexFile:
        RTE_Crash(Data_Exception(__CONTEXT__, DATA_FILE_TYPE_INVALID, SAPDB_ToString(FileType)));
        break;
    case Data_FixSizeTempFile:
        g04build_temp_tree_id (FileId, oldcontext);
        break;
    default:
        RTE_Crash(Data_Exception(__CONTEXT__, DATA_FILE_TYPE_INVALID, SAPDB_ToString(FileType)));
    }
}
/* --------------------------------------------------------------------------- */
Data_PageAccessManager::Data_PageAccessManager(
                                               Data_PageAccessManager&         Pam,
                                               const Data_FileType             FileType,
                                               const Data_PageRecoveryMode&    RecoveryMode,
                                               const Data_PageNo               RootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::Data_PageAccessManager(pam)", DataPam_Trace, 5);
    
    *this = Data_PageAccessManager ( *(Pam.m_Current.curr_trans), FileType, RecoveryMode, RootPageNo);
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::GetPage (Data_BasePage                   &Page,
                                      const Data_PageNo           &PageNo,
                                      const Data_PageRecoveryMode &RecoveryMode,
                                      Data_AccessMode              AccessMode,
                                      SAPDBErr_MessageList        &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::GetPage", DataPam_Trace, 5);

    tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    if ( trError != e_ok )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_CALLED_WITH_ERROR, "PAM::GetPage");
        Kernel_AppendBasisError (trError, errlist, "");
        return false;
    }

    if ( Page.IsAssigned() )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_PAGE_ALREADY_ASSIGNED);
        Page.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( RecoveryMode.UseStaticPageAddressing() )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_RECOVERY_MODE_STATIC_NOT_POSSIBLE);
        return false;
    }

    tbd_node_ptrs Nptrs;

    Nptrs.np_ptr()   = NULL;
    Nptrs.np_cbptr() = NULL;

    if ( Data_ForStructureChange == AccessMode )
        bd13GetNode (m_Current, PageNo, plmLockWithSVPSync_ebd00,
                     nr_for_update, Nptrs);
    else
        bd13GetNode (m_Current, PageNo, plmLock_ebd00,
                     ConvertToRequestMode(AccessMode), Nptrs);

    if ( DataPam_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "PAMget: pno: " << PageNo
        << " ("            << Data_AddressingModeStrings [RecoveryMode.GetAddressingMode()]
        << "/"             << Data_RecoverabilityStrings [RecoveryMode.GetRecoverability()]
        << ") access: "    << Data_AccessModeStrings  [Page.m_AccessMode]
        << ", trError: "   << Kernel_BasisErrorToString(trError);
    }

    // PTS 1126056 UH 2003-12-09
    // RegisterToPage must be called before return or bd13release must be called
    // otherwise the cached page is not released.
    RegisterToPage (Page, Nptrs, RecoveryMode, AccessMode);

    if ( trError != e_ok )
    {
        Page.AppendContextInfo(__CONTEXT__,errlist, "PAM::GetPage");
        Kernel_AppendBasisError (trError, errlist, "bd13GetNode");
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::GetPage (Data_BasePage                   &Page,
                                      const Data_PageNo           &PageNo,
                                      Data_AccessMode              AccessMode,
                                      SAPDBErr_MessageList        &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::GetPage2", DataPam_Trace, 5);

    return GetPage (Page, PageNo, Page.RecoveryMode(), AccessMode, errlist);
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::GetPage (Data_BasePage                   &Page,
                                      const Data_PageNo           &PageNo,
                                      SAPDBErr_MessageList        &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::GetPage3", DataPam_Trace, 5);

    return GetPage (Page, PageNo, Page.RecoveryMode(), Page.AccessMode(), errlist);
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::ReleasePage (Data_BasePage            &Page,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::ReleasePage", DataPam_Trace, 5);

    // because this is used within a destructor the state must be checked here

    if ( Page.NodePtr() != NULL && Page.GetHint() != NULL )
    {
        tbd_node_ptrs     Nptrs;
        const Data_PageNo AuxPageNo = Page.PageNo();
        Data_PageId       pageid;

        Page.PageId (pageid);

        Nptrs.np_ptr()   = Page.NodePtr();
        Nptrs.np_cbptr() = STATIC_CAST(tbd_node*,Page.GetHint());

              tgg00_BasisError &trError  = m_Current.curr_trans->trError_gg00;
        const tgg00_BasisError  AuxError = trError;

        trError = e_ok;

        if ( Data_Changed == Page.m_ReleaseMode )
            b13w_release_node (Nptrs, m_Current);
        else
            b13r_release_node (Nptrs, m_Current, ConvertToChainInfo( Page.m_LruChainInfo ));

        if ( DataPam_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "PAMrel: pno: " << AuxPageNo
            << " ("            << Data_AddressingModeStrings [pageid.PageRecoveryMode().GetAddressingMode()]
            << "/"             << Data_RecoverabilityStrings [pageid.PageRecoveryMode().GetRecoverability()]
            << ") access: "    << Data_AccessModeStrings  [Page.m_AccessMode]
            << ", release: "   << Data_ReleaseModeStrings [Page.m_ReleaseMode]
            << ", trError: "   << Kernel_BasisErrorToString(trError);
        }

        if ( e_ok != trError )
        {
            errlist = Data_Exception(__CONTEXT__, DATA_PAGE_INFO,
                                      "PAM::ReleasePage",
                                      SAPDB_ToString(AuxPageNo,_T_d),
                                      Data_AddressingModeStrings [pageid.PageRecoveryMode().GetAddressingMode()],
                                      Data_RecoverabilityStrings [pageid.PageRecoveryMode().GetRecoverability()],
                                      Data_AccessModeStrings  [Page.m_AccessMode],
                                      "-",
                                      "-" );
            Kernel_AppendBasisError (trError, errlist, 
                              Data_Changed == Page.m_ReleaseMode
                              ? "b13w_release_node"
                              : "b13r_release_node");
            Page.SetNodePtr (NULL);
            Page.SetHint    (NULL);
            return false;
        }
        trError = AuxError;
    }
    Page.SetNodePtr (NULL);
    Page.SetHint    (NULL);
    return true;
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::NewPage (Data_BasePage&             Page,
                                      Data_PageRecoveryMode  Mode,
                                      SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::NewPage", DataPam_Trace, 5);

	tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    if ( trError != e_ok )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_CALLED_WITH_ERROR, "PAM::NewPage");
        Kernel_AppendBasisError (trError, errlist,"");
        return false;
    }

    if ( Page.IsAssigned() )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_PAGE_ALREADY_ASSIGNED);
        Page.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( Mode.UseStaticPageAddressing() )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_RECOVERY_MODE_STATIC_NOT_POSSIBLE);
        return false;
    }

    tbd_node_ptrs Nptrs;

    Nptrs.np_ptr()   = NULL;
    Nptrs.np_cbptr() = NULL;

    b13new_node (LEAF_LEVEL_BD00, Nptrs, m_Current);

    if ( DataPam_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "PAMnew: t"   << m_Current.curr_trans->trTaskId_gg00
        << ", pno: "     << Nptrs.np_ptr()->nd_id()
        << " ("          << Data_AddressingModeStrings [Mode.GetAddressingMode()]
        << "/"           << Data_RecoverabilityStrings [Mode.GetRecoverability()]
        << ") FileRoot: "<< SAPDB_ToString(m_Current.curr_tree_id.fileRoot_gg00());
    }

    RegisterToPage (Page, Nptrs, Mode, Data_ForUpdate);

    if ( trError != e_ok )
    {
        Page.AppendContextInfo(__CONTEXT__,errlist, "PAM::NewPage");
        Kernel_AppendBasisError (trError, errlist, "b13new_node");
        return false;
    }

    // With new pages with will not work !!!
    Nptrs.np_ptr()->nd_trans_id() = m_Current.curr_trans->trWriteTransId_gg00;

    if ( m_Current.curr_tree_id.fileRoot_gg00() == NIL_PAGE_NO_GG00 )
        m_Current.curr_tree_id.fileRoot_gg00() = Nptrs.np_ptr()->nd_id();

    Page.InitializeFrame();
    return true;
}

/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::FreePage (Data_BasePage            &Page,
                                       SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::FreePage", DataPam_Trace, 5);

	tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    if ( trError != e_ok )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_CALLED_WITH_ERROR, "PAM::FreePage");
        Kernel_AppendBasisError (trError, errlist,"");
        return false;
    }

    if ( ! Page.IsAssigned() )
    {
        Page.AppendContextInfo(__CONTEXT__,errlist,"PAM::FreePage() error");
        return false;
    }

    tbd_node_ptrs Nptrs;

    Nptrs.np_ptr()   = Page.NodePtr();
    Nptrs.np_cbptr() = static_cast<tbd_node*>(Page.GetHint());

    const Data_PageNo           AuxPageNo    = Page.PageNo();
    const Data_PageRecoveryMode RecoveryMode = Page.RecoveryMode();

    Page.SetNodePtr (NULL);
    Page.SetHint    (NULL);

    b13free_node (Nptrs, m_Current);

    if ( DataPam_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "PAMfreePage: pno: " << AuxPageNo
        << " ("            << Data_AddressingModeStrings [RecoveryMode.GetAddressingMode()]
        << "/"             << Data_RecoverabilityStrings [RecoveryMode.GetRecoverability()]
        << ") access: "    << Data_AccessModeStrings     [Page.m_AccessMode]
        << ", release: "   << Data_ReleaseModeStrings    [Page.m_ReleaseMode]
        << ", trError: "   << Kernel_BasisErrorToString(trError);
    }

    if ( e_ok != trError )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_PAGE_INFO,
                                  "PAM::FreePage",
                                  SAPDB_ToString(AuxPageNo),
                                  Data_AddressingModeStrings [RecoveryMode.GetAddressingMode()],
                                  Data_RecoverabilityStrings [RecoveryMode.GetRecoverability()],
                                  Data_AccessModeStrings  [Page.m_AccessMode],
                                  Data_ReleaseModeStrings [Page.m_ReleaseMode],
                                  "-" );
        Kernel_AppendBasisError (trError, errlist, "b13free_node");
        return false;
    }
    return true;
}


/* --------------------------------------------------------------------------- */
bool Data_PageAccessManager::FreePageNo (
        const Data_PageNo&           PageNo,
        const Data_PageRecoveryMode& RecoveryMode,
        const SAPDB_Int4             PageConverterVersion,
        const Data_Level             Level,
        SAPDBErr_MessageList        &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_PageAccessManager::FreePageNo", DataPam_Trace, 5);

    tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    if ( trError != e_ok )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_CALLED_WITH_ERROR, "PAM::FreePageNo");
        Kernel_AppendBasisError (trError, errlist,"");
        return false;
    }

    bd13FreePageNo (PageNo, PageConverterVersion, Level, m_Current);

    if ( DataPam_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "PAM::FreePageNo: pno: " << PageNo
        << " ("            << Data_AddressingModeStrings [RecoveryMode.GetAddressingMode()]
        << "/"             << Data_RecoverabilityStrings [RecoveryMode.GetRecoverability()]
        << ") trError: "   << Kernel_BasisErrorToString(trError);
    }

    if ( e_ok != trError )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_PAGE_INFO,
                                  "FreePage",
                                  SAPDB_ToString(PageNo),
                                  Data_AddressingModeStrings [RecoveryMode.GetAddressingMode()],
                                  Data_RecoverabilityStrings [RecoveryMode.PageIsRecoverable()],
                                  "-",
                                  "-",
                                  "-" );
        Kernel_AppendBasisError (trError, errlist,"bd13FreePageNo");
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
void Data_PageAccessManager::AppendContextInfo (SAPDB_Char  const * const  filename,
                                                SAPDB_UInt4 const          linenumber,
                                                SAPDBErr_MessageList      &errlist,
                                                SAPDB_Char  const * const  title) const
{
    const tgg00_FileId       &fileid = m_Current.curr_tree_id;
    const tgg00_TransContext &trans  = *(m_Current.curr_trans); // ctor assures that ptr != 0

    errlist = errlist + Data_Exception(filename,linenumber,DATA_PAM_CONTEXT,
                                       (title!=0?title:"PAM"),
                                       SAPDB_ToString(trans.trTaskId_gg00),
                                       SAPDB_ToString(trans.trWriteTransId_gg00.gg90GetInt4()),
                                       SAPDB_ToString(fileid.fileRoot_gg00()));
//        fileid.fileType_gg00().addElement (ftsConcurrent_egg00,ftsPerm_egg00);

    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00, errlist, "error");
}

/* --------------------------------------------------------------------------- */
void Data_PageAccessManager::WriteBadFileInfo ()
{
    b06file_opmsg (0, m_Current.curr_tree_id);
    b06write_filename_and_root (m_Current.curr_tree_id);
}
