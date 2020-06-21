
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2006 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

#
# include dependencies (do not edit)
#
DBMSrvBHist_Backup.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,hcn36.h,hcn90.h,\
    gcn003.h
DBMSrvBHist_Backup.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Copy.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp,gcn00.h,\
    gcn003.h,:SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp,\
    hcn36.h
DBMSrvBHist_Copy.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Copy.hpp,hcn36.h
DBMSrvBHist_Copy.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,gcn003.h
DBMSrvBHist_EBIDHistoryFile.cpp inc=hcn42.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDHistoryFile.hpp
DBMSrvBHist_EBIDHistoryFile.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvBHist_EBIDList.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDList.hpp,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Reply.hpp,hcn30.h,hcn35.h,hcn35b.h,hcn90.h
DBMSrvBHist_EBIDList.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp
DBMSrvBHist_File.cpp inc=hcn36.h,heo06.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp
DBMSrvBHist_File.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp
DBMSrvBHist_FileDBM.cpp inc=hcn42.h,heo06.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp
DBMSrvBHist_FileLineEBF.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileLineEBF.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
DBMSrvBHist_FileLineEBF.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvBHist_FileWriterEBF.cpp inc=hcn42.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileWriterEBF.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileLineEBF.hpp
DBMSrvBHist_FileWriterEBF.hpp inc=hni34.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvBHist_History.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDHistoryFile.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistoryFile.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_MediaHistoryFile.hpp,\
    hcn35.h,hcn35b.h,hcn42.h,hcn90.h,heo06.h
DBMSrvBHist_History.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Reply.hpp,gcn00.h,gcn003.h,hcn36.h
DBMSrvBHist_HistoryFile.cpp inc=hcn42.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistoryFile.hpp
DBMSrvBHist_HistoryFile.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvBHist_HistorySession.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,hcn31.h,hcn42.h,hcn90.h
DBMSrvBHist_HistorySession.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp
DBMSrvBHist_LineProducer.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMSrvBHist_LineProducer.hpp inc=
DBMSrvBHist_ListProducer.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp
DBMSrvBHist_ListProducer.hpp inc=
DBMSrvBHist_MediaHistoryFile.cpp inc=hcn42.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_MediaHistoryFile.hpp
DBMSrvBHist_MediaHistoryFile.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvBHist_Part.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,hcn36.h,hcn90.h
DBMSrvBHist_Part.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp,gcn00.h,\
    gcn003.h,:SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp
DBMSrvBHist_PartDBM.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,hcn30.h,hcn36.h
DBMSrvBHist_RestorePlan.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlan.hpp
DBMSrvBHist_RestorePlan.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp
DBMSrvBHist_RestorePlanner.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlanner.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,hcn20.h,hcn30.h,\
    hcn35b.h,hcn31.h,hcn40.h,hcn90.h
DBMSrvBHist_RestorePlanner.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlan.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp

