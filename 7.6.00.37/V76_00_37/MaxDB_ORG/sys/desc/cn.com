# *******************************************************************************
#
#  module:       cn.com
#
#  ------------------------------------------------------------------------------
#
#  responsible:  BerndV
#
#  special area: Database Manager
#
#  description:  COM Description for all c(++) sources of the cn layer
#
#  see also:
#
#  ------------------------------------------------------------------------------
# *******************************************************************************
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2006 SAP AG
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

*       -DSAPDB$RELVER noshrglob
&if $OS in [ WIN32 ]
# *       -DBVMEMDEBUG -D_DEBUG -D_CRTDBG_MAP_ALLOC  # MSDEV memory debug
*-clu   -DCLUSTER_SERVER_VERSION
&endif

dbmapi.idl binary

gcn00.h     inc=gsp00,geo572.h,gcn00_1.h,ToolsCommon/Tools_Arguments.hpp,geo47.h
gcn00_1.h   inc=hin100.h,DBMSrvMsg_Info.hpp
gcn002.h    inc=geo002.h,DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp
gcn003.h

hcn10.h     inc=gcn00.h,DBMSrv_Command.hpp
vcn10*.cpp  inc=hcn10.h,hcn20.h,hcn30.h,hcn31.h,hcn32.h,hcn40.h,hcn41.h,hcn42.h,\
hcn43.h,hcn45.h,hcn46.h,hcn50.h,hcn51.h,hcn80.h,hcn81.h,hcn83.h,\
hcn90.h,vsp001,vsp009c,heo02.h,heo06.h,hsp100.h,\
DBMSrvHSS_Nodes.hpp,\
DBMSrvMsg_RTEError.hpp,\
DBMSrvInfrReply_Reply2MsgList.hpp,\
DBMSrvBHist_RestorePlanner.hpp,DBMSrvBHist_HistorySession.hpp,\
:SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp,\
:SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp,\
DBMSrvShM_ShmAdmin.hpp,DBMSrvKnl_GlobalSessions.hpp,DBMSrvShM_ShmMigrator.hpp,\
DBMSrvFrm_Server.hpp,\
DBMLog_Logger.hpp,DBMLog_LogFile.hpp,DBMLog_SynchronizedLogFile.hpp,DBMSrvMsg_Error.hpp,\
:SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp \

vcn10-cli.cpp   -DDBMCLI

vcn11.cpp   inc=hcn14.hpp,SAPDB_string.h,Tools_PipeCall.hpp

vcn12.cpp   inc=gsp09.h,heo01.h,heo02.h,heo13.h,hcn10.h,hcn13.h,hcn14.hpp,\
hcn90.h,hsp100.h,heo06.h,DBMCli_Stuff.hpp,RTE_GetDBRootOfDB.h,\
RTE_UNIXAuthenticate.hpp,geo43.h,Tools_PipeCall.hpp,\
RTESys_ProcessID.hpp,RTESys_SystemInfo.h

vcn12dbm*.cpp      setbuildinfo inc=heo01.h,hsp02.h,hsp77.h,geo002.h,RTEConf_ParameterAccess.hpp,Tools_PipeCall.hpp,SDBMsg_DBMSrv.h,RTE_DBRegister.hpp
vcn12dbm-start.cpp -DDBMSTART
vcn12dbm-stop.cpp  -DDBMSTOP
vcn12dbm-reg.cpp   -DDBMREG
vcn12dbm-unreg.cpp -DDBMUNREG

hcn13.h     inc=
vcn13.cpp   inc=hcn13.h,SAPDB_UTF8Basis.hpp,SAPDB_string.h

hcn14.h     inc=gsp00,geo47.h
hcn14.hpp   inc=hcn14.h,Tools_DynamicUTF8String.hpp
vcn14*.cpp  inc=hcn14.hpp,hcn90c.h,gsp09.h,,heo02.h,hsp02.h,vsp004,heo03.h,heo13.h,hsp77.h,heo01.h,gip00.h,SAPDB_string.h,\
                RTEComm_URIBuilder.hpp,\
                DBMSrvUsr_ChallengeResponseClient.hpp
vcn14-u.cpp  -DDBMUNICODE
vcn14-ut.cpp -DDBMUNICODE -DDBMUNICODETRACE
vcn14-p.cpp  -DNOWINAPI

vcn15.cpp   inc=DBMCliEvtDisp_EventListener.hpp,DBMCliEvtCm_Event.hpp

# dbm scripting
&if $OS in [ WIN32 ]
vcn16-a.c   inc=hsp100.h,hcn14.h -I$ACTIVEPERL_INCLUDE -DWIN32 -DMSWIN32 -DACTIVE_PERL -TP
vcn16-c.c   inc=hsp100.h,hcn14.h -I$COREPERL_INCLUDE -DWIN32 -DMSWIN32 -DWIN32IO_IS_STDIO
&else
vcn16-c.c   inc=hsp100.h,hcn14.h -I$COREPERL_INCLUDE
&endif

vcn17.c   inc=hsp100.h,hcn14.h -I$PYTHON_INCLUDE
dbm.pm ascii ->$INSTMISC/SAP/DBTech/
# end dbm scripting

vcn18.cpp inc=hsp100.h,RTE_FileUser.hpp,RTE_DBRegister.hpp,RTE_MessageList.hpp,\
SAPDBErr_Assertions.hpp,RTE_UNIXAuthenticate.hpp,SAPDB_string.h

hcn20.h   inc=gcn00.h,\
              :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp
vcn20.cpp inc=hcn90.h,hcn20.h,hcn23.h,hcn29.h,hcn42.h,hcn52.h,heo02.h,hcn50.h,\
hcn51.h,heo06.h,heo11.h,hsp100.h,KernelParam_IOManager.hpp,\
DBMLog_Logger.hpp,DBMSrvKnl_State.hpp,DBMSrvKnl_Session.hpp,\
DBMSrvMsg_RTEError.hpp,DBMSrv_Reply.hpp,DBMSrvMsg_NewRTEError.hpp,\
hsp02.h,Tools_HexBuffer.hpp


hcn21.h   inc=gcn00.h,\
              :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp

vcn21.cpp inc=hcn90.h,hcn21.h,hcn29.h,heo06.h,heo02.h,DBMSrvMsg_RTEError.hpp,DBMSrvDep_DirtyReply.hpp

hcn23.h   inc=gcn00.h,\
              :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp

vcn23.cpp inc=hcn90.h,hcn23.h,hcn29.h

hcn28.h   inc=gcn00.h,\
              :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp
vcn28.cpp inc=hsp100.h,hcn28.h,hcn29.h,hcn30.h,hcn42.h,hcn51.h,hcn90.h,KernelParam_IOManager.hpp,KernelParam_Converter.hpp,\
              KernelParam_SharedSQL.hpp,DBMSrv_Reply.hpp,DBMSrvDep_DirtyReply.hpp,DBMSrvMsg_RTEError.hpp

hcn29.h   inc=gcn00.h,\
              :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp

& if $MACH in [ PA11, PA20W, HP_IA64 ]
# Optimization leads to XParam files, where first entry is deleted with first direct_putparam command
vcn29.cpp inc=hcn90.h,hcn28.h,hcn29.h,heo02.h,hsp02.h,heo06.h,hcn21.h,hcn23.h,hcn50.h,hcn52.h,hsp100.h,heo01.h,\
              :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,DBMSrvDep_DirtyReply.hpp,DBMSrvMsg_RTEError.hpp,\
              DBMSrvMsg_NewRTEError.hpp \
          -NO
&else
&if $COMPILER != ICC
vcn29.cpp inc=hcn90.h,hcn28.h,hcn29.h,heo02.h,hsp02.h,heo06.h,hcn21.h,hcn23.h,hcn50.h,hcn52.h,hsp100.h,heo01.h,\
              :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,DBMSrvDep_DirtyReply.hpp,DBMSrvMsg_RTEError.hpp,\
              DBMSrvMsg_NewRTEError.hpp \
          -NO
&else
vcn29.cpp inc=hcn90.h,hcn28.h,hcn29.h,heo02.h,hsp02.h,heo06.h,hcn21.h,hcn23.h,hcn50.h,hcn52.h,hsp100.h,heo01.h,\
              :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,DBMSrvDep_DirtyReply.hpp,DBMSrvMsg_RTEError.hpp,\
              DBMSrvMsg_NewRTEError.hpp
&endif
&endif

hcn30.h   inc=gcn003.h,gcn00.h,DBMSrv_Reply.hpp
vcn30.cpp inc=hcn90.h,hcn42.h,hcn20.h,hcn30.h,hcn36.h,heo02.h,heo06.h,hcn50.h,hcn51.h,DBMSrvBTools_ToolEnum.hpp,\
              DBMSrvMsg_RTEError.hpp,DBMSrvDep_DirtyReply.hpp

hcn31.h   inc=gcn003.h,gcn00.h,DBMSrv_Reply.hpp,DBMSrvKnl_Session.hpp
vcn31.cpp inc=hcn90.h,hcn31.h,hcn20.h,hcn30.h,hcn50.h,hcn53.h,heo02.h,heo06.h,heo11.h,gkb03,hcn35b.h,hcn35c.h,geo002xp.h,hcn51.h,hsp77.h,\
              DBMSrvBHist_History.hpp,DBMSrvBMedia_MediaGroup.hpp,DBMSrvKnl_State.hpp,DBMSrvKnl_GlobalSessions.hpp,\
              DBMSrvMsg_RTEError.hpp,DBMSrv_Reply.hpp

hcn32.h   inc=gcn003.h,gcn00.h
vcn32.cpp inc=DBMSrvBHist_EBIDList.hpp,hcn90.h,hcn32.h


hcn35.h     inc=hcn35d.h,hni33.h,DBMSrvBTools_ToolEnum.hpp
vcn35.cpp   inc=hcn35.h,hcn351.h,hcn352.h,hcn353.h,hcn36.h,SAPDB_string.h

hcn35a.h    inc=hcn35.h,hcn35d.h
vcn35a.cpp  inc=gcn003.h,hcn35a.h,hni00b.h,hcn36.h

hcn35b.h    inc=DBMSrvBHist_EBIDList.hpp,gcn00.h,gcn003.h,hcn35a.h,hcn35e.h,hcn36.h,hcn90.h
vcn35b.cpp  inc=hcn20.h,hcn31.h,hcn32.h,hcn35b.h,hcn36.h,hcn42.h,hcn50.h,hcn51.h,hcn53.h,DBMSrv_Reply.hpp,\
                DBMSrvHSS_Nodes.hpp,DBMSrvBMedia_MediaGroup.hpp,DBMSrvKnl_GlobalSessions.hpp

hcn35c.h    inc=gcn00.h,gcn003.h,hcn35a.h,hcn35e.h
vcn35c.cpp  inc=hcn35c.h,hcn36.h,hcn42.h,hcn51.h,hcn90.h,DBMSrvBHist_History.hpp,DBMSrvBLabel_Label.hpp,DBMSrvBMedia_MediaGroup.hpp,DBMSrvHSS_Nodes.hpp

hcn35d.h    inc=hni34.h,hni35.h,DBMSrvMsg_Error.hpp
vcn35d.cpp  inc=gcn003.h,hcn35d.h,hcn36.h

hcn35e.h    inc=gcn003.h,hni31.h,hcn35d.h
vcn35e.cpp  inc=gcn00.h,hcn20.h,hcn35e.h,hcn36.h,hcn42.h,hcn51.h,hcn53.h,SAPDB/ToolsCommon/Tools_List.hpp,\
                DBMSrvInfrReply_ReplyBuffer.hpp

hcn351.h    inc=hcn35.h,hcn35a.h
vcn351.cpp  inc=hcn351.h,hcn36.h,hni34.h,SAPDB_string.h

hcn352.h    inc=hni31.h,hcn35.h,hcn36.h,Tools_Vector.hpp,ToolsTime_TimeFormat.hpp,Tools_Vector.hpp
vcn352.cpp  inc=hcn352.h,hni34.h,hcn36.h,ToolsTime_Time.hpp,\
            ToolsTime_TimeFormatter.hpp,Tools_CommandlineToArgumentsConverter.hpp,SAPDB_string.h

hcn353.h    inc=hni31.h,hcn35.h
vcn353.cpp  inc=gcn003.h,hni34.h,hcn353.h,hcn36.h,hcn42.h,DBMSrvBHist_EBIDHistoryFile.hpp

hcn36.h     inc=hni00b.h,hni33.h,hni34.h,hni35.h
vcn36.cpp   inc=hni31.h,hcn36.h,SAPDB_string.h

vcn37.cpp   inc=hni32.h,hni34.h,hni35.h,hcn36.h

hcn38.h     inc=hni00b.h,hni32.h,hni33.h,hni34.h
vcn38.cpp   inc=hcn38.h,hcn382.h,hcn383.h,hcn384.h,hcn36.h,hcn385.h,hcn386.h,SAPDB_string.h

hcn381.h    inc=hcn38.h
vcn381.cpp  inc=hcn36.h,hcn381.h,hcn385.h,SAPDB_string.h

hcn382.h    inc=hcn38.h,hcn381.h,hcn386.h
vcn382.cpp  inc=hcn36.h,hcn382.h,hcn385.h

hcn383.h    inc=hcn38.h,hcn381.h,hcn386.h
vcn383.cpp  inc=hcn36.h,hcn383.h,hcn385.h,SAPDB_string.h

hcn384.h    inc=hcn38.h,hcn381.h,hcn386.h
vcn384.cpp  inc=hcn36.h,hcn384.h,hcn385.h,SAPDB_string.h

hcn385.h    inc=hni34.h,hni35.h
vcn385.cpp  inc=hcn36.h,hcn385.h,SAPDB_string.h

vcn386.cpp  inc=hcn36.h,hcn385.h,hcn386.h

hcn40.h   inc=gcn00.h,DBMSrv_Reply.hpp

vcn40*.cpp inc=hcn90.h,hcn42.h,hcn20.h,hcn31.h,hcn36.h,hcn40.h,hcn44.h,hcn46.h,\
hcn50.h,hcn51.h,vsp001,gsp09.h,heo02.h,heo03.h,gip00.h,\
heo06.h,hsp100.h,geo43.h,heo11.h,hsp77.h,\
RTE_UserProfileContainer.hpp,Tools_System.hpp,DBMSrvHSS_Nodes.hpp,\
RTE_UNIXAuthenticate.hpp,KernelParam_IOManager.hpp,Tools_PipeCall.hpp,ToolsParsersUtil_IdentifierCheck.hpp,\
RTE_DBRegister.hpp,DBMSrvKnl_Session.hpp,DBMSrvKnl_GlobalSessions.hpp,DBMSrvCmd_SilentDispatcherUsingCommand.hpp,\
DBMSrvVol_Volumes.hpp,DBMLog_Logger.hpp,ToolsMeMa_GuardedOperation.hpp,\
DBMSrvMsg_RTEError.hpp,DBMSrvMsg_NewRTEError.hpp,DBMSrvCmdDb_DbDrop.hpp,DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp,\
DBMSrvProc_ServerProcess.hpp,DBMSrvProc_EnvironmentVariable.hpp

hcn41.h   inc=gcn00.h
vcn41.cpp inc=hcn90.h,hcn41.h,hcn42.h,hcn50.h,hcn51.h,hcn53.h,hcn20.h,\
vsp009c,vsp001,SAPDB/DBM/Srv/DBMSrvHSS_Nodes.hpp,RTE_UNIXAuthenticate.hpp,\
ToolsParsersUtil_IdentifierCheck.hpp,RTE_UserProfileContainer.hpp,DBMSrv_Reply.hpp,DBMSrvMsg_RTEError.hpp

hcn42.h   inc=gcn00.h
vcn42.cpp inc=heo01.h,heo06.h,geo43_0.h,hcn90.h,hcn20.h,hcn40.h,hcn42.h,hcn54.h,DBMSrvScd_Persistor.hpp,\
              DBMSrvMsg_RTEError.hpp,DBMSrvDep_DirtyReply.hpp,DBMSrvMsg_NewRTEError.hpp,DBMSrvShM_Persistence.hpp,SAPDB_Names.h

hcn43.h   inc=gcn00.h
vcn43.cpp inc=Tools_System.hpp,DBMSrvProc_ServerProcess.hpp,DBMSrvKnl_State.hpp,\
              heo01.h,hcn40.h,hcn43.h,hcn50.h,hcn90.h

vcn44.cpp inc=hcn44.h,:extern/zlib/zlib.h,:SAPDB/SAPDBCommon/SAPDB_string.h

hcn45.h   inc=gcn00.h
vcn45.cpp inc=hcn45.h,hcn20.h,hcn40.h,hcn50.h,hcn90.h,DBMSrvKnl_State.hpp,DBMSrvKnl_Session.hpp

hcn46.h   inc=gcn00.h
vcn46.cpp inc=hcn46.h,hsp100.h,hcn20.h,hcn40.h,hcn50.h,hcn90.h,\
              DBMSrvHSS_Nodes.hpp,DBMSrvKnl_Session.hpp

hcn50.h   inc=gcn00.h,DBMSrvMsg_Error.hpp
vcn50.cpp inc=hcn20.h,hcn42.h,hcn50.h,hcn51.h,hcn90.h,hsp02.h,geo47.h,heo11.h,heo02.h,\
              RTE_UserProfileContainer.hpp,DBMSrvKnl_State.hpp,DBMCliEvtDisp_Permissions.hpp,\
              DBMSrvKnl_Session.hpp,ToolsParsersUtil_IdentifierCheck.hpp

hcn51.h   inc=gsp00,Msg_List.hpp
vcn51.cpp inc=hcn51.h,hcn90.h,hcn42.h,heo06.h,DBMSrvMsg_RTEError.hpp

hcn52.h   inc=gsp00,gcn002.h,DBMSrvMsg_Error.hpp
vcn52.cpp inc=hcn52.h,hcn42.h,hcn90.h,heo06.h,heo02.h,DBMSrvMsg_NewRTEError.hpp,DBMSrvMsg_RTEError.hpp

hcn53.h   inc=gsp00,gcn00_1.h
vcn53.cpp inc=hcn53.h,heo06.h,heo02.h,gcn00_1.h,hcn42.h

hcn54.h   inc=gsp00
vcn54.cpp inc=gcn00.h,hcn54.h,heo06.h,:extern/zlib/zlib.h,DBMSrvMsg_RTEError.hpp

hcn80.h   inc=gcn00.h,hcn50.h
vcn80.cpp inc=vsp009c,hcn20.h,hcn31.h,hcn51.h,hcn80.h,hcn90.h,\
              DBMSrvKnl_DbAndSqlCommands.hpp,\
              DBMSrvCmdDb_DbConnect.hpp,\
              DBMSrvCmdDb_DbRelease.hpp,\
              DBMSrvCmdDb_DbExecuteNice.hpp,\
              ToolsParsersUtil_IdentifierCheck.hpp,\
              DBMSrvMsg_RTEError.hpp

hcn81.h   inc=gcn00.h
vcn81.cpp inc=DBMSrvKnl_GlobalSessions.hpp,hcn50.h,hcn90.h,hcn81.h,\
              DBMSrv_Reply.hpp,DBMSrvMsg_SQLError.hpp

hcn82.h   inc=gcn00.h,vsp0031
vcn82.cpp inc=hcn90.h,hcn51.h,hcn80.h,hcn82.h,geo47.h,DBMSrvKnl_State.hpp

hcn83.h   inc=gcn00.h
vcn83.cpp inc=hcn50.h,hcn90.h,hcn83.h,:SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp

hcn90.h    inc=gcn00.h,Msg_List.hpp,hcn90c.h
vcn90.cpp  inc=hcn90.h,heo06.h,heo02.h,hsp02.h,heo01.h,vsp001,vsp009c,geo00_2.h,\
               DBMSrv_Reply.hpp,DBMSrvMsg_Error.hpp,DBMSrvDep_DirtyReply.hpp,\
               RTE_GetDBRootOfDB.h,RTE_DBRegister.hpp,RTE_ActiveInstallation.hpp,RTE_ActiveDatabase.hpp
                
hcn90c.h   inc=gsp00
vcn90c.cpp inc=hcn90c.h,hsp02.h,SAPDB_string.h

#Test
vcn800.cpp -x=compcpc++  -I$SAPDBSDK/incl

# RFC Server
hcn900rfcsrv.h       dep=:sap/700/saprfc.h,:sap/700/sapitab.h,:sap/700/sapuc.h,:sap/700/sapucx.h

vcn900rfcsrv.cpp     inc=hcn900rfcsrv.h
vcn900rfcsrv-u.cpp   inc=hcn900rfcsrv.h -DSAPwithUNICODE

hcn901dbmrfc.h       inc=hin100.h,hcn900rfcsrv.h,hcn904table.h,:SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
vcn901dbmrfc.cpp     inc=hcn901dbmrfc.h,hcn903dbmsrv.h,hsp100.h setbuildinfo
vcn901dbmrfc-u.cpp   inc=hcn901dbmrfc.h,hcn903dbmsrv.h,hsp100.h -DSAPwithUNICODE setbuildinfo

vcn902rfcmain.cpp    inc=heo670.h,hcn901dbmrfc.h,:SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp -except
vcn902rfcmain-u.cpp  inc=heo670.h,hcn901dbmrfc.h,:SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp -DSAPwithUNICODE -except

hcn903dbmsrv.h       inc=gsp00
vcn903dbmsrv.cpp     inc=hcn903dbmsrv.h,hcn14.h,heo02.h,heo13.h,hcn90.h

hcn904table.h        inc=hcn900rfcsrv.h
vcn904table.cpp      inc=hcn904table.h
vcn904table-u.cpp    inc=hcn904table.h -DSAPwithUNICODE


cserv.pcf inc= ascii ->$INSTROOT/env/ unpack=no concom=yes \
&if defined LC_STATE
	LC_STATE=$LC_STATE
&else
	LC_STATE=DEV
&endif
